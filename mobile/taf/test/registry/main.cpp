#include "Registry.h"
#include "AdminReg.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "servant/QueryF.h"
#include "servant/Application.h"

#include <iostream>

using namespace std;
using namespace taf;

taf::Communicator _comm;

int test_regNode(RegistryPrx & regPtr)
{

	NodeInfo ni;
	ni.nodeObj = "taf.tafnode1.NodeObj@tcp -h 172.25.38.67 -p 19385 -t 6000"; 
	ni.endpointIp = "10.1.36.39"; 
	ni.endpointPort = 12456;
	ni.dataDir = "/usr/local/app/node";
	ni.version = TAF_VERSION;

	LoadInfo li;
	li.avg1 = 1.1;
	li.avg5 = 1.5;
	li.avg15 = 1.15;

	int ret = regPtr->registerNode("node_test", ni, li);
	cout<< "registerNode return " << ret<< endl;

	return 0;
}

int test_keepAlive(RegistryPrx & regPtr)
{

	LoadInfo li;
	li.avg1 = 1.1;
	li.avg5 = 1.5;
	li.avg15 = 1.15;

	regPtr->keepAlive("node1", li);

	return 0;
}

int test_destroy(RegistryPrx & regPtr)
{
	regPtr->destroy("node1");
	return 0;
}

int test_getServers(RegistryPrx & regPtr)
{
	cout<<"test_getServers"<<endl;
	vector<ServerDescriptor> vServers = regPtr->getServers("Comm", "DbServer", "node1");

	for(unsigned i = 0; i<vServers.size(); i++)
	{
		ServerDescriptor & server = vServers[i];
		cout<<"==============="<<endl;
		cout<<"\tapp:\t"<<server.application;
		cout<<"\tserver:\t"<<server.serverName;
		cout<<"\tnode:\t"<<server.nodeName;
		cout<<"\tbase:\t"<<server.basePath;
		cout<<"\texepath:\t"<<server.exePath;
		cout<<"\tstate:\t"<<server.settingState<<endl;
		cout<<"\tprofile:\t"<<server.profile<<endl;

		map<string, AdapterDescriptor>::iterator it1;
		for(it1=server.adapters.begin(); it1 != server.adapters.end(); it1++)
		{
			cout<<"---adapter name : "<<it1->first<<endl;
			cout<<"\tname:"<<it1->second.adapterName;
			cout<<"\tthreadNum:"<<it1->second.threadNum;
			cout<<"\tprotocol:"<<it1->second.endpoint;
			cout<<"\tmax conn:"<<it1->second.maxConnections;
			cout<<"\tallow ip:"<<it1->second.allowIp<<endl;
			cout<<"\tservant:"<<it1->second.servant<<endl;
			cout<<"\tcap:"<<it1->second.queuecap<<endl;
			cout<<"\tqueuetimeout:"<<it1->second.queuetimeout<<endl;
		}
	}
	
	return 0;
}

int test_updateServer(RegistryPrx & regPtr)
{
	ServerStateInfo info;
	info.serverState = taf::Active;
	info.processId = 16666;
	regPtr->updateServer("10.1.36.39","Comm", "DbServer",  info);
	return 0;
}

int test_reportVersion(RegistryPrx & regPtr)
{
	regPtr->reportVersion("Comm", "DbServer", "node1", "Taf_Beta01_Build002");
	return 0;
}

int test_getNodeTemplate(RegistryPrx & regPtr)
{
    string sTemplate;
    regPtr->getNodeTemplate("node_test", sTemplate);
    cout << sTemplate;

    return 0;
}


int test_findObjectById(QueryFPrx & queryPtr)
{
	vector<EndpointF> activeEp, inactiveEp;
    
    string id = "Comm.DbServer.DbObj";
    timeval tStart,tEnd;
    gettimeofday(&tStart, NULL);

    activeEp= queryPtr->findObjectById(id);

    gettimeofday(&tEnd, NULL);
    int iInterval=(tEnd.tv_sec-tStart.tv_sec)*1000000+tEnd.tv_usec-tStart.tv_usec;
    if(iInterval > 1000) cout << iInterval << endl;

    return 0;
    ///////////////////
    cout<<"findObjectById:"<< endl;
	for(unsigned i=0; i<activeEp.size(); i++)
	{
		cout<<activeEp[i].host<<":"<<activeEp[i].port<<":"<<activeEp[i].timeout<<"," << activeEp[i].istcp<< "|";
	}
	cout<<endl;

    cout<<"findObjectById4All:"<< endl;
    queryPtr->findObjectById4All(id, activeEp, inactiveEp);

    cout<<"active:"<< endl;
	for(unsigned i=0; i<activeEp.size(); i++)
	{
		cout<<activeEp[i].host<<":"<<activeEp[i].port<<":"<<activeEp[i].timeout<<"," << activeEp[i].istcp<< "|";
	}
    cout<< endl;
    cout<<"inactive:"<< endl;
	for(unsigned i=0; i<inactiveEp.size(); i++)
	{
		cout<<inactiveEp[i].host<<":"<<inactiveEp[i].port<<":"<<inactiveEp[i].timeout<<"," << inactiveEp[i].istcp<< "|";
	}
    cout<< endl;

	return 0;
}

int test_getAllApplicationNames(AdminRegPrx & adminPtr)
{
    string sResult;
	vector<string > vApps = adminPtr->getAllApplicationNames(sResult);

	for(unsigned i=0; i<vApps.size(); i++)
	{
		    cout<<vApps[i]<<endl;
	}

	return 0;
}
/*
struct test_
{
    std::string adapterName;
    std::string threadNum;
    std::string endpoint;
    taf::Int32 maxConnections;
    std::string allowIp;
    std::string servant;
    taf::Int32 queuecap;
    taf::Int32 queuetimeout;
};

DEFINE_JCE_COPY_STRUCT(taf, AdapterDescriptor, test_);
*/
int main(int argc, char *argv[])
{
    try
	{
        /*
        AdapterDescriptor a1;
        test_ b1;
        jce_copy_struct(a1, b1);
        jce_copy_struct(b1, a1);
        */

        //_comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 10.1.36.40 -p 17890 -t 3000");
       // _comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.25.38.67 -p 17890 -t 3000");
        _comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 127.0.0.1 -p 17890 -t 3000");

		//Query==========================================
		QueryFPrx queryPtr;
		_comm.stringToProxy("taf.tafregistry.QueryObj@tcp -h 127.0.0.1 -p 17890 -t 3000", queryPtr);

        //TafRollLogger::getInstance()->logger()->setLogLevel("INFO");
		test_findObjectById(queryPtr);

		//Registry==========================================
		RegistryPrx regPtr;
        //_comm.stringToProxy("taf.tafregistry.RegistryObj", regPtr);
        _comm.stringToProxy("taf.tafregistry.RegistryObj@tcp -h 127.0.0.1 -p 17891 -t 3000", regPtr);

		//test_regNode(regPtr);
		//test_keepAlive(regPtr);
		//test_destroy(regPtr);
		//test_getServers(regPtr);
		//test_updateServer(regPtr);
		//test_reportVersion(regPtr);
		//test_getNodeTemplate(regPtr);

		//Admin==========================================
		AdminRegPrx adminPtr;
        _comm.stringToProxy("taf.tafregistry.AdminRegObj", adminPtr);

		cout<<endl<<"-------"<<endl;

		/*
		test_getAllApplicationNames(adminPtr);

		cout<<endl<<"-------"<<endl;

		cout<<endl<<"get  node:"<< adminPtr->getAllNodeNames().size();
		cout<<endl<<"ping node:"<< adminPtr->pingNode("node1");
		cout<<endl<<"shut node:"<< adminPtr->shutdownNode("node1");

		cout<<endl<<"-------"<<endl;

		cout<<endl<<"get svr  :"<< adminPtr->getAllServerIds().size();
		cout<<endl<<"svr state:"<< adminPtr->getServerState("COMM", "DbServer", "node1");
		cout<<endl<<"svr pid  :"<< adminPtr->getServerPid("COMM", "DbServer", "node1");
		cout<<endl<<"start svr:"<< adminPtr->startServer("COMM", "DbServer", "node1");
		cout<<endl<<"stop  svr:"<< adminPtr->stopServer("COMM", "DbServer", "node1");
		cout<<endl<<"patch svr:"<< adminPtr->patchServer("COMM", "DbServer", "node1", "temp_dir", true);
		cout<<endl<<"enablesvr:"<< adminPtr->enableServer("COMM", "DbServer", "node1", 1);
		*/
		/*
		*/

//		cout<<endl;
	}
	catch(TafException &ex)
	{
        cout << ex.what() << endl;
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

//    cout << "**********************" << endl;
	return 0;
}



