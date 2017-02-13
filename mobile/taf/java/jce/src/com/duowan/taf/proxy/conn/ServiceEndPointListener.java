package com.duowan.taf.proxy.conn;

public interface ServiceEndPointListener {

	/**
	 * 当service全部连接都中断时触发
	 * @param service
	 */
	public void onAllSessoinClose( ServiceEndPoint service );
	/**
	 * 当service一个连接中断时触发
	 * @param service
	 */
	public void onSessionClose( ServiceEndPoint service );
	/**
	 * 当service一个连接建立时触发
	 * @param service
	 */
	public void onSessionOpen( ServiceEndPoint service );
	
	public void onResetGrid( ServiceEndPoint service , int newGridValue) ;
	
}
