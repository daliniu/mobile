package com.duowan.taf.proxy;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import com.duowan.netutil.nio.mina2.fliter.codec.ProtocolCodecFilter;
import com.duowan.taf.ClientInfo;
import com.duowan.taf.EndpointF;
import com.duowan.taf.QueryFPrxHelper;
import com.duowan.taf.StatFPrxHelper;
import com.duowan.taf.StatMicMsgHead;
import com.duowan.taf.cnst.JCESERVERSUCCESS;
import com.duowan.taf.holder.JceArrayListHolder;
import com.duowan.taf.proxy.codec.JceCodecFactory;
import com.duowan.taf.proxy.conn.EndPointAdapter;
import com.duowan.taf.proxy.conn.ProxyConnRouter;
import com.duowan.taf.proxy.conn.ServiceEndPointInfo;
import com.duowan.taf.proxy.exec.TafProxyConfigException;
import com.duowan.taf.proxy.exec.TafProxyException;
import com.duowan.taf.proxy.utils.NameThreadFactory;
import com.duowan.taf.proxy.utils.TafUtils;

/**
 * 通讯器 负责创建Proxy 内部会定期上报模块调用信息
 * 
 * @author albertzhu
 * 
 */
public class Communicator {

	QueryFPrxHelper queryPrx = null;
	StatFPrxHelper statPrxPrx = null;
	
	
	/**
	 * 存放已经生成的Proxy 避免重复创建
	 */
	ConcurrentHashMap<ProxyConfig, ServantProxy> jceProxyMap = new ConcurrentHashMap<ProxyConfig, ServantProxy>();
	/**
	 * 上报模块调用信息间隔时间 为了适应ping的需要10秒唤醒1次，每6次上报一回
	 */
	
	//private static volatile ScheduledExecutorService reportTaskExecutor;
	/**
	 * 网络包编码解析过滤器
	 */
	private ProtocolCodecFilter protocolCodec = new ProtocolCodecFilter(new JceCodecFactory(true));
	/**
	 * 网络接口适配器
	 */
	EndPointAdapter endPointAdapter = new EndPointAdapter(protocolCodec);
	/**
	 * 客户端配置
	 */
	CommunicatorConfig communicatorConfig = null;
	boolean _initialized = false;
	// 默认的模块调用信息统计区间
	//List<Integer> defaultTimeStatInterv = new ArrayList<Integer>();
	//CopyOnWriteArrayList<Integer> timeStatInterv = new CopyOnWriteArrayList<Integer>();

	TafThreadPool _tafThreadPool = null;

	//CommunStatFPrxCallback communStatFPrxCallback = new CommunStatFPrxCallback();

	public Communicator() {
		communicatorConfig = CommunicatorConfig.getDefaultConfig();
		TafLoggerCenter.info(ClientInfo.showString());
	}

	/**
	 * 指定客户端初始化配置文件
	 * 
	 * @param configFileName
	 */
	public Communicator(String configFileName) {
		communicatorConfig = CommunicatorConfig.parseQueryConfig(configFileName);
		TafLoggerCenter.info(ClientInfo.showString());
	}

	protected Communicator(CommunicatorConfig config) {
		communicatorConfig = config;

		TafLoggerCenter.info(ClientInfo.showString());
	}
	
	/**
	 * 根据对象名获取所有连接信息
	 * @param objectName
	 */
	public List<EndpointF> getEndpoint4All(String objectName) {
		if(queryPrx == null) {
			this.createQueryProxy();
		}
		return queryPrx.findObjectById(objectName);
		//createProxyConfigByName(objectName).serviceInfos.getAllServices();
	}

	/**
	 * 指定objectName 创建proxy
	 * 
	 * @param <T>
	 * @param objectName
	 * @param api
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public <T> T stringToProxy(String objectName, Class api) {
		initialize();
		try {
			ProxyConfig config = createProxyConfigByName(objectName);
			putProxyToMap(api, config);
			return (T) jceProxyMap.get(config);
		} catch (Exception e) {
			throw new TafProxyException("init proxy error \n" + e, e);
		}
	}

	/**
	 * 如果map中没有proxy，创建proxy然后放入map中
	 * 
	 * @param api
	 * @param config
	 * @throws ClassNotFoundException
	 * @throws NoSuchMethodException
	 * @throws InstantiationException
	 * @throws IllegalAccessException
	 * @throws InvocationTargetException
	 */
	private void putProxyToMap(Class api, ProxyConfig config) throws ClassNotFoundException, NoSuchMethodException,
			InstantiationException, IllegalAccessException, InvocationTargetException {
		if (!jceProxyMap.containsKey(config)) {
			ProxyConnRouter conns = new ProxyConnRouter(communicatorConfig.getModulename(), config, endPointAdapter);
			Class newoneClass = Class.forName(api.getName());
			Constructor cons = newoneClass.getConstructor();
			ServantProxy proxy = (ServantProxy) cons.newInstance();
			if (config.fromRegisterCenter) {
				proxy.taf_init(config, conns, ProxyStatTool.Instance.getStatIntervals(), queryPrx);
				proxy.setStatPrxPr(statPrxPrx);
			} else {
				// 自己指定IP端口 则不从配置中心查询
				proxy.taf_init(config, conns, ProxyStatTool.Instance.getStatIntervals(), null);
				proxy.setStatPrxPr(statPrxPrx);
			}
			proxy.setMaxSampleCount(communicatorConfig.getMaxSampleCount());
			proxy.setSampleRate(communicatorConfig.getSampleRate());
			jceProxyMap.putIfAbsent(config, proxy);
		}
	}

	public void setProperty(Map<String, String> properties) {
		communicatorConfig.setProperty(properties);
	}

	public void setProperty(String name, String value) {
		communicatorConfig.setProperty(name, value);
	}

	public String getProperty(String name, String defaultValue) {
		return communicatorConfig.getProperty(name, defaultValue);
	}

	/**
	 * locator地址
	 * 
	 * @return
	 */
	public String getLocator() {
		return communicatorConfig.getLocator();
	}

	/**
	 * locator地址
	 * 
	 * @param locator
	 */
	public void setLocator(String locator) {
		communicatorConfig.setLocator(locator);
		closeQueryProxy();
		createQueryProxy();
	}

	/**
	 * 模块间调用
	 * 
	 * @return
	 */
	public String getStat() {
		return communicatorConfig.getStat();
	}

	/**
	 * 模块间调用
	 * 
	 * @param stat
	 */
	public void setStat(String stat) {
		communicatorConfig.setStat(stat);
		createReportProxy();
	}

	/**
	 * 本地模块名
	 * 
	 * @return
	 */
	public String getModulename() {
		return communicatorConfig.getModulename();
	}

	/**
	 * 本地模块名
	 * 
	 * @param modulename
	 */
	public void setModulename(String modulename) {
		communicatorConfig.setModulename(modulename);
	}

	/**
	 * 调用超时时间
	 * 
	 * @return
	 */
	public int getMaxInvokeTimeout() {
		return communicatorConfig.getAsyncInvokeTimeout();
	}

	/**
	 * 调用超时时间
	 * 
	 * @param maxInvokeTimeout
	 */
	public void setMaxInvokeTimeout(int maxInvokeTimeout) {
		communicatorConfig.setAsyncInvokeTimeout(maxInvokeTimeout);
	}


	/**
	 * 同步调用超时时间
	 * 
	 * @return
	 */
	public int getSyncTimeout() {
		return communicatorConfig.getSyncInvokeTimeout();
	}

	/**
	 * 同步调用超时时间
	 * 
	 * @param maxInvokeTimeout
	 */
	public void setSyncTimeout(int maxInvokeTimeout) {
		communicatorConfig.setSyncInvokeTimeout(maxInvokeTimeout);
	}
	
	/**
	 * 刷新端口时间
	 * 
	 * @return
	 */
	public int getRefreshEndpointInterval() {
		return communicatorConfig.getRefreshEndpointInterval();
	}

	/**
	 * 刷新端口时间
	 * 
	 * @param refreshEndpointInterval
	 */
	public void setRefreshEndpointInterval(int refreshEndpointInterval) {
		communicatorConfig.setRefreshEndpointInterval(refreshEndpointInterval);
	}

	/**
	 * 异步发送线程个数
	 * 
	 * @return
	 */
	public int getSendThreadNum() {
		return communicatorConfig.getSendThreadNum();
	}

	/**
	 * 异步发送线程个数
	 * 
	 * @param sendThreadNum
	 */
	public void setSendThreadNum(int sendThreadNum) {
		communicatorConfig.setSendThreadNum(sendThreadNum);
	}

	/**
	 * 异步回调线程个数
	 * 
	 * @return
	 */
	public int getAsyncCallBackThreadNum() {
		return communicatorConfig.getAsyncCallBackThreadNum();
	}

	/**
	 * 异步回调线程个数
	 * 
	 * @param asyncthreadNum
	 */
	public void setAsyncCallbackThreadNum(int asyncthreadNum) {
		communicatorConfig.setAsyncCallbackThreadNum(asyncthreadNum);
	}

	/**
	 * 获取模块间调用信息耗时统计阀值
	 * 
	 * @return
	 */
	public Set<Integer> getStatInterv() {
		Set<Integer> interv = new HashSet<Integer>(ProxyStatTool.Instance.getStatIntervals());
		return interv;
	}

	/**
	 * 重置模块间调用信息耗时统计阀值
	 */
	public void resetStatInterv() {
		ProxyStatTool.Instance.resetStatInterv();
	}

	/**
	 * 增加模块间调用信息耗时统计阀值
	 * 
	 * @param interv
	 */
	public void addStatInterv(int interv) {
		ProxyStatTool.Instance.addStatInterv(interv);
	}

	public synchronized TafThreadPool getTafThreadPool() {
		if (null == _tafThreadPool) {
			_tafThreadPool = new TafThreadPool(communicatorConfig.getSendThreadNum(), communicatorConfig
					.getAsyncCallBackThreadNum());
		}
		return _tafThreadPool;
	}

	// 以下为私有方法//////////////////////////////////////////////////////////////////

	protected StatFPrxHelper getStatPrx() {
		if(statPrxPrx ==null) {
			synchronized(this) {
				if(statPrxPrx ==null) {
					initialize();
				}
			}
		}
		return statPrxPrx;
	}
	/**
	 * 初始化
	 */
	public synchronized void initialize() {
		if (!_initialized) {
			createReportProxy();
			ProxyStatTool.Instance.init(this.statPrxPrx);
			TafTimer.initProxyStat(ProxyStatTool.Instance);
			_initialized = true;
		}
	}

	/**
	 * 对传入的字符串进行解析 创建ClientConfig
	 * 
	 * @param objectName
	 * @return
	 */
	private ProxyConfig createProxyConfigByName(String objectName) {
		ProxyConfig config = null;
		int point = objectName.indexOf("@");
		if (point > 0 && point < objectName.length()) {
			config = communicatorConfig.getConfig(objectName, getTafThreadPool());
		} else {
			createQueryProxy();
			config = getProxyConfigFromRegister(objectName);
			if (null == config) {
				throw new TafProxyConfigException("getProxyConfigFromRegister " + objectName + " failed");
			}
			config.fromRegisterCenter = true;
		}
		return config;
	}



	/**
	 * 创建QueryFPrxHelper
	 */
	private synchronized void createQueryProxy() {
		if (null != communicatorConfig.getLocator()) {
			if (null == queryPrx) {
				try {
					ProxyConfig proxyConfig = communicatorConfig.getConfig(communicatorConfig.getLocator(),
							getTafThreadPool());
					ProxyConnRouter conns = new ProxyConnRouter(communicatorConfig.getModulename(), proxyConfig,
							endPointAdapter);
					queryPrx = new QueryFPrxHelper();
					queryPrx.taf_init(proxyConfig, conns, ProxyStatTool.Instance.getStatIntervals(), queryPrx);
					queryPrx.taf_referNodeInterv(1000*60*60);//一个小时更新一次主控配置
					queryPrx.setMaxSampleCount(communicatorConfig.getMaxSampleCount());
					queryPrx.setSampleRate(communicatorConfig.getSampleRate());
					jceProxyMap.putIfAbsent(proxyConfig, queryPrx);
				} catch (Exception e) {
					TafLoggerCenter.info("createQueryProxy error", e);
				}
			}
		} else {
			// 需要使用queryPrx 却没有配置属性
			throw new TafProxyConfigException("not set communicator locator property");
		}
	}

	/**
	 * 关闭当前的queryPrx 重设Locator的时候触发
	 */
	private synchronized void closeQueryProxy() {
		if (null != queryPrx) {
			queryPrx.closeAllConns();
			jceProxyMap.remove(queryPrx.taf_proxyConfig());
			queryPrx = null;
			TafLoggerCenter.info("reSet queryProxy success");
		}
	}

	/**
	 * 创建StatPrxHelper 如果未设置stat 则返回null
	 */
	private synchronized void createReportProxy() {
		if (null != communicatorConfig.getStat()) {
			if (null == statPrxPrx) {
				try {
					createQueryProxy();
					ProxyConfig reportConfig = getProxyConfigFromRegister(communicatorConfig.getStat());
					if (null != reportConfig) {
						ProxyConnRouter conns = new ProxyConnRouter(communicatorConfig.getModulename(), reportConfig,
								endPointAdapter);
						statPrxPrx = new StatFPrxHelper();
						reportConfig.statName = communicatorConfig.getStat();
						reportConfig.locatorName = communicatorConfig.getLocator();
						statPrxPrx.taf_init(reportConfig, conns, ProxyStatTool.Instance.getStatIntervals(), queryPrx);
						statPrxPrx.setMaxSampleCount(communicatorConfig.getMaxSampleCount());
						statPrxPrx.setSampleRate(communicatorConfig.getSampleRate());
						if(queryPrx != null) {
							queryPrx.setStatPrxPr(statPrxPrx);
						}
						statPrxPrx.setStatPrxPr(statPrxPrx);
						jceProxyMap.putIfAbsent(reportConfig, statPrxPrx);
					} else {
						TafLoggerCenter.error("not set queryObj property, can not query reportConfig");
					}
				} catch (Exception e) {
					TafLoggerCenter.info("createReportProxy error" ,e);
				}
			}
		}
	}
	
	ConcurrentHashMap<String, ConcurrentHashMap<String,ProxyConfig>> loctorProxyConfigMap = new ConcurrentHashMap<String, ConcurrentHashMap<String,ProxyConfig>>();

	/**
	 * 从注册中心拉取.proxy的配置信息
	 * 
	 * @param objectName
	 * @return
	 */
	private ProxyConfig getProxyConfigFromRegister(String objectName) {
		if ( !loctorProxyConfigMap.containsKey(communicatorConfig.getLocator()) ) {
			loctorProxyConfigMap.putIfAbsent(communicatorConfig.getLocator(), new ConcurrentHashMap<String,ProxyConfig>());
		} else {
			ConcurrentHashMap<String,ProxyConfig> configMap = loctorProxyConfigMap.get(communicatorConfig.getLocator());
			if (configMap.containsKey(objectName)) {
				return configMap.get(objectName);
			}
		}
		if (null == queryPrx) {
			createQueryProxy();
		}
		if (null != queryPrx) {
			ProxyConfig fromRegisterconfig = new ProxyConfig(objectName, getTafThreadPool());
			// 设定默认的超时时间
			fromRegisterconfig.setSyncTimeoutMill(communicatorConfig.getSyncInvokeTimeout());
			fromRegisterconfig.setAsyncTimeoutMill(communicatorConfig.getAsyncInvokeTimeout());
			try {
				JceArrayListHolder activeEp = new JceArrayListHolder();
				JceArrayListHolder inactiveEp = new JceArrayListHolder();
				int _ret = queryPrx.findObjectByIdInSameGroup(objectName, activeEp, inactiveEp);
				if (_ret == JCESERVERSUCCESS.value) {
					ServiceInfos serviceInfos = new ServiceInfos(fromRegisterconfig.objectName);
					serviceInfos
							.setServices(activeEp.value, inactiveEp.value);
					fromRegisterconfig.serviceInfos = serviceInfos;
				} else {
					TafLoggerCenter.info(objectName + " findObjectById4All error , return " + _ret);
					throw new TafProxyConfigException("can not get " + objectName + " nodeList from register");
				}
				TafLoggerCenter.info("get config from register " + fromRegisterconfig);
			} catch (Exception ex) {
				TafLoggerCenter.info("get config from register erroe " + ex);
				Pair<ArrayList<ServiceEndPointInfo>, ArrayList<ServiceEndPointInfo>> p = TafUtils.getServerInfoData(fromRegisterconfig
						.getObjectName(), fromRegisterconfig.syncTimeoutMill, fromRegisterconfig.asyncTimeoutMill,
						fromRegisterconfig.serviceConnNum);
				if (null != p && (p.k.size() > 0 || p.v.size() > 0)) {
					ServiceInfos serviceInfos = new ServiceInfos(fromRegisterconfig.objectName);
					serviceInfos.setServiceInfos(p.k, p.v);
					fromRegisterconfig.serviceInfos = serviceInfos;
					TafLoggerCenter.debug("callback_findObjectById4All get config from cache " + serviceInfos);
				} else {
					TafLoggerCenter.info("cacheServerData " + fromRegisterconfig.getObjectName()
							+ " serviceList is empty ");
				}

			}
			loctorProxyConfigMap.get(communicatorConfig.getLocator()).putIfAbsent(objectName, fromRegisterconfig);
			return fromRegisterconfig;
		} else {
			throw new TafProxyConfigException("can not init queryPrx");
		}

	}

	
	//单独调用上报接口填入的信息
	//ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody> reportMsg = new ConcurrentHashMap<StatMicMsgHead, SendMsgStatBody>();
	
	/**
	 * 上报信息
	 * @param strModuleName 被调模块. 一般采用app.servername 例如:Comm.BindServer
	 * @param strInterfaceName 被调接口名
	 * @param strModuleIp 被调ip
	 * @param iPort 被调port
	 * @param eResult 成功STAT_SUCC，超时 STAT_TIMEOUT，异常STAT_EXC.
	 * @param iSptime 耗时 毫秒
	 * @param iReturnValue 接口返回值,不区分返回值 直接填为0(由于返回值多样化，taf框架的自动上报也未启用此字段)
	 */
	public void report(String strModuleName, String strInterfaceName, String strModuleIp, int iPort,
			StatResult eResult, int iSptime, int iReturnValue) {
		initialize();
		StatTool tool = ProxyStatTool.Instance.getDefaultStatTool();
		StatMicMsgHead head = tool.addStat(communicatorConfig.getModulename(), strModuleName, strInterfaceName,
				"", strModuleIp, iPort, iReturnValue);
		tool.addInvokeTime(head, iSptime, eResult);
	}

}