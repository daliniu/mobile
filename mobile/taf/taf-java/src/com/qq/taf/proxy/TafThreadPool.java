package com.qq.taf.proxy;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;

import com.qq.taf.proxy.codec.JceMessage;
import com.qq.taf.proxy.utils.NameRunnable;

/**
 * taf线程池
 * 异步发送和回调的时候使用
 * @author albertzhu, fanzhang
 *
 */
public class TafThreadPool {
	
	private static final int MAX_QUEUE=500000;
	int asyncSendThreadNum ; // 异步发送的线程个数
	int asyncCallbackThread; // 异步回调的线程个数
	private ExecutorService asyncSendPool;
	private ExecutorService asyncCallBackPool;
	private LinkedBlockingQueue<SendMessage> sendQueue=new LinkedBlockingQueue<SendMessage>(MAX_QUEUE);
	private LinkedBlockingQueue<JceMessage> recvQueue=new LinkedBlockingQueue<JceMessage>(MAX_QUEUE);
	
	public TafThreadPool( int asyncSendThreadNum , int asyncCallbackThread ) {
		this.asyncSendThreadNum = asyncSendThreadNum;
		this.asyncCallbackThread = asyncCallbackThread;
		//先初始化回调线程池
		initCallbackPool();
		initSendPool();
	}

	public void sendAsyncMsg(SendMessage sm) {
		if(!sendQueue.offer(sm)) TafLoggerCenter.info("AsyncCall|send queue is full");
	} 
	
	public void receiveMsg( JceMessage msg ) {
		if(!recvQueue.offer(msg)) TafLoggerCenter.info("AsyncCall|recv queue is full");
	}
	
	//文档中说明回调处理一定要快 必要的时候加二级线程池
	private void initSendPool() {
		if ( null != asyncSendPool ) {
			asyncSendPool.shutdown();
		}
		asyncSendPool = Executors.newFixedThreadPool(asyncSendThreadNum);
		for (int i = 0; i < asyncSendThreadNum; i++) {
			String name = "aysncSendWorker"+ "_" + i;
			NameRunnable r = new NameRunnable(new SendWorker(),name);
			asyncSendPool.submit(r);
		}
	}
	
	private void initCallbackPool() {
		if ( null != asyncCallBackPool ) {
			asyncCallBackPool.shutdown();
		}
		asyncCallBackPool = Executors.newFixedThreadPool(asyncCallbackThread);
		for (int i = 0; i < asyncCallbackThread; i++) {
			String name = "aysncCallbackWorker"+ "_" + i;
			NameRunnable r = new NameRunnable(new CallbackWorker(),name);
			asyncCallBackPool.submit(r);
		}
	}
	

	/**
	 * 异步信息发送线程 从本地buffer中轮流取信息发送
	 */
	class SendWorker implements Runnable {
		public void run() {
			while(true) {
				try {
					SendMessage msg=sendQueue.take();
					msg.proxy.doSendMsg(msg);
				} catch(Throwable t) {
					TafLoggerCenter.info("AsyncCall|send error",t);
				}
			}
		}
	}
	
	/**
	 * 回调线程
	 */
	class CallbackWorker implements Runnable {
		public void run() {
			while(true) {
				try {
					JceMessage msg = recvQueue.take();
					if (null != msg.getFuture().getHandler() || null != msg.getFuture().getServantProxyCallback() ) { // 异步调用的回应
						String methodName = msg.getFuture().getSFuncName();
						ServantProxyCallback callback = msg.getFuture().getServantProxyCallback();
						if ( null != callback ) {
							ServantProxyThreadData.INSTANCE.setData(((ServantProxyCallback)callback)._data);
							callback._onDispatch(methodName, msg.getResponse());
						} else {
							CallbackHandler handler = msg.getFuture().getHandler();
							handler._onDispatch(methodName, msg.getResponse());
						}
					} else {
						// 异步回调信息居然没有设置callBackHandler
						TafLoggerCenter.info("AsyncCall|CallBackHander not found");
					}
				} catch(Throwable t) {
					TafLoggerCenter.info("AsyncCall|recv error",t);
				}
			}
		}
	}
	
}
