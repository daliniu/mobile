package com.qq.taf.proxy;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Map.Entry;

import com.qq.taf.proxy.exec.TafProxyConfigException;

/**
 * 通讯器配置文件
 * @author albertzhu
 *<one>
  <application>
    #proxy需要的配置
    <client>
        #地址
        locator                     = QueryObj@tcp -h 10.1.36.39 -p 17890 -t 5000
        #最大超时时间(毫秒)
        sync-invoke-timeout         = 3000
        async-invoke-timeout         = 3000
        #刷新端口时间间隔(毫秒)
        refresh-endpoint-interval   = 60000
        #模块间调用[可选]
        stat                        = taf.tafstat.StatObj
        #网络发送线程个数
        sendthread                  = 3
        #网络接收线程个数
        recvthread                  = 3
        #网络异步回调线程个数
        asyncthread                 = 3
        #模块名称
        modulename                  = one.onetest
        #是否启用set部署
        enableset
        #set名称，格式：NAME.AREA.GROUPID
		setdivision
    </client>      
  </application>
</one>
 */
public class CommunicatorConfig  {

	public final static String locator_Key = "locator";
	public final static String modulename_Key = "modulename";
	public final static String stat_Key = "stat";
	public final static String sync_invoke_Key = "sync-invoke-timeout";
	public final static String async_invoke_Key = "async-invoke-timeout";
	public final static String refresh_Key = "refresh-endpoint-interval";
	public final static String sendthread_Key = "sendthread";
	public final static String asyncthread_Key = "asyncthread";
	public final static String maxSampleCount_Key = "max-sample-count";
	public final static String sampleRate_Key = "sample-rate";
	public final static String LOCAL_IP_KEY="localip";
	
	public final static int defaultMaxSampleCount = 200 ;
	public final static int  defaultSampleRate = 1000 ;
	public final static String IS_ENABLE_SET="enableset";
	public final static String SET_NAME="setdivision";

	
	int defaultServiceConnNum = 1; 
	HashMap<String,String> propertyMap = new HashMap<String,String>();
	
	public static String localIP="127.0.0.1";
	
	public CommunicatorConfig(String locator , String stat , String modulename, int syncInvokeTimeout, int asyncInvokeTimeout, 
			int refreshEndpointInterval, int sendThreadNum , int asyncthreadNum , boolean isEnableSet, String setName) {
		setLocator(locator);
		setStat(stat);
		setModulename(modulename);
		setSyncInvokeTimeout(syncInvokeTimeout);
		setAsyncInvokeTimeout(asyncInvokeTimeout);
		setRefreshEndpointInterval(refreshEndpointInterval);
		setSendThreadNum(sendThreadNum);
		setAsyncCallbackThreadNum(asyncthreadNum);
		if(isEnableSet) {
			enableSet(setName);
		}
		else {
			disableSet();
		}
	}
	
	public CommunicatorConfig(String locator , String stat , String modulename, int syncInvokeTimeout, int asyncInvokeTimeout, 
			int refreshEndpointInterval, int sendThreadNum , int asyncthreadNum) {
		this(locator , stat , modulename, syncInvokeTimeout, asyncInvokeTimeout, 
				refreshEndpointInterval, sendThreadNum , asyncthreadNum, false , "");
	}
	
	public void enableSet(String setName) {
		if(setName!=null) {
			setProperty(IS_ENABLE_SET, "Y");
			setProperty(SET_NAME, setName);
		}
	}
	
	public void disableSet() {
		setProperty(IS_ENABLE_SET, "N");
		setProperty(SET_NAME, "");
	}
	
	public boolean isEnableSet() {
		return getProperty(IS_ENABLE_SET, "N").equals("Y");
	}
	
	public String getSetName() {
		return getProperty(SET_NAME, null);
	}
	

	public void setProperty(Map<String, String> properties) {
		for ( Entry<String,String> e : properties.entrySet() )  {
			propertyMap.put(e.getKey(), e.getValue());
		}
	}
	
	public void setProperty(String name , String value) {
		propertyMap.put(name, value);
	}
	
	public String getProperty(String name , String defaultValue) {
		if ( propertyMap.containsKey(name) ) {
			return propertyMap.get(name);
		}
		return defaultValue;
	}
	
	public static String getAppFilePath() {
		URL u = CommunicatorConfig.class.getResource(".");
		if ( null == u ) {
			u = CommunicatorConfig.class.getResource("/");
			if ( null == u ) {
				return "";
			}
		}
		String path = u.toString().toLowerCase();
		
		if ( path.startsWith("file:") ) {
			path = path.substring(5);
		} else if ( path.startsWith("jar:") ) {
			path = path.substring(4);
		}
		int point = path.indexOf("/web-inf/") ;
		if ( point > 0 ) {
			path = path.substring(0,point);
		}
		String temp = "/usr/local/";
		point = path.indexOf(temp) ;
		if ( point > 0 ) {
			path = path.substring(point+temp.length());
		} 
		temp = "/webapps/";
		point = path.indexOf(temp) ;
		if ( point > 0 ) {
			path = path.substring(point+temp.length());
		}
		return path;
	}
	
	/**
	 * 不指定配置文件时候的设置
	 * @return
	 */
	public static CommunicatorConfig getDefaultConfig() {
		String path = getAppFilePath();
		if ( path.length() == 0  ) {
			path = "unknownPath";
		}
		TafLoggerCenter.info("CommunicatorConfig Path:"+path);
		CommunicatorConfig config = new CommunicatorConfig(null , null , path, 3000,3000, 60000, 9 , 9, false, null);
		int maxSampleCount = 200 ;
		int sampleRate = 1000 ;
		config.setMaxSampleCount(maxSampleCount);
		config.setSampleRate(sampleRate);
		TafLoggerCenter.info("getDefaultConfig "+ config);
		return config;
	}
	
	/**
	 * 解析CommunicatorConfig配置
	 * 
	 * @param fileName
	 * @return
	 */
	public static CommunicatorConfig parseQueryConfig(String fileName) {
		File file = new File(fileName);
		String locator = null;
		String stat = null;
		String modulename = "Java";
		int asyncInvokeTimeout = 3000;
		int syncInvokeTimeout = 3000;
		int refreshEndpointInterval = 60000;
		int sendThreadNum = 3;
		int asyncthreadNum = 3;
		int maxSampleCount = 200 ;
		int sampleRate = 1000 ;
		boolean isEnableSet = false;
		String setName = null;
		TafLoggerCenter.info("try parse config "+file.getAbsolutePath());
		if (!file.exists()) {
			throw new TafProxyConfigException("file " + fileName + " is not exists ");
		}
		Properties props = new Properties();
		InputStream in = null;
		try {
			in = new BufferedInputStream(new FileInputStream(fileName));
			props.load(in);
			locator = props.getProperty(locator_Key);
			if (null == locator) {
				throw new TafProxyConfigException("config file error , can not find locator");
			}
			stat = props.getProperty(stat_Key);
			String temp = props.getProperty(modulename_Key);
			if (null != temp && !temp.toLowerCase().equals("java")) {
				modulename = temp;//"java."+temp;
			} else {
				modulename = getAppFilePath();
				if ( modulename.length() == 0  ) {
					modulename = "noname";
				} 
//				else {
//					modulename = modulename;//"java."+modulename;
//				}
			}
			temp = props.getProperty(maxSampleCount_Key);
			if (null != temp) {
				try {
					maxSampleCount = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(sampleRate_Key);
			if (null != temp) {
				try {
					sampleRate = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(sync_invoke_Key);
			if (null != temp) {
				try {
					syncInvokeTimeout = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(async_invoke_Key);
			if (null != temp) {
				try {
					asyncInvokeTimeout = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(refresh_Key);
			if (null != temp) {
				try {
					refreshEndpointInterval = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(sendthread_Key);
			if (null != temp) {
				try {
					sendThreadNum = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp = props.getProperty(asyncthread_Key);
			if (null != temp) {
				try {
					asyncthreadNum = Integer.parseInt(temp);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
			temp=props.getProperty(LOCAL_IP_KEY);
			if(temp!=null && temp.trim().length()!=0) localIP=temp;
			
			temp=props.getProperty(IS_ENABLE_SET);
			if(temp!=null && temp.trim().toUpperCase().equals("Y")) {
				isEnableSet = true;
			}
			
			temp = props.getProperty(SET_NAME);
			if(temp!= null) {
				setName = temp.trim();
			}
		} catch (Exception e) {
			e.printStackTrace();
			throw new TafProxyConfigException("read file " + fileName + " error " + e);
		} finally {
			if (null != in) {
				try {
					in.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		try {
			CommunicatorConfig config = new CommunicatorConfig(locator , stat , modulename, syncInvokeTimeout,asyncInvokeTimeout, 
					refreshEndpointInterval, sendThreadNum , asyncthreadNum, isEnableSet, setName);
			config.setMaxSampleCount(maxSampleCount);
			config.setSampleRate(sampleRate);
			TafLoggerCenter.info("get config from file " + fileName + " " + config);
			return config;
		} catch (Exception e) {
			throw new TafProxyConfigException("prase config item " + locator + " error " + e);
		}
	}
	
	/**
	 * 解析出一个proxConfig 设定超时时间 刷新端口时间
	 * @param content
	 * @return
	 */
	public ProxyConfig getConfig(String content,TafThreadPool tafThreadPool, int configHashCode) {
		String objectName = "";
		int point = content.indexOf("@");
		if (point > 0) {
			objectName = content.substring(0, point);
		} else {
			objectName = content;
		}
		ProxyConfig config = new ProxyConfig(objectName,tafThreadPool, configHashCode);
		config.setSyncTimeoutMill(getSyncInvokeTimeout());
		config.setAsyncTimeoutMill(getAsyncInvokeTimeout());
		config.setRefreshEndpointInterval(getRefreshEndpointInterval());
		config.setEnableSet(isEnableSet());
		config.setSetName(getSetName());
		if (point > 0) {
			String temp = content.substring(point + 1);
			config.setQueryEndpoints(temp,defaultServiceConnNum);
		}
		config.locatorName = objectName;
		if ( null != getStat() ) {
			config.statName = getStat();
		}
		TafLoggerCenter.info("create config from  " + config);
		return config;
	}

	public int getMaxSampleCount() {
		return Integer.parseInt(getProperty(maxSampleCount_Key ,String.valueOf(defaultMaxSampleCount)));
	}
	
	public void setMaxSampleCount(int maxSampleCount) {
		setProperty(maxSampleCount_Key,String.valueOf(maxSampleCount));
	}
	
	public int getSampleRate() {
		return Integer.parseInt(getProperty(sampleRate_Key ,String.valueOf(defaultSampleRate)));
	}
	
	public void setSampleRate(int sampleRate) {
		setProperty(sampleRate_Key,String.valueOf(sampleRate));
	}
	
	/**
	 * locator地址
	 * @return
	 */
	public String getLocator() {
		return getProperty(locator_Key ,null);
	}
	/**
	 * locator地址
	 * @param locator
	 */
	public void setLocator(String locator) {
		if ( null != locator && locator.length() > 0) {
			setProperty(locator_Key,locator);
		}
	}
	/**
	 * 模块间调用
	 * @return
	 */
	public String getStat() {
		return getProperty(stat_Key ,null);
	}
	/**
	 * 模块间调用
	 * @param stat
	 */
	public void setStat(String stat) {
		if ( null != stat ) {
			stat = stat.trim();
			if (stat.length() > 0) {
				setProperty(stat_Key,stat);
			}
		}
	}
	/**
	 * 本地模块名
	 * @return
	 */
	public String getModulename() {
		return getProperty(modulename_Key ,null);
	}
	/**
	 * 本地模块名
	 * @param modulename
	 */
	public void setModulename(String modulename) {
		if ( null != modulename) {
			modulename = modulename.trim();
			if (modulename.length() > 0) {
				setProperty(modulename_Key,modulename);
			}
		}
	}
	/**
	 * 同步调用超时时间
	 * @return
	 */
	public int getSyncInvokeTimeout() {
		return Integer.parseInt(getProperty(sync_invoke_Key ,null));
	}
	/**
	 * 同步调用超时时间
	 * @param AsyncInvokeTimeout
	 */
	public void setSyncInvokeTimeout(int syncInvokeTimeout) {
		if ( syncInvokeTimeout < 100 ) {
			syncInvokeTimeout = 100;
		}
		setProperty(sync_invoke_Key,String.valueOf(syncInvokeTimeout));
	}
	
	/**
	 * 异步调用超时时间
	 * @return
	 */
	public int getAsyncInvokeTimeout() {
		return Integer.parseInt(getProperty(async_invoke_Key ,null));
	}
	/**
	 * 异步调用超时时间
	 * @param AsyncInvokeTimeout
	 */
	public void setAsyncInvokeTimeout(int AsyncInvokeTimeout) {
		if ( AsyncInvokeTimeout < 100 ) {
			AsyncInvokeTimeout = 100;
		}
		setProperty(async_invoke_Key,String.valueOf(AsyncInvokeTimeout));
	}
	/**
	 * 刷新端口时间
	 * @return
	 */
	public int getRefreshEndpointInterval() {
		return Integer.parseInt(getProperty(refresh_Key ,null));
	}
	/**
	 * 刷新端口时间
	 * @param refreshEndpointInterval
	 */
	public void setRefreshEndpointInterval(int refreshEndpointInterval) {
		if ( refreshEndpointInterval < 1000 ) {
			refreshEndpointInterval = 1000;
		}
		setProperty(refresh_Key,String.valueOf(refreshEndpointInterval));
	}
	/**
	 * 异步发送线程个数
	 * @return
	 */
	public int getSendThreadNum() {
		return Integer.parseInt(getProperty(sendthread_Key ,null));
	}
	/**
	 * 异步发送线程个数
	 * @param sendThreadNum
	 */
	public void setSendThreadNum(int sendThreadNum) {
		if ( sendThreadNum < 1  ) {
			sendThreadNum = 1 ;
		}
		setProperty(sendthread_Key,String.valueOf(sendThreadNum));
	}
	/**
	 * 异步回调线程个数
	 * @return
	 */
	public int getAsyncCallBackThreadNum() {
		return Integer.parseInt(getProperty(asyncthread_Key ,null));
	}
	/**
	 * 异步回调线程个数
	 * @param asyncthreadNum
	 */
	public void setAsyncCallbackThreadNum(int asyncthreadNum) {
		if ( asyncthreadNum < 1  ) {
			asyncthreadNum = 1 ;
		}
		setProperty(asyncthread_Key,String.valueOf(asyncthreadNum));
	}
	
	public static void main(String[] args) {
		String a = "jar:file:/usr/local/resin_mpet/webapps/webapp_mpet/WEB-INF/lib/taf-proxy-1.0.0.jar!/com/qq/taf/proxy/";
		String path = a.toLowerCase();
		int point = path.indexOf("/web-inf/") ;
		if ( point > 0 ) {
			path = path.substring(0,point);
		}
		point = path.indexOf("/usr/local/") ;
		if ( point > 0 ) {
			path = path.substring(point+11);
		}
		System.out.println(path);
//		String content = "taf.tafregistry.QueryObj@tcp -h 172.27.205.110 -p 17890";
//		String objectName = "";
//		int point = content.indexOf("@");
//		if (point > 0) {
//			objectName = content.substring(0, point);
//		} else {
//			objectName = content;
//		}
//		ProxyConfig config = new ProxyConfig(objectName,null);
//		if (point > 0) {
//			String temp = content.substring(point + 1);
//			config.setQueryEndpoints(temp,1);
//		}
//		LoggerCenter.info("create config from  " + config);
	}

}
