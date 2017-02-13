package com.duowan.taf.proxy;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import com.duowan.taf.cnst.Const;
import com.duowan.taf.cnst.JCEMESSAGETYPEGRID;
import com.duowan.taf.cnst.JCEMESSAGETYPEHASH;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;
import com.duowan.taf.proxy.exec.TafProxyConfigException;

/**
 * 暴露给用户的服务器选择接口
 * 
 * @author albertzhu
 * 
 */
public class TAdapterSelector {

	// 服务器列表选择指针
	AtomicInteger hashPoint = new AtomicInteger();

	/**
	 * 选择可用的服务器
	 * 
	 * @param serviceInfos
	 *            服务器相关信息
	 * @param invokeInfo
	 *            本次调用相关信息
	 * @return
	 */
	public ServiceEndPointInfo getAliveConn(ServiceInfos serviceInfos, InvokeInfo invokeInfo) {
		ServiceEndPointInfo endpoint = null;
		if (invokeInfo.isMsgType(JCEMESSAGETYPEGRID.value)) {
			endpoint = getGridSelect(serviceInfos, invokeInfo);
			return endpoint;
		}
		endpoint = getHashSelect(serviceInfos, invokeInfo);
		if (null == endpoint) {
			endpoint = getNextSelect(serviceInfos, invokeInfo);
		}
		return endpoint;
	}

	/**
	 * 纯hash选择 没有设置hash直接返回null
	 * 
	 * @param serviceInfos
	 * @param invokeInfo
	 * @return
	 */
	public ServiceEndPointInfo getHashSelect(ServiceInfos serviceInfos, InvokeInfo invokeInfo) {
//		if (invokeInfo.isMsgType(JCEMESSAGETYPEHASH.value)) {
//			List<ServiceEndPointInfo> allServices = serviceInfos.getGridServices(0);
//			ServiceEndPointInfo alivePoint = allServices.get(invokeInfo.getInvokeHash() % allServices.size());
//			if (serviceCanUse(serviceInfos,alivePoint)) {
//				return alivePoint;
//			} else {
//				List<ServiceEndPointInfo> activeServices = serviceInfos.getGridActiveServices(0);
//				if (null != activeServices && activeServices.size() > 0) {
//					return activeServices.get(invokeInfo.getInvokeHash() % activeServices.size());
//				}
//			}
//		}
		if (invokeInfo.isMsgType(JCEMESSAGETYPEHASH.value)) {
			//直接从非灰度活的列表里面取
			List<ServiceEndPointInfo> activeUnGridService = serviceInfos.getGridActiveServices(0);
			if(activeUnGridService!= null && activeUnGridService.size()>0) {
				return activeUnGridService.get(invokeInfo.getInvokeHash()%activeUnGridService.size());
			}
			else {
				TafLoggerCenter.info(serviceInfos.objectName+" do not have active service in getHashSelect hash invoke");
			}
		}
		return null;
	}

	AtomicInteger nextPoint = new AtomicInteger();

	/**
	 * 纯顺序选择
	 * 
	 * @param serviceInfos
	 * @param invokeInfo
	 * @return
	 */
	public ServiceEndPointInfo getNextSelect(IServicesInfo serviceInfos, InvokeInfo invokeInfo) {
		List<ServiceEndPointInfo> allServices = serviceInfos.getGridServices(0);
		if (allServices.size() > 0) {
			if (nextPoint.get() == Integer.MAX_VALUE) {
				nextPoint.set(Integer.MAX_VALUE % allServices.size());
			}
			List<ServiceEndPointInfo> activeServices = serviceInfos.getGridActiveServices(0);
			if (null != activeServices && activeServices.size() > 0) {
				return activeServices.get(nextPoint.incrementAndGet() % activeServices.size());
			}
		}
		return null;
	}

	AtomicInteger gridPoint = new AtomicInteger();

	/**
	 * 灰度选择
	 * 
	 * @param serviceInfos
	 * @param invokeInfo
	 * @return
	 */
	public ServiceEndPointInfo getGridSelect(ServiceInfos serviceInfos, InvokeInfo invokeInfo) {
		if (invokeInfo.isMsgType(JCEMESSAGETYPEGRID.value)) {
			List<ServiceEndPointInfo> gridServices = serviceInfos.getGridServices(invokeInfo.getInvokeGrid());
			if (gridServices.size() > 0) {
				if (invokeInfo.getInvokeHash() == Const.INVALID_HASH_CODE) {
					// 顺序选择
					if (gridPoint.get() == Integer.MAX_VALUE) {
						gridPoint.set(Integer.MAX_VALUE % gridServices.size());
					}
					List<ServiceEndPointInfo> gridActiveServices = serviceInfos.getGridActiveServices(invokeInfo
							.getInvokeGrid());
					if (null != gridActiveServices && gridActiveServices.size() > 0) {
						ServiceEndPointInfo alivePoint = gridActiveServices.get(gridPoint.incrementAndGet()
								% gridActiveServices.size());
						return alivePoint;
					} else {
						TafLoggerCenter.info(invokeInfo.getObjectName() + " get gridActiveServices "
								+ invokeInfo.getInvokeGrid() + " is null");
					}
				} else {
					// hash选择
//					ServiceEndPointInfo alivePoint = gridServices.get(invokeInfo.getInvokeHash() % gridServices.size());
//					if (serviceCanUse(serviceInfos,alivePoint)) {
//						return alivePoint;
//					} else {
//						List<ServiceEndPointInfo> gridActiveServices = serviceInfos.getGridActiveServices(invokeInfo
//								.getInvokeGrid());
//						if (null != gridActiveServices && gridActiveServices.size() > 0) {
//							return gridActiveServices.get(invokeInfo.getInvokeHash() % gridActiveServices.size());
//						}
//					}
					
					List<ServiceEndPointInfo> gridActiveServices = serviceInfos.getGridActiveServices(invokeInfo
							.getInvokeGrid());
					if (null != gridActiveServices && gridActiveServices.size() > 0) {
						return gridActiveServices.get(invokeInfo.getInvokeHash() % gridActiveServices.size());
					}
					else {
						TafLoggerCenter.info(serviceInfos.objectName+" do not have active service in getGridSelect hash invoke");
					}
				}
			} else {
				if (serviceInfos.isNoGrid()) {
					throw new TafProxyConfigException("gridValue is " + invokeInfo.getInvokeGrid()
							+ ", but no gridService can use");
				}
			}
		}
		return null;
	}

	/**
	 * service是否可用
	 * 
	 * @param alivePoint
	 * @return
	 */
	public boolean serviceCanUse(ServiceInfos infos , ServiceEndPointInfo alivePoint) {
		return !infos.failedService(alivePoint) && !alivePoint.isFailed();
	}

}
