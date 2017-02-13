package com.qq.taf.server;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.server.proxy.PropertyFPrx;
import com.qq.taf.server.proxy.PropertyFPrxHelper;
import com.qq.taf.server.proxy.StatPropInfo;
import com.qq.taf.server.proxy.StatPropMsgBody;
import com.qq.taf.server.proxy.StatPropMsgHead;
import com.qq.taf.server.util.GeneralException;

/**
 * 属性上报
 * @author fanzhang
 */
public class PropertyReporterFactory {
	
	private static class ReportThread extends Thread {
		
		private static final int MAX_REPORT_SIZE=1400;
		private PropertyFPrx propertyFPrx;
		private int reportInterval;
		private Map<String,PropertyReporter> reporters;
		
		public ReportThread(Map<String,PropertyReporter> reporters,Communicator comm,String obj,int reportInterval) {
			propertyFPrx=comm.stringToProxy(obj, PropertyFPrxHelper.class);
			this.reportInterval=reportInterval;
			this.reporters=reporters;
			setName("PropertyReporter");
		}
		
		@Override
		public void run() {
			while(true) {
				try {
					Map<StatPropMsgHead,StatPropMsgBody> sendData=new HashMap<StatPropMsgHead, StatPropMsgBody>();
					int sendLen=0;
					for(PropertyReporter reporter:reporters.values()) {
						ReportData data=reporter.getReportData();
						if(sendLen+data.len<=MAX_REPORT_SIZE) {
							sendData.put(data.head,data.body);
							sendLen+=data.len;
						} else {
							propertyFPrx.async_reportPropMsg(null, sendData);
							sendData.clear();
							sendData.put(data.head, data.body);
							sendLen=data.len;
						}
					}					
					if(sendData.size()!=0) propertyFPrx.async_reportPropMsg(null, sendData);
					sleep(reportInterval);
				} catch(Throwable t) {
					SystemLogger.record("PropertyReporterFactory|ReportThread|error",t);
				}
			}
		}
		
	}
	
	private static class ReportData {
		
		public StatPropMsgHead head;
		public StatPropMsgBody body;
		public int len;
		
		public ReportData(StatPropMsgHead head,StatPropMsgBody body,int len) {
			this.head=head;
			this.body=body;
			this.len=len;
		}
		
	}
	
	public static class PropertyReporter {
		
		private static final int PROPERTY_PROTOCOL_LEN=50;
		private Collection<Policy> policies;
		private String propRptName;
		private String moduleName;
		
		public PropertyReporter(String moduleName,String propRptName,Policy[] policies) {
			this.moduleName=moduleName;
			this.propRptName=propRptName;
			Map<String,Policy> map=new HashMap<String, Policy>();
			for(Policy policy:policies) map.put(policy.desc(), policy);
			this.policies=map.values();
		}
		
		public void report(int value) {
			for(Policy policy:policies) policy.set(value);
		}
		
		public ReportData getReportData() {
			int len=0;
			StatPropMsgHead head=new StatPropMsgHead(moduleName,"",propRptName);
			len+=moduleName.length()+propRptName.length();
			int size=policies.size();
			len+=PROPERTY_PROTOCOL_LEN+size;
			StatPropInfo[] statPropInfos=new StatPropInfo[size];
			int i=0;
			for(Policy policy:policies) {
				String desc=policy.desc();
				String value=policy.get();
				statPropInfos[i++]=new StatPropInfo(desc,value);
				len+=desc.length()+value.length();
				policy.clear();
			}
			StatPropMsgBody body=new StatPropMsgBody(statPropInfos);
			return new ReportData(head,body,len);
		}
		
	}
	
	public static interface Policy {
		
		public String desc();
		public String get();
		public void set(int value);
		public void clear();
		
	}
	
	public static class Sum implements Policy {
		
		private int sum=0;

		public synchronized void clear() {
			sum=0;
		}

		public String desc() {
			return "Sum";
		}

		public synchronized String get() {
			return sum+"";
		}

		public synchronized void set(int value) {
			sum+=value;
		}
		
	}
	
	public static class Min implements Policy {

		private int min=0;
		
		public synchronized void clear() {
			min=0;
		}

		public String desc() {
			return "Min";
		}

		public synchronized String get() {
			return min+"";
		}

		public synchronized void set(int value) {
			if(value!=0 && value<min) min=value;
		}
		
	}
	
	public static class Max implements Policy {
		
		private int max=Integer.MIN_VALUE;

		public synchronized void clear() {
			max=Integer.MIN_VALUE;
		}

		public String desc() {
			return "Max";
		}

		public synchronized String get() {
			return max+"";
		}

		public synchronized void set(int value) {
			if(value>max) max=value;
		}
		
	}
	
	public static class Distr implements Policy {
		
		private static final Integer[] II={};
		private Integer[] data;
		private int[] result;
		
		public Distr(List<Integer> list) {
			if(list==null || list.size()==0) GeneralException.raise("ProperyReportFactory|Distr|initial list object can not be empty");
			this.data=new HashSet<Integer>(list).toArray(II);
			Arrays.sort(data);
			result=new int[data.length];
			for(int i=0;i<result.length;i++) result[i]=0;
		}

		public synchronized void clear() {
			for(int i=0;i<result.length;i++) result[i]=0;
		}

		public String desc() {
			return "Distr";
		}

		public synchronized String get() {
			StringBuilder sb=new StringBuilder();
			for(int i=0;i<data.length;i++) sb.append(i!=0?",":"").append(data[i]).append("|").append(result[i]);
			return sb.toString();
		}

		public synchronized void set(int value) {
			for(int i=0;i<data.length;i++) {
				if(data[i]>value) {
					result[i]++;
					break;
				}
			}
		}
		
	}
	
	public static class Avg implements Policy {

		private int count=0;
		private int sum=0;
		
		public synchronized void clear() {
			count=0;
			sum=0;
		}

		public String desc() {
			return "Avg";
		}

		public synchronized String get() {
			if(count==0) return "0";
			return (double)sum/count+"";
		}

		public synchronized void set(int value) {
			count++;
			sum+=value;
		}
		
	}
	
	public static class Count implements Policy {

		private int count=0;
		
		public synchronized void clear() {
			count=0;
		}

		public String desc() {
			return "Count";
		}

		public synchronized String get() {
			return count+"";
		}

		public synchronized void set(int value) {
			count++;
		}
		
	}
	
	private static final PropertyReporterFactory INSTANCE=new PropertyReporterFactory();
	private Map<String, PropertyReporter> reporters;
	private ReportThread reportThread;
	private String moduleName;
	private boolean configed;
	
	private PropertyReporterFactory() {
		reporters=new ConcurrentHashMap<String, PropertyReporter>();
		configed=false;
	}

	public static PropertyReporterFactory getInstance() {
		return INSTANCE;
	}
	
	public synchronized void setPropertyInfo(Communicator comm,String obj,String moduleName,int reportInterval) {
		if(configed) GeneralException.raise("PropertyReporterFactory|setPropertyInfo|error|this method should be called once at most");
		this.moduleName=moduleName;
		reportThread=new ReportThread(reporters,comm,obj,reportInterval);
		reportThread.start();
		configed=true;
	}
	
	public synchronized void createPropertyReporter(String propRptName,Policy... policies) {
		if(policies.length==0) GeneralException.raise("PropertyReporterFactory|at least one policy instance should be given");
		if(!reporters.containsKey(propRptName)) {
			PropertyReporter reporter=new PropertyReporter(moduleName,propRptName,policies);
			reporters.put(propRptName,reporter);
		}			
	}
	
	public PropertyReporter getPropertyReporter(String propRptName) {
		return reporters.get(propRptName);
	}
}
