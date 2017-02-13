#include "SecretServer.h"
#include "SecretImp.h"

using namespace std;

SecretServer g_app;

extern SecretHashMap   g_secret_hashmap;
extern ContentHashMap   g_content_hashmap;

/////////////////////////////////////////////////////////////////
void
SecretServer::initialize()
{
	//initialize application here:
	//...

	addServant<SecretImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".SecretObj");

        addConfig("SecretServer.conf");
 
        initHashMap();
}
/////////////////////////////////////////////////////////////////
void
SecretServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
void
SecretServer::initHashMap()
{
    LOG->debug() << "StatServer::initHashMap begin" << endl;

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "SecretServer.conf");

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

    string sContentFile = ServerConfig::DataPath + "/" +tConf.get("/main/hashmap/<contentfile>","content");
    string sContentPath = TC_File::extractFilePath(sContentFile);

    iMinBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<cminBlock>","64"));
    iMaxBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap/<cmaxBlock>","256"));
    iFactor         = TC_Common::strto<float>(tConf.get("/main/hashmap/<cfactor>","1.5"));
    iSize           = TC_Common::toSize(tConf.get("/main/hashmap/<csize>"), 1024*1024*20);

    if(!TC_File::makeDirRecursive(sContentPath))
    {
        LOG->error()<<"cannot create hashmap file "<< sContentPath <<endl;
        exit(0);
    }

    try
    {
        g_content_hashmap.initDataBlockSize(iMinBlock, iMaxBlock, iFactor);
        if(TC_File::isFileExist(sContentFile))
        {
            iSize = TC_File::getFileSize(sContentFile);
        }
        g_content_hashmap.initStore(sContentFile.c_str(), iSize);
        g_content_hashmap.setAutoErase(true);

        LOG->debug() << "init hash map succ" << endl;
    }
    catch(TC_HashMap_Exception &e)
    {
        TC_File::removeFile(sContentFile, false);
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
