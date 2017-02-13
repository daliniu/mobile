#include "PostServer.h"
#include "PostImp.h"
#include "PostCache.h"

using namespace std;

PostServer g_app;

/////////////////////////////////////////////////////////////////
void
PostServer::initialize()
{
	//initialize application here:
	//...
        addConfig("PostServer.conf");

	addServant<PostImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".PostObj");
        
        _pKeywordPrx=Application::getCommunicator()->stringToProxy<Comm::KeywordPrx>("Comm.KeywordServer.KeywordObj");

        PostCache::init();

        TC_Config tConf;
        tConf.parseFile(ServerConfig::BasePath + "PostServer.conf");

        senstiveLevel=TC_Common::strto<int>(tConf.get("/main/<senslevel>","1"));
        blackListLevel=TC_Common::strto<int>(tConf.get("/main/<blacklevel>","2"));
}
/////////////////////////////////////////////////////////////////
void
PostServer::destroyApp()
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
//
bool PostServer::spanCheck(taf::Int64 lUId,const std::string &content)
{
    if(lUId<10000)
    {
        return true;
    }
    TC_ThreadLock::Lock lock(spanLock);
    map<taf::Int64,pair<string,int> >::iterator it=mpSpanCache.find(lUId);
    
    if(it==mpSpanCache.end())
    {
        mpSpanCache.insert(make_pair(lUId,make_pair(content,taf::TC_TimeProvider::getInstance()->getNow())));
        return true;
    }
    else
    {
        if(it->second.first==content)
        {
            if(TC_TimeProvider::getInstance()->getNow()-it->second.second<30)
            {
                return false;
            }
        }
        else
        {
            it->second.first=content;
            it->second.second=TC_TimeProvider::getInstance()->getNow();
        }
        return true;
    }
}
