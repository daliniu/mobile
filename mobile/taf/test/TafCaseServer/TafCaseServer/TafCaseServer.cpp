#include "TafCaseServer.h"
#include "TafCaseServantImp.h"

using namespace std;

TafCaseServer g_app;

/////////////////////////////////////////////////////////////////
void
TafCaseServer::initialize()
{
	//initialize application here:
	//...

	addServant<TafCaseServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".TafCaseServantObj");
}
/////////////////////////////////////////////////////////////////
void
TafCaseServer::destroyApp()
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
