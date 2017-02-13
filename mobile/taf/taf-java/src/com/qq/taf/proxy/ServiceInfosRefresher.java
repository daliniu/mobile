package com.qq.taf.proxy;

import java.util.ArrayList;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.taf.EndpointF;
import com.qq.taf.QueryFPrxCallback;
import com.qq.taf.QueryFPrxHelper;
import com.qq.taf.cnst.JCESERVERSUCCESS;
import com.qq.taf.holder.JceArrayListHolder;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;
import com.qq.taf.proxy.utils.TafUtils;

public class ServiceInfosRefresher implements Runnable {
	
	public static final ServiceInfosRefresher INSTANCE = new ServiceInfosRefresher();
	
	private ConcurrentHashMap<ServantProxy, ServiceInfos> configs;
	
	private ConcurrentHashMap<ServantProxy, Object> infos;
	
	private static int refreshInterval = 60*1000; 
	

	public ServiceInfosRefresher() {
		configs = new ConcurrentHashMap<ServantProxy, ServiceInfos>(128);
		infos = new ConcurrentHashMap<ServantProxy, Object>(128);
	}
	
	public static int getRefreshInterval() {
		return refreshInterval;
	}
	
	public static void setRefreshInterval(int intl) {
		refreshInterval = intl; 
	}
	
	public void addServantProxy(ServantProxy proxy) {
		if(!infos.contains(proxy)) {
			infos.putIfAbsent(proxy, new Object());
		}
		if(!configs.contains(proxy) && proxy.queryFPrx == null) {
			ServiceInfos serviceInfos = new ServiceInfos(proxy.proxyConfig.objectName);
			serviceInfos.setServiceInfos((ArrayList<ServiceEndPointInfo>)proxy.proxyConfig.serviceInfos.getActiveServices(), (ArrayList<ServiceEndPointInfo>)proxy.proxyConfig.serviceInfos.getInActiveServices());
			configs.putIfAbsent(proxy, serviceInfos);
		}
	}
	
	public void asyncGetServiceInfos(ServantProxy proxy) {
		if(proxy.canAsyncRefresh()) {
			asyncRefreshConfig(proxy, proxy.queryFPrx, proxy.proxyConfig.objectName);
			proxy.setProxyAyncRefreshTime();
		}
		else {
			TafLoggerCenter.info(proxy.proxyConfig.objectName+" can not async refresh config for refreshing in 2s or queryPrx is null");
		}
	}
	
	private ServiceInfos cloneServiceInfos(ServiceInfos info) {
		ServiceInfos serviceInfos = new ServiceInfos(info.objectName);
		serviceInfos.setServiceInfos((ArrayList<ServiceEndPointInfo>)info.getActiveServices(), (ArrayList<ServiceEndPointInfo>)info.getInActiveServices());
		return serviceInfos;
	}
	
	public ServiceInfos getServiceInfos(ServantProxy proxy) {
		ServiceInfos info = configs.get(proxy);
		if(info != null) {
			//防止引用内容被修改，需要额外拷贝一份
			return cloneServiceInfos(info);
		}
		else {
			synchronized(infos.get(proxy)) {
				info = configs.get(proxy);
				if(info == null) {
					info = refreshConfig(proxy, proxy.queryFPrx, proxy.proxyConfig.objectName);
					if(info != null) {
						configs.putIfAbsent(proxy, info);
					}
					else {
						TafLoggerCenter.error(proxy.serviceInfos.objectName+" sync get config is null");
					}
				}
			}
			if(info!=null) {
				return cloneServiceInfos(info);
			}
			return new ServiceInfos(proxy.serviceInfos.objectName);
		}
	}
	
	private void asyncRefreshConfig(ServantProxy proxy, QueryFPrxHelper queryFPrx, String objectName) {
		try {
			TafLoggerCenter.debug(objectName+" asyncRefreshConfig, "+proxy.proxyConfig.enableSet+", "+proxy.proxyConfig.setName+" "+proxy.proxyConfig.hashCode());
			QueryServiceInfoCallback queryServiceCallBack = new QueryServiceInfoCallback(proxy, objectName);
			ayncFindObjectByQuery(proxy.proxyConfig.enableSet, proxy.proxyConfig.setName, queryFPrx, queryServiceCallBack, objectName);
			//queryFPrx.async_findObjectById4All(queryServiceCallBack, objectName);
		} catch (Exception e) {
			TafLoggerCenter.error(objectName + " asyncRefreshConfig error", e);
		}
	}
	
	@SuppressWarnings("unchecked")
	private ServiceInfos refreshConfig(ServantProxy proxy, QueryFPrxHelper queryFPrx, String objectName) {
		boolean querySucc = false;
		ServiceInfos info = null;
		try {
			TafLoggerCenter.debug(objectName+" refreshConfig, "+proxy.proxyConfig.enableSet+", "+proxy.proxyConfig.setName);
			JceArrayListHolder activeEp = new JceArrayListHolder();
			JceArrayListHolder inactiveEp = new JceArrayListHolder();
			int _ret = findObjectByQuery(proxy.proxyConfig.enableSet, proxy.proxyConfig.setName, queryFPrx, objectName, activeEp, inactiveEp); 
				//queryFPrx.findObjectByIdInSameGroup(objectName, activeEp, inactiveEp);
			if (_ret == JCESERVERSUCCESS.value) {
				info = parseRespEndPointInfo(objectName, (activeEp.getValue()), (inactiveEp.getValue()));
				querySucc = true;
			} else {
				TafLoggerCenter.error(objectName + " findObjectByQuery error , return " + _ret);
			}
		} catch (Exception e) {
			TafLoggerCenter.error(objectName + " findObjectByQuery error " + e, e);
		}
		if (!querySucc) {
			ProxyConfig proxyConfig = proxy.proxyConfig;
			Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = TafUtils.getServerInfoData(proxyConfig
					.getObjectName(), proxyConfig.syncTimeoutMill, proxyConfig.asyncTimeoutMill,
					proxyConfig.serviceConnNum);
			//可用列表为空 认为全部可用
			if ( null != p  ) {
				info = parseRespServicePointInfo(objectName, p.k, p.v);
			}
		}
		return info;

	}
	
	private int findObjectByQuery(boolean isOpenSet, String setId, QueryFPrxHelper queryFPrx, String objectName, JceArrayListHolder activeEp, JceArrayListHolder inactiveEp) {
		if(isOpenSet) {
			return queryFPrx.findObjectByIdInSameSet(objectName, setId, activeEp, inactiveEp);
		}
		else {
			return queryFPrx.findObjectByIdInSameGroup(objectName, activeEp, inactiveEp);
		}
	}
	
	private void ayncFindObjectByQuery(boolean isOpenSet, String setId, QueryFPrxHelper queryFPrx, QueryServiceInfoCallback queryServiceCallBack, String objectName) {
		if(isOpenSet) {
			queryFPrx.async_findObjectByIdInSameSet(queryServiceCallBack, objectName, setId);
		}
		else {
			queryFPrx.async_findObjectByIdInSameGroup(queryServiceCallBack, objectName);
		}
	}
	/**
	 * 解析注册中心返回的endPoint列表并刷新proxyConns
	 * 
	 * @param activeEp
	 * @param inactiveEp
	 */
	private ServiceInfos parseRespEndPointInfo(String objectName, ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
		//可用列表为空 认为全部可用
		if(activeEp.size()>0) {
			//完全相信从主控拉取的列表 
			ServiceInfos info = new ServiceInfos(objectName);
			info.setServices(activeEp, inactiveEp);
			TafLoggerCenter.debug(objectName+" cacheServerData get config from " + info);
			return info;
		}
		else {
			//如果本次取得的活动列表为0，暂用上次取得的活动列表
			TafLoggerCenter.error(objectName+" callback_findObjectById4All get config from register active size = 0");
			return null;
		}
	}
	
	private ServiceInfos parseRespServicePointInfo(String objectName, ArrayList<ServiceEndPointInfo> activeEp, ArrayList<ServiceEndPointInfo> inactiveEp) {
		//完全相信主控
		if(activeEp.size()>0) {
			ServiceInfos info = new ServiceInfos(objectName);
			info.setServiceInfos(activeEp, inactiveEp);
			TafLoggerCenter.debug(objectName+" cacheServerData get config from " + info);
			return info;
		}
		else {
			//如果本次取得的活动列表为0，暂用上次取得的活动列表
			TafLoggerCenter.error(objectName+" callback_findObjectById4All get config from register active size = 0");
			return null;
		}
	}
	
	
	/**
	 * 从register中心查询服务器信息的回调方法
	 * 
	 * @author albertzhu
	 * 
	 */
	class QueryServiceInfoCallback extends QueryFPrxCallback {
		
		private ServantProxy proxy;
		private String objectName;
		
		public QueryServiceInfoCallback(ServantProxy proxy, String objectName) {
			this.proxy = proxy;
			this.objectName = objectName;
		}
		
		@Override
		public void callback_findObjectById_exception(int ret) {
			TafLoggerCenter.info(objectName+ " callback_findObjectById_exception " + ret);
		}


		@Override
		public void callback_findObjectById4All_exception(int ret) {
			TafLoggerCenter.info(objectName + " callback_findObjectById4All_exception " + ret);
		}

		@Override
		public void callback_findObjectById(ArrayList<EndpointF> _ret) {
			TafLoggerCenter.info(objectName + "callback_findObjectById is not used");
			
		}

		@Override
		public void callback_findObjectById4All(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				refreshRouter(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(objectName, proxy.proxyConfig.getEndPoingListString());
				TafLoggerCenter.debug(objectName+" callback_findObjectById4All finish");
			} else {
				refreshByLocal();
				TafLoggerCenter.error(objectName + "async findObjectById4All error , return " + _ret);
			}
			
		}

		@Override
		public void callback_findObjectByIdInSameGroup(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				refreshRouter(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(objectName, proxy.proxyConfig.getEndPoingListString());
				TafLoggerCenter.debug(objectName+" callback_findObjectByIdInSameGroup finish");

			} else {
				refreshByLocal();
				TafLoggerCenter.error(objectName + "async findObjectByIdInSameGroup error , return " + _ret);
			}
			
		}

		@Override
		public void callback_findObjectByIdInSameGroup_exception(int _iRet) {
			TafLoggerCenter.info(objectName + " callback_findObjectByIdInSameGroup_exception " + _iRet);
			
		}

		private void refreshRouter(ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			ServiceInfos info = parseRespEndPointInfo(objectName, activeEp, inactiveEp);
			if(info != null) {
				configs.put(proxy, info);
				ServiceInfos cinfo = cloneServiceInfos(info);
				proxy.proxyConnRouter.refreshEndPoints(cinfo);
				proxy.proxyConfig.setServiceInfos(cinfo);
				TafLoggerCenter.debug(objectName+" async refresh serviceinfos:"+info);
			}
		}
		
		private void refreshRouter2(ArrayList<ServiceEndPointInfo> activeEp, ArrayList<ServiceEndPointInfo> inactiveEp) {
			ServiceInfos info = parseRespServicePointInfo(objectName, activeEp, inactiveEp);
			if(info != null) {
				configs.put(proxy, info);
				ServiceInfos cinfo = cloneServiceInfos(info);
				proxy.proxyConnRouter.refreshEndPoints(cinfo);
				proxy.proxyConfig.setServiceInfos(cinfo);
				TafLoggerCenter.debug(objectName+" async refresh serviceinfos:"+info);
			}
		}
		
		private void refreshByLocal() {
			ProxyConfig proxyConfig = proxy.proxyConfig;
			Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = TafUtils.getServerInfoData(proxyConfig
					.getObjectName(), proxyConfig.syncTimeoutMill, proxyConfig.asyncTimeoutMill,
					proxyConfig.serviceConnNum);
			refreshRouter2(p.k, p.v);
		}
		
		@Override
		public void callback_findObjectByIdInSameStation(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				refreshRouter(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(objectName, proxy.proxyConfig.getEndPoingListString());
				TafLoggerCenter.debug(objectName+" callback_findObjectByIdInSameStation finish");

			} else {
				refreshByLocal();
				TafLoggerCenter.error(objectName + "async findObjectByIdInSameStation error , return " + _ret);
			}
		}

		@Override
		public void callback_findObjectByIdInSameStation_exception(int _iRet) {
			TafLoggerCenter.info(objectName + " callback_findObjectByIdInSameStation_exception " + _iRet);
			
		}

		@Override
		public void callback_findObjectByIdInSameSet(int _ret,
				ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
			if (_ret == JCESERVERSUCCESS.value) {
				refreshRouter(activeEp, inactiveEp);
				// 存储从register取回的结果
				TafUtils.setServerInfoData(objectName, proxy.proxyConfig.getEndPoingListString());
				TafLoggerCenter.debug(objectName+" callback_findObjectByIdInSameSet finish");

			} else {
				refreshByLocal();
				TafLoggerCenter.error(objectName + " async findObjectByIdInSameSet error , return " + _ret);
			}
		}

		@Override
		public void callback_findObjectByIdInSameSet_exception(int _iRet) {
			TafLoggerCenter.info(objectName + " callback_findObjectByIdInSameSet_exception " + _iRet);
			
		}
	}


	@Override
	public void run() {
		final Set<ServantProxy> proxys = infos.keySet();
		for(ServantProxy proxy:proxys) {
			if(proxy.queryFPrx != null) {
				asyncRefreshConfig(proxy, proxy.queryFPrx, proxy.proxyConfig.objectName);
			}
			else {
				TafLoggerCenter.info(proxy.proxyConfig.objectName +" proxy.queryFPrx is null, can not refresh serviceInfo");
			}
		}
	}
	
	

}
