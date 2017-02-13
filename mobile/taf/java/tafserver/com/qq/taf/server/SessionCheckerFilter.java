package com.qq.taf.server;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;

import com.qq.netutil.nio.mina2.core.filterchain.IoFilterAdapter;
import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.taf.proxy.TimerProvider;
import com.qq.taf.server.util.GeneralException;
import com.qq.taf.server.util.ip.IPList;

/**
 * 连接情况检查器
 * @author fanzhang
 */
public class SessionCheckerFilter extends IoFilterAdapter {

	private static class ServantSessionManager {
		
		private interface IpChecker {
			public boolean check(InetAddress address);
		}
		
		private class AllowDenyIpChecker implements IpChecker {
			public boolean check(InetAddress address) {
				IPList allow=adapConf.getAllow();
				IPList deny=adapConf.getDeny();
				if(allow.contains(address)) return true;
				if(deny.contains(address)) return false;
				return allow.isEmpty();
			}
		}
		
		private class DenyAllowIpChecker implements IpChecker {
			public boolean check(InetAddress address) {
				IPList allow=adapConf.getAllow();
				IPList deny=adapConf.getDeny();
				if(deny.contains(address)) return false;
				if(allow.contains(address)) return true;
				return allow.isEmpty();
			}
		}
		
		private AdapterConfig adapConf;
		private Map<InetSocketAddress,IoSession> sessions;
		private IpChecker ipChkr;
		
		public ServantSessionManager(AdapterConfig adapConf) throws Throwable {
			this.adapConf=adapConf;
			sessions=new ConcurrentHashMap<InetSocketAddress, IoSession>();
			ipChkr=adapConf.getOrder().equals("allow,deny")?new AllowDenyIpChecker():new DenyAllowIpChecker();
		}
		
		public boolean check(IoSession session) {
			InetSocketAddress remoteSocketAddress=(InetSocketAddress)session.getRemoteAddress();
			InetAddress remoteAddress=remoteSocketAddress.getAddress();
			boolean pass0=ipChkr.check(remoteAddress);
			boolean pass=pass0 && sessions.size()<adapConf.getMaxConns();
			if(pass) {
				sessions.put(remoteSocketAddress, session); 
			} else {
				session.close(true);
				String msg=!pass0?"ConnectionIpForbidden":"ConnectionMaxNumberReached";
				SystemLogger.record(msg+"|"+adapConf.getServant()+"|"+session.getLocalAddress().toString()+"|"+remoteSocketAddress.toString());
			}
			return pass;
		}
		
		public void remove(IoSession session) {
			sessions.remove(session.getRemoteAddress());
		}
		
		public Map<InetSocketAddress,IoSession> getSessions() {
			return sessions;
		}
		
		public AdapterConfig getAdapterConfig() {
			return adapConf;
		}
		
		public int getSessionCount() {
			return sessions.size();
		}
		
	}	
	
	private class SessionIoTimeoutTask implements Runnable,ThreadFactory {
		
		public Thread newThread(Runnable r) {
			return new Thread(r,"SessionIoTimeout");
		}
		
		public void run() {
			for(InetSocketAddress bindAddress:srvtSssnMngrs.keySet()) checkByServant(bindAddress, srvtSssnMngrs.get(bindAddress));
		}
		
		private void checkByServant(InetSocketAddress bindAddress,ServantSessionManager srvtSssnMngr) {
			long now=TimerProvider.getNow();
			int timeout=srvtSssnMngr.getAdapterConfig().getEndpoint().getTimeout();
			String servant=srvtSssnMngr.getAdapterConfig().getServant();
			Map<InetSocketAddress,IoSession> sessions=srvtSssnMngr.getSessions();
			for(InetSocketAddress remoteAddress:sessions.keySet()) {
				IoSession session=sessions.get(remoteAddress);
				if(session.getLastIoTime()+timeout<now) {
					session.close(true);
					SystemLogger.record("ConnectionIoTimeout|"+servant+"|"+bindAddress.toString()+"|"+remoteAddress.toString());
				}
			}
		}
		
	}
	
	private static final SessionCheckerFilter INSTANCE=new SessionCheckerFilter();
	private Map<InetSocketAddress, ServantSessionManager> srvtSssnMngrs;
	private ScheduledExecutorService sssnIoTimeoutChkr;
	
	private SessionCheckerFilter() {
		try {
			srvtSssnMngrs=new HashMap<InetSocketAddress, ServantSessionManager>();
			for(AdapterConfig adapConf:AdapterConfig.values()) {
				InetSocketAddress isa=new InetSocketAddress(InetAddress.getByName(adapConf.getEndpoint().getHost()),adapConf.getEndpoint().getPort());
				srvtSssnMngrs.put(isa,new ServantSessionManager(adapConf));
			}
			SessionIoTimeoutTask siit=new SessionIoTimeoutTask();
			sssnIoTimeoutChkr=Executors.newSingleThreadScheduledExecutor(siit);
			sssnIoTimeoutChkr.scheduleAtFixedRate(siit, 300, 300, TimeUnit.SECONDS);
		} catch(Throwable t) {
			GeneralException.raise(t);
		}
	}
	
	public static SessionCheckerFilter getInstance() {
		return INSTANCE;
	}
	
	public Map<InetSocketAddress,Map<InetSocketAddress,Long>> getSessionStatus() {
		Map<InetSocketAddress, Map<InetSocketAddress,Long>> data=new HashMap<InetSocketAddress, Map<InetSocketAddress,Long>>();
		for(InetSocketAddress bindAddress:srvtSssnMngrs.keySet()) {
			Map<InetSocketAddress,Long> data0=new HashMap<InetSocketAddress, Long>();
			data.put(bindAddress, data0);
			Map<InetSocketAddress, IoSession> sessions=srvtSssnMngrs.get(bindAddress).getSessions();
			for(InetSocketAddress remoteAddress:sessions.keySet()) data0.put(remoteAddress,sessions.get(remoteAddress).getLastIoTime());
		}
		return data;
	}
	
	public int getSessionCount(InetSocketAddress bindAddress) {
		return srvtSssnMngrs.get(bindAddress).getSessionCount();
	}
	
	private ServantSessionManager findServantSessionManager(IoSession session) {
		return srvtSssnMngrs.get(session.getLocalAddress());
	}
	
	@Override
	public void sessionCreated(NextFilter nextFilter, IoSession session) throws Exception {
		if(findServantSessionManager(session).check(session)) super.sessionCreated(nextFilter, session); 
	}
	
	@Override
	public void sessionClosed(NextFilter nextFilter, IoSession session) throws Exception {
		findServantSessionManager(session).remove(session);
		super.sessionClosed(nextFilter, session);
	}

	@Override
	public void messageReceived(NextFilter nextFilter, IoSession session, Object message) throws Exception {
		if(!session.isClosing() && session.isConnected()) super.messageReceived(nextFilter, session, message);
	}
	
}
