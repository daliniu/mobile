package com.duowan.taf.proxy;

/**
 * 本次调用的相关信息
 * 
 * @author albertzhu
 * 
 */
public interface InvokeInfo {
	public String getObjectName();

	public String getMethodName();

	public int getInvokeHash();

	public int getInvokeGrid();

	public boolean isMsgType(int messageType);

	public boolean isNeedReturn();
	
	public boolean isAsync();
}
