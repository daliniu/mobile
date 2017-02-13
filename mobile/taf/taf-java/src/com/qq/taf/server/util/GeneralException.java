package com.qq.taf.server.util;

/**
 * 通用的异常包装
 * @author fanzhang
 */
public class GeneralException extends RuntimeException {
	
	private static final long serialVersionUID = 1L;

	public GeneralException(String msg) {
		super(msg);
	}
	
	public GeneralException(String msg,Throwable th) {
		super(msg,th);
	}
	
	public GeneralException(Throwable th) {
		super(th);
	}
	
	public static void raise(String msg) {
		throw new GeneralException(msg);
	}
	
	public static void raise(String msg,Throwable th) {
		throw new GeneralException(msg,th);
	}
	
	public static void raise(Throwable th) {
		throw new GeneralException(th);
	}

}
