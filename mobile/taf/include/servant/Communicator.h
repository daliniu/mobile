#ifndef __TAF_COMMUNICATOR_H_
#define __TAF_COMMUNICATOR_H_

#include "util/tc_thread.h"
#include "util/tc_config.h"
#include "util/tc_singleton.h"
#include "util/tc_timeout_queue.h"
#include "servant/Global.h"
#include "servant/ServantProxy.h"
#include "servant/ServantProxyFactory.h"
#include "servant/ObjectProxyFactory.h"
#include "servant/AsyncProcThread.h"
#include "servant/FDReactor.h"
#include "servant/QueueTimeout.h"

#define CONFIG_ROOT_PATH "/taf/application/client"

namespace taf
{
////////////////////////////////////////////////////////////////////////
/**
 * 客户端配置
 */
struct ClientConfig
{
    /**
     * 客户端IP地址
     */
    static string LocalIp;
    /**
     * 客户端模块名称
     */
    static string ModuleName;
    /**
     * 客户端所有的IP地址
     */
    static set<string>  SetLocalIp;
   /**
   *客户端是否打开set分组
   */
   static bool SetOpen;
   /**
   *客户端set分组
   */
   static string SetDivision;

};
////////////////////////////////////////////////////////////////////////
/**
 * 通信器,用于创建和维护客户端proxy
 */
class Communicator : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * 构造函数
     */
    Communicator();

    /**
     * 采用配置构造
     * @param conf
     * @param path
     */
    Communicator(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * 析够
     * 析够时自动接收相关线程
     */
    ~Communicator();

    /**
     * 生成代理
     * @param T
     * @param objectName
     * @return T
     */
    template<class T> T stringToProxy(const string& objectName)
    {
        T prx = NULL;

        stringToProxy<T>(objectName, prx);

        return prx;
    }

    /**
     * 生成代理
     * @param T
     * @param objectName
     * @param proxy
     */
    template<class T> void stringToProxy(const string& objectName, T& proxy)
    {

        ServantPrx prx = getServantProxy(objectName);

        proxy = (typename T::element_type*)(prx.get());
    }

public:
    /**
     * 获取属性
     * @param name
     * @param dft, 缺省值
     * @return string
     */
    string getProperty(const string& name, const string& dft = "");

    /**
     * 设置属性
     * @param properties
     */
    void setProperty(const map<string, string>& properties);

    /**
     * 设置某一个属性
     * @param name
     * @param value
     */
    void setProperty(const string& name, const string& value);

    /**
     * 设置属性
     * @param conf
     * @param path
     */
    void setProperty(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * 上报统计
     * @return StatReport*
     */
    StatReport* getStatReport();

    /**
     * 重新加载属性
     */
    int reloadProperty(string & sResult);
    
    /**
     * 获取obj对应可用ip port列表  如果启用分组,只返回同分组的服务端ip 
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint(const string & objName);

    /**
     * 获取obj对应可用ip port列表 包括所有IDC的
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All(const string & objName);
    

    /**
     * 设置隧道透传
     * @param sTunnel 隧道obj名称
     * @param sRuler  规则
     * setTunnel("MDWWX.*.*|Comm.*.*","MDWWX.TunnelServer.TunnelObj")
     */

    void setTunnel(const string &sRuler,const string &sTunnel);

    /**
     * 结束
     */
    void terminate();
    
    
    /**
     * 获取对象生成器
     * @return ObjectProxyFactoryPtr
     */
    ObjectProxyFactory* objectProxyFactory();

    /**
     * 获取对象代理生成器
     * @return ServantProxyFactoryPtr
     */
    ServantProxyFactory* servantProxyFactory();

protected:
    /**
     * 初始化
     */
    void initialize();

    /**
     * 是否析构中
     * @return bool
     */
    bool isTerminating();

    /**
     * 由Property初始化客户端配置
     */
    void initClientConfig();


    /**
     * fd反应器
     * @return FDReactorPtr
     */
    FDReactor* fdReactor();

    /**
     * 获取异步处理线程
     * @return AsyncProcThreadPtr
     */
    AsyncProcThread* asyncProcThread();

    /**
     * 获取通用对象
     * @param objectName
     * @return ServantPrx
     */
    ServantPrx getServantProxy(const string& objectName);


    /**
     * 根基是否启用隧道，获取隧道名称
     * @param objectName
     * @return string
     */
    string getTunnelName(const string& objectName);

    /**
     * 上报模块间调用
     * @param req
     */
    void reportToStat(const ReqMessagePtr& req, const string& obj, int result);

    /**
     * 上报模块间调用 采样
     * @param req
     */
    void sampleToStat(const ReqMessagePtr& req, const string& obj, const string& ip);

    /**
     * 框架内部需要直接访问通信器的类
     */
    friend class ReqMessage;

    friend class BaseProxy;

    friend class FDReactor;

    friend class QueueTimeout;

    friend class AdapterProxy;

    friend class ServantProxy;

    friend class ObjectProxy;

    friend class ServantProxyFactory;

    friend class ObjectProxyFactory;

    friend class AsyncProcThread;

    friend class AsyncProcThreadRunner;

protected:
    bool _initialized;

    bool _terminating;
        
    map<string, vector<string> > _mTunnel;

    map<string, string> _properties;

    ObjectProxyFactoryPtr _objectProxyFactory;

    ServantProxyFactoryPtr _servantProxyFactory;

    StatReportPtr _statReportPtr;

    FDReactorPtr _fdReactor;

    QueueTimeoutPtr _qTimeout;

    AsyncProcThreadPtr _asyncProcThread;
};
////////////////////////////////////////////////////////////////////////
}
#endif
