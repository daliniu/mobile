package com.duowan.taf.proxy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import com.duowan.jutil.string.StringUtil;
import com.duowan.taf.EndpointF;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;

/**
 * proxy的配置文件 内部使用 调用者不用关心
 * 
 * @author albertzhu
 * 
 */
public class ProxyConfig {

	/**
	 * 是否是从配置中心获取的
	 */
	protected boolean fromRegisterCenter = false;

	String objectName;

	int refreshEndpointInterval;
	int asyncTimeoutMill;
	int syncTimeoutMill;
	String queryEndpoints;
	/**
	 * 在异步调用的时候防止冲击服务器端 有多少个未回应信息前不继续发送 设置为-1则不生效
	 */
	int availableKey = -1;
	TafThreadPool tafThreadPool;

	int serviceConnNum = 1;

	public ServiceInfos serviceInfos = null;

	public String locatorName = "";

	public String statName = "";

	int readBufferSize;
	int writeBufferSize;

	long idleStatusInteval =  60000*2; //2分钟没有操作连接就关闭
	
	public long getIdleStatusInteval() {
		return idleStatusInteval;
	}

	public void setIdleStatusInteval(long idleStatusInteval) {
		this.idleStatusInteval = idleStatusInteval;
	}
	
	public ProxyConfig(String objectName, TafThreadPool tafThreadPool) {
		this.objectName = objectName;
		this.tafThreadPool = tafThreadPool;
		serviceInfos = new ServiceInfos(objectName);
	}

	public int getAsyncTimeoutMill() {
		return asyncTimeoutMill;
	}

	public void setAsyncTimeoutMill(int timeoutMill) {
		this.asyncTimeoutMill = timeoutMill;
	}

	public int getSyncTimeoutMill() {
		return syncTimeoutMill;
	}

	public void setSyncTimeoutMill(int syncTimeoutMill) {
		this.syncTimeoutMill = syncTimeoutMill;
	}

	public void setQueryEndpoints(String queryEndpoints, int serviceConnNum) {
		serviceInfos.setServices(queryEndpoints, syncTimeoutMill, asyncTimeoutMill, serviceConnNum);
		this.queryEndpoints = queryEndpoints;
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		sb.append(getObjectName() + "@");
		List<ServiceEndPointInfo> allServiceInfos = serviceInfos.getAllServices();
		for (ServiceEndPointInfo node : allServiceInfos) {
			sb.append(node.toFullString() + ":");
		}
		String s = sb.toString();
		if (s.endsWith(":")) {
			s = s.substring(0, s.length() - 1);
		}
		return s;
	}

	/**
	 * 得到缓存服务器信息需要的字符串
	 * @return
	 */
	public String getEndPoingListString() {
		StringBuffer sb = new StringBuffer();
		List<ServiceEndPointInfo> allServiceInfos = serviceInfos.getAllServices();
		for (ServiceEndPointInfo node : allServiceInfos) {
			sb.append(node.getStoreString() + ":");
		}
		String s = sb.toString();
		if (s.endsWith(":")) {
			s = s.substring(0, s.length() - 1);
		}
		return s;
	}

	public static Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> parseEndPoingConfig(String objectName,
			int syncInvokeTimeout, int asyncInvokeTimeout, int serviceConnNum, String src) {
		ArrayList<ServiceEndPointInfo> activeEpList = new ArrayList<ServiceEndPointInfo>();
		ArrayList<ServiceEndPointInfo> inactiveEpList = new ArrayList<ServiceEndPointInfo>();
		String[] items = StringUtil.split(src, ":");
		for (String item : items) {
			ServiceEndPointInfo node = ServiceEndPointInfo.parseNode(objectName, item, syncInvokeTimeout,
					asyncInvokeTimeout, serviceConnNum);
			if (node.isActive()) {
				activeEpList.add(node);
			} else {
				inactiveEpList.add(node);
			}
		}
		Collections.sort(activeEpList);
		Collections.sort(inactiveEpList);
		Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = new Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>>();
		p.k = activeEpList;
		p.v = inactiveEpList;
		return p;
	}

	public String hashString() {
		StringBuffer sb = new StringBuffer();
		sb.append(getObjectName() + "@");
		List<ServiceEndPointInfo> allServiceInfos = serviceInfos.getAllServices();
		for (ServiceEndPointInfo node : allServiceInfos) {
			sb.append(node.toString() + ":");
		}
		String s = sb.toString();
		if (s.endsWith(":")) {
			s = s.substring(0, s.length() - 1);
		}
		return s;
	}

	@Override
	public boolean equals(Object obj) {
		return obj instanceof ProxyConfig && (((ProxyConfig) obj).hashString().equals(hashString()));
	}

	@Override
	public int hashCode() {
		return hashString().hashCode();
	}

	public String getObjectName() {
		return objectName;
	}

	public int getRefreshEndpointInterval() {
		return refreshEndpointInterval;
	}

	public void setRefreshEndpointInterval(int refreshEndpointInterval) {
		this.refreshEndpointInterval = refreshEndpointInterval;
	}

	public int getAvailableKey() {
		return availableKey;
	}

	public void setAvailableKey(int availableKey) {
		this.availableKey = availableKey;
	}

	public TafThreadPool getTafThreadPool() {
		return tafThreadPool;
	}

	public int getReadBufferSize() {
		return readBufferSize;
	}

	public void setReadBufferSize(int readBufferSize) {
		this.readBufferSize = readBufferSize;
	}

	public int getWriteBufferSize() {
		return writeBufferSize;
	}

	public void setWriteBufferSize(int writeBufferSize) {
		this.writeBufferSize = writeBufferSize;
	}

}
