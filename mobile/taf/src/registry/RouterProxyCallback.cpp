#include "RouterProxyCallback.h"
#include "log/taf_logger.h"
#include "util/tc_common.h"

RouterProxyCallback::RouterProxyCallback(const std::string & id,const FUNID eFnId, const taf::JceCurrentPtr& current)
:_id(id),_eFnId(eFnId),_current(current)
{
}
 RouterProxyCallback::~RouterProxyCallback()
{
}
 void RouterProxyCallback::doDaylog(const vector<taf::EndpointF> &activeEp, const vector<taf::EndpointF> &inactiveEp,const string& sSetid)
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

	FDLOG("query") << _current->getIp() << ":"<< _current->getPort() << "|" << _id << "|" <<sSetid<<"|"<< sEpList<< endl;

 }

void RouterProxyCallback::callback_getServerList(taf::Int32 ret,  const vector<routerProxy::ServerInfo>& result)
{
	if(ret == taf::JCESERVERSUCCESS)
	{
		try{

			vector<taf::EndpointF> activeEp;
			vector<taf::EndpointF> inactiveEp;

			for(size_t i=0;i < result.size();i++)
			{
				taf::EndpointF epf;
				epf.host = result[i].host;
				epf.port = result[i].port;
				epf.istcp = result[i].isTCP;

				activeEp.push_back(epf);
			}

			switch(_eFnId)
			{
				case FUNID_findObjectByIdInSameGroup:
				{
					taf::QueryF::async_response_findObjectByIdInSameGroup(_current,0,activeEp,inactiveEp);
					//LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|[" << _id <<"]|" << _current->getIp()<<"|"<<_eFnId<< endl;
				}
				break;
				case FUNID_findObjectByIdInSameSet:
				{
					taf::QueryF::async_response_findObjectByIdInSameSet(_current,0,activeEp,inactiveEp);
					//LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|[" << _id <<"]|" << _current->getIp()<<"|"<<_eFnId<< endl;
				}
				break;
				case FUNID_findObjectById4Any:
				{
					taf::QueryF::async_response_findObjectById4Any(_current,0,activeEp,inactiveEp);
				}
				break;
				case FUNID_findObjectById:
				{
					taf::QueryF::async_response_findObjectById(_current,activeEp);
				}
				break;
				case FUNID_findObjectById4All:
				{
					taf::QueryF::async_response_findObjectById4All(_current,0,activeEp,inactiveEp);
				}
				break;
				case FUNID_findObjectByIdInSameStation:
				{
					taf::QueryF::async_response_findObjectByIdInSameStation(_current,0,activeEp,inactiveEp);
				}
				break;
				default:
				{
					//如果之前没有响应，则此处异常情况下需要响应
					if (!_current->isResponse())
					{
						_current->sendResponse(taf::JCESERVERUNKNOWNERR);
					}

					LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|no request cmd [" << _id <<"]|" << _current->getIp()  << endl;
				}
				break;
			}

			doDaylog(activeEp,inactiveEp);

		}catch(exception& e)
		{
			if (!_current->isResponse())
			{
				_current->sendResponse(taf::JCESERVERUNKNOWNERR);
			}
			LOG->error()<< __FUNCTION__ << ":" << __LINE__ <<"exception :"<<e.what()<<endl;
		}
	}
	else
	{
		if (!_current->isResponse())
		{
			_current->sendResponse(ret);
		}

		LOG->error()<< __FUNCTION__ << ":" << __LINE__ <<" "<<_id<<"|"<<ret<<endl;
	}
}

void RouterProxyCallback::callback_getServerList_exception(taf::Int32 ret)
{
	if (!_current->isResponse())
	{
		_current->sendResponse(ret);
	}

	LOG->error()<< __FUNCTION__ << ":" << __LINE__ <<" "<<_id<<"|"<<ret<<endl;
}
