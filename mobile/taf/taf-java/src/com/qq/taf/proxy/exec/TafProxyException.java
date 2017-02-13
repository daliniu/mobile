package com.qq.taf.proxy.exec;


public class TafProxyException extends TafException{

	public TafProxyException(String string) {
		super(string);
	}

	public TafProxyException(String objectName, Exception ex) {
		super(objectName,ex);
	}

}
