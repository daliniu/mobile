package com.qq.taf.server;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.TafTimer;
import com.qq.taf.server.PropertyReporterFactory;

/**
 * 通讯器
 * @author fanzhang
 */
public class ClientCommunicator extends Communicator {

	public ClientCommunicator() {
		super(ClientConfig.INSTANCE);
		String property=getProperty(ClientConfig.KEY_PROPERTY,null);
		if(property!=null) {
			int reportInterval=Integer.parseInt(getProperty(ClientConfig.KEY_REPORT_INTERVAL, "60000"));
			PropertyReporterFactory.getInstance().setPropertyInfo(this, property, getModulename(), reportInterval);
		}
		ServerStatTool.Instance.init(super.getStatPrx(), ClientConfig.INSTANCE);
		TafTimer.initServerStat(ServerStatTool.Instance);
	}
	
}
