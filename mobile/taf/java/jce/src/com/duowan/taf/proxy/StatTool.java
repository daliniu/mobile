package com.duowan.taf.proxy;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import com.duowan.taf.StatFPrxHelper;
import com.duowan.taf.StatMicMsgBody;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.proxy.utils.TafUtils;

/**
 * 统计工具
 * @author easezhang
 *
 */
public class StatTool {
	
	private final static int BATCH_REPORTS = 10;
	
	private ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> statInfos = new ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody>();
	
	public static final List<Integer> DefaultTimeStatInterv = new ArrayList<Integer>();
	static {
		DefaultTimeStatInterv.add(5);
		DefaultTimeStatInterv.add(10);
		DefaultTimeStatInterv.add(50);
		DefaultTimeStatInterv.add(100);
		DefaultTimeStatInterv.add(200);
		DefaultTimeStatInterv.add(500);
		DefaultTimeStatInterv.add(1000);
		DefaultTimeStatInterv.add(2000);
		DefaultTimeStatInterv.add(3000);
	}
	
	public static StatMicMsgHead getHead(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue) {
		return TafUtils.getHead(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort, returnValue);
	}
	
	public void setPointStatInterv(StatMicMsgHead head, List<Integer> _timeStatInterv) {
		if(statInfos.containsKey(head)) {
			statInfos.get(head).setPointStatInterv(_timeStatInterv);
		}
	}
	
	public SendMsgStatBody getStatBody(StatMicMsgHead head) {
		SendMsgStatBody body = statInfos.get(head);
		if(body==null) {
			body = new SendMsgStatBody(DefaultTimeStatInterv);
			statInfos.putIfAbsent(head, body);
		}
		return body;
	}
	
	public StatMicMsgHead addStat(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue, List<Integer> _timeStatInterv) {
		StatMicMsgHead head = TafUtils.getHead(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort, returnValue);
		List<Integer> invs = _timeStatInterv;
		if(invs == null) {
			invs = DefaultTimeStatInterv;
		}
		if(!statInfos.containsKey(head)) {
			statInfos.putIfAbsent(head, new SendMsgStatBody(invs));
		}
		return head;
	}
	
	public StatMicMsgHead addStat(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue) {
		return addStat(masterName, slaveName, interfaceName, masterIp, slaveIp, slavePort, returnValue, DefaultTimeStatInterv);
	}
	
	public void addSuccTime(StatMicMsgHead head, int costTimeMill) {
		getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusSucc);
	}
	
	public void addTimeoutTime(StatMicMsgHead head, int costTimeMill) {
		getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusTimeout);
	}
	
	public void addSuccExce(StatMicMsgHead head, int costTimeMill) {
		getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusExec);
	}
	
	public void addInvokeTime(StatMicMsgHead head, long costTimeMill, StatResult eResult) {
		if ( eResult == StatResult.STAT_SUCC) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusSucc);
		} else if ( eResult == StatResult.STAT_EXCE ) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusExec);
		} else if ( eResult == StatResult.STAT_TIMEOUT) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusTimeout);
		}
	}
	
	public void addInvokeTime(StatMicMsgHead head, long costTimeMill, int eResult) {
		if ( eResult == SendMsgStatBody.CallStatusSucc) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusSucc);
		} else if ( eResult == SendMsgStatBody.CallStatusExec ) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusExec);
		} else if ( eResult == SendMsgStatBody.CallStatusTimeout) {
			getStatBody(head).onCallFinished(costTimeMill, SendMsgStatBody.CallStatusTimeout);
		}
	}
	
	public void doReportStat(StatFPrxHelper statPrxPrx) {
		int successCount = 0;
		int errorCount = 0;
		HashMap<StatMicMsgHead, StatMicMsgBody> newMap = new HashMap<StatMicMsgHead, StatMicMsgBody>();
		int i = 0;
		long start = System.currentTimeMillis();
		for (Entry<StatMicMsgHead, SendMsgStatBody> sendMsgStatBodyEntry : statInfos.entrySet()) {
			SendMsgStatBody body = sendMsgStatBodyEntry.getValue();
			HashMap<Integer, Integer> _intervalCount = new HashMap<Integer, Integer>();
			//为了迅速统计完成后clear
			for (Entry<Integer, AtomicInteger> bentry : body.intervalCount.entrySet()) {
				_intervalCount.put(bentry.getKey(), bentry.getValue().get());
			}
			StatMicMsgBody mbody = new StatMicMsgBody(body.getCount(), body.getTimeoutCount(), body.getExecCount(),
					_intervalCount, body.getTotalRspTime(), body.getMaxRspTime(), body.getMinRspTime());
			body.clearStatData();
			newMap.put(sendMsgStatBodyEntry.getKey(), mbody);
			TafLoggerCenter.info(sendMsgStatBodyEntry.getKey().masterName + " " + " report call "
					+ sendMsgStatBodyEntry.getKey().slaveIp + ":" + sendMsgStatBodyEntry.getKey().slavePort + " "
					+ sendMsgStatBodyEntry.getKey().slaveName + "." + sendMsgStatBodyEntry.getKey().interfaceName
					+ ":" + mbody.count+"_"+mbody.execCount+"_"+mbody.timeoutCount+"_"+mbody.totalRspTime+"_"+mbody.maxRspTime+"_"+mbody.minRspTime);
			i++;
			if (i % BATCH_REPORTS == 0) {
				try {
					statPrxPrx.async_reportMicMsg(null, newMap, true);
					++successCount;
				} catch (Exception e) {
					TafLoggerCenter.error("reportMsg error ", e);
					++errorCount;
				}
				newMap = new HashMap<StatMicMsgHead, StatMicMsgBody>();
			}
		}
		if (newMap.size() > 0) {
			try {
				statPrxPrx.async_reportMicMsg(null,newMap, true);
				successCount++;
			} catch (Exception e) {
				errorCount++;
				TafLoggerCenter.error("finally reportMsg error objectName " , e);
			}
		}
		TafLoggerCenter.info("reportMsg " +  " success:" + successCount + " fail:" + errorCount
				+ " costTime:" + (System.currentTimeMillis() - start));
	}
	
	public ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> getStats() {
		return this.statInfos;
	}
	
	public int size() {
		return this.statInfos.size();
	}
	
	public List<Integer> getStatIntervals() {
		Collection<SendMsgStatBody> collection =  statInfos.values();
		if(!collection.isEmpty()) {
			for(SendMsgStatBody body:collection) {
				if(body != null) {
					return body.timeStatInterv;
				}
			}
		}
		return DefaultTimeStatInterv;
	}
}
