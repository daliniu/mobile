package com.qq.taf.server;

import com.qq.taf.server.NotifyObserver;

/**
 * 管理Servant的实现
 * @author fanzhang
 */
public class AdminServantImpl extends AdminServant {

	@Override
	public String notify(String command, JceCurrent jc) {
		return NotifyObserver.getInstance().notify(command, jc);
	}

	@Override
	public void shutdown(JceCurrent jc) {
		Application.INSTANCE.stop();
	}

}
