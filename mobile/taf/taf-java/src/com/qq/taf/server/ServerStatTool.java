package com.qq.taf.server;

import com.qq.taf.StatFPrxHelper;
import com.qq.taf.StatMicMsgHead;
import com.qq.taf.proxy.CommunicatorConfig;
import com.qq.taf.proxy.StatTool;
import com.qq.taf.proxy.utils.TafUtils;

public class ServerStatTool implements Runnable {
	public static final ServerStatTool Instance = new  ServerStatTool();
	
	private StatFPrxHelper statPrx;
	
	private StatTool tool = new StatTool();
	private String setArea;
	private String setId;
	
	public void init(StatFPrxHelper statPrx, CommunicatorConfig config) {
		this.statPrx = statPrx;
		String setName = config.getSetName();
		if(config.isEnableSet() && setName!=null) {
			String [] tmpStrs = setName.split(".");
			if(tmpStrs.length >= 3) {
				setArea = tmpStrs[1];
				setId = tmpStrs[2];
			}
			else {
				setArea = "";
				setId = "";
			}
		}
		else {
			setArea = "";
			setId = "";
		}
	}
	
	public void addStat(String masterName, String slaveName, String interfaceName, String masterIp,
			String slaveIp, int slavePort, int returnValue, long callTimeMill, int callStatus) {
		StatMicMsgHead head = TafUtils.getHead(masterName, slaveName, interfaceName, masterIp,
				slaveIp, slavePort, returnValue, setArea, setId);
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
