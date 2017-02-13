#ifndef __ROUTER_PROXY_CALLBACK_H
#define __ROUTER_PROXY_CALLBACK_H

#include "routerProxy.h"
#include "QueryF.h"

enum FUNID
{
	FUNID_findObjectById = 0,
	FUNID_findObjectById4Any = 1,
	FUNID_findObjectById4All = 2,
	FUNID_findObjectByIdInSameGroup = 3,
	FUNID_findObjectByIdInSameStation = 4,
	FUNID_findObjectByIdInSameSet = 5
};

class RouterProxyCallback:public routerProxy::RouterProxyServicePrxCallback
{
public:
		RouterProxyCallback(const std::string & id,const FUNID eFnId, const taf::JceCurrentPtr& current);
        virtual ~RouterProxyCallback();
        virtual void callback_getServerList(taf::Int32 ret,  const vector<routerProxy::ServerInfo>& result);
        virtual void callback_getServerList_exception(taf::Int32 ret);
private:
		void doDaylog(const vector<taf::EndpointF> &activeEp, const vector<taf::EndpointF> &inactiveEp,const string& sSetid="");
private:
	string _id;
	FUNID _eFnId;
	taf::JceCurrentPtr  _current;
};
#endif


