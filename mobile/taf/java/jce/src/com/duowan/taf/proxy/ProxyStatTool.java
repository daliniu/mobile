package com.duowan.taf.proxy;

import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;

import com.duowan.taf.StatFPrxHelper;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.StatSampleMsg;
import com.duowan.taf.cnst.JCEMESSAGETYPESAMPLE;

public class ProxyStatTool implements Runnable {
	
	public final static ProxyStatTool Instance = new ProxyStatTool();
	
	private StatTool statTool = new StatTool();
	
	private final static int MAX_SAMPLE_MSGS = 1000;
	
	private ConcurrentHashMap<ServantProxy, StatTool> proxyStats = new ConcurrentHashMap<ServantProxy, StatTool>();
	
	private ConcurrentHashMap<ServantProxy, ConcurrentLinkedQueue<StatSampleMsg>> proxySamples = new ConcurrentHashMap<ServantProxy, ConcurrentLinkedQueue<StatSampleMsg>>();
	
	private CopyOnWriteArrayList<Integer> timeStatInterv = new CopyOnWriteArrayList<Integer>();
	
	private int runCount = 0;
	
	private StatFPrxHelper selfStatPrxPrx;
	
	public void init(StatFPrxHelper statPrxPrx) {
		if(statPrxPrx != null) {
			this.selfStatPrxPrx = statPrxPrx;
		}
	}
	
	public ProxyStatTool() {
		timeStatInterv.addAll(StatTool.DefaultTimeStatInterv);
	}
	
	public void resetStatInterv() {
		timeStatInterv.clear();
		timeStatInterv.addAll(StatTool.DefaultTimeStatInterv);
		setPointStatInterv();
	}
	
	public CopyOnWriteArrayList<Integer> getStatIntervals() {
		return timeStatInterv;
	}
	
	public StatTool addStatTool(ServantProxy proxy) {
		StatTool statTool = proxyStats.get(proxy);
		if(statTool == null) {
			statTool = new StatTool();
			proxyStats.putIfAbsent(proxy, statTool);
		}
		return statTool;
	}
	
	public StatTool getStatTool(ServantProxy proxy) {
		StatTool tool = proxyStats.get(proxy);
		if(tool == null) {
			tool = addStatTool(proxy);
		}
		return tool;
	}
	
	public StatTool getDefaultStatTool() {
		return statTool;
	}
	
	public void setPointStatInterv() {
		for (Entry<ServantProxy, StatTool> entry : proxyStats.entrySet()) {
			ServantProxy proxy = entry.getKey();
			proxy.taf_pointStatInterv(timeStatInterv);
		}
	}
	
	public void setPointStatInterv(ServantProxy proxy) {
		for (Entry<StatMicMsgHead, SendMsgStatBody> entry : getStatTool(proxy).getStats().entrySet()) {
			entry.getValue().setPointStatInterv(timeStatInterv);
		}
	}
	
	public void addStatInterv(int interv) {
		boolean add = false;
		for (int i = 0; i < timeStatInterv.size(); i++) {
			if (interv == timeStatInterv.get(i)) {
				return;
			}
			if (interv < timeStatInterv.get(i)) {
				timeStatInterv.add(i, interv);
				add = true;
				break;
			}
		}
		if (!add) {
			timeStatInterv.add(interv);
		}
		setPointStatInterv();
	}
	
	public void addSample(ServantProxy proxy, String remoteHost, String processName, SendMessage msg) {
		if(msg.isMsgType(JCEMESSAGETYPESAMPLE.value)) {
			StatSampleMsg sample=msg.sample;
			if(sample!=null) {
				sample.slaveIp=remoteHost;
				sample.masterName=processName;
				proxySamples.putIfAbsent(proxy, new ConcurrentLinkedQueue<StatSampleMsg>());
				if(proxySamples.get(proxy).size() > MAX_SAMPLE_MSGS) {
					TafLoggerCenter.error("can not add sample for" + proxy.taf_proxyName()+", queue size>"+MAX_SAMPLE_MSGS);
					return;
				}
				proxySamples.get(proxy).add(sample);
			}
		}
	}
	
	public void doSample(ServantProxy proxy) {
		ConcurrentLinkedQueue<StatSampleMsg> samples = proxySamples.get(proxy);
		if(samples == null) {
			return;
		}
		int sampleSize = samples.size();
		if ( sampleSize > 0 ) {
			StatSampleMsg[] msgs = new StatSampleMsg[sampleSize];
			for ( int i = 0 ; i < sampleSize ; i++ ) {
				msgs[i] = samples.poll();
			}
			int maxArrsySize = 10 ;
			int i = 0 ;
			StatFPrxHelper helper = proxy.getStatPrxPr();
			while ( sampleSize > maxArrsySize ) {
				StatSampleMsg[] dst = new StatSampleMsg[maxArrsySize];
				System.arraycopy(msgs, i*maxArrsySize, dst, 0, maxArrsySize);
				if(helper != null) {
					helper.reportSampleMsg(dst);
				}
				sampleSize = sampleSize - maxArrsySize ;
				i++;
			}
			StatSampleMsg[] dst = new StatSampleMsg[sampleSize];
			System.arraycopy(msgs, msgs.length-sampleSize, dst, 0, sampleSize);
			if(helper != null) {
				helper.reportSampleMsg(msgs);
			}
		}
	}
	
	public int getDefaultStatSize() {
		return this.statTool.size();
	}
	
	
	public void run() {
		try {
			runCount++;
			boolean report = false ;
			
			for (Entry<ServantProxy, StatTool> entry : this.proxyStats.entrySet()) {
				ServantProxy proxy = entry.getKey();
				StatFPrxHelper statPrxPrx = proxy.getStatPrxPr();
				proxy.checkConnStat();
				// doPing
				if (runCount % 6 == 0) {
					runCount = 0;
					report = true ;
					proxy.checkSampleMap();
					if (null != statPrxPrx) {
						try {
							// Proxy的模块调用信息统计区间
							ProxyStatTool.Instance.getStatTool(proxy).doReportStat(statPrxPrx);;
						} catch (Exception e) {
							TafLoggerCenter.error("ReportStatWorker reportStatMsg error objectName " + proxy.proxyConfig.objectName, e);
						}
						try {
							// Proxy的样本信息统计区间
							ProxyStatTool.Instance.doSample(proxy);
						} catch (Exception e) {
							TafLoggerCenter.error("ReportStatWorker reportSampleMsg error objectName " +  proxy.proxyConfig.objectName, e);
						}
					}
					else {
						TafLoggerCenter.info("statPrxPrx is null, have no need to stat for "+proxy.proxyConfig.objectName);
					}
				}
			}
			if ( report ) {
				if (null != selfStatPrxPrx) {
					try {
						// 单独调用上报接口填入的信息
						statTool.doReportStat(selfStatPrxPrx);
					} catch (Exception e) {
						TafLoggerCenter.error("ReportStatWorker error objectName ", e);
					}
				} else if (ProxyStatTool.Instance.getDefaultStatSize() > 0 ) {
					TafLoggerCenter.info("statPrxPrx is null, have no need to stat for custom stat");
				}
			}
		} catch (Exception e) {
			TafLoggerCenter.error("ReportStatWorker error " + e, e);
		}
	}
}
