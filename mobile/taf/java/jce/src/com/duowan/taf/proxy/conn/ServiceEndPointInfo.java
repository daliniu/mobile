package com.duowan.taf.proxy.conn;

import com.duowan.jutil.string.StringUtil;
import com.duowan.taf.EndpointF;
import com.duowan.taf.proxy.TimerProvider;
import com.duowan.taf.proxy.exec.TafProxyConfigException;

/**
 * 远程service端口信息
 * @author albertzhu
 *
 */
public class ServiceEndPointInfo implements Comparable<ServiceEndPointInfo>{
	
	String protocol;
	String remoteHost;
	int remotePort;
	private int grid ;
	protected boolean active = true ;
	
	public ServiceEndPointInfo clone() {
		ServiceEndPointInfo newInfo = new ServiceEndPointInfo(objectName,protocol,remoteHost,remotePort,syncInvokeTimeout,asyncInvokeTimeout,grid);
		return newInfo;
	}
	
	public boolean isActive() {
		return active;
	}

	public void setActive(boolean active) {
		this.active = active;
	}

	private int allowConnNum = 1;
	private long failedTime; //连接失败发生的时间
	private long timeoutTime;//超时发生的时间
	String objectName ;
	int failedWaitTimeMill = 60000 ;
	
	int syncInvokeTimeout ;
	int asyncInvokeTimeout;

	EndPointEventListen listener ;

	public static ServiceEndPointInfo parseNode(String content , int syncInvokeTimeout , int asyncInvokeTimeout) {
		return parseNode("not set ObjectName",content,syncInvokeTimeout,asyncInvokeTimeout,1);
	}
	
	public static ServiceEndPointInfo parseNode(String objectName , String content , int syncInvokeTimeout , int asyncInvokeTimeout) {
		return parseNode(objectName,content,syncInvokeTimeout,asyncInvokeTimeout,1);
	}
	
	public static ServiceEndPointInfo parseNode(String objectName , String content , int syncInvokeTimeout ,int asyncInvokeTimeout, int serviceConnNum) {
		// tcp -h 127.0.0.1 -p 20000 -t 5000
		content = content.toLowerCase();
		int point = content.indexOf("  ");
		while ( point > -1 ) {
			content = StringUtil.replaceAll(content, "  ", " ");
			point = content.indexOf("  ");
		}
		String[] items = StringUtil.split(content, " ");
		if (items.length < 5) {
			throw new TafProxyConfigException("node config error " + content);
		}
		String host = "";
		int port = 0 ;
		int active = 1 ;
		for ( int i = 0 ; i < items.length ; i++  ) {
			if ( items[i].equals("-h") ) {
				host = items[i+1];
			} else if ( items[i].equals("-p") ) {
				port = Integer.parseInt(items[i+1]);
			} else if ( items[i].equals("-a") ) {
				active = Integer.parseInt(items[i+1]);
			}
		}
		ServiceEndPointInfo config = new ServiceEndPointInfo(objectName,items[0], host, port,syncInvokeTimeout,asyncInvokeTimeout );
		config.allowConnNum = serviceConnNum;
		config.active = ( active != 0 );
		return config;
	}

	public ServiceEndPointInfo( String objectName,EndpointF endpointF) {
		this.objectName = objectName;
		if ( endpointF.istcp == 0) {
			this.protocol = "udp";
		} else if (endpointF.istcp == 1) {
			this.protocol = "tcp";
		}
		this.remoteHost = endpointF.host;
		this.remotePort = endpointF.port;
		this.syncInvokeTimeout = endpointF.timeout;
		this.asyncInvokeTimeout = endpointF.timeout;
		this.setGrid(endpointF.grid);
	}
	
	public ServiceEndPointInfo(String objectName, String protocol, String remoteHost, int remotePort , int syncInvokeTimeout , int asyncInvokeTimeout) {
		this(objectName,protocol,remoteHost,remotePort,syncInvokeTimeout,asyncInvokeTimeout,0);
	}
	
	public ServiceEndPointInfo(String objectName,String protocol, String remoteHost, int remotePort , int syncInvokeTimeout , int asyncInvokeTimeout ,int grid ) {
		this.objectName = objectName;
		this.protocol = protocol;
		this.remoteHost = remoteHost;
		this.remotePort = remotePort;
		this.syncInvokeTimeout = syncInvokeTimeout;
		this.asyncInvokeTimeout = asyncInvokeTimeout;
		this.setGrid(grid);
	}

	public String getKey() {
		return getSessionKey(protocol, remoteHost, remotePort);
	}

	public static String getSessionKey(String protocol, String remoteHost, int remotePort) {
		return protocol + ">" + remoteHost + ":" + remotePort;
	}

	public String getProtocol() {
		return protocol;
	}

	public String getRemoteHost() {
		return remoteHost;
	}

	public int getRemotePort() {
		return remotePort;
	}

	public int getAllowConnNum() {
		return allowConnNum;
	}

	//允许proxy设置网络连接个数
	protected void setAllowConnNum(int allowConnNum) {
		this.allowConnNum = allowConnNum;
	}
	
	public boolean isFailed() {
		return failedTime > 0 ;
	}
	
	public void setFailed( boolean b ) {
		if ( b ) {
			failedTime = TimerProvider.getNow();
		} else {
			failedTime = 0 ;
		}
	}
	
	long getFailedTime() {
		return failedTime;
	}
	
	long getTimeoutTime() {
		return timeoutTime;
	}
	
	public void setTimeout( boolean b ) {
		if ( b ) {
			timeoutTime = TimerProvider.getNow();
		} else {
			timeoutTime = 0 ;
		}
	}
	
	public boolean isTimeoutTime() {
		return timeoutTime > 0 ;
	}
	
	boolean shouldTryConn() {
		if ( isFailed() ) {
			if ( TimerProvider.getNow() - failedTime > (failedWaitTimeMill)  ) {
				return true ;
			} else {
				return false ;
			}
		} else {
			return false ;
		}
	}

	@Override
	public int hashCode() {
		return toString().hashCode();
	}

	@Override
	public String toString() {
		return getKey();
	}

	public String getStoreString() {
		int a = 0 ;
		if ( active ) {
			a = 1 ;
		}
		return protocol+" -h "+remoteHost+" -p "+remotePort+" -t 3000 -a "+a+" -g "+getGrid() ;
	}
	
	public String toFullString() {
		return getKey()+" syncTimeout:"+syncInvokeTimeout+" active:"+active+" asyncTimeout:"+asyncInvokeTimeout+" grid:"+getGrid();
	}

	@Override
	public boolean equals(Object obj) {
		return obj instanceof ServiceEndPointInfo && ((ServiceEndPointInfo) obj).toString().equals(toString());
	}

	public String getObjectName() {
		return objectName;
	}

	public int getFailedWaitTimeSecond() {
		return failedWaitTimeMill;
	}

	public void setFailedWaitTimeSecond(int failedWaitTimeSecond) {
		this.failedWaitTimeMill = failedWaitTimeSecond;
	}
	


	public void setGrid(int grid) {
		this.grid = grid;
	}

	public int getGrid() {
		return grid;
	}

	public int getSyncInvokeTimeout() {
		return syncInvokeTimeout;
	}

	public void setSyncInvokeTimeout(int syncInvokeTimeout) {
		this.syncInvokeTimeout = syncInvokeTimeout;
	}

	public int getAsyncInvokeTimeout() {
		return asyncInvokeTimeout;
	}

	public void setAsyncInvokeTimeout(int asyncInvokeTimeout) {
		this.asyncInvokeTimeout = asyncInvokeTimeout;
	}

	public long getMinTimeoutMill() {
		return syncInvokeTimeout < asyncInvokeTimeout ? syncInvokeTimeout : asyncInvokeTimeout ;
	}

	public int compareTo(ServiceEndPointInfo o) {
		return getKey().hashCode() - o.getKey().hashCode();
	}
}
