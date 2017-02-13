#include "PropertyDimenServer.h"
#include "PropertyDimenImp.h"

PropertyHashMap g_hashmap;
int PropertyDimenServer::g_iInsertInterval;
string PropertyDimenServer::g_sClonePatch;
void PropertyDimenServer::initialize()
{
    try
    {
        //增加对象
        LOG->debug() << "addServant PropertyObj" << endl;
        //addConfig("tafPropertyDimenServer.conf");
		//string sfile = ServerConfig::BasePath + "tafPropertyDimenServer.conf";
        //g_pconf->parseFile(sfile);
        addServant<PropertyDimenImp>( ServerConfig::Application + "." + ServerConfig::ServerName +".PropertyDimenObj" );       
        g_iInsertInterval  = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<insertInterval>","10"));
        if(g_iInsertInterval < 1) g_iInsertInterval = 1;
             
        initHashMap();    
        _pReapThread = new PropertyDimenReapThread();
        
        _pReapThread->start();
    }
    catch ( exception& ex )
    {
        LOG->error() << "PropertyDimenServer::initialize catch exception:" << ex.what() << endl;
        exit( 0 );
    }
    catch ( ... )
    {
        LOG->error() << "PropertyDimenServer::initialize unknow  exception  catched" << endl;
        exit( 0 );
    }
}
void PropertyDimenServer::initHashMap()
{
    LOG->debug() << "PropertyDimenServer::initHashMap begin" << endl;
    string sFile        = ServerConfig::DataPath + "/" + g_pconf->get("/taf/hashmap<file>", "hashmap.txt");
    g_sClonePatch       = ServerConfig::DataPath + "/" + g_pconf->get("/taf/hashmap<clonePatch>","clone");
    string sPath        = TC_File::extractFilePath(sFile);
    int iMinBlock       = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<minBlock>","100"));
    int iMaxBlock       = TC_Common::strto<int>(g_pconf->get("/taf/hashmap<maxBlock>","200"));
    float iFactor       = TC_Common::strto<float>(g_pconf->get("/taf/hashmap<factor>","1.5"));
    int iSize           = TC_Common::toSize(g_pconf->get("/taf/hashmap<size>"), 1024*1024*10);
  
    if(!TC_File::makeDirRecursive(sPath))
    {
        LOG->debug()<<"cannot create hashmap file "<<sPath<<endl;
        exit(0);
    }
    if(!TC_File::makeDirRecursive(g_sClonePatch))
    {
        LOG->debug()<<"cannot create hashmap file "<<g_sClonePatch<<endl;
        exit(0);
    }

    g_hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
    if(TC_File::isFileExist(sFile))
    {
        iSize = TC_File::getFileSize(sFile);
    }
    g_hashmap.initStore( sFile.c_str(), iSize );

    LOG->debug() << "init hash map succ" << endl;
}

void PropertyDimenServer::destroyApp()
{
    LOG->debug() << "PropertyDimenServer::destroyApp ok" << endl;
}

void PropertyDimenServer::getTimeInfo(time_t &tTime,string &sDate,string &sFlag)
{
     //3G统计要求
    //loadIntev 单位为分钟 
    string sTime,sHour,sMinute;
    time_t t    = TC_TimeProvider::getInstance()->getNow();
    t           = (t/(g_iInsertInterval*60))*g_iInsertInterval*60;    //要求必须为Intev整数倍
    tTime       = t;
    t           = (t%3600 == 0?t-60:t);                                 //要求将9点写作0860  
    sTime       = TC_Common::tm2str(t,"%Y%m%d%H%M");
    sDate       = sTime.substr(0,8);
    sHour       = sTime.substr(8,2);
    sMinute     = sTime.substr(10,2);
    sFlag       = sHour +  (sMinute=="59"?"60":sMinute);                //要求将9点写作0860     
}

