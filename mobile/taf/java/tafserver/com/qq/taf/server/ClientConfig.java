package com.qq.taf.server;

import com.qq.taf.proxy.CommunicatorConfig;
import com.qq.taf.server.util.Config;

/**
 * 与客户端通讯器相关的配置
 * @author fanzhang
 */
public class ClientConfig extends CommunicatorConfig {

	public static final String KEY_PROPERTY="property";
	public static final String KEY_REPORT_INTERVAL="report-interval";
	public static ClientConfig INSTANCE;
	
	public ClientConfig(
			String locator , 
			String stat , 
			String moduleName, 
			int syncInvokeTimeout, 
			int asyncInvokeTimeout, 
			int refreshEndpointInterval, 
			int sendThreadNum , 
			int asyncThreadNum 
	) {
		super(locator,stat,moduleName,syncInvokeTimeout,asyncInvokeTimeout,refreshEndpointInterval,sendThreadNum,asyncThreadNum);
	}

	public static void init(Config conf) {
		String locator=conf.get("/taf/application/client<locator>",null);
		int syncInvokeTimeout=conf.getInt("/taf/application/client<sync-invoke-timeout>",3000);
		int asyncInvokeTimeout=conf.getInt("/taf/application/client<async-invoke-timeout>", 3000);
		int refreshEndpointInterval=conf.getInt("/taf/application/client<refresh-endpoint-interval>",60000);
		String stat=conf.get("/taf/application/client<stat>",null);
		int sendThreadNum=conf.getInt("/taf/application/client<sendthread>",3);
		int asyncThreadNum=conf.getInt("/taf/application/client<asyncthread>",3);
		String moduleName=conf.get("/taf/application/client<modulename>","");
		String property=conf.get("/taf/application/client<property>",null);
		String reportInterval=conf.get("/taf/application/client<report-interval>","60000");
		int maxSampleCount=conf.getInt("/taf/application/client<max-sample-count>",100);
		int sampleRate=conf.getInt("/taf/application/client<sample-rate>",1000);
		INSTANCE=new ClientConfig(locator,stat,moduleName,syncInvokeTimeout,asyncInvokeTimeout,refreshEndpointInterval,sendThreadNum,asyncThreadNum);
		INSTANCE.setProperty(KEY_PROPERTY, property);
		INSTANCE.setProperty(KEY_REPORT_INTERVAL, reportInterval);
		INSTANCE.setMaxSampleCount(maxSampleCount);
		INSTANCE.setSampleRate(sampleRate);
	}
	
}
