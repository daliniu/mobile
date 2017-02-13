#include "CircleServer.h"
#include "CircleImp.h"

using namespace std;

CircleServer g_app;

/////////////////////////////////////////////////////////////////
void
CircleServer::initialize()
{
    //initialize application here:
    //...

    addServant<CircleImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".CircleObj");

    addConfig("CircleServer.conf");

    _pUniqIDPrx=Application::getCommunicator()->stringToProxy<MDW::UniqIDPrx>("MDW.UniqIDServer.UniqIDObj");
        
}
/////////////////////////////////////////////////////////////////
void
CircleServer::destroyApp()
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
