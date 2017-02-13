#include "QueryImp.h"
#include "util/tc_clientsocket.h"


extern routerProxy::RouterProxyServicePrx _routerPrx;

void QueryImp::initialize()
{
    LOG->debug()<<"begin QueryImp init"<<endl;

    //初始化配置db连接
    extern TC_Config * g_pconf;
    _db.init(g_pconf);

}


vector<EndpointF> QueryImp::findObjectById(const string & id, taf::JceCurrentPtr current)
{

    vector<EndpointF> eps = _db.findObjectById(id);

    string sEpList;
    for(size_t i = 0; i < eps.size(); i++)
    {
        sEpList += eps[i].host + ":" + TC_Common::tostr(eps[i].port) + ";";
    }
    FDLOG("query") << current->getIp() << ":"<< current->getPort() << "|" << id << "|" << sEpList << endl;

    return eps;
}

taf::Int32 QueryImp::findObjectById4Any(const std::string & id,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current)
{
	if(IsTaServerObjById(id))
	{
		return findTaServerObjById(id,FUNID_findObjectById4Any,current);
	}

    int iRet = _db.findObjectById4All(id, activeEp, inactiveEp);

	ostringstream os;
	doDaylog(id,activeEp,inactiveEp,current,os);

    return iRet;
}

int QueryImp::findObjectById4All(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current)
{
	if(IsTaServerObjById(id))
	{
		return findTaServerObjById(id,FUNID_findObjectById4All,current);
	}

    ostringstream os;

    int iRet = _db.findObjectByIdInGroupPriority(id,current->getIp(),activeEp, inactiveEp,os);

	doDaylog(id,activeEp,inactiveEp,current,os);

    return iRet;

    /*
    int iRet = _db.findObjectById4All(id, activeEp, inactiveEp);

    string sEpList;
    for(size_t i = 0; i < activeEp.size(); i++)
    {
        sEpList += activeEp[i].host + ":" + TC_Common::tostr(activeEp[i].port) + ";";
    }
    sEpList += "|";
    for(size_t i = 0; i < inactiveEp.size(); i++)
    {
        sEpList += inactiveEp[i].host + ":" + TC_Common::tostr(inactiveEp[i].port) + ";";
    }

    FDLOG("query") << current->getIp() << ":"<< current->getPort() << "|" << id << "|" << sEpList << endl;


    return iRet;
    */
}

int QueryImp::findObjectByIdInSameGroup(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,
            taf::JceCurrentPtr current)
{
    ostringstream os;
    LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << id << "|" << current->getIp() << endl);

	if(IsTaServerObjById(id))
	{
		return findTaServerObjById(id,FUNID_findObjectByIdInSameGroup,current);
	}

    //int iRet = _db.findObjectByIdInSameGroup(id,current->getIp(),activeEp, inactiveEp,os);
    int iRet = _db.findObjectByIdInGroupPriority(id, current->getIp(), activeEp, inactiveEp, os);

	doDaylog(id,activeEp,inactiveEp,current,os);

    return iRet;
}

Int32 QueryImp::findObjectByIdInSameStation(const std::string & id, const std::string & sStation, vector<taf::EndpointF> &activeEp, vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current)
{
	if(IsTaServerObjById(id))
	{
		return findTaServerObjById(id,FUNID_findObjectByIdInSameStation,current);
	}

    ostringstream os;

    int iRet = _db.findObjectByIdInSameStation(id, sStation, activeEp, inactiveEp, os);

	doDaylog(id,activeEp,inactiveEp,current,os);

    return iRet;
}

Int32 QueryImp::findObjectByIdInSameSet(const std::string & id,const std::string & setId,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current)
{
	vector<string> vtSetInfo = TC_Common::sepstr<string>(setId,".");

	if (vtSetInfo.size()!=3 ||(vtSetInfo.size()==3&&(vtSetInfo[0]=="*"||vtSetInfo[1]=="*")))
	{
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|set full name error[" << id << "_" << setId <<"]|" << current->getIp() << endl;
		return -1;
	}

	if(IsTaServerObjById(id))
	{
		return findTaServerObjById(id,FUNID_findObjectByIdInSameSet,current);
	}

	ostringstream os;
	bool bSetPrint = true;
	int iRet = _db.findObjectByIdInSameSet(id, vtSetInfo, activeEp, inactiveEp, os);
	if (-1 == iRet)
	{//未启动set，启动ip分组策略
		iRet = findObjectByIdInSameGroup(id, activeEp, inactiveEp, current);
		bSetPrint = false;
	}
	else if (-2 == iRet)
	{//启动了set，但未找到任何服务节点
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|no one server found for [" << id << "_" << setId <<"]|" << current->getIp() << endl;
		return -1;
	}
	else if (-3 == iRet)
	{//启动了set，但未找到任何地区set,严格上不应该出现此类情形,配置错误或主调设置错误会引起此类错误
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|no set area found [" << id << "_" << setId <<"]|" << current->getIp()  << endl;
		return -1;
	}

	if (bSetPrint)
	{
		doDaylog(id,activeEp,inactiveEp,current,os,setId);
	}


	return iRet;


}

bool QueryImp::IsTaServerObjById(const std::string & id)
{
	return ('#' == id[0])?true:false;
}

int QueryImp::findTaServerObjById(const std::string & id,const FUNID eFnId, const taf::JceCurrentPtr& current)
{
	int ret = 0;

	LOGINFO( __FUNCTION__ << ":" << __LINE__ << "|" << id << "|" << current->getIp() << endl);

	if(_routerPrx)
	{
		try{

			//异步回调回来了再响应
			current->setResponse(false);

			routerProxy::RouterProxyServicePrxCallbackPtr cb = new RouterProxyCallback(id,eFnId,current);

			_routerPrx->async_getServerList(cb,id);

		}catch(exception& e)
		{
			LOG->error()<< __FUNCTION__ << ":" << __LINE__<<"exception :"<<e.what()<<endl;
			ret = -1;

			if (!current->isResponse())
			{
				current->sendResponse(taf::JCESERVERUNKNOWNERR);
			}
		}
	}
	return ret;
}

void QueryImp::doDaylog(const string& id,const vector<taf::EndpointF> &activeEp, const vector<taf::EndpointF> &inactiveEp,
							  const taf::JceCurrentPtr& current,const ostringstream& os,const string& sSetid)
 {
	string sEpList;
	for(size_t i = 0; i < activeEp.size(); i++)
	{
		sEpList += activeEp[i].host + ":" + TC_Common::tostr(activeEp[i].port) + ";";
	}

	sEpList += "|";

	for(size_t i = 0; i < inactiveEp.size(); i++)
	{
		sEpList += inactiveEp[i].host + ":" + TC_Common::tostr(inactiveEp[i].port) + ";";
	}

	FDLOG("query") << current->getIp() << ":"<< current->getPort() << "|" << id << "|" <<sSetid << "|" << sEpList <<os.str()<< endl;
 }

