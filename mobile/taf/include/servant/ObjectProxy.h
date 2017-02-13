#ifndef __TAF_OBJECT_PROXY_H_
#define __TAF_OBJECT_PROXY_H_

#include "servant/Communicator.h"
#include "servant/Message.h"
#include "servant/AdapterProxy.h"
#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/AppProtocol.h"
#include "servant/Global.h"

namespace taf
{
class QueryEndpointImp;

/**
 * 用于保存服务端的一组列表
 * (按服务端灰度状态进行分组)
 */
struct AdapterProxyGroup
{
    AdapterProxyGroup();

    vector<AdapterProxy*> adapterProxys;

    vector<AdapterProxy*> adapterProxysDeleted;

    vector<AdapterProxy*> allProxys;

    int32_t lastRoundPosition;

    AdapterProxy* getHashProxy(int64_t hashCode);

    AdapterProxy* getNextValidProxy();
};

struct SocketOpt
{
    int level;

    int optname;

    const void *optval;

    socklen_t optlen;
};

///////////////////////////////////////////////////////////////////
/**
 * 每个objectname在进程空间唯一有一个objectproxy
 * 管理收发的消息队列
 */
class ObjectProxy : public TC_HandleBase, public TC_ThreadMutex //public BaseProxy
{
public:
    /**
     * 构造函数
     * @param comm
     * @param name
     */
    ObjectProxy(Communicator* comm, const string& name);

    /**
     * 析构函数
     */
    ~ObjectProxy();

    /**
     * 构造timeoutqueue
     *
     * @param comm
     * @param timeout
     *
     * @return TC_TimeoutQueue<ReqMessagePtr>*
     */
    static TC_TimeoutQueue<ReqMessagePtr>* createTimeoutQueue(Communicator *comm, int timeout);

    /**
     * 初始化消息队列等
     */
    void initialize();

    /**
     * 方法调用
     * @param req
     * @return int
     */
    int invoke(ReqMessagePtr& req);

    /**
     * 从队列中获取一个请求
     * @param req
     * @return bool
     */
    bool popRequest(ReqMessagePtr& req);

    /**
     * 获取object名称
     * @return const string&
     */
    const string& name() const;

    /**
     * 获取消息队列超时，超时后的消息被丢弃
     * @return int
     */
    int timeout() const;

    /**
     * 设置消息队列超时，超时后的消息被丢弃
     * @param msec
     */
    void timeout(int msec);

    /**
     * 超时策略获取和设置
     * @return CheckTimeoutInfo&
     */
    CheckTimeoutInfo& checkTimeoutInfo();

    /**
     * 设置协议解析器
     * @return UserProtocol&
     */
    void setProxyProtocol(const ProxyProtocol& protocol);

    /**
     * 获取协议解析器
     * @return ProxyProtocol&
     */
    ProxyProtocol& getProxyProtocol();

    /**
    *设置套接口选项
    */
    void setSocketOpt(int level, int optname, const void *optval, socklen_t optlen);

     /**
     *获取套接字选项
     */
     vector<SocketOpt>& getSocketOpt();

    /**
     * 获取可用服务列表 如果启用分组,只返回同分组的服务端ip
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * 获取所有服务列表  包括所有IDC
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All();

    /**
     * 获取所有服务列表  包括所有ID活动服务和非活动服务
     * @param vecActive    活动服务集合
     * @param vecInactive 非活动服务集合
     */
    void getEndpoint4All(vector<EndpointInfo> & vecActive, vector<EndpointInfo> & vecInactive);

    /**
     * 获取所有服务列表 包括指定归属地
     *  @return void
     */
    void getEndpoint4All(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);

    /**
     * 获取刷新服务列表的间隔
     * @return int
     */
    int refreshEndpointInterval() const;

    /**
     * 设置刷新服务列表的间隔
     * @param msecond
     */
    void refreshEndpointInterval(int msecond);

     /**
     * 设置缓存服务列表的间隔
     * @param msecond
     */
    void cacheEndpointInterval(int msecond);

    /**
     * 返回超时队列对象
     */
    inline TC_TimeoutQueue<ReqMessagePtr>* getTimeoutQueue() { return _timeoutQueue; }

    /**
     * 重置AdapterProxy的灰度状态
     * @param gridFrom
     * @param gridTo
     * @param adapter
     * @return bool
     */
    bool resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter);

    /**
     * 加载locator
     * @param
     * @return AdapterPrx
     */
    int loadLocator();

    /**
     * 设置PUSH类消息的callback对象
     * @param cb
     */
    void setPushCallbacks(const ServantProxyCallbackPtr& cb);

    /**
     * 获取PUSH类消息的callback对象
     */
    ServantProxyCallback* getOneRandomPushCallback();

    /**
     * 处理队列超时
     */
    void handleQueueTimeout();

    /**
     * 退出时处理阻塞同步调用的请求
     */
    void handleQuitReqMessage();
    /**
     * 设置是否可用共享内存通信
     */
    void setBusCommuEnable(bool enable) { _bBusCommuEnable = enable; }

    /**
     * 检查是否可以用共享内存通信
     */
    bool getBusCommuEnable() { return _bBusCommuEnable; }


    /**
     * 设置使用tunnel
     */
    void setTunnelEnabled(bool enable) { _bTunneleEnable = enable; }

    /**
     * 检查是否设置使用tunnel
     */
    bool getTunnelEnabled() { return _bTunneleEnable; }


protected:
    /**
     * 定时刷新服务列表（由registry）
     */
    void refreshEndpointInfos();

    /**
     * 排序服务列表
     */
    void sortEndpointInfos();

    /**
     * 刷新本地缓存列表，如果服务下线了，要求删除本地缓存
     */
    void refreshEndpoints(const set<EndpointInfo>& vecActive, const set<EndpointInfo>& vecInactive);
    //void printEndpointInfos();

    /**
     * 初始队列超时
     *
     * @param timeoutQueue
     */
    void handleQueueTimeout(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue);

    /**
     * 退出时处理阻塞同步调用的请求
     *
     * @param timeoutQueue
     */
    void handleQuitReqMessage(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue);

    /**
     * 选取一个AdapterProxy
     * @param req
     * @return AdapterPrx
     */
    AdapterProxy* selectAdapterProxy(const ReqMessagePtr& req);

    /**
     * 创建adapterproxy
     *
     * @param comm
     * @param ep
     * @param op
     * @return AdapterProxy*
     */
    AdapterProxy* createAdapterProxy(Communicator* comm, const EndpointInfo &ep);

    /**
     * 轮询选取
     * @param adapters
     * @return AdapterPrx
     */
    AdapterProxy* selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group);

    /**
     * 保存adapter到文件
     */
    void setEndPointToCache();

    /**
     * 合并服务列表
     *
     * @param del
     * @param add
     */
    void mergeEndPoint(set<EndpointInfo> &del, set<EndpointInfo> &add);

private:
    Communicator* _comm;

    bool    _bBusCommuEnable;

    string _name;

    string _locator;

    int32_t _timeout;

    //int32_t _maxTransNum;

    string _queryFObjectName;

    bool _isDirectProxy;

    bool _serverHasGrid;

    time_t _lastRefreshEndpointTime;

    int32_t _refreshEndpointInterval;

    time_t _lastCacheEndpointTime;

    int32_t _cacheEndpointInterval;

    int32_t _roundStartIndex;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

    ServantProxyCallback* _queryEndpoint;

    set<EndpointInfo> _activeEndpoints; //存活节点

    map<int32_t, AdapterProxyGroup> _adapterGroups; //按照灰度状态分组adapterproxy

    vector<AdapterProxy*>           _adapters;

    CheckTimeoutInfo _checkTimeoutInfo;

    ProxyProtocol _proxyProtocol;

    vector<SocketOpt> _SocketOpts;

    vector<ServantProxyCallbackPtr> _pushCallbacks;

    bool  _bTunneleEnable;

private:
    int32_t _lastRefreshEndpoint4AllTime;

    set<EndpointInfo> _activeEndpoints4All; //所有存活节点 包括不在同IDC的

    std::map<std::string, std::pair<std::vector<TC_Endpoint>, std::vector<TC_Endpoint> > > _mapEndpoints;//缓存不同归属地的节点

    pair<std::vector<EndpointInfo>, std::vector<EndpointInfo> > _allEndpoints; //<active,inactive>,所有活的和非活的节点
};
///////////////////////////////////////////////////////////////////////////////////
}
#endif
