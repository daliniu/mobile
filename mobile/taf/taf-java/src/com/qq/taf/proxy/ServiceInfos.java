package com.qq.taf.proxy;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.ReentrantReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock.ReadLock;
import java.util.concurrent.locks.ReentrantReadWriteLock.WriteLock;

import com.qq.jutil.string.StringUtil;
import com.qq.taf.EndpointF;
import com.qq.taf.proxy.conn.ServiceEndPoint;
import com.qq.taf.proxy.conn.ServiceEndPointInfo;

/**
 * 存放registerCenter返回的service信息
 * 
 * @author albertzhu
 * 
 */
public class ServiceInfos implements IServicesInfo {

	Map<Integer, List<ServiceEndPointInfo>> gridServices = new HashMap<Integer, List<ServiceEndPointInfo>>();

	Map<Integer, List<ServiceEndPointInfo>> gridActiveServices = new HashMap<Integer, List<ServiceEndPointInfo>>();
	
	List<ServiceEndPointInfo> srcActiveServices = new ArrayList<ServiceEndPointInfo>();

	List<ServiceEndPointInfo> activeServices = new ArrayList<ServiceEndPointInfo>();

	List<ServiceEndPointInfo> inactiveServices = new ArrayList<ServiceEndPointInfo>();

	List<ServiceEndPointInfo> allServices = new ArrayList<ServiceEndPointInfo>();
	
	private HashSet<String> activeServiceKeys = new HashSet<String>();

	ReentrantReadWriteLock rwl = new ReentrantReadWriteLock();

	ReadLock rl = rwl.readLock();
	WriteLock wl = rwl.writeLock();

	String objectName;

	public ServiceInfos(String objectName) {
		this.objectName = objectName;
	}

	public void setServiceFail(ServiceEndPointInfo serviceinfo) {
		try {
			wl.lock();
			for (ServiceEndPointInfo e : allServices) {
				if (e.getKey().equals(serviceinfo.getKey())) {
					e.setFailed(true);
					activeServices.remove(e);
					activeServiceKeys.remove(e.getKey());
					if (!inactiveServices.contains(e)) {
						inactiveServices.add(e);
					}
					if (null != gridActiveServices.get(e.getGrid())) {
						gridActiveServices.get(e.getGrid()).remove(e);
					}
				}
			}
		} finally {
			wl.unlock();
		}
		TafLoggerCenter.debug(objectName + " " + serviceinfo + " setServiceFail activeServices:" + activeServices
				+ " inactiveServices:" + inactiveServices);
	}

	public void setServiceFail(ServiceEndPoint service) {
		try {
			wl.lock();
			for (ServiceEndPointInfo e : allServices) {
				if (e.getKey().equals(service.getInfo().getKey())) {
					e.setFailed(true);
					activeServices.remove(e);
					activeServiceKeys.remove(e.getKey());
					if (!inactiveServices.contains(e)) {
						inactiveServices.add(e);
					}
					if ( null != gridActiveServices.get(e.getGrid()) ) {
						gridActiveServices.get(e.getGrid()).remove(e);
					}
				}
			}
		} finally {
			wl.unlock();
		}
		TafLoggerCenter.debug(objectName + " " + service.getInfo() + " setServiceFail activeServices:" + activeServices
				+ " inactiveServices:" + inactiveServices);
	}

	public void setServiceGrid(ServiceEndPoint service, int newGridValue) {
		String key = service.getInfo().getKey();
		List<ServiceEndPointInfo> allServices = getAllServices();
		try {
			wl.lock();
			for (ServiceEndPointInfo s : allServices) {
				if (s.getKey().equals(key)) {
					int oldGrid = s.getGrid();
					gridServices.get(oldGrid).remove(s);
					gridActiveServices.get(oldGrid).remove(s);
					s.setGrid(newGridValue);
					if ( null == gridServices.get(newGridValue) ) {
						gridServices.put(newGridValue, new ArrayList<ServiceEndPointInfo>());
					}
					if (!gridServices.get(newGridValue).contains(s)) {
						gridServices.get(newGridValue).add(s);
					}
					if ( null == gridActiveServices.get(newGridValue) ) {
						gridActiveServices.put(newGridValue, new ArrayList<ServiceEndPointInfo>());
					}
					if (!gridActiveServices.get(newGridValue).contains(s)) {
						gridActiveServices.get(newGridValue).add(s);
					}
					TafLoggerCenter.info(objectName + " setServiceGrid,old gridValue is "+oldGrid+", now gridValue is " + newGridValue
							+" "+ service.getInfo());
				}
			}
		} finally {
			wl.unlock();
		}
		TafLoggerCenter.info(objectName + " " + service.getInfo() + " setServiceGrid gridServices:" + gridServices
				+ " gridActiveServices:" + gridActiveServices);
	}

	public void setReconnInterv(int reconnInterv) {
		try {
			wl.lock();
			for (ServiceEndPointInfo e : allServices) {
				e.setFailedWaitTimeSecond(reconnInterv);
			}
		} finally {
			wl.unlock();
		}
	}

	public void setServiceActive(ServiceEndPoint service) {
		try {
			wl.lock();
			for (ServiceEndPointInfo e : allServices) {
				if (e.getKey().equals(service.getInfo().getKey())) {
					e.setFailed(false);
					if (!activeServices.contains(e)) {
						activeServices.add(e);
						activeServiceKeys.add(e.getKey());
					}
					if ( null == gridActiveServices.get(e.getGrid()) ) {
						gridActiveServices.put(e.getGrid(), new ArrayList<ServiceEndPointInfo>());
					}
					if (!gridActiveServices.get(e.getGrid()).contains(e)) {
						gridActiveServices.get(e.getGrid()).add(e);
					}
					inactiveServices.remove(e);
				}
			}
		} finally {
			wl.unlock();
		}
		TafLoggerCenter.debug(objectName + " " + service.getInfo() + " setServiceActive activeServices:" + activeServices
				+ " inactiveServices:" + inactiveServices);
	}

	/* (non-Javadoc)
	 * @see com.qq.taf.proxy.IServicesInfo#getActiveServices()
	 */
	public List<ServiceEndPointInfo> getActiveServices() {
		try {
			rl.lock();
			return activeServices;
		} finally {
			rl.unlock();
		}
	}

	/* (non-Javadoc)
	 * @see com.qq.taf.proxy.IServicesInfo#getInactiveServices()
	 */
	public List<ServiceEndPointInfo> getInActiveServices() {
		try {
			rl.lock();
			return inactiveServices;
		} finally {
			rl.unlock();
		}
	}
	
	public boolean failedService( ServiceEndPointInfo info) {
		try {
			rl.lock();
			return inactiveServices.contains(info);
		} finally {
			rl.unlock();
		}
	}

	/* (non-Javadoc)
	 * @see com.qq.taf.proxy.IServicesInfo#getGridServices(int)
	 */
	public List<ServiceEndPointInfo> getGridServices(int grid) {
		try {
			rl.lock();
			return gridServices.get(grid);
		} finally {
			rl.unlock();
		}
	}

	/* (non-Javadoc)
	 * @see com.qq.taf.proxy.IServicesInfo#getGridActiveServices(int)
	 */
	public List<ServiceEndPointInfo> getGridActiveServices(int grid) {
		try {
			rl.lock();
			return gridActiveServices.get(grid);
		} finally {
			rl.unlock();
		}
	}

	/* (non-Javadoc)
	 * @see com.qq.taf.proxy.IServicesInfo#getAllServices()
	 */
	public List<ServiceEndPointInfo> getAllServices() {
		try {
			rl.lock();
			return allServices;
		} finally {
			rl.unlock();
		}
	}

	/**
	 * 服务器是否只有一种状态
	 * 
	 * @return
	 */
	public boolean isNoGrid() {
		try {
			rl.lock();
			return gridServices.size() == 1;
		} finally {
			rl.unlock();
		}
	}

	public String toString() {
		StringBuffer sb = new StringBuffer();
		sb.append(objectName);
		for (ServiceEndPointInfo e : allServices) {
			sb.append(";" + e.toFullString());
		}
		return sb.toString();
	}

	public void setServices(String queryEndpoints, int syncInvokeTimeout, int asyncInvokeTimeout, int serviceConnNum) {
		ArrayList<ServiceEndPointInfo> activeEpList = new ArrayList<ServiceEndPointInfo>();
		ArrayList<ServiceEndPointInfo> inactiveEpList = new ArrayList<ServiceEndPointInfo>();
		String[] items = StringUtil.split(queryEndpoints, ":");
		for (String item : items) {
			ServiceEndPointInfo node = ServiceEndPointInfo.parseNode(objectName,item, syncInvokeTimeout, asyncInvokeTimeout,
					serviceConnNum);
			if (node.isActive()) {
				activeEpList.add(node);
			} else {
				inactiveEpList.add(node);
			}
		}
		setServiceInfos(activeEpList, inactiveEpList);
	}

	public ArrayList<ServiceEndPointInfo> transEndpintF2ServiceEndPointInfo(ArrayList<EndpointF> oldFs, boolean active) {
		ArrayList<ServiceEndPointInfo> infos = new ArrayList<ServiceEndPointInfo>(oldFs.size());
		for (int i = 0; i < oldFs.size(); i++) {
			ServiceEndPointInfo info = new ServiceEndPointInfo(objectName,oldFs.get(i));
			info.setActive(active);
			infos.add(info);
		}
		Collections.sort(infos);
		return infos;
	}

	public void setServices(ArrayList<EndpointF> activeEp, ArrayList<EndpointF> inactiveEp) {
		setServiceInfos(transEndpintF2ServiceEndPointInfo(activeEp,true), transEndpintF2ServiceEndPointInfo(inactiveEp,false));
	}


	void setServiceInfos(ArrayList<ServiceEndPointInfo> activeEp, ArrayList<ServiceEndPointInfo> inactiveEp) {

		Map<Integer, List<ServiceEndPointInfo>> _gridServices = new HashMap<Integer, List<ServiceEndPointInfo>>();
		Map<Integer, List<ServiceEndPointInfo>> _gridActiveServices = new HashMap<Integer, List<ServiceEndPointInfo>>();
		List<ServiceEndPointInfo> _activeServices = new ArrayList<ServiceEndPointInfo>();
		HashSet<String> _activeServiceKeys = new HashSet<String>();
		List<ServiceEndPointInfo> _inactiveServices = new ArrayList<ServiceEndPointInfo>();
		List<ServiceEndPointInfo> _allServices = new ArrayList<ServiceEndPointInfo>();
		List<ServiceEndPointInfo> _srcActiveServices = new ArrayList<ServiceEndPointInfo>();

		for (ServiceEndPointInfo e : activeEp) {
			if (!_gridServices.containsKey(e.getGrid())) {
				_gridServices.put(e.getGrid(), new ArrayList<ServiceEndPointInfo>());
			}
			if (!_gridActiveServices.containsKey(e.getGrid())) {
				_gridActiveServices.put(e.getGrid(), new ArrayList<ServiceEndPointInfo>());
			}
			_gridServices.get(e.getGrid()).add(e);
			_gridActiveServices.get(e.getGrid()).add(e);
			_activeServices.add(e);
			_activeServiceKeys.add(e.getKey());
			_allServices.add(e);
			_srcActiveServices.add(e);
		}
		for (ServiceEndPointInfo e : inactiveEp) {
			if (!_gridServices.containsKey(e.getGrid())) {
				_gridServices.put(e.getGrid(), new ArrayList<ServiceEndPointInfo>());
			}
			_gridServices.get(e.getGrid()).add(e);
			_inactiveServices.add(e);
			_allServices.add(e);
		}
		try {
			wl.lock();
			srcActiveServices = _srcActiveServices;
			gridServices = _gridServices;
			gridActiveServices = _gridActiveServices;
			activeServices = _activeServices;
			activeServiceKeys = _activeServiceKeys;
			inactiveServices = _inactiveServices;
			allServices = _allServices;
		} finally {
			wl.unlock();
		}
		StringBuffer sb = new StringBuffer();
		sb.append(objectName + " setServices gridServices:" + gridServices + " gridActiveServices:"
				+ gridActiveServices);
		sb.append(" allServices: ");
		for ( ServiceEndPointInfo i : allServices) {
			sb.append(i+";");
		}
		TafLoggerCenter.debug(sb.toString());
	}

	public boolean isActive(ServiceEndPointInfo info) {
		return activeServiceKeys.contains(info.getKey());
	}
	
	public boolean isActive(ServiceEndPoint point) {
		return activeServiceKeys.contains(point.getKey());
	}
	
	public String getObjectName() {
		return objectName;
	}

	public void setObjectName(String objectName) {
		this.objectName = objectName;
	}

	public List<ServiceEndPointInfo> getSrcActiveServices() {
		return srcActiveServices;
	}

	@Override
	public ServiceEndPointInfo getGridActiveService(int grid, int seq) {
		try {
			rl.lock();
			List<ServiceEndPointInfo> list = gridActiveServices.get(grid);
			if(list == null||list.size() == 0) {return null;}
			else {
				return list.get(Math.abs(seq)%list.size());
			}
		} finally {
			rl.unlock();
		}
	}

	@Override
	public boolean hasGridServices(int grid) {
		try {
			rl.lock();
			List<ServiceEndPointInfo> list = gridServices.get(grid);
			return !(list == null||list.isEmpty());
		} finally {
			rl.unlock();
		}
	}

}
