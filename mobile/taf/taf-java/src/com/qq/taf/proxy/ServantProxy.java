package com.qq.taf.proxy;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.atomic.AtomicInteger;

import com.qq.sim.Millis100TimeProvider;
import com.qq.tac2.jdt.share.AsyncClientInfoBox;
import com.qq.tac2.jdt.share.AsyncClientRuntimeInfo;
import com.qq.taf.QueryFPrxHelper;
import com.qq.taf.ResponsePacket;
import com.qq.taf.StatFPrxHelper;
import com.qq.taf.StatMicMsgHead;
import com.qq.taf.StatSampleMsg;
import com.qq.taf.cnst.Const;
import com.qq.taf.cnst.JCEASYNCCALLTIMEOUT;
import com.qq.taf.cnst.JCEPROXYCONNECTERR;
import com.qq.taf.cnst.JCESERVERSUCCESS;
import com.qq.taf.cnst.JCEVERSION;
import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.proxy.conn.ProxyConnRouter;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;
import com.qq.taf.proxy.exec.TafCallTimeoutException;
import com.qq.taf.proxy.exec.TafException;
import com.qq.taf.proxy.exec.TafProxyException;
import com.qq.taf.proxy.exec.TafProxyNoConnException;
import com.qq.taf.proxy.utils.TafUtils;

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
	QueryFPrxHelper queryFPrx = null;
	ServiceInfos serviceInfos;
	//private ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> statmsg = new ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody>();

	private ServantProxyRouter router;
	private String taf_proxyName = "ServantProxy";
	
	private StatFPrxHelper statPrx;
	
	long lastAsyncRefreshTime = 0; 
	
	void setProxyAyncRefreshTime() {
		lastAsyncRefreshTime = Millis100TimeProvider.INSTANCE.currentTimeMillis();
	}
	
	boolean canAsyncRefresh() {
		return queryFPrx!=null && Millis100TimeProvider.INSTANCE.currentTimeMillis()-this.lastAsyncRefreshTime > 2000;
	}
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
		ServiceInfosRefresher.INSTANCE.addServantProxy(this);
	}
	
	public int getID() {
		return proxyConfig.hashCode();
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
	 * @see com.qq.taf.proxy.ServantInterface#taf_hash(int)
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
	 * @see com.qq.taf.proxy.ServantInterface#taf_hash()
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
	 * com.qq.taf.proxy.ServantInterface#taf_tAdapterSelector(com.qq.taf.proxy
	 * .TAdapterSelector)
	 */
	public void taf_tAdapterSelector(TAdapterSelector selector) {
		proxyConnRouter.setSelector(selector);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_tAdapterSelector()
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
		//统一采用1分钟刷新，该方法
		//this.referNodeInterv = referNodeInterv;
	}
	
	/**
	 * 获取proxy从locator查询的间隔 默认是1000*60
	 * 
	 * @param millSecond
	 */
	public long taf_referNodeInterv() {
		return ServiceInfosRefresher.getRefreshInterval();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_sync_timeout(int)
	 */
	public void taf_sync_timeout(int millSecond) {
		proxyConfig.syncTimeoutMill = millSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_sync_timeout()
	 */
	public int taf_sync_timeout() {
		return proxyConfig.syncTimeoutMill;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_async_timeout(int)
	 */
	public void taf_async_timeout(int millSecond) {
		proxyConfig.asyncTimeoutMill = millSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_async_timeout()
	 */
	public int taf_async_timeout() {
		return proxyConfig.asyncTimeoutMill;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_refreshEndPInterval(int)
	 */
	public void taf_refreshEndPInterval(int referMillSecond) {
		proxyConfig.refreshEndpointInterval = referMillSecond;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_refreshEndPInterval()
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
	 * @seecom.qq.taf.proxy.ServantInterface#taf_set_router(com.qq.taf.proxy.
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
	 * @see com.qq.taf.proxy.ServantInterface#taf_Router()
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
	 * @see com.qq.taf.proxy.ServantInterface#taf_invoke(java.lang.String,
	 * byte[], java.util.Map)
	 */
	public byte[] taf_invoke(String methodName, byte[] sBuffer, Map<String, String> map) {
		return taf_invoke(null, methodName, sBuffer, map, new HashMap<String, String>(), true, false);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.qq.taf.proxy.ServantInterface#taf_invoke(java.lang.String,
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
			if (needReturn) {
				ResponsePacket response = (ResponsePacket) result;
				if (response.iRet != JCESERVERSUCCESS.value) {
					throw new TafProxyException(proxyConfig.getObjectName()+"#"+methodName,TafException.makeException(response.iRet));
				} else {
					return response.sBuffer;
				}
			} else {
				return new byte[] {};
			}
		}
	}

	void doSendMsg(SendMessage msg) {
		// 第一次同步刷新 同步刷新也要有保护间隔 2秒
		proxyConnRouter.initConfigs();
		long time = Millis100TimeProvider.INSTANCE.currentTimeMillis() - msg.getFuture().getStartTime();
		if (time > msg.getFuture().allowWaiteTimeMill) {
			// 还未发送已经超时
			StatMicMsgHead head = TafUtils.getHead(proxyConnRouter.getProcessName(),
					msg.getRequestPacket().sServantName, msg.getRequestPacket().sFuncName, "", "0.0.0.0", 0, 0, proxyConfig.getSetArea(), proxyConfig.getSetId());
			
			ProxyStatTool.Instance.addStatTool(proxyConnRouter.getServantProxy())
				.addTimeoutTime(head, (int)(time));
		
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
				ServiceInfosRefresher.INSTANCE.asyncGetServiceInfos(this);
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
		this.taf_del_hash();
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

	/**
	 * 关闭所有网络连接
	 */
	protected void closeAllConns() {
		proxyConnRouter.closeAllConns();
	}

	/**
	 * 设置连接数
	 */
	@Deprecated
	protected void setConnNum(int connNum) {
		proxyConnRouter.setConnNum(connNum);
	}

	protected void checkConnStat() {
		proxyConnRouter.checkConnStat();
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
	
	protected boolean tacAsync_taf_invoke(com.qq.tac2.jdt.share.AsyncClientInfoBox __box__, String methodName,
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
