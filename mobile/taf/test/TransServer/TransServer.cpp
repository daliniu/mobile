#include "TransServer.h"
#include "TransImp.h"

using namespace std;

/////////////////////////////////////////////////////////////////
void
TransServer::initialize()
{
	//(0)绑定Servant
	addServant<TransImp>("Test.TransServer.TransObj");

	//(1)设置监听端口的协议解析器
	addServantProtocol("Test.TransServer.TransObj", AppProtocol::parseStream<0, uint32_t, true>);
}

/////////////////////////////////////////////////////////////////
void TransServer::destroyApp()
{
	//destroy application here:
	//...
}

/////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	try
	{
        TransServer server;

		server.main(argc, argv);

		server.waitForShutdown();
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
