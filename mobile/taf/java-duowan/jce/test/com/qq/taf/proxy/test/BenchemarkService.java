package com.qq.taf.proxy.test;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

import com.qq.jutil.j4log.Logger;
import com.qq.netutil.nio.LoggerCenter;
import com.qq.netutil.nio.mina2.core.filterchain.DefaultIoFilterChainBuilder;
import com.qq.netutil.nio.mina2.core.service.IoHandlerAdapter;
import com.qq.netutil.nio.mina2.core.session.IdleStatus;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolCodecFilter;
import com.qq.netutil.nio.mina2.fliter.executor.ExecutorFilter;
import com.qq.netutil.nio.mina2.transport.socket.nio.NioSocketAcceptor;
import com.qq.taf.ResponsePacket;
import com.qq.taf.proxy.test.BenchemarkService;
import com.qq.taf.cnst.JCESERVERSUCCESS;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.jce.JceOutputStream;
import com.qq.taf.jce.JceUtil;
import com.qq.taf.proxy.codec.JceCodecFactory;
import com.qq.taf.proxy.codec.JceMessage;

/**
 * 
 * @author albertzhu
 java -Xmx512m -cp
         /usr/local/app/onetest/classes:/usr/local/
         app/onetest/javalib/netTool-
         1.0.0.jar:/usr/local/app/onetest/javalib/jutil
         -1.0.2.jar:/usr/local/app/onetest/javalib/jceClient-1.0.0.jar
         com.qq.taf.proxy.simpletest.BenchemarkService 172.27.31.54
 */
public class BenchemarkService extends IoHandlerAdapter {

	final Logger logger = Logger.getLogger("BechmarkLog");
	AtomicLong echoCount = new AtomicLong();
	
	String serverName = "";
	
	public BenchemarkService(String serverName ) {
		this.serverName = serverName;
	}
	
	String host;

	public String getHost() {
		return host;
	}

	public void setHost(String host) {
		this.host = host;
	}

	public int getPort() {
		return port;
	}

	public void setPort(int port) {
		this.port = port;
	}

	int port;
	ExecutorFilter threadFliter = null;
	int queueSize = 100;
	int coreThreadSize = 6;
	int maxThreadSize = 32;
	BlockingQueue<Runnable> taskQueue = new ArrayBlockingQueue<Runnable>(queueSize);

	ExecutorService threadPool;
	NioSocketAcceptor acceptor = new NioSocketAcceptor();
	private volatile ScheduledExecutorService taskExecutor;
	int checkConnNumInterval = 1 * 1000; // 每隔1秒检查一次连接数

	public void bind() throws IOException {
		threadPool = new ThreadPoolExecutor(coreThreadSize, maxThreadSize, 3, TimeUnit.SECONDS, taskQueue);
		acceptor.setReuseAddress(true);
		acceptor.getSessionConfig().setTcpNoDelay(true);
		acceptor.setHandler(this);
		acceptor.getSessionConfig().setReadBufferSize(2048);
		acceptor.getSessionConfig().setIdleTime(IdleStatus.BOTH_IDLE, 10);
		DefaultIoFilterChainBuilder chain = acceptor.getFilterChain();
		chain.addLast("codec", new ProtocolCodecFilter(new JceCodecFactory(false)));
		threadFliter = new ExecutorFilter(threadPool);
		chain.addLast("threadPool", threadFliter);
		if (null == host) {
			acceptor.bind(new InetSocketAddress(port));
		} else {
			acceptor.bind(new InetSocketAddress(host, port));
		}
		if (null != taskExecutor) {
			taskExecutor.shutdown();
		}
		taskExecutor = Executors.newScheduledThreadPool(1);
		taskExecutor.scheduleWithFixedDelay(new Runnable() {
			public void run() {
				long count = echoCount.get();
				echoCount.set(0);
				logger.info("echo " + count + " oneSecond , wait size "+taskQueue.size());
			}

		}, checkConnNumInterval, checkConnNumInterval, TimeUnit.MILLISECONDS);
		System.out.println("bind at " + host + ":" + port + " success");
	}

	public void shutDown() {
		try {
			threadPool.shutdown();
		} catch (Exception e) {
			e.printStackTrace();
			LoggerCenter.info(" shutDown threadPool error " + e);
		}
		try {
			if (null != threadFliter) {
				threadFliter.destroy();
			}
		} catch (Exception e) {
			e.printStackTrace();
			LoggerCenter.info(" shutDown threadFliter error " + e);
		}
		try {
			acceptor.unbind();
		} catch (Exception e) {
			e.printStackTrace();
			LoggerCenter.info(" unbind acceptor error " + e);
		}
	}

	public final void sessionCreated(IoSession session) throws Exception {
		LoggerCenter.info("sessionCreated " + session);
	}

	public final void sessionOpened(IoSession session) throws Exception {
		LoggerCenter.info("sessionOpened " + session);
	}

	public final void sessionClosed(IoSession session) throws Exception {
		LoggerCenter.info(session + " closed");
	}

	public final void sessionIdle(IoSession session, IdleStatus status) throws Exception {
		// LoggerCenter.info("sessionIdle " + session + " " + status);
	}

	public final void exceptionCaught(IoSession session, Throwable cause) throws Exception {
		LoggerCenter.info(session + " " + session.getServiceAddress() + " exceptionCaught ", cause);
		// session.close();
	}

	public final void messageReceived(IoSession session, Object message) throws Exception {
		JceMessage jceMessage = (JceMessage) message;
		try {
			threadPool.execute(new LimitRunnable(session, jceMessage));
			echoCount.incrementAndGet();
		} catch (Exception ex) {
			LoggerCenter.info("add task to threadpool failed " + ex);
		}
	}

	public final void messageSent(IoSession session, Object message) throws Exception {
		JceMessage jceMessage = (JceMessage) message;
		LoggerCenter.info("sended msg " + jceMessage);
	}

	String echo(String s) {
		return s;
	}

	class LimitRunnable implements Runnable {
		IoSession session;
		JceMessage requestMsg;

		public LimitRunnable(IoSession session, JceMessage msg) {
			this.session = session;
			this.requestMsg = msg;
		}

		public void run() {
			try {
				
				
				 com.qq.taf.jce.JceInputStream _is = new com.qq.taf.jce.JceInputStream(requestMsg.getRequest().sBuffer);
			     int i  = (int) _is.read(0, 1, true);
			     String a  = (String) _is.read("", 2, true);
				logger.info("receive sServantName:"+requestMsg.getRequest().sServantName);
				logger.info("receive sFuncName:"+requestMsg.getRequest().sFuncName+"("+i+","+a+")");
				ResponsePacket response = new ResponsePacket();
				response.cPacketType = requestMsg.getRequest().cPacketType;
				response.iRequestId = requestMsg.getRequest().iRequestId;
				response.iRet = 0;
				response.iVersion = requestMsg.getRequest().iVersion;
				com.qq.taf.jce.JceOutputStream _os = new com.qq.taf.jce.JceOutputStream(0);
				 _os.write(JCESERVERSUCCESS.value, 0);
				 _os.write("1", 1);
				 _os.write("2", 3);
				 byte[] _sBuffer = com.qq.taf.jce.JceUtil.getJceBufArray(_os.getByteBuffer());
				 response.sBuffer = _sBuffer;
				JceOutputStream respOut = new JceOutputStream(0);
				response.writeTo(respOut);
				JceMessage responseMsg = JceMessage.createJceMessage(false, response.iRequestId, JceUtil.getJceBufArray(respOut.getByteBuffer()));
				session.write(responseMsg);
			} catch (Exception e) {
				e.printStackTrace();
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}

	}

	public int getQueueSize() {
		return queueSize;
	}

	public void setQueueSize(int queueSize) {
		this.queueSize = queueSize;
	}

	public int getCoreThreadSize() {
		return coreThreadSize;
	}

	public void setCoreThreadSize(int coreThreadSize) {
		this.coreThreadSize = coreThreadSize;
	}

	public int getMaxThreadSize() {
		return maxThreadSize;
	}

	public void setMaxThreadSize(int maxThreadSize) {
		this.maxThreadSize = maxThreadSize;
	}

	public static void main(String[] args) {
		String host = "127.0.0.1";
		if (args.length > 0) {
			host = args[0];
		}
		int port = 22224;
		if (args.length > 1) {
			port = Integer.parseInt(args[1]);
		}
		int coreSize = 16;
		if (args.length > 2) {
			coreSize = Integer.parseInt(args[2]);
		}
		int maxThreadSize = 32;
		if (args.length > 3) {
			maxThreadSize = Integer.parseInt(args[3]);
		}
		int queueSize = 500;
		if (args.length > 4) {
			queueSize = Integer.parseInt(args[4]);
		}
		BenchemarkService s = new BenchemarkService("server1");
		s.setCoreThreadSize(coreSize);
		s.setMaxThreadSize(maxThreadSize);
		s.setQueueSize(queueSize);
		s.setHost(host);
		s.setPort(port);
		try {
			s.bind();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
