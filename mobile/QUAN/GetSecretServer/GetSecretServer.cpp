#include "GetSecretServer.h"
#include "GetSecretImp.h"

using namespace std;

GetSecretServer g_app;

extern SecretHashMap   g_secret_hashmap;
extern CircleHashMap   g_circle_hashmap;
/////////////////////////////////////////////////////////////////
void
GetSecretServer::initialize()
{
	//initialize application here:
	//...

	addServant<GetSecretImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".GetSecretObj");
        addConfig("GetSecretServer.conf");
 
        initHashMap();
}
/////////////////////////////////////////////////////////////////
void
GetSecretServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
void
GetSecretServer::initHashMap()
{
    LOG->debug() << "StatServer::initHashMap begin" << endl;

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "GetSecretServer.conf");

	// ³õÊ¼»¯CircleCache
    CommHashMap::getInstance()->initHashMap(tConf,"CircleCache",g_circle_hashmap);

    string sSecretFile = ServerConfig::DataPath + "/" +tConf.get("/main/hashmap/<secretfile>","secret");
    string sSecretPath = TC_File::extractFilePath(sSecretFile);

    int iMinBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<sminBlock>","64"));
    int iMaxBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<smaxBlock>","256"));
    float iFactor       = TC_Common::strto<float>(tConf.get("/main/hashmap/<sfactor>","1.5"));
    size_t iSize    = TC_Common::toSize(tConf.get("/main/hashmap/<ssize>"), 1024*1024*20);

    if(!TC_File::makeDirRecursive(sSecretPath))
    {
        LOG->error()<<"cannot create hashmap file "<< sSecretPath <<endl;
        exit(0);
    }

    try
    {
        g_secret_hashmap.initDataBlockSize(iMinBlock, iMaxBlock, iFactor);
        if(TC_File::isFileExist(sSecretFile))
        {
            iSize = TC_File::getFileSize(sSecretFile);
        }
        g_secret_hashmap.initStore(sSecretFile.c_str(), iSize);
        g_secret_hashmap.setAutoErase(true);

        LOG->debug() << "init hash map succ" << endl;
    }
    catch(TC_HashMap_Exception &e)
    {
        TC_File::removeFile(sSecretFile, false);
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
