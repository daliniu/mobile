package com.qq.taf.proxy;

import java.util.List;

import com.qq.taf.proxy.conn.ServiceEndPointInfo;


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
	//public abstract List<ServiceEndPointInfo> getGridServices(int grid);
	
	/**
	 * 是否有灰度服务
	 * @param grid 灰度值
	 * @return
	 */
	public abstract boolean hasGridServices(int grid);

	/**
	 * 获取相应灰度可用的service
	 * 
	 * @param grid
	 * @return
	 */
	//public abstract List<ServiceEndPointInfo> getGridActiveServices(int grid);
	
	/**
	 * 按序号取模获取活跃endpointinfo
	 * @param grid 灰度值
	 * @param seq 序号
	 * @return
	 */
	public abstract ServiceEndPointInfo getGridActiveService(int grid, int seq);

	/**
	 * 得到所有的服务器列表
	 * 
	 * @return
	 */
	public abstract List<ServiceEndPointInfo> getAllServices();

}