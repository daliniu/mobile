#include "TCServer.h"
#include "HelloImp.h"
#include "echo.h"
#include "http.h"
#include "string.h"

using namespace std;

TCServer g_app;

string ip;
string port;

/////////////////////////////////////////////////////////////////
void
TCServer::initialize()
{
	//initialize application here:
	//...

    TafRollLogger::getInstance()->logger()->setLogLevel(TC_RollLogger::INFO_LOG);
	addServant<HelloImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".HelloObj");

    bind<HttpHandle>("tcp -h " + ip + " -p " + port + " -t 50000");

	//bind<HttpHandle>("tcp -h 172.25.38.21 -p 8080 -t 5000");
//	bind<EchoHandle>("udp -h 127.0.0.1 -p 8082 -t 5000");
//	bind<EchoHandle>("udp -h 172.25.38.67 -p 8082 -t 5000");
//  bind<EchoHandle>("tcp -h 172.25.38.14 -p 8882 -t 5000");
//  bind<StringHandle>("tcp -h 172.25.38.67 -p 8082 -t 5000");
}

/////////////////////////////////////////////////////////////////
void
TCServer::destroyApp()
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
        ip = argv[1];
        port = argv[2];

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

/*
单存采用TC_EpollServer的形式
#include "echo.h"
#include "http.h"
#include "string.h"

using namespace std;
using namespace taf;

class TCServer : public TC_EpollServer
{
public:
    template<typename T>
    void bind(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(this);

        //设置adapter名称, 唯一
        lsPtr->setName(str);
        //设置绑定端口
        lsPtr->setEndpoint(str);
        //设置最大连接数
        lsPtr->setMaxConns(20000);
        //设置启动线程数
        lsPtr->setHandleNum(10);
        //设置协议解析器
        lsPtr->setProtocol(&T::parse);
        //设置逻辑处理器
        lsPtr->setHandle<T>();

        //绑定对象
        TC_EpollServer::bind(lsPtr);
    }
};

int main(int argc, char *argv[])
{
    try
	{
        TC_RollLogger logger;

        TCServer ss;
        ss.setLocalLogger(&logger);

        ss.bind<HttpHandle>("tcp -h 172.25.38.67 -p 8083 -t 5000");
        ss.bind<EchoHandle>("tcp -h 127.0.0.1 -p 8082 -t 5000");
        ss.bind<EchoHandle>("udp -h 127.0.0.1 -p 8082 -t 5000");
        ss.bind<EchoHandle>("tcp -h 172.25.38.14 -p 8882 -t 5000");
        ss.bind<StringHandle>("tcp -h 172.25.38.67 -p 8082 -t 5000");

        ss.waitForShutdown();
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}

*/

