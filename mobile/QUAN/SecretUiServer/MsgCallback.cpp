#include "MsgCallback.h"


void MsgCallback::callback_getPushMsg(taf::Int32 ret,  const QUAN::PushMsgRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    Msg::async_response_getPushMsg(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void MsgCallback::callback_getPushMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    PushMsgRsp tRsp;
    Msg::async_response_getPushMsg(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void MsgCallback::callback_getMsgNum(taf::Int32 ret,  const QUAN::GetMsgNumRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    Msg::async_response_getMsgNum(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void MsgCallback::callback_getMsgNum_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::GetMsgNumRsp tRsp;
    Msg::async_response_getMsgNum(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void MsgCallback::callback_addMsg(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	// Çå³ýMsg»º´æ
	this->cleanMsgCache(_tAddMsgReq.vUserIds);

    // Msg::async_response_addMsg(_tCurrent,ret);

    ostringstream os;
    os<< ret<<"|" <<_sLog<< endl;
    LOG->debug()<<os.str();

    FDLOG() <<os.str();
}

void MsgCallback::callback_addMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Msg::async_response_addMsg(_tCurrent,ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_atMsg(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	// Çå³ýMsg»º´æ
	this->cleanMsgCache(_tAtMsgReq.vUserIds);

    Msg::async_response_atMsg(_tCurrent,ret);

    ostringstream os;
    os<< ret<<"|" <<_sLog<< endl;
    LOG->debug()<<os.str();

    FDLOG() <<os.str();
}

void MsgCallback::callback_atMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Msg::async_response_atMsg(_tCurrent,ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_changeMsgStatus(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	// Çå³ýMsg»º´æ
	vector<Int64> vUserId;
	vUserId.push_back(_tChangeMsgReq.tUserId.lUId);
	this->cleanMsgCache(vUserId);

    // Msg::async_response_changeMsgStatus(_tCurrent,ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}
void MsgCallback::callback_changeMsgStatus_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Msg::async_response_changeMsgStatus(_tCurrent,ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::cleanMsgCache(vector<Int64> const& vUserId)
{
	if (_pMsgProxy)
	{
		LOG->debug() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << simpledisplay(vUserId) << endl;
		FDLOG() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << simpledisplay(vUserId) << endl;
		size_t iNum = _pMsgProxy->getEndpoint4All().size();
		for (size_t i = 0; i < iNum; i++)
		{
			_pMsgProxy->taf_hash(i)->async_cleanCache(NULL, vUserId);
		}
	}
}

void MsgCallback::cleanSysMsgCache(Int64 lUserId)
{
	if (_pMsgProxy)
	{
		LOG->debug() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << lUserId << endl;
		FDLOG() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << lUserId << endl;
		size_t iNum = _pMsgProxy->getEndpoint4All().size();
		for (size_t i = 0; i < iNum; i++)
		{
			_pMsgProxy->taf_hash(i)->async_cleanSysCache(NULL, lUserId);
		}
	}
}

void MsgCallback::callback_getSysMsg(taf::Int32 ret, const QUAN::GetSysMsgRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	this->cleanSysMsgCache(_lUserId);
    Msg::async_response_getSysMsg(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_getSysMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	GetSysMsgRsp tRsp;
    Msg::async_response_getSysMsg(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_pushSysMsg(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;
   
	this->cleanSysMsgCache(_lUserId);
    Msg::async_response_pushSysMsg(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_pushSysMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Msg::async_response_pushSysMsg(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_getNewSysMsg(taf::Int32 ret, const QUAN::GetNewSysMsgRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    Msg::async_response_getNewSysMsg(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void MsgCallback::callback_getNewSysMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	GetNewSysMsgRsp tRsp;
    Msg::async_response_getNewSysMsg(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void MsgCallback::callback_pushBindMsg(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<endl;

	if (_pMsgProxy)
	{
		size_t iNum = _pMsgProxy->getEndpoint4All().size();
		for (size_t i = 0; i < iNum; i++)
		{
			_pMsgProxy->taf_hash(i)->async_bindMsg(NULL, _tBindMsgReq);
		}
	}

    Msg::async_response_pushBindMsg(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;

}

void MsgCallback::callback_pushBindMsg_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Msg::async_response_pushBindMsg(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}