package com.duowan.taf.proxy.exec;


public class TafProxyOverloadException extends TafException{

	public TafProxyOverloadException(int ret) {
		super(ret);
	}
	
	int allowBufferSize ;
	
	int size ;
	
	String objectName;

	public TafProxyOverloadException(String objectName, int allowBufferSize, int size) {
		super(objectName+" allowBuff is "+allowBufferSize + " , now is "+size);
		this.allowBufferSize = allowBufferSize ;
		this.size = size ;
		this.objectName = objectName ;
	}

	public int getAllowBufferSize() {
		return allowBufferSize;
	}

	public int getSize() {
		return size;
	}

	public String getObjectName() {
		return objectName;
	}

}
