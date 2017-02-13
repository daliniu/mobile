package com.duowan.taf.proxy.conn;

import com.duowan.netutil.nio.mina2.core.session.IoSession;
/**
 * io事件监听接口
 * @author albertzhu
 *
 */
interface EndPointEventListen {

	void onSessionOpened(IoSession session) throws Exception;
	
	void onSessionOpenedFailed() ;

	void onSessionClosed(IoSession session) throws Exception;

	void onExceptionCaught(IoSession session, Throwable cause) throws Exception;

	void onMessageReceived(IoSession session, Object message) throws Exception;

	public String getListenKey();

	void onMessageSent(IoSession session, Object message) throws Exception;

}
