#include "TestAddConfig.h"
#include "TestImp.h"

using namespace std;

CFileConfig g_conf;
TestAddConfig g_app;

/////////////////////////////////////////////////////////////////
void
TestAddConfig::initialize()
{
	//initialize application here:
	//...
	addConfig("Test.conf");
	addServant<TestImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".TestObj");

	string sConfpath = ServerConfig::BasePath + "Test.conf";
	g_conf.Init(sConfpath);
	string BindIp = g_conf["Test\\BindIp"];
}
/////////////////////////////////////////////////////////////////
void
TestAddConfig::destroyApp()
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
