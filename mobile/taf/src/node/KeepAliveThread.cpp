#include "KeepAliveThread.h"
#include "RegistryProxy.h"

KeepAliveThread::KeepAliveThread( )
{
    _bShutDown          = false;
    _pRegistryPrx       = NULL;
    _tRunTime           = time(0);
    _tNodeInfo          = _tPlatformInfo.getNodeInfo();
    _iHeartTimeout      = TC_Common::strto<int>(g_pconf->get("/taf/node/keepalive<heartTimeout>","10"));
    _iMonitorInterval   = TC_Common::strto<int>(g_pconf->get("/taf/node/keepalive<monitorInterval>","2"));
    _iSynInterval       = TC_Common::strto<int>(g_pconf->get("/taf/node/keepalive<synStatInterval>","60"));
    _sSynStatBatch      = g_pconf->get("/taf/node/keepalive<synStatBatch>","Y");
    _iMonitorInterval   = _iMonitorInterval>10?10:(_iMonitorInterval<1?1:_iMonitorInterval);
}

KeepAliveThread::~KeepAliveThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}
void KeepAliveThread::terminate()
{
    LOG->debug() << "[KeepAliveThread terminate.]" << endl;

    _bShutDown = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void KeepAliveThread::run()
{
    bool bLoaded        = false;
    bool bRegistered    = false;
    while (!_bShutDown)
    {
        try
        {
            //获取主控代理
            if(!_pRegistryPrx)
            {
                _pRegistryPrx = AdminProxy::getInstance()->getRegistryProxy();
                if(!_pRegistryPrx)
                {
                    LOG->debug()<<"RegistryProxy init  fail !  retry it after "+TC_Common::tostr(_iMonitorInterval)+" second"<<endl;
                }
            }
            //注册node信息
            if(_pRegistryPrx && bRegistered == false)
            {
                bRegistered = registerNode();
            }

            //加载服务
            if( bLoaded == false )
            {
                bLoaded = loadAllServers();
            }

			//检查服务的limit配置是否需要更新
			if( bLoaded )
			{
				ServerFactory::getInstance()->setAllServerResourceLimit();
			}
            //检查服务
            checkAlive();

            // 上报node状态
            if(reportAlive() != 0)
            {
                bRegistered = false; //registry服务重启  需要重新注册
            }
        }
        catch(exception& e)
        {
            LOG->error()<<"KeepAliveThread::run catch exception|"<<e.what()<<endl;
        }
        catch(...)
        {
            LOG->error()<<"KeepAliveThread::run catch unkown exception|"<<endl;
        }
        TC_ThreadLock::Lock lock(*this);

        timedWait(_iMonitorInterval*1000);
    }
}

bool KeepAliveThread::registerNode()
{
    LOG->debug() << "registerNode begin===============|node name|"<< _tNodeInfo.nodeName<< endl;
    try
    {
        int iRet = _pRegistryPrx->registerNode(_tNodeInfo.nodeName, _tNodeInfo, _tPlatformInfo.getLoadInfo());

        if(iRet == 0)
        {
            LOG->debug()<<"register node succ"<<endl;
            return true;
        }
    }
    catch ( exception& e )
    {
        LOG->error() << "KeepAliveThread::registerNode catch exception|"<<e.what() << endl;
    }
    LOG->debug()<<"register node fail"<<endl;
    return false;
}

bool KeepAliveThread::loadAllServers()
{
    LOG->debug() << "load server begin===============|node name|"<< _tNodeInfo.nodeName<< endl;
    try
    {
        if(ServerFactory::getInstance()->loadServer())
        {
            LOG->debug()<<"load server succ"<<endl;
            return true;
        }
    }
    catch ( exception& e )
    {
        LOG->error() << "KeepAliveThread::loadAllServers catch exception|"<<e.what() << endl;
    }
    LOG->debug()<<"load server fail"<<endl;
    return false;
}

int KeepAliveThread::reportAlive()
{
    try
    {
        static time_t tReport;
        time_t tNow =  TC_TimeProvider::getInstance()->getNow();
        if(tNow - tReport > _iHeartTimeout)
        {
            tReport = tNow;
            LOG->debug() << "node keep alive  ----------------------------------------------------|" << time( 0 ) << "|" << pthread_self() << '\n' << endl;
            int iRet = _pRegistryPrx->keepAlive(_tNodeInfo.nodeName, _tPlatformInfo.getLoadInfo());

            return iRet;
        }
    }
    catch ( exception& e )
    {
        LOG->error() << "KeepAliveThread::reportAlive catch exception|"<<e.what() << endl;
    }
    return 0;
}

int KeepAliveThread::synStat()
{
    try
    {
        int iRet = -1;
        vector<ServerStateInfo> v;
        _vStat.swap(v);
        LOG->debug() << "node syn stat  ----------------------------------------------------size|" << v.size() << "|" <<_sSynStatBatch<<"|"<< pthread_self() << '\n' << endl;

        if(v.size() > 0)
        {
            iRet = _pRegistryPrx->updateServerBatch(v);
        }
        return iRet;
    }
    catch ( exception& e )
    {
        string s = e.what();
        if(s.find("server function mismatch exception") != string::npos)
        {
            _sSynStatBatch = "N";
        }
        LOG->error() << "KeepAliveThread::synStat catch exception|"<<s<< endl;
    }
    LOG->debug()<<"KeepAliveThread::synStat fail,set SynStatBatch = "<<_sSynStatBatch<<endl;
    return -1;
}

void KeepAliveThread::checkAlive()
{
    static time_t tSyn = 0;
    bool bNeedSynServerState = false;
    time_t tNow =  TC_TimeProvider::getInstance()->getNow();
    if(tNow - tSyn > _iSynInterval)
    {
        tSyn = tNow;
        bNeedSynServerState = true;
    }
    string sServerId;
    _vStat.clear();
    map<string, ServerGroup> mmServerList = ServerFactory::getInstance()->getAllServers();
    map<string, ServerGroup>::const_iterator it = mmServerList.begin();
    for(;it != mmServerList.end(); it++)
    {
        map<string, ServerObjectPtr>::const_iterator  p = it->second.begin();
        for(;p != it->second.end(); p++)
        {
            try
            {
                sServerId   = it->first+"."+p->first;
                ServerObjectPtr pServerObjectPtr = p->second;
                if(!pServerObjectPtr)
                {
                    LOG->debug()<<sServerId<<"|=NULL|"<<endl;
                    FDLOG()<<sServerId<<" NULL"<< endl;
                    continue;
                }
                pServerObjectPtr->doMonScript();
                if(TC_TimeProvider::getInstance()->getNow()-_tRunTime<ServantHandle::HEART_BEAT_INTERVAL*5)
                {
                    //等待心跳包
                    continue;
                }

				//add corelimit checking
				pServerObjectPtr->checkCoredumpLimit();
				//end add

                //add by edwardsu(checkServer时，上报Server所占用的内存给主控)
                pServerObjectPtr->checkServer(_iHeartTimeout);
                //end add

                //cache 同步server 状态
                if(bNeedSynServerState == true)
                {
                    ServerInfo   tServerInfo;
                    tServerInfo.application = it->first;
                    tServerInfo.serverName  = p->first;
                    ServerDescriptor tServerDescriptor = pServerObjectPtr->getServerDescriptor();
                    tServerDescriptor.settingState = pServerObjectPtr->isEnabled()== true?"active":"inactive";
                    g_serverInfoHashmap.set(tServerInfo,tServerDescriptor);

                    ServerStateInfo tServerStateInfo;
                    tServerStateInfo.serverState    = (pServerObjectPtr->IsEnSynState()?pServerObjectPtr->getState():taf::Inactive);
                    tServerStateInfo.processId      = pServerObjectPtr->getPid();
                    tServerStateInfo.nodeName       = _tNodeInfo.nodeName;
                    tServerStateInfo.application    = it->first;
                    tServerStateInfo.serverName     = p->first;

                    if(TC_Common::lower(_sSynStatBatch) == "y")
                    {
                        _vStat.push_back(tServerStateInfo);
                    }
                    else
                    {
                        pServerObjectPtr->asyncSynState();
                    }
                }
            }
            catch(exception &e)
            {
                LOG->error() << "KeepAliveThread::checkAlive "<<sServerId<<" catch exception|"<<e.what() << endl;
            }
        }
    }
    if(bNeedSynServerState)
    {
        synStat();
    }
}
