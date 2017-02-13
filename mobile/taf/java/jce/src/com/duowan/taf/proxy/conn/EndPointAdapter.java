package com.duowan.taf.proxy.conn;

import java.net.InetSocketAddress;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.duowan.netutil.nio.mina2.core.filterchain.DefaultIoFilterChainBuilder;
import com.duowan.netutil.nio.mina2.core.future.ConnectFuture;
import com.duowan.netutil.nio.mina2.core.future.IoFutureListener;
import com.duowan.netutil.nio.mina2.core.service.IoHandlerAdapter;
import com.duowan.netutil.nio.mina2.core.session.IdleStatus;
import com.duowan.netutil.nio.mina2.core.session.IoSession;
import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolCodecFilter;
import com.duowan.netutil.nio.mina2.fliter.executor.ExecutorFilter;
import com.duowan.netutil.nio.mina2.transport.socket.nio.NioDatagramConnector;
import com.duowan.netutil.nio.mina2.transport.socket.nio.NioSocketConnector;
import com.duowan.taf.proxy.TafLoggerCenter;
import com.duowan.taf.proxy.utils.NameThreadFactory;

/**
 * 网络连接接口 
 * 根据请求打开连接并转发IoSession事件
 * 
 * @author albertzhu
 * 
 */
public class EndPointAdapter extends IoHandlerAdapter {

	//private final ConcurrentHashMap<String, EndPointEventListen> listeners = new ConcurrentHashMap<String, EndPointEventListen>();
	ConcurrentLinkedQueue<ServiceEndPointInfo> connRequestQueue = new ConcurrentLinkedQueue<ServiceEndPointInfo>();
	boolean running = true;
	//默认的处理器个数
	private static final int defaultProcessorCount = Runtime.getRuntime().availableProcessors() + 1;
	NioSocketConnector socketConnector ;
	NioDatagramConnector datagramConnector ;
	ExecutorService threadPool = Executors.newCachedThreadPool(new NameThreadFactory("EndPointAdapter"));
	ExecutorFilter threadFliter = new ExecutorFilter(threadPool);

	
	

	public EndPointAdapter(int processorCount , ProtocolCodecFilter codecFliter) {
		socketConnector = new NioSocketConnector();
		datagramConnector = new NioDatagramConnector();
		
		DefaultIoFilterChainBuilder socketChain = socketConnector.getFilterChain();
		socketChain.addLast("codec", codecFliter);
		socketChain.addLast("threadPool", threadFliter);
		socketConnector.setHandler(this);

		DefaultIoFilterChainBuilder datagramChain = datagramConnector.getFilterChain();
		datagramChain.addLast("codec", codecFliter);
		datagramChain.addLast("threadPool", threadFliter);
		datagramConnector.setHandler(this);
		Thread consumer = new Thread(new ConsumerRequestWorker(),"ConsumerRequestWorker");
		consumer.start();
	}
	
	public EndPointAdapter(ProtocolCodecFilter codecFliter) {
		this(defaultProcessorCount,codecFliter);
	}

//	/**
//	 * Adds a new listener.
//	 */
//	public void add(EndPointEventListen listener) {
//		if ( listeners.containsKey(listener.getListenKey()) ) {
//			//LoggerCenter.info("also has ioEventListener for "+listener.getListenKey()+" in map");
//		} else {
//			listeners.put(listener.getListenKey(), listener);
//			LoggerCenter.info("add ioEventListener for "+listener.getListenKey()+" in map");
//		}
//	}
//
//	/**
//	 * Removes an existing listener.
//	 */
//	public void remove(EndPointEventListen listener) {
//		listeners.remove(listener.getListenKey());
//		LoggerCenter.info("remove ioEventListener for "+listener.getListenKey()+" in map");
//	}

	/**
	 * 开启连接 非阻塞方法 立即返回
	 * 
	 * @param connReq
	 */
	public void openConn(ServiceEndPointInfo connReq , EndPointEventListen listener ) {
//		add(listener);
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
				while (running && !connRequestQueue.isEmpty()) {
					final ServiceEndPointInfo request = connRequestQueue.poll();
					try {
						IoFutureListener<ConnectFuture> connFutureListener = new IoFutureListener<ConnectFuture>() {
							public void operationComplete(ConnectFuture future) {
								if ( future.isConnected() ) {
									future.getSession().setAttribute(listen_session_key, request.listener);
									try {
										request.listener.onSessionOpened(future.getSession());
									} catch (Exception e) {
										TafLoggerCenter.info("connect error "+request, e);
										request.listener.onSessionOpenedFailed();
									}
								} else {
									TafLoggerCenter.error("error , can not open conn for "+request+" "+future.getException(),future.getException());
									request.listener.onSessionOpenedFailed();
								}
							}
						};
						ConnectFuture future ;
						if (request.protocol.equals("tcp")) {
							future = socketConnector.connect(new InetSocketAddress(request.remoteHost, request.remotePort));
						} else if (request.protocol.equals("udp")){
							future = datagramConnector.connect(new InetSocketAddress(request.remoteHost, request.remotePort));
						} else {
							throw new RuntimeException("unknown protocol "+request.protocol);
						}
						future.addListener(connFutureListener);
					}
					catch(Throwable th) {
						TafLoggerCenter.error("error , can not open conn for "+request, th);
					}
				}
			}
		}
	}


	// 以下为继承IoHandlerAdapter的代码
	// //////////////////////////////////////////////
	@Override
	public void exceptionCaught(IoSession session, Throwable cause) throws Exception {
		getListener(session).onExceptionCaught(session, cause);
		session.close();
	}

	@Override
	public void messageReceived(IoSession session, Object message) throws Exception {
		getListener(session).onMessageReceived(session, message);
	}

	@Override
	public void messageSent(IoSession session, Object message) throws Exception {
		getListener(session).onMessageSent(session, message);
	}

	@Override
	public void sessionClosed(IoSession session) throws Exception {
		getListener(session).onSessionClosed(session);
	}

	@Override
	public void sessionOpened(IoSession session) throws Exception {
		//getListener(session).onSessionOpened(session);
	}
	
	@Override
	public void sessionIdle(IoSession session, IdleStatus status) throws Exception {
//		TafLoggerCenter.info("-------------------------->session "+session+" idle "+(idleStatusInteval/1000)+" seconds ,closed");
//		session.close();
	}

	// //////////////////////////////////////////////

//	private EndPointEventListen getListener(ServiceEndPointInfo request) {
//		String key = getSessionKey(request);
//		EndPointEventListen listener = listeners.get(key);
//		if (null == listener) {
//			throw new TafProxyException("no listener care " + request);
//		}
//		return listener;
//	}
//	
	private EndPointEventListen getListener(IoSession session) {
		return (EndPointEventListen) session.getAttribute(listen_session_key);
//		String key = getSessionKey(session);
//		EndPointEventListen listener = listeners.get(key);
//		if (null == listener) {
//			throw new TafProxyException("no listener care " + session);
//		}
//		return listener;
	}

	private String getSessionKey(ServiceEndPointInfo request) {
		return ServiceEndPointInfo.getSessionKey(request.getProtocol(), request.getRemoteHost(), request.getRemotePort());
	}
	
	private String getSessionKey(IoSession session) {
		String protocol = "tcp";
		if (session.getTransportMetadata().isConnectionless()) {
			protocol = "udp";
		}
		String remoteHost = ((InetSocketAddress) session.getRemoteAddress()).getAddress().getHostAddress();
		int remotePort = ((InetSocketAddress) session.getRemoteAddress()).getPort();
		// String localHost = ((InetSocketAddress)
		// session.getLocalAddress()).getAddress().getHostAddress();
		// int localPort = ((InetSocketAddress)
		// session.getLocalAddress()).getPort();
		// ConnMetadata metadata = new ConnMetadata(protocol, localHost,
		// localPort, remoteHost, remotePort);
		return ServiceEndPointInfo.getSessionKey(protocol, remoteHost, remotePort);
	}
}
