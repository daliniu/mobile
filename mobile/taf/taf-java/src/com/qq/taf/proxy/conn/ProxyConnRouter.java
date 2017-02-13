package com.qq.taf.proxy.conn;

import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import com.qq.taf.ResponsePacket;
import com.qq.taf.StatMicMsgHead;
import com.qq.taf.StatSampleMsg;
import com.qq.taf.cnst.JCEASYNCCALLTIMEOUT;
import com.qq.taf.cnst.JCEPROXYCONNECTERR;
import com.qq.taf.cnst.JCEVERSION;
import com.qq.taf.proxy.ProxyConfig;
import com.qq.taf.proxy.ProxyStatTool;
import com.qq.taf.proxy.SendMessage;
import com.qq.taf.proxy.SendMsgStatBody;
import com.qq.taf.proxy.ServantFuture;
import com.qq.taf.proxy.ServantProxy;
import com.qq.taf.proxy.ServiceInfos;
import com.qq.taf.proxy.ServiceInfosRefresher;
import com.qq.taf.proxy.TAdapterSelector;
import com.qq.taf.proxy.TafLoggerCenter;
import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.proxy.exec.TafCallTimeoutException;
import com.qq.taf.proxy.exec.TafException;
import com.qq.taf.proxy.exec.TafProxyNoConnException;
import com.qq.taf.proxy.utils.TafUtils;
import com.qq.taf.proxy.utils.TimeoutHandler;

/**
 * 维持了proxy的所有连接 负责刷新 新建连接
 * 
 * @author albertzhu
 * 
 */
public class ProxyConnRouter implements ServiceEndPointListener {

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

	volatile ServiceInfos serviceInfos;

	ConcurrentHashMap<String, ServiceEndPoint> existsServiceMap = new ConcurrentHashMap<String, ServiceEndPoint>();
	ConcurrentHashMap<String, ServiceEndPoint> aliveServiceMap = new ConcurrentHashMap<String, ServiceEndPoint>();

	private Boolean isInit = new Boolean(false);

	FutureTimeoutHandler timeoutHandler;
	
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
	
	public String toString() {
		return objectName+"'s Router(ID="+proxy.getID()+")";
	}
	/**
	 * 初始化配置 将配置中的服务器地址信息加载 建立连接 线程安全
	 */
	public void initConfigs() {
		ServiceInfos tmpInfos = null;
		if(!isInit) {
			synchronized(isInit) {
				if(!isInit) {
					if(serviceInfos.getSrcActiveServices().isEmpty()) {
						TafLoggerCenter.info(this+" initConfigs from registry");
						try {
							tmpInfos = ServiceInfosRefresher.INSTANCE.getServiceInfos(proxy);
							if(tmpInfos!=null&&!tmpInfos.getSrcActiveServices().isEmpty()) {
								refreshEndPoints(tmpInfos);
							}
							else {
								refreshEndPoints(serviceInfos);
							}
						}
						catch(Throwable th) {
							TafLoggerCenter.error(this+" init from registry error, and initConfigs from "+serviceInfos);
							refreshEndPoints(serviceInfos);
						}
					}
					else {
						TafLoggerCenter.info(objectName+" initConfigs from "+serviceInfos);
						refreshEndPoints(serviceInfos);
					}
					isInit = true;
				}
			}
		}
	}
	
	/**
	 *更新从register中心取得service列表
	 * 
	 * @param newServiceInfos
	 * @threadSafe
	 */
	public void refreshEndPoints(ServiceInfos newServiceInfos) {
		TafLoggerCenter.debug(this+" "+objectName+" refreshEndPoints start " );
		List<ServiceEndPointInfo> activeServices = newServiceInfos.getActiveServices();
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
					TafLoggerCenter.info(this+" removeNotexistService " + key);
				} catch (Exception e) {
					TafLoggerCenter.error(this+" removeNotexistService " + key + " error " + e, e);
				}
			}
		}
		// 移除下发列表中标记失败的service
		List<ServiceEndPointInfo> inactiveServices = newServiceInfos.getInActiveServices();
		for (ServiceEndPointInfo serviceInfo : inactiveServices) {
			try {
				removeService(serviceInfo.getKey());
				TafLoggerCenter.info(this+ " refresh removeInactiveService " + serviceInfo.getKey());
			} catch (Exception e) {
				TafLoggerCenter.error(this+" refresh removeInactiveService " + serviceInfo + " error " + e, e);
			}
		}
		this.serviceInfos = newServiceInfos;
		doConn();
		destory.set(false);
		TafLoggerCenter.debug(this+" refreshEndPoints end, serviceInfos="+serviceInfos );
	}

	/**
	 * 从存活service列表中移除一个service
	 * 外面调用时会加锁
	 * @param service
	 */
	private void removeService(String infoKey) {
		ServiceEndPoint point = existsServiceMap.remove(infoKey);
		if ( null != point ) {
			//移除连接同时设置服务不可用
			serviceInfos.setServiceFail(point);
			point.closeAll();
		}
		point = aliveServiceMap.remove(infoKey);
		if (point!=null) {
			point.closeAll();
		}
	}

	/**
	 * 设置每个服务端口允许的连接数
	 * 
	 * @param connNum
	 */
	@SuppressWarnings("deprecation")
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
				TafLoggerCenter.error(this+" setConnNum("+connNum+") error", e);
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
				TafLoggerCenter.error(this+" closeAllConns error", e);
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


	Object waitConnLock = new Object();

	// /////////////////////////////////////////////////////////////////////////////

	AtomicBoolean doingConn = new AtomicBoolean();
	AtomicLong lastConnTime = new AtomicLong(0);

	private ServiceEndPoint getAliveEndPoint(SendMessage msg) {
		ServiceEndPoint endPoint = null;
		if (aliveServiceMap.size() == 0) {
			if(serviceInfos.getActiveServices().isEmpty()) {//非主控方式创建的服务，可能活动列表为空
				synchronized(this) {
					if(serviceInfos.getActiveServices().isEmpty()) {
						serviceInfos = ServiceInfosRefresher.INSTANCE.getServiceInfos(proxy);
						TafLoggerCenter.info(this+" serviceInfos's activeService is null first, refresh from refresher");
					}
				}
			}
			doConn();
		}
		if (aliveServiceMap.size() == 0) {
			synchronized (waitConnLock) {
				try {
					waitConnLock.wait(400);
				} catch (InterruptedException e) {
					TafLoggerCenter.info("interrupted wait", e);
				}
			}
			TafLoggerCenter.info(this+" "+objectName + " try400 sleep ");
		}
		int maxTryCount = serviceInfos.getAllServices().size();
		int i = 0 ;
		ServiceEndPointInfo endPointInfo = null ;
		if(serviceInfos.getActiveServices().isEmpty()) {//非主控方式创建的服务，可能活动列表为空
			synchronized(this) {
				if(serviceInfos.getActiveServices().isEmpty()) {
					serviceInfos = ServiceInfosRefresher.INSTANCE.getServiceInfos(proxy);
					TafLoggerCenter.info(this+" serviceInfos's activeService is null, refresh from refresher");
				}
			}
		}
		while ( i < maxTryCount ) {
			endPointInfo = selector.getAliveConn(serviceInfos, msg);
			if (null != endPointInfo) {
				endPoint = aliveServiceMap.get(endPointInfo.getKey());
				if (endPoint == null) {
					//已经有一个连接 但不知道会不会被Selector命中 多循环几次看看
					TafLoggerCenter.info(this+" can not find this service " + endPointInfo + " " + aliveServiceMap + " "
							+ objectName + " i:"+i+" maxTryCount:"+maxTryCount);
					endPoint = new ServiceEndPoint(this, endPointInfo, timeoutHandler);
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
					TafLoggerCenter.info("interrupted wait", e);
				}
			}
		}
		if (null == endPoint) {
			throw new TafProxyNoConnException("can not find this service " + endPointInfo + " " + aliveServiceMap + " "
					+ objectName);
		}
		return endPoint;

	}

	/**
	 * 
	 * @param now 是否一定要连接
	 */
	private void doConn() {
		boolean flag = false;
		try {
			if(doingConn.compareAndSet(false, true)) {
				flag = true;
				ServiceEndPoint tmpPoint  = null;
				List<ServiceEndPointInfo> services = serviceInfos.getSrcActiveServices();
				for (ServiceEndPointInfo si : services) {
					TafLoggerCenter.info("check "+proxyConfig.getObjectName()+" service "+si);
					if ( si.active == false && si.protocol.equals("udp") ) {
						TafLoggerCenter.info("not check failed udp server "+si);
					} else {
						if (existsServiceMap.containsKey(si.getKey())) {
							TafLoggerCenter.info(this+" get "+si+" from existServiceMap ");
							tmpPoint = existsServiceMap.get(si.getKey());
							//tmpPoint.checkEndPointSize();
						} else {
							TafLoggerCenter.info(this+" create "+si+" from existServiceMap");
							tmpPoint = new ServiceEndPoint(this, si, timeoutHandler);
							//tmpPoint.checkEndPointSize();
							existsServiceMap.putIfAbsent(si.getKey(), tmpPoint);
						}
						if(serviceInfos.isActive(tmpPoint)) {
							//如果是活的，还要刷新到活的endpoint map里
							TafLoggerCenter.info(this+" add "+tmpPoint+" to aliveServiceMap");
							aliveServiceMap.putIfAbsent(si.getKey(), tmpPoint);
						}
					}
				}
			}
		}
		catch(Throwable th) {
			TafLoggerCenter.error(this.serviceInfos.getObjectName()+" doConn error ", th);
		}
		finally {
			if(flag) {
				doingConn.set(false);
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
					future.getRemoteHost(), future.getRemotePort(), 0, proxyConfig.getSetArea(), proxyConfig.getSetId());
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
					TafLoggerCenter.info(this+" checkConnStat error", e);
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
			if (null != serviceInfo) {
				serviceInfos.setServiceFail(serviceInfo);
				aliveServiceMap.remove(serviceInfo.getKey());
				TafLoggerCenter.info(this+" getAliveEndPoint failed,remove aliveServiceMap " + serviceInfo.getKey() + " for "
						+ objectName);
			} else {
				TafLoggerCenter.error(this+" one serviceInfo is null  for " + objectName);
			}
		} catch (Exception e) {
			TafLoggerCenter.error(this+" getAliveEndPoint call onAllSessoinClose error " + objectName + " " + e);
		}
	}

	public void onAllSessoinClose(ServiceEndPoint service) {
		try {
			if (!service.aliveSession.isOpen()) {
				serviceInfos.setServiceFail(service);
				ServiceEndPoint p = aliveServiceMap.remove(service.getInfo().getKey());
				if ( null != p  ) {
					TafLoggerCenter.info(this+" remove aliveServiceMap " + service + " for " + objectName);
				}
			} else {
				TafLoggerCenter.error(service.getInfo().getKey()+" warn failed service has alive session,can not remove");
			}
		} catch (Exception e) {
			TafLoggerCenter.error(this+" onAllSessoinClose error " + objectName + " " + e);
		}
	}

	public  void onSessionClose(ServiceEndPoint service) {
		onAllSessoinClose(service);
	}

	public  void onSessionOpen(ServiceEndPoint service) {
		try {
				if (aliveServiceMap.containsKey(service.getInfo().getKey())) {
					//当一个service有多个连接的时候触发
					TafLoggerCenter.info(this+" also has alive service " + service + " in map.");
				}
				serviceInfos.setServiceActive(service);
				aliveServiceMap.putIfAbsent(service.getInfo().getKey(), service);
				synchronized (waitConnLock) {
					waitConnLock.notifyAll();
				}
				TafLoggerCenter.info(this+" put aliveServiceMap " + service + " for " + objectName);
		} catch (Exception e) {
			TafLoggerCenter.error(this+" onSessionOpen error " + objectName + " " + e);
		}
	}

	/**
	 * 重置灰度标识
	 */
	public void onResetGrid(ServiceEndPoint service, int newGridValue) {
		try {
			serviceInfos.setServiceGrid(service, newGridValue);
		} catch (Exception e) {
			TafLoggerCenter.error("onResetGrid error " + objectName + " " + e);
		}
	}

}
