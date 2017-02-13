#include "TestServer.h"
#include "util/tc_common.h"

void Server::initialize()
{
    //增加对象
    addServant<TestObj>(ServerConfig::Application + "." + ServerConfig::ServerName +".TestObj");
    addServant<TestExObj>(ServerConfig::Application + "." + ServerConfig::ServerName +".TestExObj");
    LOG->debug() << "Server::initialize ok" << endl;
}

void Server::destroyApp()
{
    cout << "Server::destroyApp ok" << endl;
}

bool TestObj::procAdmin(const string& command, const string& params, string& result)
{
    result = string("[") + command + " proc succ]";

    return true;
}

Server1::TestInfo TestObj::get(const Server1::TestInfo& info, taf::JceCurrentPtr)
{
    cout << "get" << endl;
    info.display(cout);

    return info;
}

int TestObj::getOut(int i, AInfo &ai1, AInfo &ai2, JceCurrentPtr current)
{
    /*
    cout << "getOut:" << i << endl;
    */
    ai1.i = 100232;
    ai1.s = "test";

    ai2.i = 10022;
    ai2.s = "tes2t";
    return i;
}

int TestObj::test(const Server1::TestInfo &tii, JceCurrentPtr current)
{
    LOG->debug() << "test" << endl;
//    tii.display(cout);
	return 0;
}

string TestObj::echo(const string &ti, JceCurrentPtr current)
{
//    static int __request = 0;

    //if (__request++%1000 == 0)
    {
        //LOG->debug() << time(NULL) << "|" << current->getRequestId() << ":" << ti << endl;
        cout << time(NULL) << "|" << current->getRequestId() << ":" << ti << endl;
    }
    //cout << time(NULL) << "|" << current._request.iRequestId << ":" << ti << endl;
    //cout << time(NULL) << "|" << current->getRequestId() << ":" << ti << endl;

    current->setResponse(false);
    async_response_echo(current, ti);

    return ti;
}

int TestObj::echo_int(int i, JceCurrentPtr current)
{
    return i;
}

ETest TestObj::testETest(ETest t, JceCurrentPtr current)
{
    return t;
}

void TestObj::initialize()
{
    cout << "TestObj::initialize" << endl;

    TAF_ADD_ADMIN_CMD_NORMAL("please", TestObj::procAdmin);
}

void TestObj::destroy()
{
    cout << "TestObj::destroy" << endl;
}

int TestExObj::test(const Server1::TestInfo &tii, JceCurrentPtr current)
{
    tii.display(cout);
	return 0;
}


int TestExObj::testEx(const Server2::TestInfo &ti, JceCurrentPtr current)
{
    ti.display(cout);
	return 0;
}

