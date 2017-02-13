#include "TestProxyServer.h"
#include "TestProxyImp.h"
#include "util/tc_file.h"
#include "util/tc_encoder.h"
using namespace std; 

TestProxyServer g_app;

struct TestProxyProtocol
{
    static int parseHttp(string &in, string &out)
    {
        try
        {
			//判断请求是否是HTTP请求
            bool b = TC_HttpRequest ::checkRequest(in.c_str(), in.length());
            if (b)
            {
                LOG->debug() << in.substr(0, in.find("\r\n\r\n")) << endl;
                out = in;
                in  = "";
                return TC_EpollServer::PACKET_FULL;
            }
            else
            {
                return TC_EpollServer::PACKET_LESS;
            }
        }
        catch(exception &ex)
        {
            LOG->debug() << "error:" << ex.what() << endl;
            return TC_EpollServer::PACKET_ERR;
        }

        return TC_EpollServer::PACKET_LESS;		//表示收到的包不完全
    }
};

/////////////////////////////////////////////////////////////////
void TestProxyServer::initialize()
{	
	//initialize application here:
	//...
	addServant<TestProxyImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".TestProxyObj");
	addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".TestProxyObj", &TestProxyProtocol::parseHttp);
}

/////////////////////////////////////////////////////////////////
void TestProxyServer::destroyApp()
{
	//destroy application here:
	//...
}

/////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
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
