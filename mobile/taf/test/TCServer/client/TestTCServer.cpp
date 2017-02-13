#include "util/tc_socket.h"
#include "util/tc_clientsocket.h"
#include "util/tc_http.h"
#include "util/tc_logger.h"
#include "util/tc_common.h"
#include "util/tc_thread_pool.h"
#include "../Hello.h"
#include "servant/Application.h"
//#include "JceProxy.h"
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

using namespace taf;
using namespace Test;

TC_RollLogger logger;


void testShortEcho()
{
    logger.debug() << pthread_self() << ":testShortEcho" << endl;

    int64_t n = TC_Common::now2us();
    int i = 10000000;
    string s;
    for(int j = 0; j < 2048; j++)
    {
        s += "a";
    }
    s += "\n";
    while(i>0)
    {
        TC_TCPClient tc;
        tc.init("127.0.0.1", 8082, 3000);

        char c[8192] = "\0";
        size_t length = s.length();

        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);

        if(iRet < 0)
        {
            logger.debug() << "testShortEcho:send recv error:" << pthread_self() << ":" << i << ":" << iRet << endl;
        }
        if(i % 5000 == 0)
        {
            int64_t n2 = TC_Common::now2us();
            int n1 = n2 - n;
            logger.debug() << "testShortEcho:" << i << ":" << n1/1000 << endl;
            n = TC_Common::now2us();
        }
        i--;
//        assert(c == s);
    }
}

void testEchoShortThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(10);
    tpool1.start();

    TC_Functor<void> cmd1(testShortEcho);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();

    logger.debug() << TC_Common::now2str() << endl;
}

void testEchoTimeout()
{

    logger.debug() << pthread_self() << ":testEchoTimeout" << endl;

    int64_t n = 0;
    int i = 2000000;
    while(i>100000)
    {
        TC_TCPClient tc;
        tc.init("127.0.0.1", 8082, 3000);

        string s = TC_Common::tostr(i) + "\n";
        char c[1024] = "\0";
        size_t length = s.length();
        int64_t n1 = TC_Common::now2us();
        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);
        int64_t n2 = TC_Common::now2us();
        n += n2 - n1;

        if(iRet < 0)
        {
            logger.debug() << "testShortEcho:send recv error:" << iRet << ":" << c << endl;
        }
        if(i % 5000 == 0)
        {
            logger.debug() << "testShortEcho:" << i << ":" << n / 1000 << endl;
        }
        i--;
        sleep(8);
//        assert(c == s);
    }
}

////////////////////////////////////////////////////////////////////////
// 

void testHttp()
{
    int i = 1;//0000;
    while(i > 0)
    {
        TC_HttpRequest stHttpReq;
        stHttpReq.setCacheControl("no-cache");
        stHttpReq.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322)");
        stHttpReq.setGetRequest("http://172.25.38.19:8080/requestpay.cgi?actcode=3&amount=3&appid=1&area=432&busiext=Mw%3D%3D&busiflow=423&busiment=5555&buyer=3&datetime=342&ip=10%2E2%2E77%2E35&mode=WEB&mpayext=3&payer=3&paypwd=C7cDGYpod78%3D&paytype=432&provider=1&returl=3&sign=1d05f882b2bcfc4a293a2ccba0d4adbf&version=1.0");
        string sSendBuffer = stHttpReq.encode();

        TC_HttpResponse stHttpRep;
        int iRet = stHttpReq.doRequest(stHttpRep, 3000);

        LOG->debug() << iRet << endl;

        string r = stHttpRep.encode();

//        if(i % 5000 == 0)
//        {
//            logger.debug() << "testHttp:" << i << ":" << r<< endl;
//        }

        
        logger.debug() << stHttpRep.getContent().size() << endl;

        --i;
    }
}

void testHttpThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(1);
    tpool1.start();

    TC_Functor<void> cmd1(testHttp);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();

    logger.debug() << TC_Common::now2str() << endl;
}

void testUdpEcho()
{
    logger.debug() << pthread_self() << ":testUdpEcho" << endl;

    int64_t n = TC_Common::now2us();
    int i = 100000;
    string s;
    for(int j = 0; j < 1000; j++)
    {
        s += "a";
    }
    s += "\n";
    while(i>0)
    {
        TC_UDPClient tc;
        tc.init("172.25.38.67", 8082, 1000);
//        tc.init("172.25.38.14", 8882, 3000);

        char c[8192] = "\0";
        size_t length = s.length();

        int iRet = tc.sendRecv(s.c_str(), s.length(), c, length);

        if(iRet < 0)
        {
            logger.debug() << "testUdpEcho:send recv error:" << pthread_self() << ":" << i << ":" << iRet << endl;
        }
        else
        {
            assert(c == s);
        }
        if(i % 5000 == 0)
        {
            int64_t n2 = TC_Common::now2us();
            int n1 = n2 - n;
            logger.debug() << "testUdpEcho:" << i << ":" << n1/1000 << endl;
            n = TC_Common::now2us();
        }
        i--;
//        assert(c == s);
    }
}

void testUdpEchoThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(10);
    tpool1.start();

    TC_Functor<void> cmd1(testUdpEcho);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();

    logger.debug() << TC_Common::now2str() << endl;
}


void testHelloEcho()
{
    taf::CommunicatorPtr c = new taf::Communicator();
    HelloPrx hPrx = c->stringToProxy<HelloPrx>("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 22345");

    int64_t n = TC_Common::now2us();
    int i = 10;//0000;
    string s;
    for(int j = 0; j < 1024; j++)
    {
        s += "a";
    }
    s += "\n";

    logger.debug() << pthread_self() << ":testHelloEcho" << endl;

    while(i>0)
    {
        string r;
        try
        {
			logger.debug() << i << endl;
            hPrx->testHello(s, r);
			logger.debug() << i << " ok"<< endl;
			assert(s == r);
        }
        catch(exception &ex)
        {
            logger.debug() << ex.what() << endl;
        }

        if(i % 10000 == 0)
        {
            int64_t n2 = TC_Common::now2us();
            int n1 = n2 - n;
            logger.debug() << "testHelloEcho:" << i << ":" << n1/1000 << endl;
            n = TC_Common::now2us();
        }
        i--;
    }
}

void testHelloEchoThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(1);
    tpool1.start();
    TC_Functor<void> cmd1(testHelloEcho);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();
    logger.debug() << TC_Common::now2str() << endl;
}
/*
void testJceHelloEcho()
{
    JceProxyPtr jpp = new JceProxy();
    logger.debug() << pthread_self() << ":testJceHelloEcho" << endl;
    try
    {
        jpp->initialize("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 22345 -t 3000");
//        jpp->initialize("Test.HelloServer.HelloObj@tcp -h 127.0.0.1 -p 8082 -t 3000");
    }
    catch(exception &ex)
    {
        logger.debug() << ex.what() << endl; 
    }
    logger.debug() << pthread_self() << ":testJceHelloEcho1" << endl; 

    int64_t n = TC_Common::now2us();
    int i = 100000;
    string s;
    for(int j = 0; j < 1024; j++)
    {
        s += "a";
    }
    s += "\n";

    logger.debug() << pthread_self() << ":testJceHelloEcho" << endl;

    while(i>0)
    {
        string r;
        try
        {
            taf::JceOutputStream<taf::BufferWriter> _os;
            _os.write(s, 1);
            taf::ResponsePacket rep;

            jpp->_invoke(taf::JCENORMAL,"testHello", _os.getByteBuffer(), rep);
            taf::JceInputStream<taf::BufferReader> _is;
            _is.setBuffer(rep.sBuffer);
            taf::Int32 _ret;
            _is.read(_ret, 0, true);
            _is.read(r, 2, true);
        }
        catch(exception &ex)
        {
            logger.debug() << ex.what() << endl;
        }

        if(i % 10000 == 0)
        {
            int64_t n2 = TC_Common::now2us();
            int n1 = n2 - n;
            logger.debug() << "testJceHelloEcho:" << i << ":" << n1/1000 << endl;
            n = TC_Common::now2us();
        }
        i--;
    }
}

void testJceHelloEchoThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(10);
    tpool1.start();
    TC_Functor<void> cmd1(testJceHelloEcho);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();
    logger.debug() << TC_Common::now2str() << endl;
}
*/
void testEchoLong()
{
    logger.debug() << pthread_self() << ":testEchoLong" << endl;

    TC_TCPClient tc;
    tc.init("127.0.0.1", 8082, 3000);
//    tc.init("172.25.38.14", 8882, 3000);

    logger.debug() << TC_Common::now2str() << endl;
    
    string sep = "\n";
    
    int64_t n = TC_Common::now2us(); 

    string s;
    for(int j = 0; j < 20; j++)
    {
        s += "a";
    }
    s += "\n";
    string c;

    int i = 100000;
    
    while(i>0)
    {
//        int64_t n1 = TC_Common::now2us();

        int iRet = tc.sendRecvBySep(s.c_str(), s.length(), c, sep);
        assert(iRet == 0);

        if(i % 5000 == 0)
        {
            int64_t n2 = TC_Common::now2us();
            int n1 = n2 - n;
            logger.debug() << "testEchoLong:" << i << ":" << n1/1000 << endl;
            n = TC_Common::now2us();
        }

        assert(c == s);

        --i;
    }
    logger.debug() << TC_Common::now2str() << endl;
}

void testEchoLongThread()
{
    logger.debug() << TC_Common::now2str() << endl;

    TC_ThreadPool tpool1;
    tpool1.init(1);
    tpool1.start();

    TC_Functor<void> cmd1(testEchoLong);
    TC_Functor<void>::wrapper_type wt1(cmd1);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }
    tpool1.waitForAllDone();
    tpool1.stop();

    logger.debug() << TC_Common::now2str() << endl;
}


void testEcho()
{
    TC_ThreadPool tpool1;
    TC_ThreadPool tpool2;
    TC_ThreadPool tpool3;

    tpool1.init(10);
    tpool2.init(10);
    tpool3.init(10);

    tpool1.start();
    tpool2.start();
    tpool3.start();

    logger.debug() << "initialize tpool ok" << endl;

    TC_Functor<void> cmd1(testEchoLong);
    TC_Functor<void> cmd2(testShortEcho);
    TC_Functor<void> cmd3(testEchoTimeout);

    TC_Functor<void>::wrapper_type wt1(cmd1);
    TC_Functor<void>::wrapper_type wt2(cmd2);
    TC_Functor<void>::wrapper_type wt3(cmd3);

    for(size_t i = 0; i < tpool1.getThreadNum(); i++)
    {
        tpool1.exec(wt1);
    }

    for(size_t i = 0; i < tpool2.getThreadNum(); i++)
    {
        tpool2.exec(wt2);
    }

    for(size_t i = 0; i < tpool3.getThreadNum(); i++)
    {
        tpool3.exec(wt3);
    }

    logger.debug() << "wait tpool finish" << endl;
    tpool1.waitForAllDone();
    tpool2.waitForAllDone();
    tpool3.waitForAllDone();

    logger.debug() << "tpool finish ok" << endl;

    tpool1.stop();
    tpool2.stop();
    tpool3.stop();

    logger.debug() << "tpool stop ok" << endl;
}


int main(int argc, char *argv[])
{
    try
    {
//        TafRollLogger::getInstance()->logger()->setLogLevel(TC_RollLogger::INFO_LOG);

//        testEcho();
//        testHelloEchoThread();
//        testJceHelloEchoThread();
//        testUdpEchoThread();
        testHttpThread();
//        testEchoLongThread();
//        testEchoShortThread();
	}
	catch(exception &ex)
	{
        logger.debug() << ex.what() << endl;
	}

	return 0;
}


