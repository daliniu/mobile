package com.duowan.taf.proxy.conn;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.duowan.netutil.nio.mina2.core.future.WriteFuture;
import com.duowan.netutil.nio.mina2.core.session.IoSession;
import com.duowan.netutil.nio.mina2.transport.socket.DatagramSessionConfig;
import com.duowan.netutil.nio.mina2.transport.socket.SocketSessionConfig;
import com.duowan.taf.PingFPrxCallback;
import com.duowan.taf.PingFPrxHelper;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.StatSampleMsg;
import com.duowan.taf.cnst.Const;
import com.duowan.taf.cnst.JCEMESSAGETYPESAMPLE;
import com.duowan.taf.cnst.JCEPROXYCONNECTERR;
import com.duowan.taf.cnst.JCESERVERRESETGRID;
import com.duowan.taf.cnst.JCESERVERSUCCESS;
import com.duowan.taf.jce.JceOutputStream;
import com.duowan.taf.jce.JceUtil;
import com.duowan.taf.proxy.ProxyStatTool;
import com.duowan.taf.proxy.TafLoggerCenter;
import com.duowan.taf.proxy.SendMessage;
import com.duowan.taf.proxy.SendMsgStatBody;
import com.duowan.taf.proxy.ServantFuture;
import com.duowan.taf.proxy.TimerProvider;
import com.duowan.taf.proxy.codec.JceMessage;
import com.duowan.taf.proxy.exec.TafException;
import com.duowan.taf.proxy.exec.TafNotSendException;
import com.duowan.taf.proxy.exec.TafProxyException;
import com.duowan.taf.proxy.exec.TafProxyNoConnException;
import com.duowan.taf.proxy.utils.TafUtils;
import com.duowan.taf.proxy.utils.TimeoutHandler;

/**
 * 一个后端到一个service的全部网络连接
 * 
 * @author albertzhu
 * 
 */
public class ServiceEndPoint implements EndPointEventListen {

	ReentrantReadWriteLock sessionListLock = new ReentrantReadWriteLock();
	List<IoSession> aliveSessions = new CopyOnWriteArrayList<IoSession>();
	Semaphore available;
	ServiceEndPointInfo info;
	String key;
	Object sessionOpenLock = new Object();
	AtomicBoolean connecting = new AtomicBoolean();
	AtomicInteger sessionsAccessPoint = new AtomicInteger();
	AtomicLong failedTimeMill = new AtomicLong(TimerProvider.getNow());
	AtomicBoolean hasConn = new AtomicBoolean(true);
	long reConnTimeInterv = 60000;
	long deReConnTimeInterv = reConnTimeInterv;
	ProxyConnRouter connRouter;
	TimeoutHandler<Integer, ServantFuture> handler;
	long connTime = TimerProvider.getNow();

	ConcurrentHashMap<Long, PingFPrxCallbackImpl> pingCallbackMap = new ConcurrentHashMap<Long, PingFPrxCallbackImpl>();
	PingFPrxHelper pingHelper = new PingFPrxHelper();


	
	public ServiceEndPoint(ProxyConnRouter connRouter, ServiceEndPointInfo info,
			TimeoutHandler<Integer, ServantFuture> handler) {
		this.connRouter = connRouter;
		this.info = info;
		key = ServiceEndPointInfo.getSessionKey(getProtocol(), getRemoteHost(), getRemotePort());
		if (connRouter.proxyConfig.getAvailableKey() > 0) {
			available = new Semaphore(connRouter.proxyConfig.getAvailableKey());
		}
		reConnTimeInterv = info.failedWaitTimeMill;
		deReConnTimeInterv = reConnTimeInterv;
		this.handler = handler;
	}

	public void ping() {
		for (IoSession session : aliveSessions) {
			try {
				SendMessage msg = connRouter.proxy.createPingPackage(pingCallbackMap.get(session.getId()));
				msg.setProxy(pingHelper);
				sendMsg(msg);
				TafLoggerCenter.info(connRouter.proxy.taf_proxyName() + " send " + connRouter.proxyConfig.getObjectName()
						+ " ping to " + session);
			} catch (Exception e) {
				TafLoggerCenter.info(connRouter.proxy.taf_proxyName() + " send " + connRouter.proxyConfig.getObjectName()
						+ " ping to " + session + " failed", e);
				pingCallbackMap.get(session.getId()).addDeadCount();
			}
		}
	}

	class PingFPrxCallbackImpl extends PingFPrxCallback {
		IoSession session;

		PingFPrxCallbackImpl(IoSession session) {
			this.session = session;
		}

		int deadCount = 0;

		int addDeadCount() {
			deadCount++;
			if (deadCount >= 3) {
				session.close();
				TafLoggerCenter.info("ping failed too many , close session " + session);
			}
			return deadCount;
		}

		@Override
		public void callback_tafPing() {
			deadCount = 0;
		}

		@Override
		public void callback_tafPing_exception(int ret) {
			deadCount++;
			TafLoggerCenter.info("ping failed " + deadCount + " , close session " + session);
			if (deadCount >= 3) {
				session.close();
				TafLoggerCenter.info("ping failed too many , close session " + session);
			}
		}

	}

	/**
	 * 是否可以发送 有连接或者失败时间到现在已经过了重连间隔 返回true
	 */
	public boolean hasConnToSend() {
		if (connecting.get() && TimerProvider.getNow() - connTime > maxConnectingTime) {
			connecting.set(false);
			TafLoggerCenter.info(info+" connecting.set(false) by hasConnToSend maxConnectingTime "+maxConnectingTime);
		}
		if (!hasConn.get() && TimerProvider.getNow() - failedTimeMill.get() > reConnTimeInterv) {
			TafLoggerCenter.info(this + " " + info.toString() + " waitTime is over try again "
					+ (TimerProvider.getNow() - failedTimeMill.get()) + " conning:" + connecting.get() + " "
					+ aliveSessions.size());
			hasConn.set(true);
		}
		return hasConn.get();
	}

	/**
	 * 连接状态最大保持时间 
	 */
	int maxConnectingTime = 10000 ;
	
	Object checkEndPointLock = new Object();

	/**
	 * 检测现有endPoint的数目
	 */
	protected void checkEndPointSize() {
		try {
			synchronized( checkEndPointLock ) {
				if (connecting.get() && TimerProvider.getNow() - connTime > maxConnectingTime) {
					connecting.set(false);
					TafLoggerCenter.info(this+" "+info.objectName+" "+info+" connecting.set(false) by checkEndPointSize maxConnectingTime "+maxConnectingTime);
				}
				if (!connecting.get()) {
					TafLoggerCenter.info(this+" "+info.objectName+" "+info+" connecting.set(true); by checkEndPointSize");
					connecting.set(true);
				}
			}
			if (connecting.get()) {
				connTime = TimerProvider.getNow();
				int openSize = 0 ;
				List<IoSession> closeSessions = new ArrayList<IoSession>();
				sessionListLock.writeLock().lock();
				try {
					//将所有不在连接状态的session清除
					for (int i = aliveSessions.size(); --i >= 0;) {
						if (!info.getProtocol().equals("udp") && !aliveSessions.get(i).isConnected() ) {
							TafLoggerCenter.info(this+" "+info.objectName+" "+info+" session "+aliveSessions.get(i) +" closed , remove it");
							aliveSessions.remove(i);
						}
					}
					TafLoggerCenter.info(this+" "+info.objectName+" "+info+" aliveSessions size: "+aliveSessions.size());
					if ( info.getAllowConnNum() == 0  ) {
						info.setAllowConnNum(1)  ;
					}
					if (aliveSessions.size() < info.getAllowConnNum()) {
						openSize = info.getAllowConnNum() - aliveSessions.size();
					} else if (aliveSessions.size() > info.getAllowConnNum()) {
						while (aliveSessions.size() > info.getAllowConnNum()) {
							IoSession session = aliveSessions.remove(0);
							closeSessions.add(session);
						}
					}
				} finally {
					sessionListLock.writeLock().unlock();
				}
				if ( openSize > 0 ) {
					for (int i = 0; i < openSize; i++) {
						connRouter.adapter.openConn(info, this);
						TafLoggerCenter.info(this+" EndPoint " +this.hashCode()+" "+ info.getObjectName() + " " + info + " try to open conn ");
					}
				} 
				for ( IoSession session : closeSessions ) {
					try {
						TafLoggerCenter.info(this+" EndPoint " + info.getObjectName() + " " + info + " try to close conn "
								+ session);
						session.close();
					} catch (Exception e) {
						TafLoggerCenter.info("checkEndPointSize close session error", e);;
					}
				}
			}
		} finally {
			if ( aliveSessions.size() > 0 ) {
				//通知router此endPoint还可用
				connRouter.onSessionOpen(this);
			} else {
				connRouter.onAllSessoinClose(this);
			}
		}

	}

	/**
	 * 当受到远程回应的时候触发 取出future
	 * 
	 * @param seq
	 * @return
	 */
	protected ServantFuture removeWaitFuture(int seq) {
		ServantFuture future = FutureCache.remove(seq);
		if (null != future && !future.isAsync()) { // 异步信息发送后就释放了信号量
			releaseAvailable();
		}
		return future;
	}

	/**
	 * 释放信号量
	 */
	private void releaseAvailable() {
		if (null != available) {
			available.release();
		}
	}

	protected ServantFuture getWaitFuture(int seq) {
		return FutureCache.get(seq);
	}

	/**
	 * 客户端过载保护机制 防止异步调用情况下冲击服务器端
	 * 
	 * @return
	 * @throws InterruptedException
	 */
	protected boolean isAllowSend() throws InterruptedException {
		if (null == available) {
			return true;
		}
		return available.tryAcquire((info.getMinTimeoutMill()) / 2, TimeUnit.MILLISECONDS);
	}

	/**
	 * 取到一个可用连接
	 * 
	 * @return
	 */
	protected IoSession getNextEndPoint() {
		sessionListLock.readLock().lock();
		try {
			if (aliveSessions.size() > 0) {
				if (sessionsAccessPoint.get() == Integer.MAX_VALUE) {
					sessionsAccessPoint.set(Integer.MAX_VALUE % aliveSessions.size());
				}
				return aliveSessions.get(sessionsAccessPoint.incrementAndGet() % aliveSessions.size());
			}
		} finally {
			sessionListLock.readLock().unlock();
		}
		return null;
	}

	/**
	 * 取到一个可用连接 如果没有 则等待0.5秒 如果仍无可用连接 抛出TafNotSendException
	 * 如果有 则判断是否处于连接状态或者超过了最大存活时间 如果符合 则关闭连接重新获取
	 * @param msg
	 * @return
	 * @throws TafNotSendException
	 */
	protected IoSession getAliveEndPoint(SendMessage msg) throws TafNotSendException {
		if (aliveSessions.size() == 0) {
			checkEndPointSize();
			long start = System.currentTimeMillis();
			synchronized (sessionOpenLock) {
				try {
					sessionOpenLock.wait(500);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			TafLoggerCenter.info(info.objectName + " getAliveEndPoint try500 sleep "
					+ (System.currentTimeMillis() - start));
		}
		IoSession session = getNextEndPoint();
		if (null == session) {
			hasConn.set(false);
			failedTimeMill.set(TimerProvider.getNow());
			throw new TafNotSendException(info, msg);
		} else {
			long sessionIdle = System.currentTimeMillis() - session.getLastIoTime() ;
			if ( !info.getProtocol().equals("udp") && !session.isConnected() || sessionIdle > connRouter.proxyConfig.getIdleStatusInteval()  ) {
				TafLoggerCenter.info("find too old session "+session+" sessionIdle "+sessionIdle+", closed.");
				closeSession(session);
				if(connecting.compareAndSet(true, false)) {
					TafLoggerCenter.info(info+" connecting.set(false) by getAliveEndPoint closeSession");
				}
				return getAliveEndPoint(msg);
			}
		}
		return session;
	}

	/**
	 * 发送信息 如果信息未能发送 则抛出异常TafNotSendException 交给上一层处理
	 * 
	 * @param request
	 * @param future
	 * @param needReturn
	 */
	protected void sendMsg(SendMessage msg) throws TafNotSendException {
		msg.getFuture().setRemoteHost(info.remoteHost);
		msg.getFuture().setRemotePort(info.getRemotePort());
		msg.getFuture().setEndPointKey(getInfo().getKey());
		
		StatMicMsgHead head = TafUtils.getHead(connRouter.processName, msg.getRequestPacket().sServantName, msg
				.getRequestPacket().sFuncName, "", info.remoteHost, info.getRemotePort(), 0);
		ProxyStatTool.Instance.addStatTool(connRouter.proxy);
		
		ProxyStatTool.Instance.addSample(connRouter.proxy, info.remoteHost, connRouter.processName, msg);
		
		IoSession session = getAliveEndPoint(msg);
		JceOutputStream requestOut = new JceOutputStream(0);
		msg.getRequestPacket().writeTo(requestOut);

		byte[] buffer = JceUtil.getJceBufArray(requestOut.getByteBuffer());
		JceMessage jceMessage = JceMessage.createJceMessage(true, msg.getFuture().getSeq(), buffer);
		// msg.getFuture().setAllowWaiteTime(config.getTimeoutSecond());
		try {
			if (isAllowSend()) {
				if (msg.isNeedReturn()) { // 需要返回 先在waitMap里注册
					// 有时候会出现回包比注册快的情况
					FutureCache.put(msg.getFuture().getSeq(), msg.getFuture(), handler);
				}
				msg.getFuture().setBindSessionId(session.getId());
				WriteFuture writeFuture = session.write(jceMessage);
				LoggerFilter.logCareServerMsg(jceMessage, info.getObjectName(), info.getKey(), true);
				try {
					writeFuture.await(info.getMinTimeoutMill());
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				if (writeFuture.isWritten()) { // 发送成功
					if (!msg.isNeedReturn()) {
						long costTime = TimerProvider.getNow() - msg.getFuture().getStartTime();
						onCallFinished(head,SendMsgStatBody.CallStatusSucc,costTime,"sendMsg", true);
					}
					if (!msg.isNeedReturn() || msg.getFuture().isAsync()) {
						releaseAvailable();// 不需要返回或者是异步信息 立即释放信号量
						msg.getFuture().setResult(new Object()); // 不需要返回
						// 通知等待的future
						// 现在是异步消息也要等发送成功后通知才能发下一个 所以也要setResult
					}
				} else { // 发送失败
					releaseAvailable(); // 释放信号量 允许下一个信息发送
					if (msg.isNeedReturn()) { // 需要返回 在waitMap里注销
						removeWaitFuture(msg.getFuture().getSeq());
					}
					long costTime = TimerProvider.getNow() - msg.getFuture().getStartTime();
					onCallFinished(head,SendMsgStatBody.CallStatusTimeout,costTime,"sendMsg", false);
					throw new TafNotSendException(info, msg);
				}
			}
		} catch (TafNotSendException e) {
			long costTime = TimerProvider.getNow() - msg.getFuture().getStartTime();
			connRouter.getStatmsg().get(head).onCallFinished(costTime, SendMsgStatBody.CallStatusExec);
			throw e;
		} catch (Exception ex) {
			long costTime = TimerProvider.getNow() - msg.getFuture().getStartTime();
			connRouter.getStatmsg().get(head).onCallFinished(costTime, SendMsgStatBody.CallStatusExec);
			throw new TafProxyException("thread is Interrupted " + ex, ex);
		}

	}

	
	/**
	 * 收到了service回应信息
	 */
	public void onMessageReceived(IoSession session, Object message) throws Exception {
		JceMessage jceMsg = (JceMessage) message;
		LoggerFilter.logCareServerMsg(jceMsg, connRouter.objectName, getKey(), false);
		ServantFuture future = removeWaitFuture(jceMsg.getSeq());
		if (null != future) {
			// 记录调用信息
			future.onNetCallFinished();
			StatMicMsgHead head = TafUtils.getHead(connRouter.processName, future.getSServantName(), future
					.getSFuncName(), "", getRemoteHost(), getRemotePort(), 0);
			
			int callStatus = SendMsgStatBody.CallStatusExec;
			if ( jceMsg.getResponse().iRet == JCESERVERSUCCESS.value ) {
				callStatus = SendMsgStatBody.CallStatusSucc;
			}
			onCallFinished(head, callStatus, future.getCostTime(),"onMessageReceived", false);
			if (jceMsg.getResponse().iRet == JCESERVERRESETGRID.value) {
				if (jceMsg.getResponse().status.containsKey(Const.STATUS_GRID_CODE)) {
					int newGridValue = Integer.parseInt(jceMsg.getResponse().status.get(Const.STATUS_GRID_CODE));
					TafLoggerCenter.info("reset grid " + this.getInfo().getGrid() + " newGridValue:" + newGridValue);
					connRouter.onResetGrid(this, newGridValue);
				}
			}
			if (jceMsg.getResponse().iRet != JCESERVERSUCCESS.value) {
				TafLoggerCenter.error("onMessageReceived:" + future.getSServantName() + "." + future.getSFuncName()
						+ " return " + jceMsg.getResponse().iRet);
				String errorMsg =  future.getSServantName() + "."
				+ future.getSFuncName();
				TafException e = TafException.makeException(jceMsg.getResponse().iRet,errorMsg);
				future.setFailure(e);
			} else {
				future.setResult(jceMsg.getResponse());
			}
			if (future.isAsync()) {
				jceMsg.setFuture(future);
				connRouter.proxyConfig.getTafThreadPool().receiveMsg(jceMsg);
			}
		} else {
			TafLoggerCenter.error(getKey() + " can not find future " + jceMsg.getSeq());
		}

	}

	/**
	 * 将连接数减为0 再从adapter的listener中去掉
	 */
	public void closeAll() {
		TafLoggerCenter.info("call closeall "+info);
		sessionListLock.writeLock().lock();
		try {
			//将所有不在连接状态的session清除
			for (int i = aliveSessions.size(); --i >= 0;) {
				if (!info.getProtocol().equals("udp") && !aliveSessions.get(i).isConnected() ) {
					TafLoggerCenter.info(info.objectName+" "+info+" session "+aliveSessions.get(i) +" closed , remove it");
					aliveSessions.remove(i);
				}
			}
			TafLoggerCenter.info(this+" "+info.objectName+" "+info+" aliveSessions size: "+aliveSessions.size());
			for ( IoSession session : aliveSessions ) {
				try {
					TafLoggerCenter.info("EndPoint " + info.getObjectName() + " " + info + " try to close conn "
							+ session);
					session.close();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		} finally {
			sessionListLock.writeLock().unlock();
		}
	}

	/**
	 * 关闭一个特定的连接
	 * 
	 * @param session
	 */
	public void closeSession(IoSession session) {
		try {
			session.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		int removed = 0 ;
		sessionListLock.writeLock().lock();
		try {
			for (int i = aliveSessions.size(); --i >= 0;) {
				TafLoggerCenter.info(this+" local session "+aliveSessions.get(i).toString());
				TafLoggerCenter.info(this+" closed session "+session.toString());
				if (aliveSessions.get(i).toString().equals(session.toString())) {
					aliveSessions.remove(i);
					removed++;
				}
			}
		} finally {
			sessionListLock.writeLock().unlock();
		}
		connRouter.onSessionClose(this);
		TafLoggerCenter.info(this+" EndPoint " + session + " closed , now size " + aliveSessions.size()+" removed:"+removed);
		if (aliveSessions.size() == 0) {
			// 如果一个连接都没有了 设置失败时间
			hasConn.set(false);
			failedTimeMill.set(TimerProvider.getNow());
			connRouter.onAllSessoinClose(this);
		}
		pingCallbackMap.remove(session.getId());
	}

	public ServiceEndPointInfo getInfo() {
		return info;
	}

	public String getRemoteHost() {
		return info.getRemoteHost();
	}

	public int getRemotePort() {
		return info.getRemotePort();
	}

	public String getProtocol() {
		return info.getProtocol();
	}

	/**
	 * 连接发生异常
	 */
	public void onExceptionCaught(IoSession session, Throwable cause) throws Exception {
		TafLoggerCenter.info(this+" client session exception " + session + " " + cause);
		//有时候onSessionClosed没有被触发
		closeSession(session);
	}

	
	/**
	 * 当连接关闭的时候触发
	 */
	public void onSessionClosed(IoSession session) throws Exception {
		TafLoggerCenter.info(this+" sessionClose "+session+" id:"+session.getId());
		try {
			Iterator<Entry<Integer, ServantFuture>> key = FutureCache.getIterator();
			ArrayList<Integer> removeFuture = new ArrayList<Integer>();
			while (key.hasNext()) {
				int seq = key.next().getKey();
				ServantFuture future = FutureCache.get(seq);
				// 找到绑定在这个session上的future
				if (null != future && future.getBindSessionId() == session.getId()) {
					removeFuture.add(seq);
				}
			}
			for (Integer seq : removeFuture) {
				ServantFuture future = FutureCache.get(seq);
				if ( null != future) {
					TafLoggerCenter.info(this+" remove future "+future.getSeq()+":"+future.getSServantName()+"."+future.getSFuncName()+" by "+session+" close");
					future.onNetCallFinished();
					removeWaitFuture(seq);
					StatMicMsgHead head = TafUtils.getHead(connRouter.processName, future.getSServantName(), future
							.getSFuncName(), "", getRemoteHost(), getRemotePort(), 0);
					onCallFinished(head, SendMsgStatBody.CallStatusExec, future.getCostTime(),"onSessionClosed", false);
					future.setFailure(new TafProxyNoConnException("conn is closed by server " + connRouter.objectName));
					if (null != future.getHandler()) {
						connRouter.createErrRespDispatch(future, JCEPROXYCONNECTERR.value);
					}
				}
			}
		} finally {
			closeSession(session);
		}
	}

	/**
	 * 当连接开启的时候触发
	 */
	public void onSessionOpened(IoSession session) throws Exception {
		TafLoggerCenter.info(this+" "+info+" connecting.set(false) by onSessionOpened;");
		connecting.set(false);
		hasConn.set(true);
		boolean conn = false;
		if (connRouter.proxyConfig.getReadBufferSize() > 0) {
			((SocketSessionConfig) session.getConfig())
					.setReceiveBufferSize(connRouter.proxyConfig.getReadBufferSize());
		}
		if (info.protocol.equalsIgnoreCase("tcp")) {
			if (connRouter.proxyConfig.getWriteBufferSize() > 0) {
				((SocketSessionConfig) session.getConfig()).setSendBufferSize(connRouter.proxyConfig
						.getWriteBufferSize());
			}
		} else if (info.protocol.equalsIgnoreCase("udp")) {
			if (connRouter.proxyConfig.getWriteBufferSize() > 0) {
				((DatagramSessionConfig) session.getConfig()).setSendBufferSize(connRouter.proxyConfig
						.getWriteBufferSize());
			}
		}
		sessionListLock.writeLock().lock();
		try {
			if (aliveSessions.size() > info.getAllowConnNum()) {
				
			} else {
				aliveSessions.add(session);
				conn = true;
			}
		} finally {
			sessionListLock.writeLock().unlock();
		}
		// 已经有连接了 重置重连间隔时间
		if (conn) {
			PingFPrxCallbackImpl impl = new PingFPrxCallbackImpl(session);
			pingCallbackMap.put(session.getId(), impl);
			TafLoggerCenter.info(this+" EndPoint " + session + " added for " + info.getObjectName() + " , now size "
					+ aliveSessions.size());
			resetReConnTimeInterv();
			synchronized (sessionOpenLock) {
				sessionOpenLock.notifyAll();
			}
			connRouter.onSessionOpen(this);
		} else {
			TafLoggerCenter.info(this+" now size " + aliveSessions.size() + " sessions , allow " + info.getAllowConnNum());
			session.close();
		}
	}

	/**
	 * 当消息发出去后触发
	 */
	public void onMessageSent(IoSession session, Object message) throws Exception {
		//System.out.println("send msg by "+session);
	}
	
	AtomicBoolean lastCallSucc = new AtomicBoolean();
	AtomicLong continueFail = new AtomicLong();
	AtomicLong timeoutCount = new AtomicLong();
	AtomicLong failedCount = new AtomicLong();
	AtomicLong succCount = new AtomicLong();
	
	/**
	 * 当一个连接接收service回应后或者发送信息超时后调用
	 * 用于切换的统计超时失败统计
	 * @param head
	 * @param callStatus
	 * @param costTime
	 * @isOneway 是否是单向调用，如果是，则不计入客户端统计
	 */
	public void onCallFinished( StatMicMsgHead head , int callStatus ,  long costTime , String eventSrc, boolean isOneWay ) {
		if(!isOneWay) {
			ProxyStatTool.Instance.getStatTool(connRouter.proxy).addInvokeTime(head, costTime, callStatus);
		}
		
		if ( callStatus == SendMsgStatBody.CallStatusSucc ) {
			continueFail.set(0);
			lastCallSucc.set(true);
			succCount.incrementAndGet();
		} else if ( callStatus == SendMsgStatBody.CallStatusTimeout ) {
			if ( !lastCallSucc.get() ) {
				continueFail.incrementAndGet();
			} else {
				lastCallSucc.set(false);
			}
			timeoutCount.incrementAndGet();
		} else if ( callStatus == SendMsgStatBody.CallStatusExec ) {
			if ( !lastCallSucc.get() ) {
				continueFail.incrementAndGet();
			} else {
				lastCallSucc.set(false);
			}
			failedCount.incrementAndGet();
		}
	}
	
	/**
	 * 检测连接当前调用成功率
	 */
	public void checkConnStat() {
		long timeout = timeoutCount.get();
		timeoutCount.set(0);
		long conFail = continueFail.get();
		continueFail.set(0);
		long fail = failedCount.get();
		failedCount.set(0);
		long succ = succCount.get();
		succCount.set(0);
		doZeroStat();
		long totalCount = timeout+fail+succ;
		if ( totalCount > connRouter.getServantProxy().taf_min_timeout_invoke() ) {
			if ( conFail > connRouter.getServantProxy().taf_frequence_fail_invoke()  ) {
				closeAll();
				connRouter.onAllSessoinClose(this);
				TafLoggerCenter.info(this+" "+info.toFullString()+" >>>>continueFail "+conFail+ " closed.");
			} else {
				double radio = TafUtils.div(timeout, totalCount, 2) ;
				//////////////////////////
				if ( radio > connRouter.getServantProxy().taf_timeout_radio()  ) {
					closeAll();
					connRouter.onAllSessoinClose(this);
					TafLoggerCenter.info(this+" "+info.toFullString()+" >>>>callFail radio "+radio+ " closed.");
				}
			}
			
		}
		
		//ping();
	}
	
	/**
	 * 状态计数器清零
	 */
	public void doZeroStat() {
		continueFail.set(0);
		timeoutCount.set(0);
		failedCount.set(0);
		succCount.set(0);
	}
	
	public String getListenKey() {
		return getKey();
	}

	public void onSessionOpenedFailed() {
		TafLoggerCenter.info(this+" "+info+" connecting.set(false) by onSessionOpenedFailed;");
		connecting.set(false);
		synchronized (sessionOpenLock) {
			sessionOpenLock.notifyAll();
		}
	}

	public String getKey() {
		return key;
	}

	public long getReConnTimeInterv() {
		return reConnTimeInterv;
	}

	public void setReConnTimeInterv(long reConnTimeInterv) {
		this.reConnTimeInterv = reConnTimeInterv;
	}

	public void resetReConnTimeInterv() {
		reConnTimeInterv = deReConnTimeInterv;
	}
}
