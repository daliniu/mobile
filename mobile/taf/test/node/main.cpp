#include <iostream>
#include "Node.h"
#include "StatQuery.h"
#include "patch/taf_patch.h"
#include "servant/Communicator.h"
#include "servant/Application.h"
#include "nodeF/NodeF.h"
#include "servant/AdminF.h"
#include "servant/QueryF.h"
#include "servant/BaseF.h"
#include "util/tc_socket.h"
#include "util/tc_clientsocket.h"
#include "Server.h"

//#include "JceProxy.h"

using namespace std;
using namespace taf;
using namespace Server1;

taf::Communicator _comm;

void  testConfig( )
{

        TC_Config conf;
        map<string, string> m;

        m["basepath"] = "1basepath";
        conf.insertDomainParam( "/taf/framework", m, true );
        m["basepath"] = "2basepath";
        m["basepath"] = "3asepath";
        conf.insertDomainParam( "/taf/framework", m, true );
        m["basepath"] = "1basepath";


        m["app"] = "app";
        m["app"] = "app2";
        m["app"] = "app3";

        conf.insertDomainParam( "/taf/framework", m, true );
        ofstream configfile( "./a.conf" );
        if ( !configfile.good() )
        {
            cout << "couldn't create configuration file: "<< endl;
        }

        configfile <<TC_Common::replace(conf.tostr(),"\\s"," ") << endl;
        configfile.close();

        cout<<"app"<<conf.get("/taf/framework<app>")<<endl;
}

void getLoad( const NodePrx& node )
{
    cout << node->getName() << endl;
    LoadInfo info = node->getLoad();
    cout << info.avg1 << "|" << info.avg5 << "|" << info.avg15 << endl;
}
/*
void    testMap()
{
    map<string,string>  m;
    map<string, string>::iterator it = m.begin();
    it->push_back("1");
}
*/


void getServerState( const NodePrx& node, const string& app, const string& name )
{
    string result;
    ServerState st = node->getState( app, name,result );
    if ( st == taf::Inactive )
    {
        cout << "Server::Inactive" << endl;
    }
    cout << st << endl;
}

void startStop( const NodePrx& node, const string& app, const string& name )
{
    string result;
    if ( node->startServer( app, name, result) != 0 )
    {
        cout << "first start erro,may be is runing" << endl;
        if ( node->stopServer( app, name,result ) == 0 )
        {
            cout << "stop server succ " << endl;
        }
        else
        {
            cout << "stop erro" << endl;
        }
        if ( node->startServer( app, name,result ) != 0 )
        {
            cout << "second start succ" << endl;
        }
    }
}


void stop( const NodePrx& node, const string& app, const string& name )
{
    string result;
    if ( node->stopServer( app, name,result ) == 0 )
    {
        cout << "stop server succ " << endl;
    }
    else
    {
        cout << "stop erro" << endl;
    }
}



void shutdown( const NodePrx& node )
{
    string result;
    if ( node->shutdown(result) == 0 )
    {
        cout << "shutdown node  succ " << endl;
    }
    else
    {
        cout << "shutdown node erro" << endl;
    }
}
void patchServer( const NodePrx& node, const string& app, const string& name )
{
    string result;
    if ( node->patch( app, name, false,"taf.onepatch.PatchObj" ,result) == 0 )
    {
        cout << "patch succ" << endl;
    }
    else
    {
        cout << "patch erro" << endl;
    }
}




int KeepAlive( const string& app, const string& name )
{
    ServerInfo serverInfo;
    serverInfo.application = app;
    serverInfo.serverName = name;
    serverInfo.pid = getpid();

    ServerFPrx pServerPtr;

    _comm.stringToProxy( "ServerObj@tcp -h 10.1.36.40 -p 19386", pServerPtr );
    // _comm.stringToProxy("TestObj@udp -h 10.1.36.39 -p 9995 -t 30000", pServerPtr);
    /*
    ServerFPrx pServerPtr = new ServerFProxy();
    //string strProxy = "TestObj@tcp -h 10.1.36.39  -p 9995 -t 30000";
    string strProxy = "TestObj@udp -h 10.1.36.39  -p 9995 -t 30000";
    cout<<"Test  KeepAlive"<<endl;
    pServerPtr->initialize(strProxy );
    cout<<"init "<<strProxy<<" succ"<<endl;
    */

    cout << "-------------------test  begin------------------------------" << endl;
//  pServerPtr->keepAlive( serverInfo );
    pServerPtr->async_keepAlive( NULL,serverInfo );
    sleep(100);
    return 0;
}

int TestDownLoad( const string& app, const string& name )
{
    /*
    PatchPrx pPtr = new PatchProxy();
    pPtr->initialize( "PatchObj@tcp  -h 10.1.36.39 -p 14567-t 40000" );
    */

    PatchPrx pPtr;

    _comm.stringToProxy( "PatchObj@tcp  -h 10.1.36.39 -p 14567-t 40000", pPtr );

    TafPatch tafPatch;
    string strLocalPatch = "/home/skingfan/taf/test/node/test";
    string strRemotePath = "/" + app + "/" + name;
    tafPatch.init( pPtr, strRemotePath, strLocalPatch );
    tafPatch.setRemove( false );
    tafPatch.download( NULL );
    return 0;
}

void getAddress(const string& host, int port, struct sockaddr_in& addr)
{
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if(addr.sin_addr.s_addr == INADDR_NONE)
    {
        struct addrinfo* info = 0;
        int retry = 5;

        struct addrinfo hints = { 0 };
        hints.ai_family = PF_INET;
        cout<<"erro|" <<strerror(errno) <<endl;
        int rs = 0;
        do
        {
            rs = getaddrinfo(host.c_str(), 0, &hints, &info);
        }
        while(info == 0 && rs == EAI_AGAIN && --retry >= 0);

        if(rs != 0)
        {
            ostringstream os;
            os << "DNSException ex:(" << strerror(errno) << ")" << rs << ":" << host << ":" << __FILE__ << ":" << __LINE__;
            throw TafException(os.str());
        }

        assert(info->ai_family == PF_INET);
        struct sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(info->ai_addr);

        addr.sin_addr.s_addr = sin->sin_addr.s_addr;
        freeaddrinfo(info);
    }
}




int shuDownByProxy( const string& app, const string& name )
{
    /*
    AdminFPrx pAdminPrx = new AdminFProxy();
    cout<< "AdminObj@tcp -h /tmp/" +app + "." +name + ".sock -p 0 -t 5000"<<endl;
    pAdminPrx->initialize( "AdminObj@tcp -h /tmp/" +app + "." +name + ".sock -p 0 -t 5000");
    */

    AdminFPrx pAdminPrx;
    cout << "AdminObj@tcp -h /tmp/" + app + "." + name + ".sock -p 0 -t 5000" << endl;
    _comm.stringToProxy( "AdminObj@tcp -h /tmp/" + app + "." + name + ".sock -p 0 -t 5000", pAdminPrx );

    pAdminPrx->shutdown();
    return 0;
}


int findObjectById( string  id )
{
    /*
    QueryFPrx queryPtr = new QueryFProxy();
    queryPtr->initialize("QueryObj@tcp -h 10.1.36.39 -p 17890 -t 3000");
    */

    QueryFPrx queryPtr;

    _comm.stringToProxy( "QueryObj@tcp -h 10.1.36.39 -p 17890 -t 3000", queryPtr );

    vector<EndpointF> vEndpoints = queryPtr->findObjectById( id );

    for ( unsigned i = 0; i < vEndpoints.size(); i++ )
    {
        cout << vEndpoints[i].host << ":" << vEndpoints[i].port << ":" << vEndpoints[i].timeout << "|";
    }
    cout << endl;

    return 0;
}

/*
void testInterface()
{
    try
    {
        typedef map<string, string> CONTEXT;
        JcePrx pPrx = new JceProxy();
        string strProxy ="TestObj@tcp -h  10.1.36.39   -p 9995 -t 30000";

        cout<<"testInterface"<<endl;
        pPrx->initialize(strProxy);
        cout<<"init "<<strProxy<<" succ"<<endl;

        string s= "test";
        taf::JceOutputStream<taf::BufferWriter> _os;
        _os.write(s, 1);
        taf::ResponsePacket rep;
        pPrx->_invoke(taf::JCE_NORMAL,"echo", _os.getBuffer(), _os.getLength(), CONTEXT(), rep);
        taf::JceInputStream<taf::BufferReader> _is;
        _is.setBuffer(rep.sBuffer.c_str(), rep.sBuffer.length());
        std::string _ret;
        _is.read(_ret, 0, true);
        cout<<"return|"<<_ret<<endl;


        /////////////////////////////////////////
    }
    catch (exception& e)
    {
        TRACE << "[_invoke exception]:" << e.what() << endl;

        throw;
    }
    catch (...)
    {
        TRACE << "[_invoke exception.]" << endl;

        throw;
    }
}


*/



void testServer()
{
        TestPrx pServerPtr;
        _comm.stringToProxy("Comm.BindServer.BindObj@tcp -h  10.1.36.39   -p 22224 -t 30000", pServerPtr);
        //_comm.stringToProxy("Comm.BindServer.BindObj", pServerPtr);

        cout<<"-------------------test  begin------------------------------"<<endl;
        time_t tBegin = TC_Common::now2us();
        for( int i=0;i<10;i++)
        {
            try
            {
                pServerPtr->echo("test");
            }
            catch(exception &e)
            {
                cout<<e.what()<<endl;
            }
        }
        time_t tEnd = TC_Common::now2us();
        cout<<"use time:"<<tEnd-tBegin<<endl;
        cout<<"echo|"<<endl;
}

int main( int argc, char* argv[] )
{
    try
    {
        //      string app="Comm";
        /*
                string app="Comm";
                string name="OidbServer";
                char s1[11]="1234567890";
                cout<<string(s1,10)<<endl;
                cout<<TC_Common::bin2str(string(s1,10))<<endl;
                cout<<TC_Common::replace(TC_Common::bin2str(string(s1,10))," ","")<<endl;
                char s2[11]="0123456789";
                cout<<string(s2,10)<<endl;
                cout<<TC_Common::bin2str(string(s2,10))<<endl;
                cout<<TC_Common::replace(TC_Common::bin2str(string(s2,10))," ","")<<endl;
        */
        //      testConfig();

                //_comm.setProperty("locator", "taf.oneregistry.QueryObj@tcp -h  172.25.38.67 -p 17890 -t 3000");
                //cout<<"ok"<<endl;
               NodePrx node =_comm.stringToProxy<taf::NodePrx>("taf.tafnode1.NodeObj@tcp -h  172.25.38.67 -p 19385 -t 3000");
               //NodePrx node =_comm.stringToProxy<taf::NodePrx>("NodeObj");

        //      node->initialize( "NodeObj@tcp -h 10.1.36.39  -p 19385 -t 30000" );
        //      node->initialize( "NodeObj@tcp -h 127.0.0.1   -p 7385 -t 30000" );
        //      KeepAlive(app, name);
        //      testServer();

         //      getLoad(node);
        //      cout<<"-----------------------------------"<<endl;


        //      for(int i=0; i<100000;i++)
        //      testInterface();
                shutdown(node);
        //      getServerState(node,app, name);
        //      startStop(node, app, name);
        //      patchServer(node,app, name);

        /*
        fork();
        fork();
        for(int i=0;i<100000;i++)
        */
        //      findObjectById("Comm.DbServer.DbObj");
        //      getServerState(node,app, name);
        //      {

        //      cout << i << endl;
        //      }


        //      struct sockaddr_in addr;
        //      getAddress("/tmp/" + app + "." + name + ".sock ", 0, addr);
        //      getAddress("/tmp/node.sock", 0, addr);
        //      shuDownByProxy(app, name);
        //      TestDownLoad(app, name);
        //      stop(node, app, name);
        /*
        vector<string> v1,v2;
        v1.push_back("1");
        v1.push_back("2");
        v1.push_back("3");
        v2=v1;
        cout<<v2.size()<<"|"<<v1.size()<<endl;
        */
    }
    catch ( exception& ex )
    {
        cout << ex.what() << endl;
    }
    cout << "**********************" << endl;
    return 0;
}


