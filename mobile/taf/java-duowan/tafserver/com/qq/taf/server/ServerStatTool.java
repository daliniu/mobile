package com.qq.taf.server;

import com.qq.taf.StatFPrxHelper;
import com.qq.taf.StatMicMsgHead;
import com.qq.taf.proxy.StatTool;
import com.qq.taf.proxy.TafLoggerCenter;
import com.qq.taf.proxy.utils.TafUtils;

public class ServerStatTool implements Runnable {
	public static final ServerStatTool Instance = new  ServerStatTool();
	
	private StatFPrxHelper statPrx;
	
	private StatTool tool = new StatTool();
	
	public void init(StatFPrxHelper statPrx) {
		if(statPrx == null) {
			TafLoggerCenter.error("can not set statProxy null ");
			return;
		}
		this.statPrx = statPrx;
	}
	
	public void addStat(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue, long callTimeMill, int callStatus) {
		StatMicMsgHead head = TafUtils.getHead(masterName, slaveName, interfaceName, masterIp,
				slaveIp, slavePort, returnValue);
		tool.getStatBody(head);
		tool.addInvokeTime(head, callTimeMill, callStatus);
	}
	
	public void run() {
		try {
			tool.doReportStat(statPrx);
		}
		catch(Throwable th) {
			SystemLogger.record("server stat report error", th);
		}
	}
	
}
