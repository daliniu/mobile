#include "servant/ObjectProxy.h"
#include "servant/Communicator.h"
#include "servant/QueryEndpoint.h"
#include "servant/Global.h"
#include "servant/AppCache.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "log/taf_logger.h"

namespace taf
{
///////////////////////////////////////////////////////////////////////////////////
AdapterProxyGroup::AdapterProxyGroup()
: lastRoundPosition(0)
{
}

AdapterProxy* AdapterProxyGroup::getHashProxy(int64_t hashCode)
{
    //hash只用active节点，去掉inactive的。
    /*
    if (allProxys.empty())
    {
        LOG->error() << "[TAF][getHashProxy allProxys is empty]" << endl;
        return NULL;
    }

    // 1 allProxys从客户端启动之后，就不会再改变，除非有节点增加
    // 2 如果有增加节点，则allProxys顺序会重新排序,之前的hash会改变
    // 3 节点下线后，需要下次启动客户端后,allPorxys内容才会生效
    size_t hash = ((int64_t)hashCode) % allProxys.size();

    //被hash到的节点在主控是active的才走在流程
    if (allProxys[hash]->isActiveInReg() && allProxys[hash]->checkActive())
    {
        return allProxys[hash];
    }
    else
    { 
    */ 

        if(adapterProxys.empty())
        {
            LOG->error() << "[TAF][getHashProxy adapterProxys is empty]" << endl;
            return NULL;

        }
        //在active节点中再次hash
        vector<AdapterProxy*> thisHash = adapterProxys;
        vector<AdapterProxy*> conn;

        do
        {
            size_t hash = ((int64_t)hashCode) % thisHash.size();

            if (thisHash[hash]->checkActive())
            {
                return thisHash[hash];
            }
            if(!thisHash[hash]->isConnTimeout())
            {
                conn.push_back(thisHash[hash]);
            }
            thisHash.erase(thisHash.begin() + hash);
        }
        while(!thisHash.empty());

        if(conn.size() > 0)
        {
            //都有问题, 随机选择一个没有connect超时的发送
            AdapterProxy *adapterProxy = conn[((int64_t)hashCode) % conn.size()];

            //该proxy可能已经被屏蔽,需重新连一次
            adapterProxy->checkActive(true);
            return adapterProxy;
        }
        return NULL;
    //}
}

AdapterProxy* AdapterProxyGroup::getNextValidProxy()
{
    if (adapterProxys.empty())
    {
        LOG->error() << "[TAF][getNextValidProxy adapterProxys is empty]" << endl;
        return NULL;
    }

    vector<AdapterProxy*> conn;

    int32_t all = (int32_t)adapterProxys.size();

    while (all-- > 0)
    {
        ++lastRoundPosition;

        lastRoundPosition %= adapterProxys.size();

        if (adapterProxys[lastRoundPosition]->checkActive() == true)
        {
            return adapterProxys[lastRoundPosition];
        }
        if(!adapterProxys[lastRoundPosition]->isConnTimeout())
        {
            conn.push_back(adapterProxys[lastRoundPosition]);
        }
    }
    if(conn.size() > 0)
    {
        //都有问题, 随机选择一个没有connect超时的发送
        AdapterProxy *adapterProxy = conn[((uint32_t)rand() % conn.size())];

        //该proxy可能已经被屏蔽,需重新连一次
        adapterProxy->checkActive(true);
        return adapterProxy;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////
ObjectProxy::ObjectProxy(Communicator* comm, const string& name)
: _comm(comm)
, _bBusCommuEnable(false)
, _name(name)
, _timeout(5*1000)
, _isDirectProxy(false)
, _serverHasGrid(false)
, _lastRefreshEndpointTime(0)
, _refreshEndpointInterval(60*1000)
, _lastCacheEndpointTime(0)
, _cacheEndpointInterval(30*60*1000)
, _roundStartIndex(0)
, _timeoutQueue(NULL)
, _queryEndpoint(NULL)
, _bTunneleEnable(false)
, _lastRefreshEndpoint4AllTime(0)
{
    _proxyProtocol.requestFunc = ProxyProtocol::tafRequest;

    _proxyProtocol.responseFunc = ProxyProtocol::tafResponse;
}

ObjectProxy::~ObjectProxy()
{
    delete _timeoutQueue;

    _timeoutQueue = NULL;

    vector<AdapterProxy*>::iterator it = _adapters.begin();

    while (it != _adapters.end())
    {
        delete *it;

        ++it;
    }
    /*
    if( _queryEndpoint != NULL)
    {
        delete _queryEndpoint;
    }
    _queryEndpoint = NULL;
    */
}

TC_TimeoutQueue<ReqMessagePtr>* ObjectProxy::createTimeoutQueue(Communicator *comm, int timeout)
{
    int size = TC_Common::strto<int>(comm->getProperty("timeout-queue-size", "50000"));

    TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue = new TC_TimeoutQueue<ReqMessagePtr>(timeout, size); //默认hash_map size设置为5W

    return timeoutQueue;
}

AdapterProxy* ObjectProxy::createAdapterProxy(Communicator* comm, const EndpointInfo &ep)
{
    AdapterProxy* ap = new AdapterProxy(comm, ep, this);

    _adapterGroups[ep.grid()].adapterProxys.push_back(ap);

    //初始化或者有新的节点部署才会到这里
    _adapterGroups[ep.grid()].allProxys.push_back(ap);

    _adapters.push_back(ap);

    return ap;
}

void ObjectProxy::initialize()
{
    _timeoutQueue = createTimeoutQueue(_comm, _timeout);

    string::size_type n = _name.find_first_of('@');

    string endpoints;

    string name = _name;

    //[直接连接]指定服务的IP和端口列表
    if (n != string::npos)
    {
        _name = name.substr(0, n);

        _isDirectProxy = true;

        endpoints = name.substr(n + 1);
    }
    //[间接连接]通过registry查询服务端的IP和端口列表
    //[间接连接]第一次使用cache
    else
    {
        _locator = _comm->getProperty("locator");

        if (_locator.find_first_not_of('@') == string::npos)
        {
            LOG->error() << "[Locator is not valid:" << _locator << "]" << endl;

            throw TafRegistryException("locator is not valid:" + _locator);
        }
        QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(_locator);

        _queryEndpoint = new taf::QueryEndpoint(prx);

        endpoints = AppCache::getInstance()->get(_name,_locator);
    }

    vector<string> eps = TC_Common::sepstr<string>(endpoints, ":", false);

    for (uint32_t i = 0; i < eps.size(); ++i)
    {
        try
        {
            TC_Endpoint ep(eps[i]);

            EndpointInfo::EType type = (ep.isTcp() ? EndpointInfo::TCP : EndpointInfo::UDP);

            string sSetDivision;
            //解析set分组信息
            if (!_isDirectProxy)
            {
                string sep = " -s ";

                size_t pos = eps[i].rfind(sep);

                if (pos != string::npos)
                {
                    sSetDivision = TC_Common::trim(eps[i].substr(pos+sep.size()));

                    size_t endPos = sSetDivision.find(" ");

                    if (endPos != string::npos)
                    {
                        sSetDivision = sSetDivision.substr(0, endPos);
                    }
                }
            }

            EndpointInfo epi(ep.getHost(), ep.getPort(), type, ep.getGrid(),sSetDivision,ep.getQos());

            _activeEndpoints.insert(epi);

            //创建adapterproxy
            createAdapterProxy(_comm, epi);
        }
        catch (...)
        {
            LOG->error() << "[endpoints parse error:" << name << ":" << eps[i] << "]" << endl;
        }
    }

    sortEndpointInfos();

    _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);
}

int ObjectProxy::loadLocator()
{
    if(_isDirectProxy)
    {
        //直接连接
        return 0;
    }

    string locator = _comm->getProperty("locator");

    if (locator.find_first_not_of('@') == string::npos)
    {
        LOG->error() << "[Locator is not valid:" << locator << "]" << endl;

        return -1;
    }

    QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(locator);

    ((QueryEndpoint*)_queryEndpoint)->setLocatorPrx(prx);

    return 0;
}

ServantProxyCallback* ObjectProxy::getOneRandomPushCallback()
{
    if (!_pushCallbacks.empty())
    {
        return _pushCallbacks[((uint32_t)rand() % _pushCallbacks.size())].get();
    }
    return NULL;
}

void ObjectProxy::setPushCallbacks(const ServantProxyCallbackPtr& cb)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    _pushCallbacks.push_back(cb);
}

const string& ObjectProxy::name() const
{
    return _name;
}

int ObjectProxy::timeout() const
{
    return _timeout;
}

void ObjectProxy::setProxyProtocol(const ProxyProtocol& protocol)
{
    _proxyProtocol = protocol;
}

ProxyProtocol& ObjectProxy::getProxyProtocol()
{
    return _proxyProtocol;
}

void ObjectProxy::setSocketOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    SocketOpt socketOpt;

    socketOpt.level        = level;
    socketOpt.optname = optname;
    socketOpt.optval     = optval;
    socketOpt.optlen     = optlen;

    _SocketOpts.push_back(socketOpt);
}

vector<SocketOpt>& ObjectProxy::getSocketOpt()
{
    return _SocketOpts;
}

CheckTimeoutInfo& ObjectProxy::checkTimeoutInfo()
{
    return _checkTimeoutInfo;
}

void ObjectProxy::timeout(int msec)
{
    //保护，异步超时时间不能小于1秒
    if (msec >= 1000)
    {
        _timeout = msec;
    }
}

vector<TC_Endpoint> ObjectProxy::getEndpoint()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    refreshEndpointInfos();

    vector<TC_Endpoint>  v;

    for(set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid());

       v.push_back(ep);
    }
    return v;
}

vector<TC_Endpoint> ObjectProxy::getEndpoint4All()
{
    //直连 直接返回初始化时候的列表
    if (_isDirectProxy)
    {
        return getEndpoint();
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

    //如果是间接连接，通过registry查询服务列表
    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now) ||
        (_activeEndpoints4All.empty() && (_lastRefreshEndpoint4AllTime + 2 < now))) //2s保护策略
    {
        _lastRefreshEndpoint4AllTime = now;

        set<EndpointInfo> activeEndpoints = ((QueryEndpoint*)_queryEndpoint)->findObjectById(_name);

        if (!activeEndpoints.empty())
        {
            _activeEndpoints4All = activeEndpoints;
        }
    }

    vector<TC_Endpoint>  v;

    for (set<EndpointInfo>::iterator it = _activeEndpoints4All.begin(); it != _activeEndpoints4All.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid(),it->qos());

       v.push_back(ep);
    }
    return v;
}

void ObjectProxy::getEndpoint4All(vector<EndpointInfo> &vecActive, vector<EndpointInfo> &vecInactive)
{
    vecActive.clear();

    vecInactive.clear();

    //直连 直接返回初始化时候的列表
    if (_isDirectProxy)
    {
        set<EndpointInfo>::const_iterator siit =  _activeEndpoints.begin();

        while (siit != _activeEndpoints.end())
        {
            vecActive.push_back(*siit);

            ++siit;
        }
        return;
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

     if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now)
         || (_allEndpoints.first.empty() && _allEndpoints.second.empty() && (_lastRefreshEndpoint4AllTime + 2 < now)))
     {
        _lastRefreshEndpoint4AllTime = now;

        set<EndpointInfo> setActive, setInactive;

        ((QueryEndpoint*)_queryEndpoint)->findObjectById4All(_name, setActive, setInactive);

        set<EndpointInfo>::const_iterator sait =  setActive.begin();

        while (sait != setActive.end())
        {
            vecActive.push_back(*sait);

            ++sait;
        }

        set<EndpointInfo>::const_iterator siit =  setInactive.begin();

        while (siit!= setInactive.end())
        {
            vecInactive.push_back(*siit);

            ++siit;
        }

        _allEndpoints.first = vecActive;

        _allEndpoints.second = vecInactive;

        return;
     }

    vecActive = _allEndpoints.first;

    vecInactive = _allEndpoints.second;
}

void ObjectProxy::getEndpoint4All(const string& sStation, vector<TC_Endpoint>& vecActive, vector<TC_Endpoint>& vecInactive)
{
    vecActive.clear();

    vecInactive.clear();

    //直连 直接返回初始化时候的列表
    if (_isDirectProxy)
    {
        vecActive = getEndpoint();

        return;
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

    map<string, pair<vector<TC_Endpoint>, vector<TC_Endpoint> > >::iterator itEndpoint = _mapEndpoints.find(sStation);

    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now)
         || (itEndpoint == _mapEndpoints.end() && (_lastRefreshEndpoint4AllTime + 2 < now)))
    {
        set<EndpointInfo> setActive, setInactive;

        ((QueryEndpoint*)_queryEndpoint)->findObjectByStation(_name, sStation, setActive, setInactive);

        for (set<EndpointInfo>::iterator it = setActive.begin(); it != setActive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecActive.push_back(ep);
        }

        for (set<EndpointInfo>::iterator it = setInactive.begin(); it != setInactive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecInactive.push_back(ep);
        }

        _mapEndpoints[sStation].first = vecActive;

        _mapEndpoints[sStation].second= vecInactive;

        return ;
    }

    vecActive = _mapEndpoints[sStation].first;

    vecInactive = _mapEndpoints[sStation].second;
}

int ObjectProxy::refreshEndpointInterval() const
{
    return _refreshEndpointInterval;
}

void ObjectProxy::refreshEndpointInterval(int msecond)
{
    //保护，刷新服务端列表的时间不能小于1秒
    _refreshEndpointInterval = msecond > 1000 ? msecond : _refreshEndpointInterval;
}

void ObjectProxy::cacheEndpointInterval(int msecond)
{
    //保护，cache服务端列表的时间不能小于60秒 默认30分钟
    _cacheEndpointInterval = msecond > 1000*60 ? msecond : _cacheEndpointInterval;
}


int ObjectProxy::invoke(ReqMessagePtr& req)
{
    //选择一个远程服务的Adapter来调用
    AdapterProxy* adp = selectAdapterProxy(req);

    if (!adp)
    {
        LOG->error() << "[TAF][invoke, " << _name << ", select adapter proxy ret NULL]" << endl;

        return -2;
    }
    req->adapter = adp;
    return adp->invoke(req);
}

//由后续的AdapterProxy取出一个消息发送
bool ObjectProxy::popRequest(ReqMessagePtr& req)
{
    //从FIFO队列中获取一条消息进行发送
    return _timeoutQueue->pop(req);
}

void ObjectProxy::mergeEndPoint(set<EndpointInfo> &del, set<EndpointInfo> &add)
{
    LOGINFO("[TAF][mergeEndPoint," << _name << ", add:" << add.size() << ",del:" << del.size() << ",isGrid:" << _serverHasGrid << "]" << endl);

    //删除本地无效的服务节点
    for (set<EndpointInfo>::iterator it = del.begin(); it != del.end(); ++it)
    {
        map<int32_t, AdapterProxyGroup>::iterator mit = _adapterGroups.find(it->grid());

        if (mit == _adapterGroups.end())
        {
            continue;
        }

        vector<AdapterProxy*>::iterator vit = mit->second.adapterProxys.begin();

        while (vit != mit->second.adapterProxys.end())
        {
            if ((*it) == (*vit)->endpoint())
            {
                //该节点在主控的状态为inactive
                (*vit)->setActiveInReg(false);

                //保持删除的对象, 不delete, 避免多线程crash, 理论上一个服务不会有太多这种删除的adapter
                mit->second.adapterProxysDeleted.push_back(*vit);

                mit->second.adapterProxys.erase(vit);

                break;
            }
            ++vit;
        }
    }

    //添加本地还没有的节点
    for (set<EndpointInfo>::iterator it = add.begin(); it != add.end(); ++it)
    {
        map<int32_t, AdapterProxyGroup>::iterator mit = _adapterGroups.find(it->grid());

        if (mit == _adapterGroups.end())
        {
            //创建新的adapterproxy
            createAdapterProxy(_comm, *it);
        }
        else
        {
            vector<AdapterProxy*>::iterator vit = mit->second.adapterProxysDeleted.begin();

            //标识在delete组中是否已经有要添加的节点
            bool bFoundInDeletedGrp = false;

            while (vit != mit->second.adapterProxysDeleted.end())
            {
                //if ((*it).equalNoSetInfo((*vit)->endpoint()))
                if ((*it) == (*vit)->endpoint())
                {
                    //该节点在主控的状态为active
                    (*vit)->setActiveInReg(true);
                    _adapterGroups[it->grid()].adapterProxys.push_back(*vit);
                    mit->second.adapterProxysDeleted.erase(vit);
                    bFoundInDeletedGrp = true;
                    break;
                }
                ++vit;
            }

            //不存在, 添加这个adapter
            //(需要两个条件同时判断，否则存在内存泄漏风险)
            if (!bFoundInDeletedGrp && vit == mit->second.adapterProxysDeleted.end())
            {
                createAdapterProxy(_comm, *it);
            }
        }
    }

    //将最新的列表保存起来，下次刷新时使用
    _activeEndpoints.clear();

    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        for (uint32_t i = 0; i < it->second.adapterProxys.size(); ++i)
        {
            _activeEndpoints.insert(it->second.adapterProxys[i]->endpoint());
        }

        LOGINFO("[TAF][mergeEndPoint," << _name << ", grid:" << it->first << ",proxy:" << it->second.adapterProxys.size() << ",delete proxy:" << it->second.adapterProxys.size()  << "]" << endl);

    }
}

void ObjectProxy::sortEndpointInfos()
{
    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        map<string, AdapterProxy*> mpAdapter;

        for (uint32_t i = 0; i < it->second.adapterProxys.size(); ++i)
        {
            mpAdapter.insert(pair<string, AdapterProxy*>(it->second.adapterProxys[i]->endpoint().desc(), it->second.adapterProxys[i]));
        }

        it->second.adapterProxys.clear();
        for (map<string, AdapterProxy*>::iterator mit = mpAdapter.begin(); mit != mpAdapter.end(); ++mit)
        {
            it->second.adapterProxys.push_back(mit->second);
        }
        LOGINFO("adapterProxys has : " << it->second.adapterProxys.size() << endl);

        map<string, AdapterProxy*> mpAllAdapter;
        for (uint32_t i = 0; i < it->second.allProxys.size(); ++i)
        {
            mpAllAdapter.insert(pair<string, AdapterProxy*>(it->second.allProxys[i]->endpoint().desc(), it->second.allProxys[i]));
        }

        it->second.allProxys.clear();

        for (map<string, AdapterProxy*>::iterator mit = mpAllAdapter.begin(); mit != mpAllAdapter.end(); ++mit)
        {
            //inactive的节点都在mergeEndPoint里面设置setActiveInReg
            it->second.allProxys.push_back(mit->second);
        }
        LOGINFO("allProxys has : " << it->second.allProxys.size() << endl);
    }
}

void ObjectProxy::refreshEndpoints(const set<EndpointInfo>& vecActive, const set<EndpointInfo>& vecInactive)
{
    /**
     * 删除allProxys已经下线的节点,
     * 判断已经下线的条件是:
     * (1)在allProxys中
     * (2)在主控状态为inactive
     * (3)不在vecInactive中的节点
     */
    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        //(1)在allProxys中
        vector<AdapterProxy*> vTempAdapter;
        bool bDelete = false;
        vector<AdapterProxy*>::iterator vit = it->second.allProxys.begin();
        while(vit != it->second.allProxys.end())
        {
            //(2)在主控状态为inactive
            if (!(*vit)->isActiveInReg())
            {
                bool bInvecInactive = false;
                for (set<EndpointInfo>::iterator itIn = vecInactive.begin(); itIn != vecInactive.end(); ++itIn)
                {
                    if((*itIn) == (*vit)->endpoint())
                    {
                        bInvecInactive = true;
                        break;
                    }
                }
                //(3)不在vecInactive中的节点,认为节点已经下线了,删除
                if(!bInvecInactive)
                {
                    bDelete = true;
                    LOG->debug()<<"refreshEndpoints: delete adaptproxy :"<<(*vit)->endpoint().desc()<<endl;
                }
                else
                {
                    vTempAdapter.push_back(*vit);
                }
            }
            else
            {
                vTempAdapter.push_back(*vit);
            }
            vit++;
        }
        //有更新,size>0做个保护
        if(bDelete && vTempAdapter.size() > 0)
        {
            it->second.allProxys.swap(vTempAdapter);
        }
    }
}

//定时刷新服务列表
void ObjectProxy::refreshEndpointInfos()
{
    try
    {
        //如果是直连的，则用初始化时候的服务列表
        //因为不会删除无效的AdapterProxy，所以不用刷新
        if (_isDirectProxy)
        {
            return;
        }

        time_t now = TC_TimeProvider::getInstance()->getNow();

        //如果是间接连接，通过registry定时查询服务列表
        if ((_lastRefreshEndpointTime + _refreshEndpointInterval/1000 < now) ||
            (_adapterGroups.empty() && (_lastRefreshEndpointTime + 2 < now))) //2s保护策略
        {
            _lastRefreshEndpointTime = now;

            //初始化时使用同步调用，以后用异步刷新
            bool sync = (_activeEndpoints.empty() || (_lastCacheEndpointTime == 0));
            if(ClientConfig::SetOpen)
            {
                ((QueryEndpoint*)_queryEndpoint)->findObjectByIdInSameSet(_name, ClientConfig::SetDivision, sync);
            }
            else
            {
                ((QueryEndpoint*)_queryEndpoint)->findObjectById4All(_name, sync);
            }
        }

        //不加锁判断是否已经有刷新
        if (!((QueryEndpoint*)_queryEndpoint)->hasRefreshed())
        {
            return;
        }

        set<EndpointInfo> activeEps;

        set<EndpointInfo> inactiveEps;

        set<int32_t> allGridCodes;

        //异步调用还没有callback回来
        if (!((QueryEndpoint*)_queryEndpoint)->hasNewEndpoints(activeEps, inactiveEps, allGridCodes))
        {
            LOGINFO("[TAF][refreshEndpointInfos,`findObjectById4All` hasNewEndpoints false:" << _name << "]" << endl);

            return;
        }

        //如果registry返回Active服务列表为空，不做更新
        if (activeEps.empty())
        {
            LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` ret activeEps is empty:" << _name << "]" << endl;

            return;
        }

        set<EndpointInfo> del; //需要删除的服务地址

        set<EndpointInfo> add; //需要增加的服务地址

        set_difference(_activeEndpoints.begin(), _activeEndpoints.end(), activeEps.begin(), activeEps.end(), inserter(del,del.begin()));

        set_difference(activeEps.begin(), activeEps.end(), _activeEndpoints.begin(), _activeEndpoints.end(), inserter(add,add.begin()));

        //如果服务端只有一种状态，则忽略路由设置
        _serverHasGrid = (allGridCodes.size() > 1 ? true : false);

        //没有需要更新的服务节点
        if (del.empty() && add.empty())
        {
            //need to refresh
            refreshEndpoints(activeEps,inactiveEps);
            return;
        }

        //合并列表
        mergeEndPoint(del, add);

        sortEndpointInfos();
        //allProxys refresh
        refreshEndpoints(activeEps,inactiveEps);
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << ":" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << "]" << endl;
    }
}

//从指定的一组adapter中选取一个有效的
AdapterProxy* ObjectProxy::selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group)
{
    //如果有hash，则先使用hash策略
    if (req->hashCode != ThreadPrivateData::INVALID_HASH_CODE)
    {
        return group.getHashProxy(req->hashCode);
    }

    return group.getNextValidProxy();
}

void ObjectProxy::setEndPointToCache()
{
    time_t now = TC_TimeProvider::getInstance()->getNow();

    //非直连的需要定时更新缓存服务列表
    if (!_isDirectProxy && _lastCacheEndpointTime + _cacheEndpointInterval/1000 < now)
    {
        _lastCacheEndpointTime = now;

        string sEndpoints = "";

        for (set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
        {
            bool isTcp = (it->type() == EndpointInfo::TCP ? true : false);

            TC_Endpoint ep(it->host(), it->port(), _timeout, isTcp, it->grid());

            if (sEndpoints != "") sEndpoints += ":";

            sEndpoints += ep.toString();

            if (!it->setDivision().empty())
            {
                sEndpoints += " -s " + it->setDivision();
            }
        }

        AppCache::getInstance()->set(_name,sEndpoints,_locator);

        LOGINFO("[TAF][setEndPointToCache,obj:" << _name << ",endpoint:" << sEndpoints << "]" << endl);
    }
}

void ObjectProxy::handleQueueTimeout()
{
    handleQueueTimeout(_timeoutQueue);

    TC_LockT<TC_ThreadMutex> lock(*this);

    vector<AdapterProxy*>::iterator it = _adapters.begin();

    while(it != _adapters.end())
    {
        handleQueueTimeout((*it)->getTimeoutQueue());

        ++it;
    }
}

void ObjectProxy::handleQueueTimeout(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue)
{
    static TC_TimeoutQueue<ReqMessagePtr>::data_functor df(&ReqMessage::timeout);

    timeoutQueue->timeout(df);
}

//从可用的服务列表选择一个服务节点
AdapterProxy* ObjectProxy::selectAdapterProxy(const ReqMessagePtr& req)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    refreshEndpointInfos();

    //没有可用的服务列表，直接返回NULL，业务会收到异常
    if (_adapterGroups.empty())
    {
        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",adapter proxy groups is empty!]" << endl;

        return NULL;
    }

    //保存服务列表到文件
    setEndPointToCache();

    int32_t gridCode = 0;

    bool isValidGrid = false;

    string gridKey = "";

    const map<string, string>& status = req->request.status;

    //如果是灰度路由消息，检测其有效性
    if (IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        map<string, string>::const_iterator keyIt = status.find(ServantProxy::STATUS_GRID_KEY);

        map<string, string>::const_iterator codeIt = status.find(ServantProxy::STATUS_GRID_CODE);

        if (keyIt != status.end() && codeIt != status.end())
        {
            gridCode = TC_Common::strto<int32_t>(codeIt->second);

            if (gridCode != ThreadPrivateData::INVALID_GRID_CODE)
            {
                isValidGrid = true;
            }
        }
    }

    //有效的路由消息，且服务端有多种状态
    if (isValidGrid == true && _serverHasGrid == true)
    {
        map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridCode);

        if (it != _adapterGroups.end())
        {
            return selectFromGroup(req, it->second);
        }

        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",grid router fail,gridKey:" << gridKey << "->gridCode:" << gridCode << "]" << endl;

        return NULL;
    }

    //非路由消息或服务端只有一种状态(有可能引起reset响应)
    return selectFromGroup(req, _adapterGroups.begin()->second);
}

//灰度路由发送到服务端，服务端检测如果状态不匹配，则返回reset消息和正确的状态
//客户端将该服务的adapter转移到正确的group，但是不会改变本地adapter的endpoint数据
//注意：如果服务端检测状态不匹配，但是服务端状态为0，则不会返回reset
bool ObjectProxy::resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    LOGINFO("[TAF][resetAdapterGrid," << _name << ",reset adapter grid:" << gridFrom << "->" << gridTo <<":" << adapter->endpoint().desc() << "]" << endl);

    map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridFrom);

    if (gridFrom == gridTo || it == _adapterGroups.end())
    {
        return false;
    }

    vector<AdapterProxy*>& vap = it->second.adapterProxys;

    for (vector<AdapterProxy*>::iterator vit = vap.begin(); vit != vap.end(); ++vit)
    {
        if ((*vit)->endpoint() == adapter->endpoint())
        {
            //放入正确的group
            _adapterGroups[gridTo].adapterProxys.push_back(*vit);

            //在现有的group中删除
            vap.erase(vit);

            _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);

            return true;
        }
    }

    vector<AdapterProxy*>& vAllap = it->second.allProxys;

    for (vector<AdapterProxy*>::iterator vit = vAllap.begin(); vit != vAllap.end(); ++vit)
    {
        if ((*vit)->endpoint() == adapter->endpoint())
        {
            //放入正确的group
            _adapterGroups[gridTo].allProxys.push_back(*vit);

            //在现有的group中删除
            vAllap.erase(vit);

            _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);

            return true;
        }
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////////
}
