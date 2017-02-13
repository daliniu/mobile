#include "UserInfoServer.h"
#include "UserInfoImp.h"


using namespace std;

UserInfoServer g_app;

/////////////////////////////////////////////////////////////////
void
UserInfoServer::initialize()
{
	//initialize application here:
	//...

	addServant<UserInfoImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".UserInfoObj");
        addConfig("UserInfoServer.conf");
        
        TC_Config tConf;
        tConf.parseFile(ServerConfig::BasePath + "UserInfoServer.conf");

        mAsyncMaster.init(5);

        initHashMap(tConf,"userCache",mUserInfoHashMap);
        initHashMap(tConf,"friendCache",mActiveFriendsHashMap);
        initHashMap(tConf,"followerCache",mActiveFollowersHashMap);
}
/////////////////////////////////////////////////////////////////
void
UserInfoServer::destroyApp()
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
