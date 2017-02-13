package com.duowan.taf.proxy.conn;

import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import com.duowan.taf.ResponsePacket;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.StatSampleMsg;
import com.duowan.taf.cnst.JCEASYNCCALLTIMEOUT;
import com.duowan.taf.cnst.JCEPROXYCONNECTERR;
import com.duowan.taf.cnst.JCEVERSION;
import com.duowan.taf.proxy.ProxyStatTool;
import com.duowan.taf.proxy.TafLoggerCenter;
import com.duowan.taf.proxy.ProxyConfig;
import com.duowan.taf.proxy.SendMessage;
import com.duowan.taf.proxy.SendMsgStatBody;
import com.duowan.taf.proxy.ServantFuture;
import com.duowan.taf.proxy.ServantProxy;
import com.duowan.taf.proxy.ServiceInfos;
import com.duowan.taf.proxy.TAdapterSelector;
import com.duowan.taf.proxy.TimerProvider;
import com.duowan.taf.proxy.codec.JceMessage;
import com.duowan.taf.proxy.exec.TafCallTimeoutException;
import com.duowan.taf.proxy.exec.TafException;
import com.duowan.taf.proxy.exec.TafNotSendException;
import com.duowan.taf.proxy.exec.TafProxyNoConnException;
import com.duowan.taf.proxy.utils.TafUtils;
import com.duowan.taf.proxy.utils.TimeoutHandler;

/**
 * 维持了proxy的所有连接 负责刷新 新建连接
 * 
 * @author albertzhu
 * 
 */
public class ProxyConnRouter implements ServiceEndPointListener {

	// 服务器列表选择指针
	AtomicInteger point = new AtomicInteger();
	// proxy名
	String objectName;
	// proxy模块名称
	String processName;
	// 网络连接接口 根据请求打开连接并转发IoSession事件
	EndPointAdapter adapter;
	// 信息序列号
	static AtomicInteger seq = new AtomicInteger();
	ServantProxy proxy;
	// 服务器选择器
	TAdapterSelector selector = new TAdapterSelector();

	// 隶属的proxy配置文件
	ProxyConfig proxyConfig;
	// 模块间调用信息
	//private ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> statmsg = new ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody>();
	// 模块间调用信息统计区间
	//List<Integer> timeStatInterv;
	ServiceInfos serviceInfos;

	ConcurrentHashMap<String, ServiceEndPoint> existsServiceMap = new ConcurrentHashMap<String, ServiceEndPoint>();
	ConcurrentHashMap<String, ServiceEndPoint> aliveServiceMap = new ConcurrentHashMap<String, ServiceEndPoint>();

	Object initLock = new Object();
	AtomicBoolean inited = new AtomicBoolean();

	FutureTimeoutHandler timeoutHandler;

	Object serviceInfoLock = new Object();
	
	/**
	 * 
	 * @param processName
	 *            proxy模块名称
	 * @param proxyConfig
	 *            隶属的proxy配置文件
	 * @param adapter
	 *            网络连接接口 根据请求打开连接并转发IoSession事件
	 */
	public ProxyConnRouter(String processName, ProxyConfig proxyConfig, EndPointAdapter adapter) {
		this.processName = processName;
		this.objectName = proxyConfig.getObjectName();
		this.adapter = adapter;
		this.serviceInfos = proxyConfig.serviceInfos;
		this.proxyConfig = proxyConfig;
		timeoutHandler = new FutureTimeoutHandler();
	}

	/**
	 * 初始化配置 将配置中的服务器地址信息加载 建立连接 线程安全
	 */
	public void initConfigs() {
		if (!inited.get()) {
			synchronized (initLock) {
				if (!inited.get()) {
					refreshEndPoints(serviceInfos);
					inited.set(true);
				}
			}
		}
	}

	boolean firstRefreshEndPoints = true ;
	
	/**
	 *更新从register中心取得service列表
	 * 
	 * @param newServiceInfos
	 * @threadSafe
	 */
	public void refreshEndPoints(ServiceInfos newServiceInfos) {
		TafLoggerCenter.info(this+" "+objectName+" refreshEndPoints start " );
		List<ServiceEndPointInfo> activeServices = newServiceInfos.getActiveServices();
		boolean now = false ;
		synchronized(serviceInfoLock) {
			if ( firstRefreshEndPoints ) {
				firstRefreshEndPoints = false ;
				now = true ;
			}
			// 移除不在新下发的service列表中的服务器
			for (Entry<String, ServiceEndPoint> entry : aliveServiceMap.entrySet()) {
				String key = entry.getKey();
				boolean exist = false;
				for (ServiceEndPointInfo info : activeServices) {
					if (info.getKey().equals(key)) {
						exist = true;
						break;
					}
				}
				if (!exist) {
					try {
						removeService(key);
						TafLoggerCenter.info(this+" "+objectName + " removeNotexistService " + key);
					} catch (Exception e) {
						TafLoggerCenter.info(this+" "+objectName + " removeNotexistService " + key + " error " + e, e);
					}
				}
			}
			// 移除下发列表中标记失败的service
			List<ServiceEndPointInfo> inactiveServices = newServiceInfos.getInActiveServices();
			for (ServiceEndPointInfo serviceInfo : inactiveServices) {
				try {
					removeService(serviceInfo.getKey());
					TafLoggerCenter.info(this+" "+objectName + " removeInactiveService " + serviceInfo.getKey());
				} catch (Exception e) {
					TafLoggerCenter.info(this+" "+objectName + " removeInactiveService " + serviceInfo + " error " + e, e);
				}
			}
			this.serviceInfos = newServiceInfos;
		}
		doConn(now);
		destory.set(false);
	}

	/**
	 * 从存活service列表中移除一个service
	 * 外面调用时会加锁
	 * @param service
	 */
	//TODO ----------- set fail
	private void removeService(String infoKey) {
		ServiceEndPoint point = existsServiceMap.remove(infoKey);
		if ( null != point ) {
			//移除连接同时设置服务不可用
			serviceInfos.setServiceFail(point);
			point.closeAll();
		}
		if (aliveServiceMap.containsKey(infoKey)) {
			aliveServiceMap.get(infoKey).closeAll();
		}
		aliveServiceMap.remove(infoKey);
		TafLoggerCenter.info(this+" remove service from map "+infoKey);
	}

	/**
	 * 设置每个服务端口允许的连接数
	 * 
	 * @param connNum
	 */
	public void setConnNum(int connNum) {
		if (destory.get()) {
			throw new RuntimeException("all service conn is closed, can not set allConnNum.");
		}
		for (Entry<String, ServiceEndPoint> entry : existsServiceMap.entrySet()) {
			try {
				ServiceEndPoint value = entry.getValue();
				value.getInfo().setAllowConnNum(connNum);
				value.checkEndPointSize();
			} catch (Exception e) {
				TafLoggerCenter.info("setConnNum("+connNum+") error", e);
			}
		}
	}

	AtomicBoolean destory = new AtomicBoolean();

	public void closeAllConns() {
		destory.set(true);
		for (Entry<String, ServiceEndPoint> entry : aliveServiceMap.entrySet()) {
			try {
				ServiceEndPoint value = entry.getValue();
				value.closeAll();
			} catch (Exception e) {
				TafLoggerCenter.info("closeAllConns error", e);
			}
		}
		aliveServiceMap.clear();
		existsServiceMap.clear();
	}

	/**
	 * 选择一个合适的服务器发送信息
	 * 
	 * @param msg
	 * @threadSafe
	 */
	public void sendMsg(SendMessage msg) {
		ServiceEndPoint endPoint = getAliveEndPoint(msg);
		endPoint.sendMsg(msg);
		TafLoggerCenter.debug(proxyConfig.getObjectName() + " " + endPoint.getInfo() + " send msg "
				+ msg.getFuture().getSeq());
	}

	/**
	 * 再次尝试发送 直接将异常通知等待的future
	 * 
	 * @param msg
	 */
	public void tryAgainSend(SendMessage msg) {
		boolean failed = true;
		try {
			getAliveEndPoint(msg).sendMsg(msg);
			failed = false ;
		} catch (Exception ex) {
			TafLoggerCenter.info(this+" tryAgainSend msg Exception " + ex, ex);
			msg.getFuture().setFailure(new TafException("second send " + ex.toString(), ex));
		} catch (Throwable ex) {
			TafLoggerCenter.info(this+" tryAgainSend msg Throwable " + ex, ex);
			msg.getFuture().setFailure(new TafException(ex.toString(), ex));
		} finally {
			if (failed && msg.getFuture().isAsync()) {
				JceMessage jceMsg = JceMessage.createErrorRespMessage(JCEPROXYCONNECTERR.value, msg.getFuture()
						.getSeq());
				jceMsg.setFuture(msg.getFuture());
				proxyConfig.getTafThreadPool().receiveMsg(jceMsg);
			}
		}
	}

	/**
	 * 创建一个失败的异步回应并激活callBackHandler
	 * 
	 * @param future
	 */
	void createErrRespDispatch(ServantFuture future, int iret) {
		String methodName = future.getSFuncName();
		ResponsePacket rs = new ResponsePacket((short) JCEVERSION.value, (byte) 0, future.getSeq(), 0, iret,
				new byte[] {}, new HashMap<String, String>());
		future.getHandler()._onDispatch(methodName, rs);
	}

	/**
	 * 创建一个msg future 并设置序列号、允许等待时间和异步调用标示
	 * 
	 * @param allowWaiteTime
	 * @param isAsync
	 * @return
	 */
	public ServantFuture createJceFuture(long allowWaiteTime, boolean isAsync) {
		ServantFuture future = new ServantFuture(getNextSeq(), allowWaiteTime);
		future.setAsync(isAsync);
		return future;
	}

	/**
	 * 序列号生成器
	 * 
	 * @return
	 */
	protected int getNextSeq() {
		return seq.incrementAndGet();
	}

	AtomicLong lastCheckFailerServiceTime = new AtomicLong(TimerProvider.getNow());

	AtomicLong lastCheckTimeoutTime = new AtomicLong(TimerProvider.getNow());
	/**
	 * 最小重连间隔时间
	 */
	int minReconnInterv = 1000;

	final int normalReconnInterv = 60000;

	int reConnInterv = 60000;

	Object waitConnLock = new Object();

	// /////////////////////////////////////////////////////////////////////////////

	AtomicBoolean doingConn = new AtomicBoolean();
	AtomicLong lastConnTime = new AtomicLong(0);

	private ServiceEndPoint getAliveEndPoint(SendMessage msg) {
		ServiceEndPoint endPoint = null;
		if (aliveServiceMap.size() == 0) {
			reConnInterv = minReconnInterv;
		}
		long now = System.currentTimeMillis();
		if (lastConnTime.get() == 0 || now - lastConnTime.get() > reConnInterv) {
			doingConn.set(true);
			lastConnTime.set(now);
			doConn(false);
		}
		if (aliveServiceMap.size() == 0) {
			synchronized (waitConnLock) {
				try {
					waitConnLock.wait(400);
				} catch (InterruptedException e) {
					TafLoggerCenter.info("getAliveEndPoint wait was interrupted", e);
				}
			}
			TafLoggerCenter.info(this+" "+objectName + " try400 sleep " + (System.currentTimeMillis() - now));
		}
		int maxTryCount = serviceInfos.getAllServices().size();
		int i = 0 ;
		ServiceEndPointInfo endPointInfo = null ;
		while ( i < maxTryCount ) {
			endPointInfo = selector.getAliveConn(serviceInfos, msg);
			if (null != endPointInfo) {
				endPoint = aliveServiceMap.get(endPointInfo.getKey());
				if (endPoint == null) {
					//已经有一个连接 但不知道会不会被Selector命中 多循环几次看看
					TafLoggerCenter.info(this+" can not find this service " + endPointInfo + " " + aliveServiceMap + " "
							+ objectName + " i:"+i+" maxTryCount:"+maxTryCount);
					//有可能是刚刷新回来的活动节点，但是还没有来得及创建连接
					endPoint = new ServiceEndPoint(this, endPointInfo, timeoutHandler);
					endPoint.checkEndPointSize();
					aliveServiceMap.putIfAbsent(endPointInfo.getKey(), endPoint);
				} else {
					break;
				}
			} else {
				TafLoggerCenter.info(this+" failed getAliveConn for " + objectName);
			}
			i++;
			synchronized (waitConnLock) {
				try {
					waitConnLock.wait(100);
				} catch (InterruptedException e) {
					TafLoggerCenter.info("getAliveEndPoint wait was interrupt", e);
				}
			}
		}
		if (null == endPoint) {
			throw new TafProxyNoConnException("can not find this service " + endPointInfo + " " + aliveServiceMap + " "
					+ objectName);
		}
		return endPoint;

	}

	Object connState = new Object();
	
	/**
	 * 
	 * @param now 是否一定要连接
	 */
	private void doConn(boolean now) {
		TafLoggerCenter.debug("check "+proxyConfig.getObjectName()+" service "+now+" doingConn:"+doingConn.get());
		boolean shouldConn = false;
		if (doingConn.compareAndSet(true, false)) {
			shouldConn = true;
		}
		TafLoggerCenter.debug("check "+proxyConfig.getObjectName()+" service "+now+" shouldConn:"+shouldConn);
		if (now || shouldConn) {
			//只检查从locator查询中标记为可用的service
			ServiceEndPoint tmpPoint  = null;
			List<ServiceEndPointInfo> services = serviceInfos.getSrcActiveServices();
			for (ServiceEndPointInfo si : services) {
				TafLoggerCenter.info("check "+proxyConfig.getObjectName()+" service "+si);
				if ( si.active == false && si.protocol.equals("udp") ) {
					TafLoggerCenter.info("not check failed udp server "+si);
				} else {
					if (existsServiceMap.containsKey(si.getKey())) {
						TafLoggerCenter.info(this+" get service from map "+si.getKey());
						tmpPoint = existsServiceMap.get(si.getKey());
						tmpPoint.checkEndPointSize();
					} else {
						TafLoggerCenter.info(this+" create service from map "+si.getKey());
						tmpPoint = new ServiceEndPoint(this, si, timeoutHandler);
						tmpPoint.checkEndPointSize();
						existsServiceMap.put(si.getKey(), tmpPoint);
					}
					if(serviceInfos.isActive(tmpPoint)) {
						//如果是活的，还要刷新到活的endpoint map里
						TafLoggerCenter.info("add "+tmpPoint+" to active services "+si);
						aliveServiceMap.putIfAbsent(si.getKey(), tmpPoint);
					}
				}
			}
		}
	}

	// ///////////////////////////////////////////////////////////////////////////////////////

	/**
	 * 设置统计区间
	 * 
	 * @param timeStatInterv
	 */
	public void setTimeStatInterv(List<Integer> timeStatInterv) {
		ProxyStatTool.Instance.setPointStatInterv(proxy);
	}

	public void setServantProxy(ServantProxy proxy) {
		this.proxy = proxy;
	}

	public ServantProxy getServantProxy() {
		return proxy;
	}
	
	public List<Integer> getTimeStatInterv() {
		return ProxyStatTool.Instance.getStatTool(proxy).getStatIntervals();
	}

	public TAdapterSelector getSelector() {
		return selector;
	}

	public void setSelector(TAdapterSelector selector) {
		this.selector = selector;
	}

	class FutureTimeoutHandler implements TimeoutHandler<Integer, ServantFuture> {
		public void timeout(Integer k, ServantFuture future, long now) {
			if (future.isAsync()) {
				// 没有线程在等待回应
				future.setFailure(new TafCallTimeoutException("asyncMsg " + future.getSeq() + " waiteResp timeout "));
				JceMessage jceMsg = JceMessage.createErrorRespMessage(JCEASYNCCALLTIMEOUT.value, future.getSeq());
				jceMsg.setFuture(future);
				proxyConfig.getTafThreadPool().receiveMsg(jceMsg);
			} else {
				future.setFailure(new TafCallTimeoutException("msg " + future.getSeq() + " waiteResp timeout "));
			}
			StatMicMsgHead head = TafUtils.getHead(processName, future.getSServantName(), future.getSFuncName(), "",
					future.getRemoteHost(), future.getRemotePort(), 0);
			String key = future.getEndPointKey();
			ServiceEndPoint endPoint = aliveServiceMap.get(key);
			if ( null != endPoint  ) {
				future.onNetCallFinished();
				endPoint.onCallFinished(head, SendMsgStatBody.CallStatusTimeout, future.getCostTime(), "", false);
			} else {
				TafLoggerCenter.info(this+" timeoutHandler endPoint "+key+" inactive");
			}
		}
	}

	public ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> getStatmsg() {
		return ProxyStatTool.Instance.getStatTool(proxy).getStats();
	}
	
	public ConcurrentLinkedQueue<StatSampleMsg> StatSampleMsgQueue = new ConcurrentLinkedQueue<StatSampleMsg>();

	long lastCheckTime = System.currentTimeMillis();
	
	/**
	 * 定时对连接状态做检测
	 */
	public void checkConnStat() {
		if ( System.currentTimeMillis() - lastCheckTime > getServantProxy().taf_timeout_interval() ) {
			for (Entry<String, ServiceEndPoint> entry : aliveServiceMap.entrySet()) {
				try {
					ServiceEndPoint endPoint = entry.getValue();
					endPoint.checkConnStat();
				} catch (Exception e) {
					TafLoggerCenter.info("checkConnStat error", e);
				}
			}
			lastCheckTime = System.currentTimeMillis();
		}
	}

	public String getProcessName() {
		return processName;
	}

	public void onAllSessoinClose(ServiceEndPointInfo serviceInfo) {
		try {
			synchronized(serviceInfoLock) {
				if (null != serviceInfo) {
					serviceInfos.setServiceFail(serviceInfo);
					aliveServiceMap.remove(serviceInfo.getKey());
					TafLoggerCenter.info(this+" getAliveEndPoint failed,remove aliveServiceMap " + serviceInfo.getKey() + " for "
							+ objectName);
				} else {
					TafLoggerCenter.info(this+" one serviceInfo is null  for " + objectName);
				}
			}
		} catch (Exception e) {
			TafLoggerCenter.info(this+" getAliveEndPoint call onAllSessoinClose error " + objectName + " " + e);
		}
	}

	public void onAllSessoinClose(ServiceEndPoint service) {
		try {
			synchronized(serviceInfoLock) {
				if ( service.aliveSessions.size() == 0 ) {
					serviceInfos.setServiceFail(service);
					ServiceEndPoint p = aliveServiceMap.remove(service.getInfo().getKey());
					if ( null != p  ) {
						TafLoggerCenter.info(this+" remove aliveServiceMap " + service.getInfo().getKey() + " for " + objectName);
					}
				} else {
					TafLoggerCenter.info("warn failed service has alive session,can not remove");
				}
			}
		} catch (Exception e) {
			TafLoggerCenter.info(this+" onAllSessoinClose error " + objectName + " " + e);
		}
	}

	public  void onSessionClose(ServiceEndPoint service) {
	
	}

	public  void onSessionOpen(ServiceEndPoint service) {
		try {
			synchronized(serviceInfoLock) {
				if (aliveServiceMap.containsKey(service.getInfo().getKey())) {
					//当一个service有多个连接的时候触发
					TafLoggerCenter.info(this+" also has alive service " + service.getInfo().getKey() + " in map.");
				}
				reConnInterv = normalReconnInterv;
				serviceInfos.setReconnInterv(reConnInterv);
				aliveServiceMap.put(service.getInfo().getKey(), service);
				serviceInfos.setServiceActive(service);
			}
			synchronized (waitConnLock) {
				waitConnLock.notifyAll();
			}
			TafLoggerCenter.info(this+" put aliveServiceMap " + service.getInfo().getKey() + " for " + objectName);
		} catch (Exception e) {
			TafLoggerCenter.info(this+" onSessionOpen error " + objectName + " " + e);
		}
	}

	/**
	 * 重置灰度标识
	 */
	public void onResetGrid(ServiceEndPoint service, int newGridValue) {
		try {
			serviceInfos.setServiceGrid(service, newGridValue);
		} catch (Exception e) {
			TafLoggerCenter.info("onResetGrid error " + objectName + " " + e);
		}
	}

}
