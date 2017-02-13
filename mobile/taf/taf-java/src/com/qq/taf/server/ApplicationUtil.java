package com.qq.taf.server;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.List;

import com.qq.taf.proxy.Communicator;
import com.qq.taf.server.LoggerFactory.Level;
import com.qq.taf.server.LoggerFactory.Logger;
import com.qq.taf.server.LoggerFactory.Type;
import com.qq.taf.server.proxy.NOTIFYLEVEL;
import com.qq.taf.server.NotifyInvoke;
import com.qq.taf.server.NotifyObserver;
import com.qq.taf.server.PropertyReporterFactory;
import com.qq.taf.server.PropertyReporterFactory.Policy;
import com.qq.taf.server.PropertyReporterFactory.PropertyReporter;

/**
 * 全局可用的辅助设施（管理接口、远程通知、本地/远程日志、属性上报等）
 * @author fanzhang
 */
public abstract class ApplicationUtil {
	
	public static final String FORMAT_DAY=LoggerFactory.FORMAT_DAY;
	public static final String FORMAT_HOUR=LoggerFactory.FORMAT_HOUR;
	public static final String FORMAT_MINUTE=LoggerFactory.FORMAT_MINUTE;
	
	public static final Level LEVEL_INFO=Level.INFO;
	public static final Level LEVEL_DEBUG=Level.DEBUG;
	public static final Level LEVEL_WARN=Level.WARN;
	public static final Level LEVEL_ERROR=Level.ERROR;
	
	public static final Type TYPE_LOCAL=Type.LOCAL;
	public static final Type TYPE_REMOTE=Type.REMOTE;
	public static final Type TYPE_BOTH=Type.BOTH;
	
	public static void addAdminCmdPrefix(String cmd,NotifyInvoke invoke) {
		NotifyObserver.getInstance().registerPrefix(cmd, invoke);
	}
	
	public static void addAdminCmdNormal(String cmd,NotifyInvoke invoke) {
		NotifyObserver.getInstance().registerNotify(cmd, invoke);
	}
	
	public static void notifyNormal(String info) {
		RemoteNotify.getInstance().notify(NOTIFYLEVEL.NOTIFYNORMAL, info);
	}
	
	public static void notifyWarn(String info) {
		RemoteNotify.getInstance().notify(NOTIFYLEVEL.NOTIFYWARN, info);
	}
	
	public static void notifyError(String info) {
		RemoteNotify.getInstance().notify(NOTIFYLEVEL.NOTIFYERROR, info);
	}
	
	public static void createLogger(String logName,String format,Level level,Type type) {
		LoggerFactory.getInstance().createLogger(logName, format, level, type);
	}
	
	public static void createLogger(String logName,String format,Type type) {
		LoggerFactory.getInstance().createLogger(logName, format, type);
	}
	
	public static void createLogger(String logName,Level level,Type type) {
		LoggerFactory.getInstance().createLogger(logName, level, type);
	}
	
	public static void createLogger(String logName,Type type) {
		LoggerFactory.getInstance().createLogger(logName, type);
	}
	
	public static Logger getLogger(String logName) {
		return LoggerFactory.getInstance().getLogger(logName);
	}
	
	public static Policy newPolicySum() {
		return new PropertyReporterFactory.Sum();
	}
	
	public static Policy newPolicyMin() {
		return new PropertyReporterFactory.Min();
	}
	
	public static Policy newPolicyMax() {
		return new PropertyReporterFactory.Max();
	}
	
	public static Policy newPolicyAvg() {
		return new PropertyReporterFactory.Avg();
	}
	
	public static Policy newPolicyCount() {
		return new PropertyReporterFactory.Count();
	}
	
	public static Policy newPolicyDistr(List<Integer> list) {
		return new PropertyReporterFactory.Distr(list);
	}
	
	public static void createPropertyReporter(String propRptName,Policy... policies) {
		PropertyReporterFactory.getInstance().createPropertyReporter(propRptName, policies);
	}
	
	public static PropertyReporter getPropertyReporter(String propRptName) {
		return PropertyReporterFactory.getInstance().getPropertyReporter(propRptName);
	}
	
	public static void cout(String msg) {
		System.out.println(msg);
	}
	
	public static void cerr(String msg) {
		System.err.println(msg);
	}
	
	public static String getStackTrace(Throwable th) {
		StringWriter sw=new StringWriter();
		th.printStackTrace(new PrintWriter(sw));
		return sw.toString();
	}
	
	public static Communicator getCommunicator() {
		return Application.INSTANCE.getCommunicator();
	}

}
