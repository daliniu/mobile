#include "NodeServer.h"
#include "NodeImp.h"
#include "ServerImp.h"
#include "RegistryProxy.h"

string NodeServer::g_sNodeIp;

BatchPatch * g_BatchPatchThread;

void NodeServer::initialize()
{
    initRegistryObj();

    addConfig("tafnode.conf");

    //增加对象
    string sNodeObj     = ServerConfig::Application + "." + ServerConfig::ServerName + ".NodeObj";
    string sServerObj   = ServerConfig::Application + "." + ServerConfig::ServerName + ".ServerObj";

    addServant<NodeImp>(sNodeObj);
    LOG->debug() << "addServant "<<sNodeObj<< endl;

    addServant<ServerImp>( sServerObj );
    LOG->debug() << "addServant "<<sServerObj<< endl;

    LOG->debug() << "ServerAdapter " << (getAdapterEndpoint("ServerAdapter")).toString()<< endl;
    LOG->debug() << "ServerAdapter " << (getAdapterEndpoint("NodeAdapter")).toString()<< endl;

    g_sNodeIp = getAdapterEndpoint("NodeAdapter").getHost();

    if(!ServerFactory::getInstance()->loadConfig())
    {
         LOG->error() << "ServerFactory loadConfig failure"<< endl;
    }
    //查看服务desc
    TAF_ADD_ADMIN_CMD_PREFIX("taf.serverdesc",NodeServer::cmdViewServerDesc);
    TAF_ADD_ADMIN_CMD_PREFIX("reloadconfig",NodeServer::cmdReLoadConfig);

    initHashMap();

    //启动KeepAliveThread
    _pKeepAliveThread   = new KeepAliveThread();
    _pKeepAliveThread->start();
    LOG->debug() << __FUNCTION__ << "|KeepAliveThread start" << endl;

    //启动批量发布线程
    PlatformInfo plat;
    int iThreads        = TC_Common::strto<int>(g_pconf->get("/taf/node<bpthreads>", "10"));

    _pBatchPatchThread  = new BatchPatch();
    _pBatchPatchThread->setPath(plat.getDownLoadDir());
    _pBatchPatchThread->start(iThreads);
    g_BatchPatchThread  = _pBatchPatchThread;
    LOG->debug() << __FUNCTION__ << "|BatchPatchThread start(" << iThreads << ")" << endl;



}
void NodeServer::initRegistryObj()
{
    string sLocator =   Application::getCommunicator()->getProperty("locator");
    vector<string> vtLocator = TC_Common::sepstr<string>(sLocator, "@");
    LOG->debug() << "locator:" << sLocator << endl;
    if (vtLocator.size() == 0)
    {
        LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|failed to parse locator" << endl;
        exit(1);
    }

    vector<string> vObj = TC_Common::sepstr<string>(vtLocator[0],".");
    if(vObj.size() != 3)
    {
        LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|failed to parse locator" << endl;
        exit(1);
    }

    //获取主控名字的前缀
    string sObjPrefix("");
    string::size_type pos = vObj[2].find("QueryObj");
    if(pos != string::npos)
    {
        sObjPrefix = vObj[2].substr(0,pos);
    }

    AdminProxy::getInstance()->setRegistryObjName("taf.tafregistry." + sObjPrefix + "RegistryObj",
                                                  "taf.tafregistry." + sObjPrefix + "QueryObj",
                                                  "taf.tafregistry." + sObjPrefix + "AdminRegObj");

    LOG->debug() << "RegistryObj:" << ("taf.tafregistry." + sObjPrefix + "RegistryObj") << endl;
    LOG->debug() << "QueryObj:" << ("taf.tafregistry." + sObjPrefix + "QueryObj") << endl;
    LOG->debug() << "AdminRegObj:" << ("taf.tafregistry." + sObjPrefix + "AdminRegObj") << endl;

}

void NodeServer::initHashMap()
{
    LOG->debug() << "NodeServer::initHashMap begin" << endl;
    string sFile        = ServerConfig::DataPath + "/" + g_pconf->get("/taf/node/hashmap<file>","__tafnode_servers");
    string sPath        = TC_File::extractFilePath(sFile);
    int iMinBlock       = TC_Common::strto<int>(g_pconf->get("/taf/node/hashmap<minBlock>","500"));
    int iMaxBlock       = TC_Common::strto<int>(g_pconf->get("/taf/node/hashmap<maxBlock>","500"));
    float iFactor       = TC_Common::strto<float>(g_pconf->get("/taf/node/hashmap<factor>","1"));
    int iSize           = TC_Common::toSize(g_pconf->get("/taf/node/hashmap<size>"), 1024*1024*10);
    if(!TC_File::makeDirRecursive(sPath))
    {
        LOG->debug()<<"cannot create hashmap file "<<sPath<<endl;
        exit(0);
    }
    try
    {
        g_serverInfoHashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
        g_serverInfoHashmap.initStore( sFile.c_str(), iSize );

        LOG->debug() << "init hash map succ" << endl;
    }
    catch(TC_HashMap_Exception &e)
    {
       TC_File::removeFile(sFile,false);
       runtime_error(e.what());
    }
}

TC_Endpoint NodeServer::getAdapterEndpoint(const string& name ) const
{
    LOG->info() << "getAdapterEndpoint "<<name<< endl;
    TC_EpollServerPtr pEpollServerPtr = Application::getEpollServer();
    assert(pEpollServerPtr);
    TC_EpollServer::BindAdapterPtr pBindAdapterPtr = pEpollServerPtr->getBindAdapter(name);
    assert(pBindAdapterPtr);
    return pBindAdapterPtr->getEndpoint();
}

bool NodeServer::cmdViewServerDesc(const string& command, const string& params, string& result)
{
    LOG->info() << "Application::cmdViewServerDesc:" << command << " " << params << endl;

    vector<string> v = TC_Common::sepstr<string>(params,".");
    if(v.size()!=2)
    {
        result = "invalid params:" + params;
        return false;
    }
    string application  = v[0];
    string serverName   = v[1];
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if(pServerObjectPtr)
    {
        ostringstream os;
        pServerObjectPtr->getServerDescriptor().display(os);
        result = os.str();
        return false;
    }
    result = "server " + params +" not exist";
    return true;
}
bool NodeServer::cmdReLoadConfig(const string& command, const string& params, string& result)
{
    LOG->debug() << "NodeServer::cmdReLoadConfig "<< endl;

    bool bRet = false;

    if(addConfig("tafnode.conf")){

        bRet = ServerFactory::getInstance()->loadConfig();
    }

    string s = bRet?"OK":"failure";
    result = "cmdReLoadConfig " + s;
    return bRet;
}

void NodeServer::destroyApp()
{
    LOG->debug() << "DemoServer::destroyApp ok" << pthread_self() << endl;
}

bool NodeServer::isValid(const string& ip)
{
    //跨网取到外网ip不对，暂不验。
    return true;


    static time_t g_tTime   = 0;
    static vector <string> g_vIp;
    vector<EndpointF> vActiveEp,vInactiveEp;
    time_t tNow = TC_TimeProvider::getInstance()->getNow();
    static  TC_ThreadLock g_tMutex;
    TC_ThreadLock::Lock  lock( g_tMutex );
    if(tNow - g_tTime > 60)
    {
        try
        {
            QueryFPrx pQueryFPrx = Application::getCommunicator()->stringToProxy<QueryFPrx>("taf.tafregistry.QueryObj");
        pQueryFPrx->findObjectById4Any( "taf.tafregistry.AdminRegObj",vActiveEp,vInactiveEp);
        /*QueryFPrx pQueryFPrx = AdminProxy::getInstance()->getQueryProxy();

            string sAdminRegObj = AdminProxy::getInstance()->getAdminRegProxyName();
            sAdminRegObj = sAdminRegObj.empty()? "taf.tafregistry.AdminRegObj":sAdminRegObj;
            pQueryFPrx->findObjectById4All(sAdminRegObj,vActiveEp,vInactiveEp);*/

            g_vIp.clear();
            for(unsigned i=0; i< vActiveEp.size();i++)
            {
                g_vIp.push_back(vActiveEp[i].host);
            }
            for(unsigned i=0; i< vInactiveEp.size();i++)
            {
                g_vIp.push_back(vInactiveEp[i].host);
            }
            g_tTime = tNow;
        }
        catch(exception &e)
        {
            LOG->error() << "NodeServer::isValid catch error: "<<e.what() << endl;
            //return true;
        }
    }
    for(unsigned i=0; i< g_vIp.size();i++)
    {
       if(g_vIp[i] == ip)
           return true;
    }
    if(g_sNodeIp == ip)
    {
        return true;
    }
    return false;
}

void NodeServer::reportServer(const string &sServerId,const string &sResult)
{
    try
    {
        //上报到notify
        NotifyPrx pNotifyPrx = Application::getCommunicator()->stringToProxy<NotifyPrx>(ServerConfig::Notify);
        if(pNotifyPrx && sResult != "")
        {
            pNotifyPrx->async_reportServer(NULL,sServerId,"", sResult);
        }

    } catch (exception &ex)
    {
         LOG->error() << "NodeServer::reportServer error:" << ex.what() << endl;
    }
}







