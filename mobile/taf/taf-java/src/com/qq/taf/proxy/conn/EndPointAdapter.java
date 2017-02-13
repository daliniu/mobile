package com.qq.taf.proxy.conn;

import java.net.InetSocketAddress;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.qq.navi.Connector;
import com.qq.navi.Handler;
import com.qq.navi.Session;
import com.qq.navi.SessionIdGenerator;
import com.qq.navi.SessionSynchronizer;
import com.qq.navi.udp.UDPConnector;
import com.qq.taf.proxy.TafLoggerCenter;
import com.qq.taf.proxy.codec.JceCodec;

/**
 * 网络连接接口 
 * 根据请求打开连接并转发IoSession事件
 * 
 * @author albertzhu
 * 
 */
public class EndPointAdapter extends Handler {

	private ConcurrentLinkedQueue<ServiceEndPointInfo> connRequestQueue = new ConcurrentLinkedQueue<ServiceEndPointInfo>();
	private boolean running = true;
	private Connector socketConnector ;
	private UDPConnector datagramConnector ;
	private SessionSynchronizer sessionSynchronizer;

	public EndPointAdapter() {
		JceCodec jceCodec=new JceCodec(true);
		socketConnector = new Connector("TafTcpClient",jceCodec,this,0,0);
		datagramConnector = new UDPConnector("TafUdpClient",jceCodec,this,0,0);
		SessionIdGenerator sessionIdGenerator=new SessionIdGenerator();
		socketConnector.setSessionIdGenerator(sessionIdGenerator);
		datagramConnector.setSessionIdGenerator(sessionIdGenerator);
		sessionSynchronizer=new SessionSynchronizer(3000,512);
		Thread consumer = new Thread(new ConsumerRequestWorker(),"ConsumerRequestWorker");
		consumer.start();
	}


	/**
	 * 开启连接 非阻塞方法 立即返回
	 * 
	 * @param connReq
	 */
	public void openConn(ServiceEndPointInfo connReq , EndPointEventListen listener ) {
		connReq.listener = listener;
		synchronized (connRequestQueue) {
			connRequestQueue.add(connReq);
			connRequestQueue.notifyAll();
		}
	}
	
	public final static String listen_session_key = "taf_listen";
	
	class ConsumerRequestWorker implements Runnable {
		public void run() {
			while (running) {
				synchronized (connRequestQueue) {
					while (connRequestQueue.isEmpty()) {
						try {
							connRequestQueue.wait();
						} catch (InterruptedException e) {
							return;
						}
					}
				}
				ServiceEndPointInfo request = null;
				while (running && !connRequestQueue.isEmpty()) {
					try {
						request = connRequestQueue.poll();
						int sessionId;
						if (request.protocol.equals("tcp")) {
							sessionId = socketConnector.connect(new InetSocketAddress(request.remoteHost, request.remotePort), -1);//request.getSyncInvokeTimeout()
						} else if (request.protocol.equals("udp")){
							sessionId = datagramConnector.connect(new InetSocketAddress(request.remoteHost, request.remotePort),-1);
						} else {
							throw new RuntimeException("unknown protocol "+request.protocol);
						}
						Session session=sessionSynchronizer.get(sessionId);
						if(session!=null) {
							session.setAttribute(listen_session_key, request.listener);
							try {
								request.listener.onSessionOpened(session);
							} catch(Throwable t) {
								TafLoggerCenter.error("onSessionOpen error",t);
								request.listener.onSessionOpenedFailed();
							}
						} else {
							TafLoggerCenter.error("error , can not open conn for "+request+" ");
							request.listener.onSessionOpenedFailed();
						}
					}
					catch(Throwable th) {
						if(request !=null) {
							request.listener.onSessionOpenedFailed();
							TafLoggerCenter.error("can not open conn for "+request+" ", th);
						}
						else {
							TafLoggerCenter.error("can not open conn for error ", th);
						}
					}
				}
			}
		}
	}


	// 以下为继承IoHandlerAdapter的代码
	// //////////////////////////////////////////////
	@Override
	public void exceptionCaught(Session session, Throwable cause) throws Throwable {
		getListener(session).onExceptionCaught(session, cause);
		session.close(true);
	}

	@Override
	public void messageRecieved(Session session, Object message) throws Throwable {
		getListener(session).onMessageReceived(session, message);
	}

	@Override
	public void sessionClosed(Session session) throws Throwable {
		getListener(session).onSessionClosed(session);
	}

	@Override
	public void sessionOpened(Session session) throws Exception {
		sessionSynchronizer.put(session);
	}
	

	private EndPointEventListen getListener(Session session) {
		return (EndPointEventListen) session.getAttribute(listen_session_key);
	}
}
