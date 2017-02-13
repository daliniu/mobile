package com.qq.taf.server;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;

import com.qq.netutil.nio.mina2.core.service.IoHandlerAdapter;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.netutil.nio.mina2.fliter.codec.ProtocolCodecFilter;
import com.qq.netutil.nio.mina2.transport.socket.nio.NioDatagramAcceptor;
import com.qq.netutil.nio.mina2.transport.socket.nio.NioSocketAcceptor;
import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.proxy.Communicator;
import com.qq.taf.proxy.codec.JceCodecFactory;
import com.qq.taf.server.util.Config;
import com.qq.taf.server.util.TAF_VERSION;

/**
 * 服务基类
 * 
 * @author fanzhang
 */
public abstract class Application extends IoHandlerAdapter {

	public static Application INSTANCE;
	private Map<String, Servant> servantNameMap;
	private Map<InetSocketAddress, Servant> servantBindMap;
	private NioSocketAcceptor socketAcceptor;
	private NioDatagramAcceptor datagramAcceptor;
	private Communicator comm;

	public Application() {
		try {
			INSTANCE = this;
			servantNameMap = new HashMap<String, Servant>();
			servantBindMap = new HashMap<InetSocketAddress, Servant>();

			Config conf = new Config(ServerConfig.CONFIG_FILE);
			ServerConfig.init(conf);
			AdapterConfig.init(conf);
			ClientConfig.init(conf);

			JceCodecFactory codecFactory=new JceCodecFactory(false);
			ProtocolCodecFilter codec = new ProtocolCodecFilter(codecFactory);

			socketAcceptor = new NioSocketAcceptor();
			socketAcceptor.getFilterChain().addLast("checker", SessionCheckerFilter.getInstance());
			socketAcceptor.getFilterChain().addLast("codec", codec);
			socketAcceptor.getSessionConfig().setTcpNoDelay(true);
			socketAcceptor.getSessionConfig().setReuseAddress(true);
			socketAcceptor.setReuseAddress(true);
			socketAcceptor.setHandler(this);

			datagramAcceptor = new NioDatagramAcceptor();
			datagramAcceptor.getFilterChain().addLast("checker", SessionCheckerFilter.getInstance());
			datagramAcceptor.getFilterChain().addLast("codec", codec);
			datagramAcceptor.getSessionConfig().setReuseAddress(true);
			datagramAcceptor.setHandler(this);

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
	public void messageReceived(IoSession session, Object message) throws Exception {
		findServant(session).messageReceived(session, message);
	}

	@Override
	public void exceptionCaught(IoSession session, Throwable cause) throws Exception {
		findServant(session).exceptionCaught(session, cause);
	}

	private Servant findServant(IoSession session) {
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
			for (Servant s : servantNameMap.values()) s.bind();
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
// 		NOTICE: unbind() may block current thread sometime. 
//		socketAcceptor.unbind();
//		datagramAcceptor.unbind();
		RemoteNotify.getInstance().report("stop", true);
		System.exit(0);
	}

//	public void start(String servantName) throws Throwable {
//		servantNameMap.get(servantName).bind();
//	}
//
//	public void stop(String servantName) throws Throwable {
//		servantNameMap.get(servantName).unbind();
//	}
//
	public Servant getServant(String servantName) {
		return servantNameMap.get(servantName);
	}

	protected abstract void initialize() throws Throwable;

}
