package com.qq.taf.proxy;

public interface ServantProxyRouter {

	/**
	 * 根据routerKey返回灰度值 目前只支持0和1
	 * @param routerKey
	 * @return
	 */
	public int getGridByKey(String routerKey) ;
	
}
