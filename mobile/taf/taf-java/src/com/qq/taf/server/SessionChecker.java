package com.qq.taf.server;

import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.navi.Handler;
import com.qq.navi.Session;
import com.qq.taf.server.util.GeneralException;
import com.qq.taf.server.util.ip.IPList;

/**
 * 连接检查器
 * @author fanzhang
 */
public class SessionChecker extends Handler {
	
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
		private Map<InetSocketAddress,Session> sessions;
		private int sessionCount;
		private IpChecker ipChkr;
		
		public ServantSessionManager(AdapterConfig adapConf) throws Throwable {
			this.adapConf=adapConf;
			sessions=new ConcurrentHashMap<InetSocketAddress, Session>();
			ipChkr=adapConf.getOrder().equals("allow,deny")?new AllowDenyIpChecker():new DenyAllowIpChecker();
			sessionCount=0;
		}
		
		public IpChecker getIpChecker() {
			return ipChkr;
		}
		
		public boolean checkAndAdd(Session session) {
			InetSocketAddress remoteSocketAddress=(InetSocketAddress)session.getRemoteAddress();
			//InetAddress remoteAddress=remoteSocketAddress.getAddress();
			//boolean pass0=ipChkr.check(remoteAddress);
			//boolean pass=pass0 && sessionCount<adapConf.getMaxConns();
			boolean pass = sessionCount<adapConf.getMaxConns();
			if(pass) {
				sessionCount++;
				sessions.put(remoteSocketAddress, session); 
			} else {
				session.close(true);
				//String msg=!pass0?"ConnectionIpForbidden":"ConnectionMaxNumberReached";
				String msg="ConnectionMaxNumberReached";
				SystemLogger.record(msg+"|"+adapConf.getServant()+"|"+session.getLocalAddress().toString()+"|"+remoteSocketAddress.toString());
			}
			return pass;
		}
		
		public void remove(Session session) {
			sessionCount--;
			sessions.remove(session.getRemoteAddress());
		}
		
		public Map<InetSocketAddress,Session> getSessions() {
			return sessions;
		}
		
		public AdapterConfig getAdapterConfig() {
			return adapConf;
		}
		
		public int getSessionCount() {
			return sessionCount;
		}
		
	}	
	
	public static final SessionChecker INSTANCE=new SessionChecker();
	private Map<InetSocketAddress, ServantSessionManager> srvtSssnMngrs;
	
	private SessionChecker() {
		try {
			srvtSssnMngrs=new HashMap<InetSocketAddress, ServantSessionManager>();
			for(AdapterConfig adapConf:AdapterConfig.values()) {
				InetSocketAddress isa=new InetSocketAddress(InetAddress.getByName(adapConf.getEndpoint().getHost()),adapConf.getEndpoint().getPort());
				srvtSssnMngrs.put(isa,new ServantSessionManager(adapConf));
			}
		} catch(Throwable t) {
			GeneralException.raise(t);
		}
	}
	
	public Map<InetSocketAddress,Map<InetSocketAddress,Long>> getSessionStatus() {
		Map<InetSocketAddress, Map<InetSocketAddress,Long>> data=new HashMap<InetSocketAddress, Map<InetSocketAddress,Long>>();
		for(InetSocketAddress bindAddress:srvtSssnMngrs.keySet()) {
			Map<InetSocketAddress,Long> data0=new HashMap<InetSocketAddress, Long>();
			data.put(bindAddress, data0);
			Map<InetSocketAddress, Session> sessions=srvtSssnMngrs.get(bindAddress).getSessions();
			for(InetSocketAddress remoteAddress:sessions.keySet()) {
				Session session=sessions.get(remoteAddress);
				long lastIoTime=Math.max(session.getLastReadTime(),session.getLastWriteTime());
				data0.put(remoteAddress,lastIoTime);
			}
		}
		return data;
	}
	
	public int getSessionCount(InetSocketAddress bindAddress) {
		return srvtSssnMngrs.get(bindAddress).getSessionCount();
	}
	
	private ServantSessionManager findServantSessionManager(Session session) {
		return srvtSssnMngrs.get(session.getLocalAddress());
	}
	
	public boolean checkIP(SocketAddress local, InetAddress remote) {
		try {
			return srvtSssnMngrs.get(local).getIpChecker().check(remote);
		}
		catch(Throwable t) {
			GeneralException.raise(t);
			return true;
		}
	}
	
	@Override
	public void sessionOpened(Session session) throws Throwable {
		findServantSessionManager(session).checkAndAdd(session);
	}
	
	@Override
	public void sessionClosed(Session session) throws Throwable {
		findServantSessionManager(session).remove(session);
	}

}
