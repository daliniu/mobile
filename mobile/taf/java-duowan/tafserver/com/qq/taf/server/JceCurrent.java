package com.qq.taf.server;

import com.qq.netutil.nio.mina2.core.session.IoSession;
import com.qq.taf.RequestPacket;


/**
 * 当前请求上下文
 * @author fanzhang
 */
public class JceCurrent {

	private boolean isResponse=true;
	private IoSession ioSession;
	private RequestPacket requestPacket;
	private int responseRet;
	
	public IoSession getIoSession() {
		return ioSession;
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

	public void setIoSession(IoSession ioSession) {
		this.ioSession = ioSession;
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
