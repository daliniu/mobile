package com.duowan.taf.proxy;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

import com.duowan.tac2.jdt.share.AsyncClientInfoBox;
import com.duowan.tac2.jdt.share.AsyncClientRuntimeInfo;
import com.duowan.taf.EndpointF;
import com.duowan.taf.QueryFPrxCallback;
import com.duowan.taf.QueryFPrxHelper;
import com.duowan.taf.ResponsePacket;
import com.duowan.taf.StatFPrxHelper;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.StatSampleMsg;
import com.duowan.taf.cnst.Const;
import com.duowan.taf.cnst.JCEASYNCCALLTIMEOUT;
import com.duowan.taf.cnst.JCEPROXYCONNECTERR;
import com.duowan.taf.cnst.JCESERVERSUCCESS;
import com.duowan.taf.cnst.JCEVERSION;
import com.duowan.taf.holder.JceArrayListHolder;
import com.duowan.taf.proxy.codec.JceMessage;
import com.duowan.taf.proxy.conn.ProxyConnRouter;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;
import com.duowan.taf.proxy.exec.TafCallTimeoutException;
import com.duowan.taf.proxy.exec.TafException;
import com.duowan.taf.proxy.exec.TafProxyException;
import com.duowan.taf.proxy.exec.TafProxyNoConnException;
import com.duowan.taf.proxy.utils.TafUtils;

/**
 * proxy的基类 在这里开始发送对象到远程服务器
 * 
 * @author albertzhu
 * 
 */
public class ServantProxy implements ServantInterface {

	public final static short version = JCEVERSION.value;

	ProxyConfig proxyConfig;
	ProxyConnRouter proxyConnRouter;
	// 从register中心取信息的间隔
	long referNodeInterv = 1000 * 60 * 1;
	// 上次从register中心取信息的时间
	AtomicLong lastRefreTime = new AtomicLong(0);
	QueryFPrxHelper queryFPrx = null;
	// 刷新服务器列表回调
	private QueryServiceInfoCallback queryServiceCallBack = new QueryServiceInfoCallback();
	ServiceInfos serviceInfos;
	//private ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> statmsg = new ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody>();

	private ServantProxyRouter router;
	private String taf_proxyName = "ServantProxy";
	
	private StatFPrxHelper statPrx;
	
	/**
	 * 采样阀值
	 */
	private int sampleRate = 10000;

	/**
	 * 每分钟最大采样次数
	 */
	private int maxSampleCount = 20;

	/**
	 * 初始化
	 * 
	 * @param config
	 *            配置信息
	 * @param conns
	 *            proxy连接
	 * @param timeStatInterv
	 *            调用统计区间分布
	 * @param queryPrx
	 *            查询注册中心proxy
	 */
	public void taf_init(ProxyConfig config, ProxyConnRouter conns, CopyOnWriteArrayList<Integer> timeStatInterv,
			QueryFPrxHelper queryPrx) {
		this.proxyConfig = config;
		serviceInfos = new ServiceInfos(config.getObjectName());
		proxyConnRouter = conns;
		proxyConnRouter.setServantProxy(this);
		proxyConnRouter.setTimeStatInterv(timeStatInterv);
		this.queryFPrx = queryPrx;
		serviceInfos.objectName = proxyConfig.objectName;
	}

	public StatFPrxHelper getStatPrxPr() {
		return this.statPrx;
	}

	public void setStatPrxPr(StatFPrxHelper statPrxPr) {
		this.statPrx = statPrxPr;
	}
	
	/**
	 * 设置时间分布
	 * 
	 * @param timeStatInterv
	 */
	public void taf_pointStatInterv(List<Integer> timeStatInterv) {
		proxyConnRouter.setTimeStatInterv(timeStatInterv);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_hash(int)
	 */
	public ServantProxy taf_hash(int hash) {
		if (hash < 0) {
			hash = -hash;
		}
		ServantProxyThreadData.INSTANCE.getData()._hashCode=hash;
		return this;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_hash()
	 */
	public int taf_hash() {
		return ServantProxyThreadData.INSTANCE.getData()._hashCode;
	}

	public void taf_del_hash() {
		ServantProxyThreadData.INSTANCE.getData()._hashCode=Const.INVALID_HASH_CODE;
	}
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.duowan.taf.proxy.ServantInterface#taf_tAdapterSelector(com.duowan.taf.proxy
	 * .TAdapterSelector)
	 */
	public void taf_tAdapterSelector(TAdapterSelector selector) {
		proxyConnRouter.setSelector(selector);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_tAdapterSelector()
	 */
	public TAdapterSelector taf_tAdapterSelector() {
		return proxyConnRouter.getSelector();
	}
	
	/**
	 * 设置proxy从locator查询的间隔 默认是1000*60
	 * 
	 * @param millSecond
	 */
	public void taf_referNodeInterv(int referNodeInterv) {
		this.referNodeInterv = referNodeInterv;
	}
	
	/**
	 * 获取proxy从locator查询的间隔 默认是1000*60
	 * 
	 * @param millSecond
	 */
	public long taf_referNodeInterv() {
		return referNodeInterv;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_sync_timeout(int)
	 */
	public void taf_sync_timeout(int millSecond) {
		proxyConfig.syncTimeoutMill = millSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_sync_timeout()
	 */
	public int taf_sync_timeout() {
		return proxyConfig.syncTimeoutMill;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_async_timeout(int)
	 */
	public void taf_async_timeout(int millSecond) {
		proxyConfig.asyncTimeoutMill = millSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_async_timeout()
	 */
	public int taf_async_timeout() {
		return proxyConfig.asyncTimeoutMill;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_refreshEndPInterval(int)
	 */
	public void taf_refreshEndPInterval(int referMillSecond) {
		proxyConfig.refreshEndpointInterval = referMillSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_refreshEndPInterval()
	 */
	public int taf_refreshEndPInterval() {
		return proxyConfig.refreshEndpointInterval;
	}

	public int taf_ServiceConnNum() {
		return proxyConfig.serviceConnNum;
	}

	public void taf_ServiceConnNum(int num) {
		proxyConfig.serviceConnNum = num;
		setConnNum(num);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @seecom.duowan.taf.proxy.ServantInterface#taf_set_router(com.duowan.taf.proxy.
	 * ServantProxyRouter)
	 */
	public void taf_set_router(ServantProxyRouter router) {
		this.router = router;
	}
	
	public void taf_del_router() {
		router=null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_Router()
	 */
	public ServantProxyRouter taf_Router() {
		return router;
	}

	public int taf_readBufferSize() {
		return proxyConfig.readBufferSize;
	}

	public void taf_readBufferSize(int readBufferSize) {
		if (readBufferSize > 0) {
			proxyConfig.readBufferSize = readBufferSize;
		} else {
			throw new IllegalArgumentException("readBufferSize must Greater than zero");
		}
	}

	public int taf_writeBufferSize() {
		return proxyConfig.writeBufferSize;
	}

	public void taf_writeBufferSize(int writeBufferSize) {
		if (writeBufferSize > 0) {
			proxyConfig.writeBufferSize = writeBufferSize;
		} else {
			throw new IllegalArgumentException("writeBuffer must Greater than zero");
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_invoke(java.lang.String,
	 * byte[], java.util.Map)
	 */
	public byte[] taf_invoke(String methodName, byte[] sBuffer, Map<String, String> map) {
		return taf_invoke(null, methodName, sBuffer, map, new HashMap<String, String>(), true, false);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.duowan.taf.proxy.ServantInterface#taf_invoke(java.lang.String,
	 * byte[], java.util.Map, java.util.Map)
	 */
	public byte[] taf_invoke(String methodName, byte[] sBuffer, Map<String, String> map, Map<String, String> status) {
		return taf_invoke(null, methodName, sBuffer, map, status, true, false);
	}


	// 以下为非public方法//////////////////////////////////////////////////////////////////

	protected ProxyConfig taf_proxyConfig() {
		return proxyConfig;
	}

	/**
	 * 发起异步调用
	 * 
	 * @param handler
	 * @param methodName
	 * @param sBuffer
	 * @param map
	 */
	protected final void taf_invokeAsync(ServantProxyCallback handler, String methodName, byte[] sBuffer,
			Map<String, String> map) {
		if (null == handler) {
			taf_invoke(handler, methodName, sBuffer, map, new HashMap<String, String>(), false, true);
		} else {
			taf_invoke(handler, methodName, sBuffer, map, new HashMap<String, String>(), true, true);
		}
	}

	
	protected final void taf_invokeAsync(CallbackHandler handler, String methodName, byte[] sBuffer,
			Map<String, String> map) {
		if (null == handler) {
			taf_invoke(handler, methodName, sBuffer, map, new HashMap<String, String>(), false, true);
		} else {
			taf_invoke(handler, methodName, sBuffer, map, new HashMap<String, String>(), true, true);
		}
	}

	/**
	 * 发起异步调用 需要传递status
	 * 
	 * @param handler
	 * @param methodName
	 * @param sBuffer
	 * @param map
	 * @param status
	 */
	protected final void taf_invokeAsync(CallbackHandler handler, String methodName, byte[] sBuffer,
			Map<String, String> map, Map<String, String> status) {
		if (null == handler) {
			taf_invoke(handler, methodName, sBuffer, map, status, false, true);
		} else {
			taf_invoke(handler, methodName, sBuffer, map, status, true, true);
		}
	}

	
	protected final void taf_invokeAsync(ServantProxyCallback handler, String methodName, byte[] sBuffer,
			Map<String, String> map, Map<String, String> status) {
		if (null == handler) {
			taf_invoke(handler, methodName, sBuffer, map, status, false, true);
		} else {
			taf_invoke(handler, methodName, sBuffer, map, status, true, true);
		}
	}
	
	/**
	 * 获取当前统计信息 供上报信息线程使用
	 * 
	 * @return
	 */
	protected final ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> taf_statMsg() {
		return proxyConnRouter.getStatmsg();
	}

	public ConcurrentLinkedQueue<StatSampleMsg> getStatSampleMsg() {
		return proxyConnRouter.StatSampleMsgQueue;
	}

	// 采样map
	ConcurrentHashMap<String, AtomicInteger> sampleMap = new ConcurrentHashMap<String, AtomicInteger>();

	/**
	 * 将调用计数器归零 由外部方法每隔1分钟调用一次
	 */
	protected void checkSampleMap() {
		for (Entry<String, AtomicInteger> entry : sampleMap.entrySet()) {
			entry.getValue().set(0);
		}
	}

	/**
	 * 是否满足了采用要求
	 * 
	 * @return
	 */
	private boolean isNeedSample(String methodName) {
		if (!sampleMap.containsKey(methodName)) {
			sampleMap.putIfAbsent(methodName, new AtomicInteger());
		}
		int invokeCount = sampleMap.get(methodName).incrementAndGet();
		if (invokeCount == 1 || (invokeCount % sampleRate == 0 && invokeCount < (sampleRate * maxSampleCount))) {
			return true;
		}
		return false;
	}

	/**
	 * 采用ServantProxyCallback的调用
	 * @param handler
	 * @param methodName
	 * @param sBuffer
	 * @param map
	 * @param status
	 * @param needReturn
	 * @param isAsync
	 * @return
	 */
	private byte[] taf_invoke(ServantProxyCallback handler, String methodName, byte[] sBuffer, Map<String, String> map,
			Map<String, String> status, boolean needReturn, boolean isAsync) {
		if(handler!=null) {
			handler._data=ServantProxyThreadData.INSTANCE.getData();
		}
		ServantFuture future = null;
		if (isAsync) {
			future = proxyConnRouter.createJceFuture(taf_async_timeout(), isAsync);
		} else {
			future = proxyConnRouter.createJceFuture(taf_sync_timeout(), isAsync);
		}
		SendMessage msg = new SendMessage(proxyConfig.getObjectName(), methodName, sBuffer, proxyConfig
				.getAsyncTimeoutMill(), map, status, future, handler, needReturn);
		return taf_invoke0(msg,handler, methodName, sBuffer, map, status, needReturn, isAsync);
	}
	
	/**
	 * 调用
	 * 
	 * @param handler
	 * @param methodName
	 * @param sBuffer
	 * @param map
	 * @param needReturn
	 * @param isAsync
	 * @return
	 */
	private byte[] taf_invoke(CallbackHandler handler, String methodName, byte[] sBuffer, Map<String, String> map,
			Map<String, String> status, boolean needReturn, boolean isAsync) {
		ServantFuture future = null;
		if (isAsync) {
			future = proxyConnRouter.createJceFuture(taf_async_timeout(), isAsync);
		} else {
			future = proxyConnRouter.createJceFuture(taf_sync_timeout(), isAsync);
		}
		SendMessage msg = new SendMessage(proxyConfig.getObjectName(), methodName, sBuffer, proxyConfig
				.getAsyncTimeoutMill(), map, status, future, handler, needReturn);
		return taf_invoke0(msg,handler, methodName, sBuffer, map, status, needReturn, isAsync);
	}

	private byte[] taf_invoke0(SendMessage msg,CallbackHandler handler, String methodName, byte[] sBuffer, Map<String, String> map,
			Map<String, String> status, boolean needReturn, boolean isAsync) {

		msg.proxy = this;
		if (isNeedSample(methodName)) msg.setSample();
		msg.setInvokeGrid();
		msg.setInvokeHash();
		msg.setInvokeDyeing();
		if (isAsync) {
			proxyConfig.tafThreadPool.sendAsyncMsg(msg);
			return new byte[] {}; // 异步调用
		} else {
			doSendMsg(msg);
			
			if(!needReturn) {
				return new byte[] {};
			}
			
			Object result = null;
			try {
				result = msg.getFuture().get(); // 等待发送完成
			} catch (InterruptedException e) {
				throw new TafProxyException(" thread is InterruptedExceptioned ");
			} catch (CancellationException e) {
				throw new TafProxyException(" thread is CancellationException ");
			} catch (TafException e) {
				TafLoggerCenter.info("sendMsg throw TafException " + e);
				throw new TafProxyException(proxyConfig.getObjectName()+"#"+methodName,e);
			} catch (Exception e) {
				TafLoggerCenter.info("sendMsg throw Exception " + e);
				throw new TafProxyException(proxyConfig.getObjectName() + " ", e);
			}
			ResponsePacket response = (ResponsePacket) result;
			if (response.iRet != JCESERVERSUCCESS.value) {
				throw new TafProxyException(proxyConfig.getObjectName()+"#"+methodName,TafException.makeException(response.iRet));
			} else {
				return response.sBuffer;
			}
			
		}
	}

	void doSendMsg(SendMessage msg) {
		// 第一次同步刷新 同步刷新也要有保护间隔 2秒
		asyncRefreshSericeInfo(lastRefreTime.get() != 0);
		proxyConnRouter.initConfigs();
		long time = TimerProvider.getNow() - msg.getFuture().getStartTime();
		if (time > msg.getFuture().allowWaiteTimeMill) {
			// 还未发送已经超时
			StatMicMsgHead head = TafUtils.getHead(proxyConnRouter.getProcessName(),
					msg.getRequestPacket().sServantName, msg.getRequestPacket().sFuncName, "", "0.0.0.0", 0, 0);
			ProxyStatTool.Instance.addStatTool(proxyConnRouter.getServantProxy())
				.addTimeoutTime(head, (int)(TimerProvider.getNow() - msg.getFuture().getStartTime()));
			
			TafLoggerCenter.info("waitSend msg timeout " + msg.getFuture().getSeq() + " inQueue time:" + time);
			msg.getFuture().setFailure(
					new TafCallTimeoutException("asyncMsg " + msg.getFuture().getSeq() + " waiteResp timeout "));
			if (msg.getFuture().isAsync()) {
				JceMessage jceMsg = JceMessage.createErrorRespMessage(JCEASYNCCALLTIMEOUT.value, msg.getFuture()
						.getSeq());
				jceMsg.setFuture(msg.getFuture());
				proxyConfig.getTafThreadPool().receiveMsg(jceMsg);
			}
		} else {
			boolean createResp = false;
			try {
				proxyConnRouter.sendMsg(msg);
			} catch (TafProxyNoConnException e) {
				// 没有连接 尝试同步刷新服务器列表
				long oldreferNodeInterv = referNodeInterv;
				referNodeInterv = 2000;
				asyncRefreshSericeInfo(false);
				referNodeInterv = oldreferNodeInterv;
				proxyConnRouter.tryAgainSend(msg);
			} catch (TafException ex) {
				TafLoggerCenter.info("send msg TafException " + ex, ex);
				createResp = true;
				msg.getFuture().setFailure(ex);
			} catch (Exception ex) {
				createResp = true;
				msg.getFuture().setFailure(new TafException(ex.toString(), ex));
				TafLoggerCenter.info("send msg error " + ex, ex);
			} catch (Throwable ex) {
				createResp = true;
				msg.getFuture().setFailure(new TafException(ex.toString(), ex));
				TafLoggerCenter.info("send msg Throwable " + ex, ex);
			} finally {
				if (createResp && msg.getFuture().isAsync()) {
					JceMessage jceMsg = JceMessage.createErrorRespMessage(JCEPROXYCONNECTERR.value, msg.getFuture()
							.getSeq());
					jceMsg.setFuture(msg.getFuture());
					proxyConfig.getTafThreadPool().receiveMsg(jceMsg);
				}
			}
		}
	}

	/**
	 * 创建一个ping package
	 */
	public SendMessage createPingPackage(ServantProxyCallback handler) {
		ServantFuture future = proxyConnRouter.createJceFuture(taf_async_timeout(), true);
		SendMessage msg = new SendMessage(proxyConfig.getObjectName(), "taf_ping", new byte[] {}, proxyConfig
				.getAsyncTimeoutMill(), new HashMap<String, String>(), new HashMap<String, String>(), future, handler,
				true);
		return msg;
	}

	private void asyncRefreshSericeInfo(boolean isAsync) {
		if (null != queryFPrx) {
			boolean action = false;
			if (lastRefreTime.get() == 0) {
				lastRefreTime.set(TimerProvider.getNow());
				action = true;
			} else {
				long interv = referNodeInterv;
				if (serviceInfos.getActiveServices().size() == 0) {
					interv = 2000;
				}
				if (TimerProvider.getNow() - lastRefreTime.get() > interv) {
					lastRefreTime.set(TimerProvider.getNow());
					action = true;
				}
			}
			if (action) {
				if (isAsync) {
					asyncRefreshNode();
				} else {
					refreshNode();
				}
			}
		}
	}

	/**
	 * 同步刷新服务器列表
	 */
	@SuppressWarnings("unchecked")
	private void refreshNode() {
		boolean querySucc = false;
		try {
			JceArrayListHolder activeEp = new JceArrayListHolder();
			JceArrayListHolder inactiveEp = new JceArrayListHolder();
			int _ret = queryFPrx.findObjectByIdInSameGroup(proxyConfig.getObjectName(), activeEp, inactiveEp);
			if (_ret == JCESERVERSUCCESS.value) {
				parseRespEndPointInfo((activeEp.getValue()), (inactiveEp.getValue()));
				querySucc = true;
			} else {
				TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectById4All error , return " + _ret);
			}
		} catch (Exception e) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectById4All error " + e, e);
		}
		if (!querySucc) {
			Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = TafUtils.getServerInfoData(proxyConfig
					.getObjectName(), proxyConfig.syncTimeoutMill, proxyConfig.asyncTimeoutMill,
					proxyConfig.serviceConnNum);
			//可用列表为空 认为全部可用
			if ( null != p  ) {
				parseRespSericePointInfo(p.k, p.v);
			}
		}

	}

	/**
	 * 异步刷新服务器列表
	 */
	private void asyncRefreshNode() {
		try {
			queryFPrx.async_findObjectById4All(queryServiceCallBack, proxyConfig.getObjectName());
		} catch (Exception e) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " async_findObjectById4All error " + e, e);
		}
	}

	/**
	 * 解析注册中心返回的endPoint列表并刷新proxyConns
	 * 
	 * @param activeEp
	 * @param inactiveEp
	 */
	private void parseRespEndPointInfo(ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
		//可用列表为空 认为全部可用
//		if ( activeEp.size() == 0 && inactiveEp.size() > 0 ) {
//			activeEp = new ArrayList<EndpointF>(inactiveEp.size());
//			for ( int i = 0 ; i < inactiveEp.size() ; i++ ) {
//				EndpointF newInfo = new EndpointF(inactiveEp.get(i).host, inactiveEp.get(i).port, inactiveEp.get(i).timeout, inactiveEp.get(i).istcp, inactiveEp.get(i).grid, inactiveEp.get(i).groupworkid, inactiveEp.get(i).grouprealid);
//				activeEp.set(i, newInfo);
//			}
//			inactiveEp = new ArrayList<EndpointF>(0);
//			TafLoggerCenter.info(this+" set all server active");
//		}
		
		//if (activeEp.size() > 0 || inactiveEp.size() > 0) {
		
		if(activeEp.size()>0) {
			//完全相信从主控拉取的列表
			serviceInfos.setServices(activeEp, inactiveEp);
			TafLoggerCenter.debug(this+" callback_findObjectById4All get config from register " + serviceInfos);
			proxyConnRouter.refreshEndPoints(serviceInfos);
		}
		else {
			//如果本次取得的活动列表为0，暂用上次取得的活动列表
			TafLoggerCenter.info(this+" callback_findObjectById4All get config from register, active size = 0");
		}
		//} else {
		//	TafLoggerCenter.info(this+" registerCenter serviceList is empty");
		//}
	}
	
	private void parseRespSericePointInfo(ArrayList<ServiceEndPointInfo> activeEp, ArrayList<ServiceEndPointInfo> inactiveEp) {
		//可用列表为空 认为全部可用
//		if ( activeEp.length == 0 && inactiveEp.length > 0 ) {
//			activeEp = new ServiceEndPointInfo[inactiveEp.length];
//			for ( int i = 0 ; i < inactiveEp.length ; i++ ) {
//				ServiceEndPointInfo newInfo = inactiveEp[i].clone();
//				newInfo.setActive(true);
//				activeEp[i] = newInfo;
//			}
//			TafLoggerCenter.info(this+" set all server active");
//		}
//		if (activeEp.length > 0 || inactiveEp.length > 0) {
//			serviceInfos.setServices(activeEp, inactiveEp);
//			TafLoggerCenter.debug(this+" cacheServerData get config from " + serviceInfos);
//			proxyConnRouter.refreshEndPoints(serviceInfos);
//		} else {
//			TafLoggerCenter.info(this+" cacheServerData " + proxyConfig.getObjectName() + " serviceList is empty ");
//		}
		if(activeEp.size()>0) {
			//完全相信从主控拉取的列表
			serviceInfos.setServiceInfos(activeEp, inactiveEp);
			TafLoggerCenter.debug(this+" callback_findObjectById4All get config from register " + serviceInfos);
			proxyConnRouter.refreshEndPoints(serviceInfos);
		}
		else {
			//如果本次取得的活动列表为0，暂用上次取得的活动列表
			TafLoggerCenter.info(this+" callback_findObjectById4All get config from register, active size = 0");
		}
	}

	/**
	 * 关闭所有网络连接
	 */
	protected void closeAllConns() {
		proxyConnRouter.closeAllConns();
	}

	/**
	 * 关闭所有网络连接
	 */
	protected void setConnNum(int connNum) {
		proxyConnRouter.setConnNum(connNum);
	}

	protected void checkConnStat() {
		proxyConnRouter.checkConnStat();
	}

	/**
	 * 从register中心查询服务器信息的回调方法
	 * 
	 * @author albertzhu
	 * 
	 */
	class QueryServiceInfoCallback extends QueryFPrxCallback {
//		@Override
//		public void callback_findObjectById(EndpointF[] _ret) {
//			TafLoggerCenter.info("callback_findObjectById is not used");
//		}

		@Override
		public void callback_findObjectById_exception(int ret) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " callback_findObjectById_exception " + ret);
		}

//		@Override
//		public void callback_findObjectById4All(int _ret, EndpointF[] activeEp, EndpointF[] inactiveEp) {
//			if (_ret == JCESERVERSUCCESS.value) {
//				parseRespEndPointInfo(activeEp, inactiveEp);
//				// 存储从register取回的结果
//				TafUtils.setServerInfoData(proxyConfig.objectName, proxyConfig.getEndPoingListString());
//
//			} else {
//				TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectById4All error , return " + _ret);
//			}
//		}

		@Override
		public void callback_findObjectById4All_exception(int ret) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " callback_findObjectById4All_exception " + ret);
		}

		@Override
		public void callback_findObjectById(ArrayList<EndpointF> _ret) {
			TafLoggerCenter.info("callback_findObjectById is not used");
			
		}

		@Override
		public void callback_findObjectById4All(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				parseRespEndPointInfo(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(proxyConfig.objectName, proxyConfig.getEndPoingListString());

			} else {
				TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectById4All error , return " + _ret);
			}
			
		}

		@Override
		public void callback_findObjectByIdInSameGroup(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				parseRespEndPointInfo(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(proxyConfig.objectName, proxyConfig.getEndPoingListString());

			} else {
				TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectByIdInSameGroup error , return " + _ret);
			}
			
		}

		@Override
		public void callback_findObjectByIdInSameGroup_exception(int _iRet) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " callback_findObjectByIdInSameGroup_exception " + _iRet);
			
		}

		@Override
		public void callback_findObjectByIdInSameStation(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				parseRespEndPointInfo(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(proxyConfig.objectName, proxyConfig.getEndPoingListString());

			} else {
				TafLoggerCenter.info(proxyConfig.getObjectName() + " findObjectByIdInSameStation error , return " + _ret);
			}
			
		}

		@Override
		public void callback_findObjectByIdInSameStation_exception(int _iRet) {
			TafLoggerCenter.info(proxyConfig.getObjectName() + " callback_findObjectByIdInSameStation_exception " + _iRet);
			
		}
	}

	public List<ServiceEndPointInfo> getAliveServices() {
		return serviceInfos.getActiveServices();
	}

	public List<ServiceEndPointInfo> getInAliveServices() {
		return serviceInfos.getInActiveServices();
	}

	public int getSampleRate() {
		return sampleRate;
	}

	public void setSampleRate(int sampleRate) {
		this.sampleRate = sampleRate;
	}

	public int getMaxSampleCount() {
		return maxSampleCount;
	}

	public void setMaxSampleCount(int maxSampleCount) {
		this.maxSampleCount = maxSampleCount;
	}

	private int tafMinTimeoutInvoke = 2;
	private int tafCheckTimeoutInterval = 10*1000;
	private int tafFrequenceFailInvoke = 500;
	private float tafRadio = 0.7f;
	private int tafTryTimeInterval = 30;

	public void taf_check_timeout(int minTimeoutInvoke, int checkTimeoutInterval, int frequenceFailInvoke, float radio,
			int tryTimeInterval) {
		this.tafMinTimeoutInvoke = minTimeoutInvoke;
		this.tafCheckTimeoutInterval = checkTimeoutInterval;
		this.tafFrequenceFailInvoke = frequenceFailInvoke;
		this.tafRadio = radio;
		this.tafTryTimeInterval = tryTimeInterval;
	}

	public void taf_frequence_fail_invoke(int frequenceFailInvoke) {
		this.tafFrequenceFailInvoke = frequenceFailInvoke;
	}

	public int taf_frequence_fail_invoke() {
		return tafFrequenceFailInvoke;
	}

	public void taf_min_timeout_invoke(int minTimeoutInvoke) {
		this.tafMinTimeoutInvoke = minTimeoutInvoke;
	}

	public int taf_min_timeout_invoke() {
		return tafMinTimeoutInvoke;
	}

	public void taf_timeout_interval(int checkTimeoutInterval) {
		this.tafCheckTimeoutInterval = checkTimeoutInterval;
	}

	public int taf_timeout_interval() {
		return tafCheckTimeoutInterval;
	}

	public void taf_timeout_radio(float radio) {
		this.tafRadio = radio;
	}

	public float taf_timeout_radio() {
		return tafRadio;
	}

	public void taf_try_time_interval(int tryTimeInterval) {
		this.tafTryTimeInterval = tryTimeInterval;
	}

	public int taf_try_time_interval() {
		return tafTryTimeInterval;
	}

	public String taf_proxyName() {
		return taf_proxyName;
	}

	public void taf_proxyName(String name) {
		this.taf_proxyName = name;
	}

	public long taf_idleStatusInteval() {
		return proxyConfig.idleStatusInteval;
	}

	public void taf_idleStatusInteval(long idleStatusInteval) {
		if (idleStatusInteval > 30000) {
			proxyConfig.idleStatusInteval = idleStatusInteval;
		} else {
			throw new IllegalArgumentException("idleStatusInteval must Greater than 30000");
		}
	}

	public int hashCode() {
		return this.proxyConfig.hashCode();
	}
	
	public boolean equals(Object obj) {
		return (this == null && obj==null)||(obj!=null && obj instanceof ServantProxy && ((ServantProxy)obj).proxyConfig.equals(this.proxyConfig));
	}
	
///////////////////////////////////////////////////////////
//tac
//////////////////////////////////////////////////////////
	
	protected boolean tacAsync_taf_invoke(com.duowan.tac2.jdt.share.AsyncClientInfoBox __box__, String methodName,
			byte[] sBuffer, Map<String, String> map) {
		return tacAsync_taf_invoke(__box__, methodName, sBuffer, map, new HashMap<String, String>());
	}

	/**
	 * tac异步变量
	 */
	private static final AsyncClientRuntimeInfo empytAsyncVar = new AsyncClientRuntimeInfo() {
		public boolean run(AsyncClientInfoBox __box__) {
			__box__.setAsyncCallReturnCode(false);
			__box__.setContinueRun(true);
			return false;
		}
	};
	
	/**
	 * 供tac调用的异步发送
	 * @param __box__
	 * @param methodName
	 * @param sBuffer
	 * @param map
	 * @param status
	 * @return
	 */
	protected boolean tacAsync_taf_invoke(AsyncClientInfoBox __box__, String methodName, byte[] sBuffer,
			Map<String, String> map, Map<String, String> status) {
		__box__.setAsyncVariables(empytAsyncVar);
		__box__.setContinueRun(false);
		__box__.suspendRequest();
		TacCallbackHandler callback = new TacCallbackHandler(__box__);
		taf_invokeAsync(callback, methodName, sBuffer, map, status);
		__box__.setAsyncCallReturnCode(true);
		return true;
	}
}
