#include "util/tc_file.h"
#include "servant/Communicator.h"
#include "servant/StatReport.h"
#include "log/taf_logger.h"

namespace taf
{

string ClientConfig::LocalIp = "127.0.0.1";

string ClientConfig::ModuleName = "unknown";

set<string> ClientConfig::SetLocalIp;

bool ClientConfig::SetOpen = false;

string ClientConfig::SetDivision = "";

//////////////////////////////////////////////////////////////////////////////////////////////
Communicator::Communicator()
: _initialized(false)
, _terminating(false)
{
}

Communicator::Communicator(TC_Config& conf, const string& domain/* = CONFIG_ROOT_PATH*/)
: _initialized(false)
, _terminating(false)
{
    try
    {
        setProperty(conf, domain);
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][Communicator fail:" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][Communicator fail.]" << endl;
    }
}

Communicator::~Communicator()
{
    terminate();

    LOG->debug() << "[TAF][~Communicator, communicator can only be destroyed when application exit.]" << endl;
}

bool Communicator::isTerminating()
{
    return _terminating;
}

void Communicator::setProperty(TC_Config& conf, const string& domain/* = CONFIG_ROOT_PATH*/)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    try
    {
        conf.getDomainMap(domain, _properties);

        string defaultValue = "dft";

        if ((defaultValue == getProperty("enableset", defaultValue)) || (defaultValue == getProperty("setdivision", defaultValue)))
        {
            _properties["enableset"] = conf.get("/taf/application<enableset>", "n");

            _properties["setdivision"] = conf.get("/taf/application<setdivision>", "NULL");
        }

        initClientConfig();
        
        //初始化隧道Obj  tunnel功能，调用不能直接连接的服务
        // 
        map<string,string> mTunnel;
        conf.getDomainMap(string(CONFIG_ROOT_PATH)+"/tunnel", mTunnel);
        
        map<string,string>::const_iterator it = mTunnel.begin();
        while( it != mTunnel.end())
        {
            setTunnel(it->first,it->second);
            it++;
        }     
        
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][communicator setProperty fail:" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][communicator setProperty fail.]" << endl;
    }
}

void Communicator::initClientConfig()
{
    ClientConfig::SetOpen = TC_Common::lower(getProperty("enableset", "n"))=="y"?true:false;

    if (ClientConfig::SetOpen)
    {
     ClientConfig::SetDivision = getProperty("setdivision");

     vector<string> vtSetDivisions = TC_Common::sepstr<string>(ClientConfig::SetDivision,".");

     string sWildCard = "*";

     if (vtSetDivisions.size()!=3
         ||(vtSetDivisions.size() == 3 && (vtSetDivisions[0]==sWildCard || vtSetDivisions[1]==sWildCard)))
     {
        //set分组名不对时默认没有打开set分组
        ClientConfig::SetOpen = false;

        setProperty("enableset","n");

        LOG->debug() << "[TAF][set division name error:" << ClientConfig::SetDivision << ", client failed to open set]" << endl;
     }
    }
    ClientConfig::LocalIp = getProperty("localip", "127.0.0.1");

    if (ClientConfig::SetLocalIp.empty())
    {
        vector<string> v = TC_Socket::getLocalHosts();
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i] != "127.0.0.1" && ClientConfig::LocalIp.empty())
            {
                ClientConfig::LocalIp = v[i];
            }

            ClientConfig::SetLocalIp.insert(v[i]);
        }
    }

    //缺省采用进程名称
    string exe = "";

    try
    {
        exe = TC_File::extractFileName(TC_File::getExePath());
    }
    catch(TC_File_Exception & ex)
    {
        //取失败则使用ip代替进程名
        exe = ClientConfig::LocalIp;
    }

    ClientConfig::ModuleName = getProperty("modulename", exe);
}

void Communicator::setProperty(const map<string, string>& properties)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _properties = properties;

    initClientConfig();
}

void Communicator::setProperty(const string& name, const string& value)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _properties[name] = value;

    initClientConfig();
}

string Communicator::getProperty(const string& name, const string& dft/* = ""*/)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    map<string, string>::iterator it = _properties.find(name);

    if (it != _properties.end())
    {
        return it->second;
    }
    return dft;
}

void Communicator::setTunnel(const string &sRuler, const string &sTunnel)
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _mTunnel[sTunnel] = TC_Common::sepstr<string>(sRuler,"|,; ");
}

void Communicator::initialize()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    if (_initialized)
    {
        return;
    }
    _initialized = true;

    _objectProxyFactory = new ObjectProxyFactory(this);

    _servantProxyFactory = new ServantProxyFactory(this);

    _fdReactor = new FDReactor();

    _fdReactor->start();

    _qTimeout = new QueueTimeout(this);

    _qTimeout->start();

    //初始化异步回调线程
    _asyncProcThread = new AsyncProcThread(this);

    int asyncThreadNum = TC_Common::strto<int>(getProperty("asyncthread", "3"));

    _asyncProcThread->start(asyncThreadNum);

    //初始化统计上报接口
    string statObj = getProperty("stat", "");

    string propertyObj = getProperty("property", "");

    string moduleName = getProperty("modulename", "");

    int iReportInterval = TC_Common::strto<int>(getProperty("report-interval", "60000"));

    int iSampleRate = TC_Common::strto<int>(getProperty("sample-rate", "1000"));

    int iMaxSampleCount = TC_Common::strto<int>(getProperty("max-sample-count", "100"));

    //stat总是有对象, 保证getStat返回的对象总是有效
    _statReportPtr = new StatReport();

    StatFPrx statPrx = NULL;

    if (!statObj.empty())
    {
        statPrx = stringToProxy<StatFPrx>(statObj);
    }

    //上报Property信息的代理
    PropertyFPrx propertyPrx = NULL;

    if (!propertyObj.empty())
    {
        propertyPrx = stringToProxy<PropertyFPrx>(propertyObj);
    }

    string sSetDivision = ClientConfig::SetOpen?ClientConfig::SetDivision:"";
    _statReportPtr->setReportInfo(statPrx, propertyPrx, ClientConfig::ModuleName, ClientConfig::LocalIp, sSetDivision, iReportInterval, iSampleRate, iMaxSampleCount);

    if (!propertyObj.empty() && !moduleName.empty())
    {
        _statReportPtr->createPropertyReport(moduleName + ".asyncqueue", PropertyReport::avg());
    }

     
    /*
    //初始化隧道Obj  tunnel功能，调用不能直接连接的服务
    // 
    _sTunnelObj = getProperty("tunnel", "");

    string sTunnelServerList = getProperty("tunnel-server-list", "");

    if(!_sTunnelObj.empty() && !sTunnelServerList.empty())
    {
        _vTunnelServerlList = TC_Common::sepstr<string>(sTunnelServerList,"|,; ");
    }
    */
}

int Communicator::reloadProperty(string & sResult)
{
    _objectProxyFactory->loadObjectLocator();

    int iReportInterval = TC_Common::strto<int>(getProperty("report-interval", "60000"));

    int iSampleRate = TC_Common::strto<int>(getProperty("sample-rate", "1000"));

    int iMaxSampleCount = TC_Common::strto<int>(getProperty("max-sample-count", "100"));

    string statObj = getProperty("stat", "");

    string propertyObj = getProperty("property", "");

    StatFPrx statPrx = NULL;

    if (!statObj.empty())
    {
        statPrx = stringToProxy<StatFPrx>(statObj);
    }

    PropertyFPrx propertyPrx = NULL;

    if (!propertyObj.empty())
    {
        propertyPrx = stringToProxy<PropertyFPrx>(propertyObj);
    }
    string sSetDivision = ClientConfig::SetOpen?ClientConfig::SetDivision:"";
    _statReportPtr->setReportInfo(statPrx, propertyPrx, ClientConfig::ModuleName, ClientConfig::LocalIp, sSetDivision, iReportInterval,iSampleRate,iMaxSampleCount);

    sResult = "locator=" + getProperty("locator", "") + "\r\n" +
              "stat=" + statObj + "\r\n" + "property=" + propertyObj + "\r\n" +
              "SetDivision=" + sSetDivision + "\r\n"  +
              "report-interval=" + TC_Common::tostr(iReportInterval) + "\r\n"+
              "sample-rate=" + TC_Common::tostr(iSampleRate) + "\r\n"+
              "max-sample-count=" + TC_Common::tostr(iMaxSampleCount) + "\r\n";

    return 0;
}

void Communicator::terminate()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _terminating = true;

    if (_initialized)
    {
        if (_asyncProcThread)
        {
            _asyncProcThread->terminate();
        }

        if (_fdReactor && _fdReactor->isAlive())
        {
            _fdReactor->terminate();

            _fdReactor->getThreadControl().join();
        }

        if (_qTimeout && _qTimeout->isAlive())
        {
            _qTimeout->terminate();

            _qTimeout->getThreadControl().join();
        }
    }
}

vector<TC_Endpoint> Communicator::getEndpoint(const string & objName)
{
    Communicator::initialize();
    return _objectProxyFactory->getObjectProxy(objName)->getEndpoint();
}

vector<TC_Endpoint> Communicator::getEndpoint4All(const string & objName)
{
    Communicator::initialize();
    return _objectProxyFactory->getObjectProxy(objName)->getEndpoint4All();
}

ServantPrx Communicator::getServantProxy(const string& objectName)
{
    Communicator::initialize();

    return _servantProxyFactory->getServantProxy(objectName);
}

string Communicator::getTunnelName(const string& objectName)
{
     Communicator::initialize();
     
     string sTunnelName;
     
     TC_LockT<TC_ThreadRecMutex> lock(*this);
     
     map<string,vector<string> >::const_iterator it  = _mTunnel.begin();     
     while( it != _mTunnel.end())
     {
        if(TC_Common::matchPeriod(objectName,it->second) == true)
        {
            sTunnelName = it->first;
            LOG->info() << "[TAF][ tunnel used, objectName:" << objectName << " tunnel :" <<sTunnelName<<" ]"<< endl;            
            break;
        }
        it++;
     }
     return sTunnelName;
}


StatReport* Communicator::getStatReport()
{
    Communicator::initialize();

    return _statReportPtr.get();
}

void Communicator::reportToStat(const ReqMessagePtr& req, const string& obj, int result)
{
    StatReport* stat = getStatReport();

    if (stat && stat->getStatPrx() && !isTerminating())
    {
        int sptime = 10*1000;

        TC_TimeProvider::getInstance()->getNow(&req->endtime);

        if (req->begtime.tv_sec != 0 && req->endtime.tv_sec != 0 && req->endtime.tv_sec >= req->begtime.tv_sec)
        {
            sptime = (req->endtime.tv_sec - req->begtime.tv_sec)*1000;

            sptime += (req->endtime.tv_usec - req->begtime.tv_usec)/1000;
        }

        EndpointInfo ep;

        if (req->adapter)
        {
            ep = req->adapter->endpoint();
        }

        string sFuncName = req->request.sFuncName;
        if(req->proxy && req->proxy->taf_tunnel_enabled())
        {
            sFuncName =  req->proxy->taf_name()+"::"+req->request.sFuncName;
        }

        stat->report(obj, ep.setDivision(), sFuncName, ep.host(), ep.port(), (StatReport::StatResult)result, sptime, 0, true);
    }
}

void Communicator::sampleToStat(const ReqMessagePtr& req, const string& obj, const string& ip)
{
    StatReport* stat = getStatReport();

    if (stat && stat->getStatPrx() && !isTerminating())
    {
        SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPESAMPLE);

        string sFuncName = req->request.sFuncName;
        if(req->proxy && req->proxy->taf_tunnel_enabled())
        {
            sFuncName =  req->proxy->taf_name()+"::"+req->request.sFuncName;
        }
        stat->doSample(obj, sFuncName, ip, req->request.status);
    }
}

FDReactor* Communicator::fdReactor()
{
    return _fdReactor.get();
}

ObjectProxyFactory* Communicator::objectProxyFactory()
{
    return _objectProxyFactory.get();
}

AsyncProcThread* Communicator::asyncProcThread()
{
    return _asyncProcThread.get();
}

ServantProxyFactory* Communicator::servantProxyFactory()
{
    return _servantProxyFactory.get();
}
///////////////////////////////////////////////////////////////
}
