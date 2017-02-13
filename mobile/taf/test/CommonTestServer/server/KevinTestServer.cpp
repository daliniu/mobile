#include "KevinTestServer.h"
#include "KevinTestServantImp.h"
#include "ThridProtoServantImp.h"

using namespace std;

KevinTestServer g_app;

/////////////////////////////////////////////////////////////////
void KevinTestServer::initialize()
{
	std::map<int, TC_EpollServer::BindAdapterPtr> mapAdapters = Application::getEpollServer()->getListenSocketInfo();
	
	for (std::map<int, TC_EpollServer::BindAdapterPtr>::iterator it = mapAdapters.begin(); it != mapAdapters.end(); it++)
	{
		LOG->debug() << "ADAPTER NAME:" << it->second->getName() 
		<< "|SERVANT NANME:" << ServantHelperManager::getInstance()->getAdapterServant(it->second->getName())
		<< "|ENDPOINT:" <<  it->second->getEndpoint().toString() << std::endl;
	
	}
	
	
	
	addServant<KevinTestServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".KevinTestServantObj");
	
	addServant<ThridProtoServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".ThridProtoServantObj");
	
	//为第三方协议的Servant增加协议解析器
		
	addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".ThridProtoServantObj", AppProtocol::parseStream<0, int, true>);
	
		
	addConfig(ServerConfig::ServerName + ".conf");
}
/////////////////////////////////////////////////////////////////
void
KevinTestServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	//获取环境变量
	
	char * p = getenv("kevin");
	
	
	std::string senv = p == NULL?"":p;
	
	std::cout << "ENV:" << senv << std::endl;
	
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
