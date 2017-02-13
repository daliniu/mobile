#include "MsgServer.h"
#include "MsgImp.h"

using namespace std;

MsgServer g_app;
extern RemindHashMap   g_remind_hashmap;
extern SysMsgHashMap   g_sysmsg_hashmap;
extern PushMsgHashMap   g_pushmsg_hashmap;

/////////////////////////////////////////////////////////////////
void
MsgServer::initialize()
{
	//initialize application here:
	//...

	addServant<MsgImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".MsgObj");

        addConfig("MsgServer.conf");


        initHashMap();
}
/////////////////////////////////////////////////////////////////
void
MsgServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
void
MsgServer::initHashMap()
{   
    LOG->debug() << "StatServer::initHashMap begin" << endl;
    
    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "MsgServer.conf");

	// 初始化push
	// _sPushUrl = tConf.get("/main/<PushUrl>", "http://183.61.6.102:8191/push?");
	_sPushUrl = tConf.get("/main/<PushUrl>", "http://push.q.duowan.com/push?");
	int iPushCount = max(TC_Common::strto<int>(tConf.get("/main/<PushCount>", "30")), 5);
	_tMsgHttp.init(_sPushUrl, iPushCount, iPushCount);

	// 初始化SysMsgCache
    CommHashMap::getInstance()->initHashMap(tConf,"SysMsgCache",g_sysmsg_hashmap);

	// 初始化PushMsgCache
    CommHashMap::getInstance()->initHashMap(tConf,"PushMsgCache",g_pushmsg_hashmap);

    string sRemindFile = ServerConfig::DataPath + "/" +tConf.get("/main/hashmap/<remindfile>","remind");
    string sRemindPath = TC_File::extractFilePath(sRemindFile);
        
    int iMinBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<sminBlock>","64"));
    int iMaxBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<smaxBlock>","256"));
    float iFactor       = TC_Common::strto<float>(tConf.get("/main/hashmap/<sfactor>","1.5"));
    size_t iSize    = TC_Common::toSize(tConf.get("/main/hashmap/<ssize>"), 1024*1024*20);

    if(!TC_File::makeDirRecursive(sRemindPath))
    {
        LOG->error()<<"cannot create hashmap file "<< sRemindPath <<endl;
        exit(0);
    }

    try
    {
        g_remind_hashmap.initDataBlockSize(iMinBlock, iMaxBlock, iFactor);
        if(TC_File::isFileExist(sRemindFile))
        {
            iSize = TC_File::getFileSize(sRemindFile);
        }
        g_remind_hashmap.initStore(sRemindFile.c_str(), iSize);
        g_remind_hashmap.setAutoErase(true);

        LOG->debug() << "init hash map succ" << endl;
    }
    catch(TC_HashMap_Exception &e)
    {
        TC_File::removeFile(sRemindFile, false);
        throw runtime_error(e.what());
    }
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}
	return -1;
}
/////////////////////////////////////////////////////////////////
