package com.qq.taf.proxy;

import java.util.concurrent.atomic.AtomicInteger;

import com.qq.taf.cnst.Const;
import com.qq.taf.cnst.JCEMESSAGETYPEGRID;
import com.qq.taf.cnst.JCEMESSAGETYPEHASH;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;
import com.qq.taf.proxy.exec.TafProxyConfigException;

/**
 * 暴露给用户的服务器选择接口
 * 
 * @author albertzhu
 * 
 */
public class TAdapterSelector {


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
		if (invokeInfo.isMsgType(JCEMESSAGETYPEHASH.value)) {
			//直接从非灰度活的列表里面取
			ServiceEndPointInfo info = serviceInfos.getGridActiveService(0, invokeInfo.getInvokeHash());
			if(info!= null ) {
				return info;
			}
			else {
				TafLoggerCenter.info(serviceInfos.objectName+" do not have active service in getHashSelect hash invoke");
			}
		}
		return null;
	}

	private AtomicInteger nextPoint = new AtomicInteger();

	/**
	 * 纯顺序选择
	 * 
	 * @param serviceInfos
	 * @param invokeInfo
	 * @return
	 */
	public ServiceEndPointInfo getNextSelect(IServicesInfo serviceInfos, InvokeInfo invokeInfo) {
		if (serviceInfos.hasGridServices(0)) {
			ServiceEndPointInfo info = serviceInfos.getGridActiveService(0, nextPoint.incrementAndGet());
			if (null != info) {
				//TafLoggerCenter.debug(nextPoint.get()+"|"+activeServices);
				return info;
			}
		}
		return null;
	}

	private AtomicInteger gridPoint = new AtomicInteger();

	/**
	 * 灰度选择
	 * 
	 * @param serviceInfos
	 * @param invokeInfo
	 * @return
	 */
	public ServiceEndPointInfo getGridSelect(ServiceInfos serviceInfos, InvokeInfo invokeInfo) {
		if (invokeInfo.isMsgType(JCEMESSAGETYPEGRID.value)) {
			if (serviceInfos.hasGridServices(invokeInfo.getInvokeGrid())) {
				if (invokeInfo.getInvokeHash() == Const.INVALID_HASH_CODE) {
					// 顺序选择
					ServiceEndPointInfo info = serviceInfos.getGridActiveService(invokeInfo.getInvokeGrid(), gridPoint.incrementAndGet());
					if (null != info) {
						return info;
					} else {
						TafLoggerCenter.info(invokeInfo.getObjectName() + " get gridActiveServices "
								+ invokeInfo.getInvokeGrid() + " is null");
					}
				} else {
					// hash选择
					ServiceEndPointInfo info = serviceInfos.getGridActiveService(invokeInfo.getInvokeGrid(), invokeInfo.getInvokeHash());
					if (null != info) {
						return info;
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
