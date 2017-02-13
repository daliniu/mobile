#include "TestServer.h"
#include "TestBaseImp.h"
#include "TestExtImp.h"
#include "util/tc_common.h"

void TestServer::initialize()
{
    //增加对象
    addServant<TestBaseImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".TestBaseObj");
    addServant<TestExtImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".TestExtObj");

    addConfig("TestServer.conf");

    TAF_ADD_ADMIN_CMD_NORMAL(TAF_CMD_LOAD_CONFIG, TestServer::procAdmin);

    TAF_NOTIFY_NORMAL("initialize");

    LOG->debug() << "Server::initialize ok" << endl;

    cout << "Server::initialize ok" << endl;
}

void TestServer::destroyApp()
{
    cout << "Server::destroyApp ok" << endl;
}

bool TestServer::procAdmin(const string& command, const string& params, string& result)
{
    result = string("[TestServer::procAdmin:") + command + " proc succ]";

    return true;
}
