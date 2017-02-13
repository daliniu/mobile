#include "BServer.h"
#include "BServantImp.h"

using namespace std;

BServer g_app;

/////////////////////////////////////////////////////////////////
void
BServer::initialize()
{
	//initialize application here:
	//...

	addServant<BServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".BServantObj");
}
/////////////////////////////////////////////////////////////////
void
BServer::destroyApp()
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
