package com.duowan.taf.proxy;

import java.util.List;

import com.duowan.taf.proxy.conn.ServiceEndPointInfo;


public interface IServicesInfo {

	/**
	 * 得到活跃的service列表
	 * 
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getActiveServices();

	/**
	 * 得到失败的service列表
	 * 
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getInActiveServices();

	/**
	 * 获取相应灰度的service
	 * 
	 * @param grid
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getGridServices(int grid);

	/**
	 * 获取相应灰度可用的service
	 * 
	 * @param grid
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getGridActiveServices(int grid);

	/**
	 * 得到所有的服务器列表
	 * 
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getAllServices();

}