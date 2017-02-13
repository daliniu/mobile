#include "ServerFactory.h"
#include "RegistryProxy.h"
#include "CommandLoad.h"


HashMap g_serverInfoHashmap;
ServerFactory::ServerFactory()
: _bReousceLimitConfChanged(false)
, _iMaxExcStopCount(3)
, _iCoreLimitTimeInterval(3)
, _iCoreLimitExpiredTime(30)
, _bCoreLimitEnable(false)
{
}

ServerObjectPtr ServerFactory::getServer( const string& application, const string& serverName )
{
    LOGINFO("Into " << __FUNCTION__ << ":" << __LINE__ << "|" << application << "|" << serverName << endl);
    Lock lock( *this );
    LOGINFO("doing " << __FUNCTION__ << ":" << __LINE__ << "|" << application << "|" << serverName << endl);
    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 != _mmServerList.end() )
    {
        map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
        if ( p2 != p1->second.end() && p2->second )                                                                                                                 //  throw ServerExistsException( application, serverName );
        {
            return p2->second;
        }
    }
    return NULL;
}


int ServerFactory::eraseServer( const string& application, const string& serverName )
{
    Lock lock( *this );

    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 == _mmServerList.end() )
    {
        return 0;
    }

    map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
    if ( p2 == p1->second.end() )
    {
        return 0;
    }

    _mmServerList[application].erase( serverName );
    if ( p1->second.empty() )
    {
        _mmServerList.erase( application );
    }
    return 0;
}

ServerObjectPtr ServerFactory::loadServer( const string& application, const string& serverName,bool enableCache,string& result)
{
    ServerObjectPtr pServerObjectPtr;
    vector<ServerDescriptor> vServerDescriptor;

    vServerDescriptor = getServerFromRegistry(application,serverName,result);

    if(vServerDescriptor.size() < 1 && enableCache == true)
    {
        vServerDescriptor = getServerFromCache(application,serverName,result);
    }
    if(vServerDescriptor.size() < 1)
    {
        result += " cannot load server description from regisrty ";
        return NULL;
    }
    for(unsigned i = 0; i < vServerDescriptor.size(); i++)
    {
        pServerObjectPtr = createServer(vServerDescriptor[i],result);
    }
    return  pServerObjectPtr;
}

vector<ServerDescriptor> ServerFactory::getServerFromRegistry( const string& application, const string& serverName, string& result)
{
    LOG->debug()<<"get server from registry ["<< application <<"."<<serverName<<"]"<<endl;
    vector<ServerDescriptor> vServerDescriptor;
    try
    {
        RegistryPrx _pRegistryPrx = AdminProxy::getInstance()->getRegistryProxy();

        if(!_pRegistryPrx)
        {
            result = "coun't get the proxy of registry.";
            LOG->error()<<result<< endl;
            return vServerDescriptor;
        }

        vServerDescriptor =_pRegistryPrx->getServers( application, serverName,_tPlatformInfo.getNodeName());
        //清空cache
        if( vServerDescriptor.size()> 0 && application == "" && serverName == "")
        {
            g_serverInfoHashmap.clear();
            LOGINFO("hashmap clear ok "<<endl);

        }
        //重置cache
        for(unsigned i = 0; i < vServerDescriptor.size(); i++)
        {
            ServerInfo tServerInfo;
            tServerInfo.application = vServerDescriptor[i].application;
            tServerInfo.serverName  = vServerDescriptor[i].serverName;
            g_serverInfoHashmap.set(tServerInfo,vServerDescriptor[i]);
            LOGINFO("hashmap set ok "<<tServerInfo.application<<"."<<tServerInfo.serverName<<endl);
        }

    } catch (exception &e)
    {
        result  = "ServerFactory::getServerFromRegistry exception:";
        result  = result + e.what();
        LOG->error()<<result<< endl;
    }
    return  vServerDescriptor;
}

vector<ServerDescriptor> ServerFactory::getServerFromCache( const string& application, const string& serverName, string& result)
{
    LOG->debug()<<"get server from cache ["<< application <<"."<<serverName<<"]"<<endl;
    ServerInfo tServerInfo;
    ServerDescriptor tServerDescriptor;
    vector<ServerDescriptor> vServerDescriptor;
    if(application != "" && serverName != "")
    {
        tServerInfo.application = application;
        tServerInfo.serverName  = serverName;
        if(g_serverInfoHashmap.get(tServerInfo,tServerDescriptor) == TC_HashMap::RT_OK)
        {
            vServerDescriptor.push_back(tServerDescriptor);
        }
    }
    else
    {
        HashMap::lock_iterator it = g_serverInfoHashmap.beginSetTime();
        while(it != g_serverInfoHashmap.end())
        {
            ServerInfo   tServerInfo;
            ServerDescriptor tServerDescriptor;
            int ret = it->get(tServerInfo,tServerDescriptor);
            if(ret != TC_HashMap::RT_OK)
            {
                result =result + "\n ServerFactory::getServerFromCache hashmap erro:"+TC_Common::tostr(ret);
                LOG->error()<<result<< endl;
                break;
            }
            if(serverName == "" && application == "")
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }
            else if(application == "" && serverName == tServerInfo.serverName)
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }
            else if(serverName == "" && application == tServerInfo.application)
            {
                vServerDescriptor.push_back(tServerDescriptor);
            }
            ++it;
        }
    }
    return vServerDescriptor;
}

ServerObjectPtr ServerFactory::createServer(const ServerDescriptor& tDesc, string& result)
{
    Lock lock( *this );
    string application  = tDesc.application;
    string serverName   = tDesc.serverName;

    map<string, ServerGroup>::const_iterator p1 = _mmServerList.find( application );
    if ( p1 != _mmServerList.end() )
    {
        map<string, ServerObjectPtr>::const_iterator p2 = p1->second.find( serverName );
        if ( p2 != p1->second.end() && p2->second )
        {
            p2->second->setServerDescriptor(tDesc);
            CommandLoad command(p2->second,_tPlatformInfo.getNodeInfo());
            int iRet = command.doProcess(result);
            if( iRet == 0)
            {
                return p2->second;
            }
            else
            {
                return NULL;
            }
        }
    }
    ServerObjectPtr pServerObjectPtr = new ServerObject(tDesc);
    CommandLoad command(pServerObjectPtr,_tPlatformInfo.getNodeInfo());
    int iRet = command.doProcess(result);
    if(iRet ==0)
    {
		if(IsCoreLimitEnable())
		{
			ServerObject::ServerLimitInfo tInfo;
			tInfo.bEnableCoreLimit = _bCoreLimitEnable;
			tInfo.bCloseCore = false;
			tInfo.eCoreType = ServerObject::EM_AUTO_LIMIT;
			tInfo.iMaxExcStopCount = _iMaxExcStopCount;
			tInfo.iCoreLimitTimeInterval = _iCoreLimitTimeInterval;
			tInfo.iCoreLimitExpiredTime = _iCoreLimitExpiredTime;

			LOG->debug()<< "setServerLimit|" << (application + "." +serverName) <<"|"<<tInfo.eCoreType<<endl;
			pServerObjectPtr->setServerLimitInfo(tInfo);
		}
        _mmServerList[application][serverName] = pServerObjectPtr;
        return pServerObjectPtr;
    }
    return NULL;
}

map<string, ServerGroup> ServerFactory::getAllServers()
{
    Lock lock( *this );
    return _mmServerList;
}


bool ServerFactory::loadConfig()
{
	string file =  ServerConfig::BasePath + "tafnode.conf";
	try{

			taf::TC_Config conf;
			conf.parseFile(file);

			vector<string> vCloseCoreSrvs = TC_Common::sepstr<string>(conf.get("/corelimit<closecore>",""),"|");

			int iMaxExcStopCnt = TC_Common::strto<int>(conf.get("/corelimit<maxstopcount>","3"));
			int iCoreLimitTimeInterval = TC_Common::strto<int>(conf.get("/corelimit<coretimeinterval>","5"));
			int iCoreLimitExpiredTime = TC_Common::strto<int>(conf.get("/corelimit<coretimeexpired>","30"));
			string sEnableCore =  TC_Common::lower(conf.get("/corelimit<corelimitenable>","false"));

			bool bCoreLimitEnable = (sEnableCore=="true")?true:false;

			vector<string>::const_iterator it = vCloseCoreSrvs.begin();

			map<string,ServerObject::ServerLimitInfo> mTemp;

			for(;it != vCloseCoreSrvs.end();it++)
			{
				ServerObject::ServerLimitInfo tInfo;

				tInfo.bEnableCoreLimit = bCoreLimitEnable;
				tInfo.bCloseCore = true;
				tInfo.eCoreType = ServerObject::EM_MANUAL_LIMIT;
				tInfo.iMaxExcStopCount = iMaxExcStopCnt;
				tInfo.iCoreLimitTimeInterval = iCoreLimitTimeInterval;
				tInfo.iCoreLimitExpiredTime = iCoreLimitExpiredTime;

				mTemp[*it] = tInfo;
				LOG->debug()<< "Load ClosecoreLimit:" << *it <<endl;
			}

			Lock lock( *this );

			_mCoreLimitConfig.clear();

			_mCoreLimitConfig = mTemp;

			_iMaxExcStopCount = iMaxExcStopCnt;
			_iCoreLimitTimeInterval = iCoreLimitTimeInterval;
			_iCoreLimitExpiredTime = iCoreLimitExpiredTime;
			_bCoreLimitEnable = bCoreLimitEnable;

			_bReousceLimitConfChanged = true;
			LOG->debug()<< "Load ClosecoreLimit|" << _bCoreLimitEnable<<"|"<<_iMaxExcStopCount<<"|"<<_iCoreLimitTimeInterval<<"|"<<_iCoreLimitExpiredTime <<endl;


	}catch (std::exception& e)
	{
		LOG->error()<< __FILE__<<":"<<__LINE__<<" exception:" << e.what() <<endl;
		return false;
	}
	catch(...)
	{
		LOG->error()<< __FILE__<<":"<<__LINE__<<" exception" <<endl;
		return false;
	}
	return true;
}
void ServerFactory::setAllServerResourceLimit()
{
	Lock lock( *this );

	//有更新才加载
	if(_bReousceLimitConfChanged)
	{
		map<string, ServerGroup>::const_iterator it = _mmServerList.begin();
		for(;it != _mmServerList.end(); it++)
		{
			map<string, ServerObjectPtr>::const_iterator  p = it->second.begin();
			for(;p != it->second.end(); p++)
			{
				string sServerId	= it->first+"."+p->first;
				ServerObjectPtr pServerObjectPtr = p->second;
				if(!pServerObjectPtr)
				{
					continue;
				}

				ServerObject::ServerLimitInfo tInfo;
				if(_mCoreLimitConfig.count(sServerId) == 1)
				{
					tInfo = _mCoreLimitConfig[sServerId];
				}
				else
				{
					tInfo.bEnableCoreLimit = _bCoreLimitEnable;
					tInfo.bCloseCore = false;
					tInfo.eCoreType = ServerObject::EM_AUTO_LIMIT;
					tInfo.iMaxExcStopCount = _iMaxExcStopCount;
					tInfo.iCoreLimitTimeInterval = _iCoreLimitTimeInterval;
					tInfo.iCoreLimitExpiredTime = _iCoreLimitExpiredTime;
				}

				LOG->debug()<< "setAllServerResourceLimit|" << sServerId <<"|"<<tInfo.eCoreType<<endl;
				pServerObjectPtr->setServerLimitInfo(tInfo);

			}
		}
		_bReousceLimitConfChanged = false;
	}

}


