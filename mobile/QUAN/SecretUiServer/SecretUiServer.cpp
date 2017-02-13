#include "SecretUiServer.h"
#include "SecretUiImp.h"

using namespace std;

SecretUiServer g_app;

/////////////////////////////////////////////////////////////////
void
SecretUiServer::initialize()
{
	//initialize application here:
	//...

	addServant<SecretUiImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".SecretUiObj");

        addConfig("SecretUiServer.conf");

        TC_Config conf;
        try 
        {   
            conf.parseFile(ServerConfig::BasePath + "/" + ServerConfig::ServerName+".conf");
        }   
        catch(...)
        {   
            exit(-2);
        }

        string sActiveUrl = conf.get("/main/<ActiveUrl>");

        int iActiveCount=max(TC_Common::strto<int>(conf.get("/main/<ActiveCount>","30")),5);

        _tActiveHttp.init(sActiveUrl, iActiveCount, iActiveCount);


}
/////////////////////////////////////////////////////////////////
void
SecretUiServer::destroyApp()
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
