

#ifndef __MsgCallback_H_
#define __MsgCallback_H_
#include "Msg.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"
#include "Base.h"


using namespace  QUAN;


/* callback of async proxy for client */
class MsgCallback: public MsgPrxCallback
{

public:
    MsgCallback(taf::JceCurrentPtr current,  MsgPrx pMsgProxy = NULL)
    {
        _tCurrent           = current;
		_pMsgProxy          = pMsgProxy;
    }

    virtual void callback_getPushMsg(taf::Int32 ret,  const QUAN::PushMsgRsp& tRsp);
    virtual void callback_getPushMsg_exception(taf::Int32 ret);

    virtual void callback_getMsgNum(taf::Int32 ret,  const QUAN::GetMsgNumRsp& tRsp);
    virtual void callback_getMsgNum_exception(taf::Int32 ret);

    virtual void callback_addMsg(taf::Int32 ret);
    virtual void callback_addMsg_exception(taf::Int32 ret);

	virtual void callback_atMsg(taf::Int32 ret);
	virtual void callback_atMsg_exception(taf::Int32 ret);

    virtual void callback_changeMsgStatus(taf::Int32 ret);
    virtual void callback_changeMsgStatus_exception(taf::Int32 ret);

	void cleanMsgCache(vector<Int64> const& vUserId);
	void cleanSysMsgCache(Int64 lUserId);

	virtual void callback_getSysMsg(taf::Int32 ret, const QUAN::GetSysMsgRsp& tRsp);
	virtual void callback_getSysMsg_exception(taf::Int32 ret);

	virtual void callback_pushSysMsg(taf::Int32 ret);
	virtual void callback_pushSysMsg_exception(taf::Int32 ret);

	virtual void callback_getNewSysMsg(taf::Int32 ret, const QUAN::GetNewSysMsgRsp& tRsp);
	virtual void callback_getNewSysMsg_exception(taf::Int32 ret);

	virtual void callback_pushBindMsg(taf::Int32 ret);
	virtual void callback_pushBindMsg_exception(taf::Int32 ret);
public:

    void setCallerLog(const string&sLog)
    {   
        _sLog       = sLog;
    }

	void setUserId(Int64 lUserId)
	{
		_lUserId = lUserId;
	}

	void setBindMsgReq(BindMsgReq const& tReq)
	{
		_tBindMsgReq = tReq;
	}

	void setAddMsgReq(AddMsgReq const& tReq)
	{
		_tAddMsgReq = tReq;
	}

	void setChangeMsgReq(ChangeMsgReq const& tReq)
	{
		_tChangeMsgReq = tReq;
	}

	void setAtMsgReq(AtMsgReq const& tReq)
	{
		_tAtMsgReq = tReq;
	}

private:
    string      _sLog;
    taf::JceCurrentPtr  _tCurrent;
	MsgPrx      _pMsgProxy;
	AddMsgReq   _tAddMsgReq;
	ChangeMsgReq _tChangeMsgReq;
	AtMsgReq     _tAtMsgReq;
	Int64        _lUserId;
	BindMsgReq   _tBindMsgReq;
};
typedef taf::TC_AutoPtr<MsgCallback> MsgCallbackPtr;

#endif
