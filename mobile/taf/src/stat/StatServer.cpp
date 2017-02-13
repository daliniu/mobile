#include "StatServer.h"
#include "servant/AppCache.h"

void StatServer::initialize()
{
    try
    {
		//关闭远程日志
        TafTimeLogger::getInstance()->enableRemote("", false);
        //增加对象
        addServant<StatImp>( ServerConfig::Application + "." + ServerConfig::ServerName +".StatObj" );
        LOG->debug() << "addServant StatObj" << endl;

        addServant<StatQueryImp>( ServerConfig::Application + "." + ServerConfig::ServerName +".StatQueryObj" );
        LOG->debug() << "addServant StatQueryObj" << endl;

        _iInsertInterval  = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<insertInterval>","5"));
        if(_iInsertInterval < 5)
		{
			_iInsertInterval = 5;
        }

        initHashMap();

		_bEnableMTTDomain =  TC_Common::strto<bool>(g_pconf->get("/taf<enableMTTDomain>","false"));
        LOG->debug() << "EnableMTTDomain :" << _bEnableMTTDomain  << endl;

		vector<string> vIpGroup = g_pconf->getDomainKey("/taf/masteripgroup");
		for (unsigned i = 0; i < vIpGroup.size(); i++)
		{
			vector<string> vOneGroup = TC_Common::sepstr<string>(vIpGroup[i], ";", true);
			if (vOneGroup.size() < 2)
			{
				LOG->error() << "StatImp::initialize wrong masterip:" << vIpGroup[i] << endl;
				continue;
			}
			_mVirtualMasterIp[vOneGroup[0]] = vOneGroup[1];
		}

		_sRandOrder = AppCache::getInstance()->get("RandOrder");
		LOG->debug() << "randorder:" << _sRandOrder << endl;

		_pReapSSDThread = new ReapSSDThread();
		_pReapSSDThread->start();

		TAF_ADD_ADMIN_CMD_PREFIX("taf.tafstat.randorder", StatServer::cmdSetRandOrder);
    }
    catch ( exception& ex )
    {
        LOG->error() << "StatServer::initialize catch exception:" << ex.what() << endl;
        exit( 0 );
    }
    catch ( ... )
    {
        LOG->error() << "StatServer::initialize unknow  exception  catched" << endl;
        exit( 0 );
    }
}

map<string, string>& StatServer::getVirtualMasterIp(void)
{
	return _mVirtualMasterIp;
}

bool StatServer::IsEnableMTTDomain(void)
{
	return _bEnableMTTDomain;
}

string StatServer::getRandOrder(void)
{
	return _sRandOrder;
}

string StatServer::getClonePath(void)
{
	return _sClonePath;
}

int StatServer::getInserInterv(void)
{
	return _iInsertInterval;
}

bool StatServer::cmdSetRandOrder(const string& command, const string& params, string& result)
{
    try
    {
        LOG->info() << "StatServer::cmdSetRandOrder:" << command << " " << params << endl;

		_sRandOrder = params;

        result = "set RandOrder [" + _sRandOrder + "] ok";

        AppCache::getInstance()->set("RandOrder",_sRandOrder);
    }
    catch (exception &ex)
    {
        result = ex.what();
    }
    return true;
}

void StatServer::initHashMap()
{

    LOG->debug() << "StatServer::initHashMap begin" << endl;
    string sHashMapFile  = ServerConfig::DataPath + "/" + ( *g_pconf )["/taf/hashmap<file>"];
    string sMasterFile  = ServerConfig::DataPath + "/" + ( *g_pconf )["/taf/hashmap<masterfile>"];
    string sSlaveFile   = ServerConfig::DataPath + "/" + ( *g_pconf )["/taf/hashmap<slavefile>"];
    _sClonePath       = ServerConfig::DataPath + "/" + g_pconf->get("/taf/hashmap<clonePatch>","clone");

    string sPath        = TC_File::extractFilePath(sHashMapFile);
    string sMasterPath  = TC_File::extractFilePath(sMasterFile);
    string sSlavePath   = TC_File::extractFilePath(sSlaveFile);

    int iMinBlock       = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<minBlock>","150"));
    int iMaxBlock       = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<maxBlock>","200"));
    float iFactor       = TC_Common::strto<float>(g_pconf->get("/taf/hashmap<factor>","1.5"));
    int iSize           = TC_Common::toSize(g_pconf->get("/taf/hashmap<size>"), 1024*1024*20);
    int iMaxPageSize    = TC_Common::strto<int>(g_pconf->get("/taf/hashmap/statQuery<maxPageSize>","20"));
    int iMaxPageNum     = TC_Common::strto<int>(g_pconf->get("/taf/hashmap/statQuery<maxPageNum>","10"));
    if(!TC_File::makeDirRecursive(sPath))
    {
        LOG->error()<<"cannot create hashmap file "<<sPath<<endl;
        exit(0);
    }

    if(!TC_File::makeDirRecursive(_sClonePath))
    {
        LOG->error()<<"cannot create hashmap file "<<_sClonePath<<endl;
        exit(0);
    }
    if(!TC_File::makeDirRecursive(sMasterPath))
    {
        LOG->error()<<"cannot create hashmap file "<<sMasterPath<<endl;
        exit(0);
    }

    if(!TC_File::makeDirRecursive(sSlavePath))
    {
        LOG->error()<<"cannot create hashmap file "<<sSlavePath<<endl;
        exit(0);
    }
    try
    {
	    LOG->info()<<"initDataBlockSize size: "<< iMinBlock << ", " << iMaxBlock << ", " << iFactor <<endl;
        g_hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
        if(TC_File::isFileExist(sHashMapFile))
        {
            iSize = TC_File::getFileSize(sHashMapFile);
        }
        g_hashmap.initStore( sHashMapFile.c_str(), iSize );
        LOG->info()<< "\n" <<  g_hashmap.desc();
        g_master_hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
        if(TC_File::isFileExist(sMasterFile))
        {
            iSize = TC_File::getFileSize(sMasterFile);
        }
        g_master_hashmap.initStore( sMasterFile.c_str(), iSize );
        g_master_hashmap.intPageInfo(iMaxPageSize,iMaxPageNum);

        g_slave_hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
        if(TC_File::isFileExist(sSlaveFile))
        {
            iSize = TC_File::getFileSize(sSlaveFile);
        }
        g_slave_hashmap.initStore( sSlaveFile.c_str(), iSize );
        g_slave_hashmap.intPageInfo(iMaxPageSize,iMaxPageNum);

        LOG->debug() << "init hash map succ" << endl;
    }
    catch(TC_HashMap_Exception &e)
    {
       TC_File::removeFile(sHashMapFile,false);
       TC_File::removeFile(sMasterFile,false);
       TC_File::removeFile(sSlaveFile,false);
       throw runtime_error(e.what());
    }
}

void StatServer::destroyApp()
{
    LOG->debug() << "StatServer::destroyApp ok" << endl;
}

void StatServer::getTimeInfo(time_t &tTime,string &sDate,string &sFlag)
{
    string sTime,sHour,sMinute;
    time_t t    = TC_TimeProvider::getInstance()->getNow();
    t           = (t/(_iInsertInterval*60))*_iInsertInterval*60; //要求必须为loadIntev整数倍
    tTime       = t;
    t           = (t%3600 == 0?t-60:t);                           //要求将9点写作0860
    sTime       = TC_Common::tm2str(t,"%Y%m%d%H%M");
    sDate       = sTime.substr(0,8);
    sHour       = sTime.substr(8,2);
    sMinute     = sTime.substr(10,2);
    sFlag       = sHour +  (sMinute=="59"?"60":sMinute);    //要求将9点写作0860
}


