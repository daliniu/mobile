#ifndef __REGISTRY_PROXY_H_
#define __REGISTRY_PROXY_H_
#include "Node.h"
#include "ServerFactory.h"
#include "util/tc_config.h"
#include "AdminReg.h"
#include "QueryF.h"
#include "servant/Communicator.h"
#include "util/tc_singleton.h"

#include "CommandStart.h"
#include "CommandPatch.h"
#include "CommandNotify.h"
#include "CommandStop.h"
#include "CommandDestroy.h"
#include "CommandAddFile.h"
#include "CommandDownload.h"

using namespace taf;
using namespace std;

class AdminProxy;
typedef TC_AutoPtr<AdminProxy> AdminPrx;

class AdminProxy : public TC_Singleton<AdminProxy>
{
public:
    RegistryPrx getRegistryProxy()
    {
        RegistryPrx pRistryPrx;
        string sRegistry = g_app.getConfig().get("/taf/node<registryObj>",_sRegistryProxyName);
        Application::getCommunicator()->stringToProxy(sRegistry, pRistryPrx);
		
        return pRistryPrx;
    }

    QueryFPrx getQueryProxy()
    {
    	QueryFPrx pQueryPrx;
		string sQuery = g_app.getConfig().get("/taf/node<queryObj>", _sQueryProxyName);
	 	Application::getCommunicator()->stringToProxy(sQuery, pQueryPrx);

	 	return pQueryPrx;
    }

    inline void setRegistryObjName(const string &sRegistryProxyName, const string &sQueryProxyName,const string &sAdminRegProxyName) 
    {
    	_sRegistryProxyName = sRegistryProxyName;
		_sQueryProxyName	= sQueryProxyName;
		_sAdminRegProxyName = sAdminRegProxyName;
    }

    inline string& getRegistryProxyName(){return _sRegistryProxyName;}

    inline string& getQueryProxyName() {return _sQueryProxyName;}	

    inline string& getAdminRegProxyName() {return _sAdminRegProxyName;}
private:
	
    string _sRegistryProxyName;
    string _sQueryProxyName;
    string _sAdminRegProxyName;
};
#endif


