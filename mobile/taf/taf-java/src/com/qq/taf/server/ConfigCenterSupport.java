package com.qq.taf.server;

import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

import com.qq.conf.httpServer.RequestHandler;
import com.qq.conf.httpServer.SocketAdapter;

/**
 * 对门户配置中心的支持
 * @author fanzhang
 */
public class ConfigCenterSupport {

	private static final ConfigCenterSupport INSTANCE=new ConfigCenterSupport();
	
	private ConfigCenterSupport() {
	}
	
	public static ConfigCenterSupport getInstance() {
		return INSTANCE;
	}
	
	public void checkAndLaunch() {
		if(ServerConfig.configCenterPort!=0 && ServerConfig.localIP!=null) 
			new Listener(ServerConfig.localIP, ServerConfig.configCenterPort).start();
	}
	
	private static class Listener extends Thread {
		
		private String ip;
		private int port;
		
		public Listener(String ip,int port) {
			this.ip=ip;
			this.port=port;
			setName("ConfigCenterSupport");
		}
		
		public void run() {
			try {
				accept();
			} catch(Throwable t) {
				SystemLogger.record("ConfigCenterSupport|start|error|"+ip+":"+port,t);
			}
		}
		
		private void accept() throws Throwable {
			ServerSocket ss=new ServerSocket(port,50,InetAddress.getByName(ip));
			SystemLogger.record("ConfigCenterSupport|start|ok|"+ip+":"+port);
			Socket s=null;
			while(true) {
				try {
					s=ss.accept();
					new RequestHandler(new SocketAdapter(s)).run();
					SystemLogger.record("ConfigCenterSupport|process|ok|"+s.getRemoteSocketAddress().toString());
				} catch(Throwable t) {
					SystemLogger.record("ConfigCenterSupport|process|error|"+(s!=null?s.getRemoteSocketAddress().toString():"null"),t);
				}
			}
		}
		
	}
	
}
