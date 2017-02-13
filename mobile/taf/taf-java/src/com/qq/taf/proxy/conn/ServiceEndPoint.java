package com.qq.taf.proxy.conn;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.Map.Entry;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

import com.qq.navi.Session;
import com.qq.sim.Millis100TimeProvider;
import com.qq.taf.StatMicMsgHead;
import com.qq.taf.cnst.Const;
import com.qq.taf.cnst.JCEPROXYCONNECTERR;
import com.qq.taf.cnst.JCESERVERRESETGRID;
import com.qq.taf.cnst.JCESERVERSUCCESS;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.ProxyStatTool;
import com.qq.taf.proxy.SendMessage;
import com.qq.taf.proxy.SendMsgStatBody;
import com.qq.taf.proxy.ServantFuture;
import com.qq.taf.proxy.TafLoggerCenter;
import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.proxy.exec.TafException;
import com.qq.taf.proxy.exec.TafNotSendException;
import com.qq.taf.proxy.exec.TafProxyNoConnException;
import com.qq.taf.proxy.utils.TafUtils;
import com.qq.taf.proxy.utils.TimeoutHandler;

/**
 * 一个后端到一个service的全部网络连接
 * 
 * @author albertzhu
 * 
 */
public class ServiceEndPoint implements EndPointEventListen {

	volatile Session aliveSession = new Session();
	private Semaphore available;
	private ServiceEndPointInfo info;
	private String key;
	private ProxyConnRouter connRouter;
	private TimeoutHandler<Integer, ServantFuture> handler;
	private AtomicBoolean doingConnect;
	
	public ServiceEndPoint(ProxyConnRouter connRouter, ServiceEndPointInfo info,
			TimeoutHandler<Integer, ServantFuture> handler) {
		this.connRouter = connRouter;
		this.info = info;
		key = ServiceEndPointInfo.getSessionKey(getProtocol(), getRemoteHost(), getRemotePort());
		if (connRouter.proxyConfig.getAvailableKey() > 0) {
			available = new Semaphore(connRouter.proxyConfig.getAvailableKey());
		}
		doingConnect = new AtomicBoolean(false);
		this.handler = handler;
	}

	/**
	 * 检测现有endPoint的数目
	 */
	protected void checkEndPointSize() {
		getNextEndPoint();
		if (aliveSession.isOpen()) {
			//通知router此endPoint还可用
			connRouter.onSessionOpen(this);
		} else {
			connRouter.onAllSessoinClose(this);
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
	 * @throws Throwable 
	 */
	protected Session getNextEndPoint() {
		if (aliveSession.isOpen()) {
			return aliveSession;
		}
		else {
			if(doingConnect.compareAndSet(false, true)) {
				//无正在创建的连接，就异步创建连接，且等待
				connRouter.adapter.openConn(info, this);
				TafLoggerCenter.info(this+" getNextEndPoint wait 500");
				synchronized(doingConnect) {
					try {
						doingConnect.wait(500);
					}
					catch(InterruptedException ex) {
						TafLoggerCenter.error(this+" getNextEndPoint wait was Interrupted in "+this);
					}
				}
			}
			else {
				if(aliveSession.isOpen()) {
					return aliveSession;
				}
				else {
					//已经有正在创建的连接,就不创建，只是等待
					TafLoggerCenter.info(this+" getNextEndPoint wait 500 for others is connecting");
					synchronized(doingConnect) {
						try {
							doingConnect.wait(500);
						}
						catch(InterruptedException ex) {
							TafLoggerCenter.error(this+" getNextEndPoint wait was Interrupted in "+this);
						}
					}
					return aliveSession;
				}
			}
			return aliveSession;
		}
	}

	/**
	 * 取到一个可用连接 如果没有 则等待0.5秒 如果仍无可用连接 抛出TafNotSendException
	 * 如果有 则判断是否处于连接状态或者超过了最大存活时间 如果符合 则关闭连接重新获取
	 * @param msg
	 * @return
	 * @throws TafNotSendException
	 */
	protected Session getAliveEndPoint(SendMessage msg) throws TafNotSendException {
		Session session = getNextEndPoint();
		if (null == session || !session.isOpen()) {
			throw new TafNotSendException(info, msg);
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
				.getRequestPacket().sFuncName, "", info.remoteHost, info.getRemotePort(), 0, connRouter.proxyConfig.getSetArea(), connRouter.proxyConfig.getSetId());
		ProxyStatTool.Instance.addStatTool(connRouter.proxy);
		
		ProxyStatTool.Instance.addSample(connRouter.proxy, info.remoteHost, connRouter.processName, msg);
		
		Session session = getAliveEndPoint(msg);
		JceOutputStream requestOut = new JceOutputStream(0);
		msg.getRequestPacket().writeTo(requestOut);

		byte[] buffer = JceUtil.getJceBufArray(requestOut.getByteBuffer());
		JceMessage jceMessage = JceMessage.createJceMessage(true, msg.getFuture().getSeq(), buffer);
		// msg.getFuture().setAllowWaiteTime(config.getTimeoutSecond());
		
		if (msg.isNeedReturn()) { // 需要返回 先在waitMap里注册
			// 有时候会出现回包比注册快的情况
			FutureCache.put(msg.getFuture().getSeq(), msg.getFuture(), handler);
		}
		msg.getFuture().setBindSessionId(session.getID());
		session.write(jceMessage);
		if (!msg.isNeedReturn()) {
			long costTime = Millis100TimeProvider.INSTANCE.currentTimeMillis() - msg.getFuture().getStartTime();
			onCallFinished(head,SendMsgStatBody.CallStatusSucc,costTime,"sendMsg", true);
		}
	}

	
	/**
	 * 收到了service回应信息
	 */
	public void onMessageReceived(Session session, Object message) throws Exception {
		JceMessage jceMsg = (JceMessage) message;
		ServantFuture future = removeWaitFuture(jceMsg.getSeq());
		if (null != future) {
			// 记录调用信息
			future.onNetCallFinished();
			StatMicMsgHead head = TafUtils.getHead(connRouter.processName, future.getSServantName(), future
					.getSFuncName(), "", getRemoteHost(), getRemotePort(), 0, connRouter.proxyConfig.getSetArea(), connRouter.proxyConfig.getSetId());
			
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
				String errorMsg =  future.getSServantName() + "." + future.getSFuncName();
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
		try {
			TafLoggerCenter.info(this+" call close all, try to close conn ");
			if(aliveSession.isOpen()) {//有可能只是new Session，而不是Adapter创建出来的
				aliveSession.close(true);
			}
		} catch (Exception e) {
			TafLoggerCenter.error(this+" closeAll session error", e);
		}
	}

	/**
	 * 关闭一个特定的连接
	 * 
	 * @param session
	 */
	public void closeSession(Session session) {
		int removed = 0 ;
		if(session == aliveSession) {
			removed = 1;
			aliveSession.close(false);
		}
		else {
			TafLoggerCenter.error(this+" close session but not aliveSession");
		}
		//sessioin关闭时不需要把router更新，可能是idle超时导致关闭
		//connRouter.onSessionClose(this);
		TafLoggerCenter.info(this+" closeSession, now size 1 removed:"+removed);
//		if (!aliveSession.isOpen()) {
//			// 如果一个连接都没有了 设置失败时间
//			connRouter.onAllSessoinClose(this);
//		}
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
	public void onExceptionCaught(Session session, Throwable cause) throws Exception {
		TafLoggerCenter.error(this+" client session exception ", cause);
	}

	
	/**
	 * 当连接关闭的时候触发
	 */
	public void onSessionClosed(Session session) throws Exception {
		TafLoggerCenter.info(this+" call onSessionClose");
		try {
			Iterator<Entry<Integer, ServantFuture>> key = FutureCache.getIterator();
			ArrayList<Integer> removeFuture = new ArrayList<Integer>();
			while (key.hasNext()) {
				int seq = key.next().getKey();
				ServantFuture future = FutureCache.get(seq);
				// 找到绑定在这个session上的future
				if (null != future && future.getBindSessionId() == session.getID()) {
					removeFuture.add(seq);
				}
			}
			for (Integer seq : removeFuture) {
				ServantFuture future = FutureCache.get(seq);
				if ( null != future) {
					TafLoggerCenter.error(this+" remove future "+future.getSeq()+":"+future.getSServantName()+"."+future.getSFuncName());
					future.onNetCallFinished();
					removeWaitFuture(seq);
					StatMicMsgHead head = TafUtils.getHead(connRouter.processName, future.getSServantName(), future
							.getSFuncName(), "", getRemoteHost(), getRemotePort(), 0, connRouter.proxyConfig.getSetArea(), connRouter.proxyConfig.getSetId());
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
	public void onSessionOpened(Session session) throws Exception {
		aliveSession = session;
		TafLoggerCenter.info(this+" call onSessionOpened");
		// 已经有连接了 重置重连间隔时间
		doingConnect.set(false);
		synchronized (doingConnect) {
			doingConnect.notifyAll();
		}
		connRouter.onSessionOpen(this);
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
	 */
	public void onCallFinished( StatMicMsgHead head , int callStatus ,  long costTime , String eventSrc,boolean isOneWay ) {
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
				TafLoggerCenter.error(this+" "+info.toFullString()+" >>>>continueFail ratio "+conFail+" closed.");
			} else {
				double radio = TafUtils.div(timeout, totalCount, 2) ;
				//////////////////////////
				if ( radio > connRouter.getServantProxy().taf_timeout_radio()  ) {
					closeAll();
					connRouter.onAllSessoinClose(this);
					TafLoggerCenter.error(this+" "+info.toFullString()+" >>>>callFail ratio "+radio+ " closed.");
				}
			}
			
		}
	}
	
	public String toString() {
		return "EndPoint(ID="+aliveSession.getID()+" "+info+") ";
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
		TafLoggerCenter.error(this+" open session Failed");
		connRouter.onSessionClose(this);//如果连接失败，需要剔除节点
		synchronized (doingConnect) {
			doingConnect.notifyAll();
		}
		doingConnect.set(false);
	}

	public String getKey() {
		return key;
	}

//	public long getReConnTimeInterv() {
//		return reConnTimeInterv;
//	}
//
//	public void setReConnTimeInterv(long reConnTimeInterv) {
//		this.reConnTimeInterv = reConnTimeInterv;
//	}
//
//	public void resetReConnTimeInterv() {
//		reConnTimeInterv = deReConnTimeInterv;
//	}
}
