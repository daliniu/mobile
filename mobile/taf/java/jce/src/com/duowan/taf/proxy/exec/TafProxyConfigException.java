package com.duowan.taf.proxy.exec;

public class TafProxyConfigException extends TafProxyException{

	public TafProxyConfigException(String string) {
		super(string);
	}

	public TafProxyConfigException(String objectName, Exception ex) {
		super(objectName,ex);
	}

}
