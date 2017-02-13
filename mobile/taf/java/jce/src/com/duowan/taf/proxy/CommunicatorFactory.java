package com.duowan.taf.proxy;

import java.util.concurrent.ConcurrentHashMap;

public class CommunicatorFactory {

	static ConcurrentHashMap<String, Communicator> CommunicatorMap = new ConcurrentHashMap<String, Communicator>();
	
	static ConcurrentHashMap<String, String> locatorMap = new ConcurrentHashMap<String, String>();
	
	static Object mapLock = new Object();
	
	/**
	 * 指定配置文件创建Communicator
	 * 同一个locator地址的Communicator只会创建一次
	 * @param configFileName
	 * @return
	 */
	public static Communicator getComm(String configFileName) {
		if ( locatorMap.containsKey(configFileName) ) {
			String locator = locatorMap.get(configFileName);
			if ( CommunicatorMap.containsKey(locator) ) {
				return CommunicatorMap.get(locator);
			} 
		} 
		CommunicatorConfig communicatorConfig = CommunicatorConfig.parseQueryConfig(configFileName);
		String locator = communicatorConfig.getLocator();
		synchronized (mapLock) {
			if ( !CommunicatorMap.containsKey(locator) ) {
				Communicator c = new Communicator(communicatorConfig);
				CommunicatorMap.putIfAbsent(locator, c);
				locatorMap.putIfAbsent(configFileName, locator);
			}
		}
		return CommunicatorMap.get(locator);	
	}
	
	/**
	 * 指定locator地址创建Communicator
	 * 同一个locator地址的Communicator只会创建一次
	 * @param locator
	 * @return
	 */
	public static Communicator getCommFLocator(String locator) {
		if ( locatorMap.containsKey(locator) ) {
			if ( CommunicatorMap.containsKey(locator) ) {
				return CommunicatorMap.get(locator);
			} 
		} 
		synchronized (mapLock) {
			if ( !CommunicatorMap.containsKey(locator) ) {
				Communicator c = new Communicator();
				c.setLocator(locator);
				CommunicatorMap.putIfAbsent(locator, c);
				locatorMap.putIfAbsent(locator, locator);
			}
		}
		return CommunicatorMap.get(locator);	
	}
	
	public static String getInfo() {
		StringBuffer sb = new StringBuffer();
		int i = 0 ;
		for ( String key : locatorMap.keySet() ) {
			sb.append(i+":"+locatorMap.get(key)+" init from "+key+" | ");
			i++;
		}
		return sb.toString();
	}
	
	public static void print() {
		System.out.println(getInfo());
	}
	
}
