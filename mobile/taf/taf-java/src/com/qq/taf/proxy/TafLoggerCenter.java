package com.qq.taf.proxy;

import com.qq.jutil.j4log.Logger;

public class TafLoggerCenter {

	static final Logger myLogger = Logger.getLogger("jceClient");
	static int logParmLen = 15;

	public static void info(String loggLine) {
		myLogger.info(loggLine);
	}

	public static void info(String loggLine, Throwable th) {
		myLogger.info(loggLine, th);
	}

	public static boolean isDebugEnabled() {
		return myLogger.isDebugEnabled();
	}

	/**
	 * 打level为warn的log
	 * 
	 * @param str
	 *            要打的log
	 */
	public static void warn(String str) {
		myLogger.warn(str);
	}

	/**
	 * 打level为warn的log，并将异常的堆栈也打出来
	 * 
	 * @param str
	 *            要打的log
	 * @param th
	 *            异常的堆栈
	 */
	public static void warn(String str, Throwable th) {
		myLogger.warn(str, th);
	}

	/**
	 * 打level为error的log
	 * 
	 * @param str
	 *            要打的log
	 */
	public static void error(String str) {
		myLogger.error(str);
	}

	/**
	 * 打level为error的堆栈
	 * 
	 * @param str
	 *            要打的log
	 * @param th
	 *            异常的堆栈
	 */
	public static void error(String str, Throwable th) {
		myLogger.error(str, th);
	}

	/**
	 * 打level为fatal的log
	 * 
	 * @param str
	 *            要打的log
	 */
	public void fatal(String str) {
		myLogger.fatal(str);
	}

	/**
	 * 打level为fatal的log，并将异常的堆栈也打出来
	 * 
	 * @param str
	 *            要打的log
	 * @param th
	 *            异常的堆栈
	 */
	public static void fatal(String str, Throwable th) {
		myLogger.fatal(str, th);
	}

	/**
	 * 打level为debug的log
	 * 
	 * @param str
	 *            要打的log
	 */
	public static void debug(String str) {
		myLogger.debug(str);
	}

	/**
	 * 打level为debug的log，并将异常的堆栈也打出来
	 * 
	 * @param str
	 *            要打的log
	 * @param th
	 *            异常的堆栈
	 */
	public static void debug(String str, Throwable th) {
		myLogger.debug(str, th);
	}

	public static String printMethod(String functionName, Object[] args) {
		StringBuilder sb = new StringBuilder();
		sb.append(functionName).append("(");
		if (null != args) {
			StringBuilder sbArgs = new StringBuilder();
			for (int i = 0; i < args.length; i++) {
				if (args[i] == null)
					sbArgs.append("NULL").append(",");
				else if (args[i] instanceof Number || args[i] instanceof Boolean)
					sbArgs.append(args[i]).append(",");
				else
					sbArgs.append(encodeStringParam(args[i].toString(), logParmLen)).append(",");
			}
			sbArgs = sbArgs.length() >= 1 ? sbArgs.deleteCharAt(sbArgs.length() - 1) : sbArgs;
			sb.append(sbArgs);
		}
		sb.append(")");
		return sb.toString();
	}

	public static String encodeStringParam(String longParam, int len) {
		if (longParam == null || longParam.length() == 0)
			return "";
		String shortParam = longParam;
		if (len > 0) {
			shortParam = longParam.length() > len ? longParam.substring(0, len) + "..(" + longParam.length() + ")"
					: longParam;
		}
		return shortParam.replaceAll(" ", "_").replaceAll("\t", "_").replaceAll("\n", "+").replace(',', '，').replace(
				'(', '（').replace(')', '）');
	}
}
