package com.qq.taf.server.ahttpclient;

public interface HttpResponseCallback {

	public void onResponse(String response);
	
	public void onRespError(int code);
	
	public void onRespError(String resp);
	
	public void onSendError(String url,String desc);
	
	public void onTimeout(String url);
	
}
