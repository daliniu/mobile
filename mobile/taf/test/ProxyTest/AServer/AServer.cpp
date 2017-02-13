#include "AServer.h"
#include "AServantImp.h"

using namespace std;

AServer g_app;

/////////////////////////////////////////////////////////////////
void
AServer::initialize()
{
	//initialize application here:
	//...

	addServant<AServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".AServantObj");
}
/////////////////////////////////////////////////////////////////
void
AServer::destroyApp()
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
