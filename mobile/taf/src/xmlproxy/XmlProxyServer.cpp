#include "XmlProxyServer.h"
#include "XmlProxyImp.h"
#include "XmlProxyProtocol.h"
#include "TransProxyManager.h"
using namespace std;

XmlProxyServer g_app;

/////////////////////////////////////////////////////////////////
void XmlProxyServer::initialize()
{
	addServant<XmlProxyImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".XmlProxyObj");

	addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".XmlProxyObj", &XmlProxyProtocol::parseXml);

	addConfig("tafxmlproxy.conf");

	TransProxyManager::getInstance()->loadproxy(ServerConfig::BasePath + "tafxmlproxy.conf");

	TAF_ADD_ADMIN_CMD_NORMAL("loadproxy", XmlProxyServer::cmdLoadProxy)
}

bool XmlProxyServer::cmdLoadProxy(const string& command, const string& params, string& result)
{
	LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|GET COMMAND:" << command << endl;

	addConfig("tafxmlproxy.conf");
	result = TransProxyManager::getInstance()->loadproxy(ServerConfig::BasePath + "tafxmlproxy.conf");
	LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|GET COMMAND:" << command << "|RES:" << result << endl;

	return true;
}

/////////////////////////////////////////////////////////////////
void
XmlProxyServer::destroyApp()
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
