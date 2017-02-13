package com.qq.taf.server;

import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.server.jce2java.ServerFPrx;
import com.qq.taf.server.jce2java.ServerFPrxHelper;
import com.qq.taf.server.jce2java.ServerInfo;

/**
 * 心跳与版本上报帮助类
 * @author fanzhang
 */
public class NodeFHelper {

	private static final NodeFHelper INSTANCE=new NodeFHelper();
	private ServerFPrx nodePrx;
	private ServerInfo si;
	
	private class HeartBeator implements Runnable,ThreadFactory {
		
		private static final int HEART_BEAT_INTERVAL = 10;
		private ScheduledExecutorService sche;
		
		public HeartBeator() {
			sche=Executors.newSingleThreadScheduledExecutor(this);
			sche.scheduleAtFixedRate(this, HEART_BEAT_INTERVAL, HEART_BEAT_INTERVAL, TimeUnit.SECONDS);
		}
		
		public void run() {
			keepAlive();
		}
		
		public Thread newThread(Runnable r) {
			return new Thread(r,"HeartBeator");
		}		
	}

	private NodeFHelper() {
	}
	
	public static NodeFHelper getInstance() {
		return INSTANCE;
	}
	
	public void setNodeInfo(Communicator comm,String obj,String app,String server) {
		si=new ServerInfo(app,server,getPid());
		nodePrx=comm.stringToProxy(obj, ServerFPrxHelper.class);
		new HeartBeator();
	}
	
	public void keepAlive() {
		try {
			nodePrx.async_keepAlive(null, si);
		} catch(Throwable t) {
			SystemLogger.record("NodeFHelper|keepAlive|error",t);
		}
	}
	
	public void reportVersion(String version) {
		try {
			nodePrx.async_reportVersion(null, si.application, si.serverName, version);
		} catch(Throwable t) {
			SystemLogger.record("NodeFHelper|reportVersion|error",t);
		}
	}

	private int getPid() {
		RuntimeMXBean rmxb = ManagementFactory.getRuntimeMXBean();
		String name=rmxb.getName();
		int pid=-1;
		try {
			pid=Integer.parseInt(name.split("@")[0]);
		} catch(Throwable t) {
		}
		return pid;
	}

}
