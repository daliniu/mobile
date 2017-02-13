package com.duowan.taf.proxy.exec;


public class TafProxyNoConnException extends TafException{

	public TafProxyNoConnException(String string) {
		super(string);
	}

	public TafProxyNoConnException(String objectName, Exception ex) {
		super(objectName,ex);
	}

}
