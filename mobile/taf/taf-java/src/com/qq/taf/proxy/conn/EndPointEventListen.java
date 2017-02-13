package com.qq.taf.proxy.conn;

import com.qq.navi.Session;
/**
 * io事件监听接口
 * @author albertzhu
 *
 */
interface EndPointEventListen {

	void onSessionOpened(Session session) throws Exception;
	
	void onSessionOpenedFailed() ;

	void onSessionClosed(Session session) throws Exception;

	void onExceptionCaught(Session session, Throwable cause) throws Exception;

	void onMessageReceived(Session session, Object message) throws Exception;

	public String getListenKey();

}
