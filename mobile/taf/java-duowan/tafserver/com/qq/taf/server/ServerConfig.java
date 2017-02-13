package com.qq.taf.server;

import com.qq.taf.server.util.Config;

/**
 * 服务基本信息
 * @author fanzhang
 */
public class ServerConfig {

	public static final String CONFIG_FILE=System.getProperty("config")!=null?System.getProperty("config"):ServerConfig.class.getResource("/config.conf").getFile();
	public static String application;
	public static String serverName; 
	public static Endpoint local;
	public static String node;
	public static String basePath;
	public static String config;
	public static String notify;
	public static String log;
	public static String logPath;
	public static String localIP;
	public static int configCenterPort;
	
	public static void init(Config conf) {
		application=conf.get("/taf/application/server<app>");
		serverName=conf.get("/taf/application/server<server>");
		String plocal=conf.get("/taf/application/server<local>","");
		local=plocal.length()==0?null:new Endpoint(plocal);
		node=conf.get("/taf/application/server<node>",null);
		basePath=conf.get("/taf/application/server<basepath>",null);
		config=conf.get("/taf/application/server<config>",null);
		notify=conf.get("/taf/application/server<notify>",null);
		log=conf.get("/taf/application/server<log>",null);
		logPath=conf.get("/taf/application/server<logpath>",null);
		localIP=conf.get("/taf/application/server<localip>",null);
		configCenterPort=conf.getInt("/taf/application/server<config-center-port>",0);
	}
	
}
