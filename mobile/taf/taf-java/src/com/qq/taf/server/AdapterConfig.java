package com.qq.taf.server;

import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.qq.taf.server.util.Config;
import com.qq.taf.server.util.GeneralException;
import com.qq.taf.server.util.ip.IPList;

/**
 * 服务适配器配置
 * @author fanzhang
 */
public class AdapterConfig {
	
	private static Map<String, AdapterConfig> adapConfMap;
	
	private Endpoint endpoint;
	private int maxConns;
	private int threads;
	private int queueCapacity;
	private int queueTimeout;
	private String servant;
	private IPList allow;
	private IPList deny;
	private String order;
	
	public int getMaxConns() {
		return maxConns;
	}
	public void setMaxConns(int maxConns) {
		this.maxConns = maxConns;
	}
	public int getQueueCapacity() {
		return queueCapacity;
	}
	public void setQueueCapacity(int queueCapacity) {
		this.queueCapacity = queueCapacity;
	}
	public int getQueueTimeout() {
		return queueTimeout;
	}
	public void setQueueTimeout(int queueTimeout) {
		this.queueTimeout = queueTimeout;
	}
	public String getServant() {
		return servant;
	}
	public void setServant(String servant) {
		this.servant = servant;
	}
	public int getThreads() {
		return threads;
	}
	public void setThreads(int theads) {
		this.threads = theads;
	}
	public Endpoint getEndpoint() {
		return endpoint;
	}
	public void setEndpoint(Endpoint endpoint) {
		this.endpoint = endpoint;
	}
	public IPList getAllow() {
		return allow;
	}
	public void setAllow(IPList allow) {
		this.allow = allow;
	}
	public IPList getDeny() {
		return deny;
	}
	public void setDeny(IPList deny) {
		this.deny = deny;
	}
	public String getOrder() {
		return order;
	}
	public void setOrder(String order) {
		this.order = order;
	}
	
	public static void init(Config conf) {
		adapConfMap=new HashMap<String, AdapterConfig>();
		
		List<String> adapterNames=conf.getSubDomainNames("/taf/application/server");
		for(String adapterName:adapterNames) {
			String path="/taf/application/server/"+adapterName;
			AdapterConfig ac=new AdapterConfig();
			ac.setEndpoint(new Endpoint(conf.get(path+"<endpoint>")));
			ac.setMaxConns(conf.getInt(path+"<maxconns>",128));
			ac.setQueueCapacity(conf.getInt(path+"<queuecap>",1024));
			ac.setQueueTimeout(conf.getInt(path+"<queuetimeout>",10000));
			String servant=conf.get(path+"<servant>");
			ac.setServant(servant);
			ac.setThreads(conf.getInt(path+"<threads>"));
			ac.setOrder(conf.get(path+"<order>","allow,deny"));
			ac.setAllow(new IPList(conf.get(path+"<allow>","")));
			ac.setDeny(new IPList(conf.get(path+"<deny>","")));
			adapConfMap.put(servant, ac);
		}
		
		if(ServerConfig.local!=null) {
			String adminServant=ServerConfig.application+"."+ServerConfig.serverName+".AdminObj";
			AdapterConfig adminAdapterConfig=new AdapterConfig();
			adminAdapterConfig.setEndpoint(ServerConfig.local);
			adminAdapterConfig.setMaxConns(128);
			adminAdapterConfig.setQueueCapacity(1024);
			adminAdapterConfig.setQueueTimeout(10000);
			adminAdapterConfig.setServant(adminServant);
			adminAdapterConfig.setThreads(1);
			adminAdapterConfig.setOrder("allow,deny");
			adminAdapterConfig.setAllow(new IPList(""));
			adminAdapterConfig.setDeny(new IPList(""));
			adapConfMap.put(adminServant,adminAdapterConfig);
		}
	}
	
	public static AdapterConfig getByServant(String servant) {
		if(!adapConfMap.containsKey(servant)) GeneralException.raise("adapter config for ["+servant+"] is absent");
		return adapConfMap.get(servant);
	}
	
	public static Set<String> keys() {
		return adapConfMap.keySet();
	}
	
	public static Collection<AdapterConfig> values() {
		return adapConfMap.values();
	}
	
}
