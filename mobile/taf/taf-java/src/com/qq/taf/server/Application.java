package com.qq.taf.server;

import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.Map;

import com.qq.navi.Acceptor;
import com.qq.navi.Handler;
import com.qq.navi.Session;
import com.qq.navi.TCPAcceptedCallBack;
import com.qq.navi.udp.UDPAcceptedCallBack;
import com.qq.navi.udp.UDPAcceptor;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.codec.JceCodec;
import com.qq.taf.server.util.Config;
import com.qq.taf.TAF_VERSION;

/**
 * 服务基类
 * 
 * @author fanzhang
 */
public abstract class Application extends Handler {

	public static Application INSTANCE;
	private Map<String, Servant> servantNameMap;
	private Map<InetSocketAddress, Servant> servantBindMap;
	private Acceptor socketAcceptor;
	private UDPAcceptor datagramAcceptor;
	private Communicator comm;

	private TCPAcceptedCallBack tcpCallBack = new TCPAcceptedCallBack() {
		@Override
		public boolean canAccept(SocketChannel sc) {
			try {
				return SessionChecker.INSTANCE.checkIP(sc.socket().getLocalSocketAddress(), ((InetSocketAddress)sc.socket().getRemoteSocketAddress()).getAddress());
			}
			catch(Throwable t) {
				SystemLogger.record("tcpCallBack invoke error", t);
				return true;
			}
		}
		
	};
	private UDPAcceptedCallBack udpCallBack = new UDPAcceptedCallBack() {
		@Override
		public boolean canAccept(SocketAddress local, SocketAddress remote) {
			try {
				return SessionChecker.INSTANCE.checkIP(local, ((InetSocketAddress)remote).getAddress());
			}
			catch(Throwable t) {
				SystemLogger.record("udpCallBack invoke error", t);
				return true;
			}
		}
	};
	public Application() {
		try {
			INSTANCE = this;
			servantNameMap = new HashMap<String, Servant>();
			servantBindMap = new HashMap<InetSocketAddress, Servant>();

			Config conf = new Config(ServerConfig.CONFIG_FILE);
			ServerConfig.init(conf);
			AdapterConfig.init(conf);
			ClientConfig.init(conf);

			JceCodec jceCodec=new JceCodec(false);
			socketAcceptor=new Acceptor("TafTcpServer",jceCodec,this,0,-1);
			socketAcceptor.setAcceptedCallBack(tcpCallBack);
			datagramAcceptor = new UDPAcceptor("TafUdpServer",jceCodec,this,0,-1);
			datagramAcceptor.setCallBack(udpCallBack);
			if(ServerConfig.local!=null) {
				addServant(AdminServantImpl.class, ServerConfig.application + "." + ServerConfig.serverName+".AdminObj");
				AdminCommands.load();
			}

			comm = new ClientCommunicator();
			if(ServerConfig.node!=null) 
				NodeFHelper.getInstance().setNodeInfo(comm, ServerConfig.node, ServerConfig.application,ServerConfig.serverName);
			if(ServerConfig.config!=null && ServerConfig.basePath!=null) 
				RemoteConfig.getInstance().setConfigInfo(comm, ServerConfig.config, ServerConfig.application,ServerConfig.serverName, ServerConfig.basePath);
			if(ServerConfig.notify!=null) 
				RemoteNotify.getInstance().setNotifyInfo(comm, ServerConfig.notify, ServerConfig.application,ServerConfig.serverName);
			if(ServerConfig.log!=null && ServerConfig.logPath!=null)
				LoggerFactory.getInstance().setConfigInfo(comm, ServerConfig.log, ServerConfig.application,ServerConfig.serverName,ServerConfig.logPath);

			ConfigCenterSupport.getInstance().checkAndLaunch();
			
			initialize();
		} catch (Throwable t) {
			ApplicationUtil.cerr("error in starting");
			ApplicationUtil.cerr(ApplicationUtil.getStackTrace(t));
			RemoteNotify.getInstance().report("error in starting", true);
			System.exit(0);
		}
	}

	public Communicator getCommunicator() {
		return comm;
	}

	@Override
	public void sessionOpened(Session session) throws Throwable {
		SessionChecker.INSTANCE.sessionOpened(session);
	}
	
	@Override
	public void sessionClosed(Session session) throws Throwable {
		SessionChecker.INSTANCE.sessionClosed(session);
	}
	
	@Override
	public void messageRecieved(Session session, Object message) throws Throwable {
		findServant(session).messageRecieved(session, message);
	}

	@Override
	public void exceptionCaught(Session session, Throwable cause) throws Throwable {
		findServant(session).exceptionCaught(session, cause);
	}

	private Servant findServant(Session session) {
		return servantBindMap.get(session.getLocalAddress());
	}

	protected boolean addConfig(String fileName) {
		JceStringHolder result = new JceStringHolder();
		boolean ok = RemoteConfig.getInstance().addConfig(fileName, result);
		RemoteNotify.getInstance().report(result.value);
		return ok;
	}

	protected boolean addAppConfig(String fileName) {
		JceStringHolder result = new JceStringHolder();
		boolean ok = RemoteConfig.getInstance().addConfig(fileName, result, true);
		RemoteNotify.getInstance().report(result.value);
		return ok;
	}

	protected Servant addServant(Class<? extends Servant> implCls, String servantName) throws Throwable {
		Servant servant = implCls.newInstance();
		servant.setAdapterConfig(AdapterConfig.getByServant(servantName));
		servant.initAcceptor(socketAcceptor, datagramAcceptor);
		servant.initExecutor();
		servantNameMap.put(servantName, servant);
		servantBindMap.put(servant.getBindAddress(), servant);
		return servant;
	}

	public void start() {
		try {
			int flag=1;
			for (Servant s : servantNameMap.values()) flag*=s.bind();
			if(flag%2==0) socketAcceptor.listen();
			if(flag%3==0) datagramAcceptor.start();
			NodeFHelper.getInstance().reportVersion(TAF_VERSION.VERSION);
			NodeFHelper.getInstance().keepAlive();
			RemoteNotify.getInstance().report("restart");
			ApplicationUtil.cout(ServerConfig.application + "." + ServerConfig.serverName + " is started.");
		} catch (Throwable t) {
			ApplicationUtil.cerr("error in starting");
			ApplicationUtil.cerr(ApplicationUtil.getStackTrace(t));
			RemoteNotify.getInstance().report("error in starting", true);
			System.exit(0);
		}
	}

	public void stop() {
		ApplicationUtil.cout(ServerConfig.application + "." + ServerConfig.serverName + " is stopped.");
		RemoteNotify.getInstance().report("stop", true);
		System.exit(0);
	}

	public Servant getServant(String servantName) {
		return servantNameMap.get(servantName);
	}

	protected abstract void initialize() throws Throwable;

}
