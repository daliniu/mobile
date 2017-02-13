package com.qq.taf.server;

import com.qq.navi.Session;
import com.qq.taf.RequestPacket;


/**
 * 当前请求上下文
 * @author fanzhang
 */
public class JceCurrent {

	private boolean isResponse=true;
	private Session session;
	private RequestPacket requestPacket;
	private int responseRet;
	
	public Session getSession() {
		return session;
	}

	public RequestPacket getRequestPacket() {
		return requestPacket;
	}
	
	public boolean isResponse() {
		return isResponse;
	}
	
	public int getResponseRet() {
		return responseRet;
	}

	public void setSession(Session session) {
		this.session = session;
	}
	
	public void setRequestPacket(RequestPacket requestPacket) {
		this.requestPacket = requestPacket;
	}

	public void setResponse(boolean isResponse) {
		this.isResponse=isResponse;
	}
	
	public void setResponseRet(int responseRet) {
		this.responseRet=responseRet;
	}
	
}
