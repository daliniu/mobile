package com.qq.taf.server;

import com.qq.taf.holder.JceStringHolder;
import com.qq.taf.server.JceCurrent;

/**
 * 管理命令需要实现的接口
 * @author fanzhang
 */
public interface NotifyInvoke {

	public boolean notify(String command,String params,JceCurrent jc,JceStringHolder result);
	
}
