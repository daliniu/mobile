package com.duowan.taf.proxy.conn;

import java.io.InputStream;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

import com.duowan.jutil.common.CongfigResource;
import com.duowan.taf.proxy.TafLoggerCenter;
import com.duowan.taf.proxy.codec.JceMessage;

/**
 * 将符合过滤条件的连接发送二进制信息打印出来
 * @author albertzhu
 *
 */
public class LoggerFilter {
	
	public static ConcurrentHashMap<String, String> careServer = new ConcurrentHashMap<String, String>();
	
	static AtomicLong checkTime = new AtomicLong(0);
	
	static void init() {
//		try {
//			Properties prop = new Properties();
//	        InputStream in = CongfigResource.loadConfigFile("tafCareSend.conf", LoggerFilter.class);
//	        prop.load(in);
//	        Set<Map.Entry<Object, Object>> entrySet = prop.entrySet();
//	        Iterator<Map.Entry<Object, Object>> it = entrySet.iterator();
//	        careServer.clear();
//	        while(it.hasNext())
//	        {   
//	            try{
//	            	Map.Entry<Object, Object> entry = it.next();
//	                String careKey = ((String)entry.getKey()).trim();
//	                String careValue = ((String)entry.getValue()).trim();
//	                careServer.put(careValue, careKey);
//	                TafLoggerCenter.info("put care "+careKey+" "+careValue);
//	            } catch (Exception ex) {
//	            	ex.printStackTrace();
//	            }
//	        }
//		} catch (Exception ex) {
//			TafLoggerCenter.info("can not find tafCareSend.conf");
//		}
        
	}
	
	public static void addCareServer( String protocol, String remoteHost, int remotePort ) {
		String key = ServiceEndPointInfo.getSessionKey(protocol, remoteHost, remotePort);
		careServer.put(key, key);
	}
	
	public static void removeCareServer( String protocol, String remoteHost, int remotePort ) {
		String key = ServiceEndPointInfo.getSessionKey(protocol, remoteHost, remotePort);
		careServer.remove(key, key);
	}
	
	public static void logCareServerMsg( JceMessage msg, String objectName , String configKey , boolean isRequest ) {
//		if ( checkTime.get() == 0 || System.currentTimeMillis() - checkTime.get() > 60000 ) {
//			checkTime.set(System.currentTimeMillis());
//			init();
//		}
//		if ( careServer.containsKey(configKey) ) {
//			if ( isRequest ) {
//				TafLoggerCenter.info(objectName+" "+configKey+" sended "+msg);
//			} else {
//				TafLoggerCenter.info(objectName+" "+configKey+" received "+msg);
//			}
//		}
	}
	
}
