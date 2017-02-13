package com.qq.taf.server;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.server.jce2java.NOTIFYLEVEL;
import com.qq.taf.server.jce2java.NotifyPrx;
import com.qq.taf.server.jce2java.NotifyPrxHelper;

/**
 * 信息上报
 * @author fanzhang
 */
public class RemoteNotify {
	
	private static final RemoteNotify INSTANCE=new RemoteNotify();
	private Communicator comm;
	private NotifyPrx notifyPrx;
	private String app;
	private String serverName;

	public static RemoteNotify getInstance() {
		return INSTANCE;
	}
	
	public int setNotifyInfo(Communicator comm,String obj,String app,String serverName) {
		this.comm=comm;
		this.app=app;
		this.serverName=serverName;
		this.notifyPrx=this.comm.stringToProxy(obj, NotifyPrxHelper.class);
		((NotifyPrxHelper)this.notifyPrx).taf_sync_timeout(500);
		return 0;
	}
	
	public void notify(NOTIFYLEVEL level,String message) {
		try {
			notifyPrx.async_notifyServer(null, app+"."+serverName, level.value(), message);
		} catch(Exception e) {
			SystemLogger.record("RemoteNotify|notify error",e);
		}
	}
	
	public void report(String result) {
		report(result,false);
	}
	
	public void report(String result,boolean sync) {
		try {
			if(sync) notifyPrx.reportServer(app+"."+serverName, Thread.currentThread().getId()+"", result);
			else notifyPrx.async_reportServer(null,app+"."+serverName, Thread.currentThread().getId()+"", result);
		} catch(Exception e) {
			SystemLogger.record("RemoteNotify|report error",e);
		}
	}
	
}
