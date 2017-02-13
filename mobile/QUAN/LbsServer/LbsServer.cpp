#include "LbsServer.h"
#include "LbsImp.h"
#include "CommHashMap.h"

using namespace std;

LbsServer g_app;

extern LbsHashMap   g_lbs_hashmap;



/////////////////////////////////////////////////////////////////
void
LbsServer::initialize()
{
    //initialize application here:
    //...

    addServant<LbsImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".LbsObj");

    addConfig("LbsServer.conf");

    initHashMap();

}
/////////////////////////////////////////////////////////////////


void LbsServer::initHashMap()
{

    LOG->debug() << __FUNCTION__ << " begin" << endl;

    TC_Config tConf;

    __TRY__
    tConf.parseFile(ServerConfig::BasePath + "LbsServer.conf");
    __CATCH__

    string sLbsFile     = ServerConfig::DataPath + "/" + tConf.get("/main/hashmap<lbsfile>","lbs");
    string sLbsPath     = TC_File::extractFilePath(sLbsFile);
    int iMinBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap<minBlock>","150"));
    int iMaxBlock       = TC_Common::strto<int>(tConf.get("/main/hashmap<maxBlock>","200"));
    float iFactor       = TC_Common::strto<float>(tConf.get("/main/hashmap<factor>","1.5"));
    taf::Int64 iSize    = TC_Common::toSize(tConf.get("/main/hashmap<size>"), 1024*1024*20);

    //taf::Int64 iMaxSize = TC_Common::strto<long long>(tConf.get("/main/hashmap<maxSize>", "10737418240"));


    if (!TC_File::makeDirRecursive(sLbsPath) )
    {
        LOG->error()<<"cannot create hashmap file "<<sLbsPath<<endl;
        exit(0);
    }

    try
    {
        LOG->info()<<"initDataBlockSize size: "<< iMinBlock << ", " << iMaxBlock << ", " << iFactor <<endl;

        g_lbs_hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
        if (TC_File::isFileExist(sLbsFile))
        {
            iSize = TC_File::getFileSize(sLbsFile);
        }
        g_lbs_hashmap.initStore( sLbsFile.c_str(), iSize );
        LOG->info()<< "\n" <<  g_lbs_hashmap.desc();

        LOG->debug() << "init hash map succ" << endl;
    } catch (TC_HashMap_Exception &e)
    {
        TC_File::removeFile(sLbsFile,false);
        throw runtime_error(e.what());
    }
}

/////////////////////////////////////////////////////////////////





void
LbsServer::destroyApp()
{
    //destroy application here:
    //...
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    } catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    } catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
