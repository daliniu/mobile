package com.qq.taf.server;

import static com.qq.taf.server.ApplicationUtil.TYPE_LOCAL;
import static com.qq.taf.server.ApplicationUtil.createLogger;
import static com.qq.taf.server.ApplicationUtil.getLogger;

import com.qq.taf.server.LoggerFactory.Logger;


/**
 * 系统按天日志记录工具
 * @author fanzhang
 */
public abstract class SystemLogger {
	
	private static final String SYS_LOG_NAME="system";
	private static final Logger SYS_LOG;
	
	static {
		createLogger(SYS_LOG_NAME, TYPE_LOCAL);
		SYS_LOG=getLogger(SYS_LOG_NAME);
	}
	
	public static void record(String msg) {
		SYS_LOG.log(msg);
	}
	
	public static void record(String msg,Throwable cause) {
		SYS_LOG.log(msg, cause);
	}
	
}
