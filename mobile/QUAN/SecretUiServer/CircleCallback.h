#ifndef __CIRCLECALLBACK_H__
#define __CIRCLECALLBACK_H__
#include "Circle.h"
#include "Msg.h"
#include "SecretUi.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"
#include "Base.h"


using namespace  QUAN;


/* callback of async proxy for client */
class CircleCallback: public CirclePrxCallback
{

public:
	CircleCallback(taf::JceCurrentPtr current, CirclePrx pCircleProxy = NULL, MsgPrx pMsgProxy = NULL)
    {
        _tCurrent           = current;
        _pCircleProxy       = pCircleProxy;
		_pMsgProxy          = pMsgProxy;
    }

	virtual void callback_createCircle(taf::Int32 ret, const QUAN::CreateCircleRsp& tRsp);
	virtual void callback_createCircle_exception(taf::Int32 ret);

	virtual void callback_searchCircle(taf::Int32 ret, const QUAN::SearchCircleRsp& tRsp);
	virtual void callback_searchCircle_exception(taf::Int32 ret);

	virtual void callback_getCircle(taf::Int32 ret, const QUAN::GetCircleRsp& tRsp);
	virtual void callback_getCircle_exception(taf::Int32 ret);

	virtual void callback_setCircleConcern(taf::Int32 ret);
	virtual void callback_setCircleConcern_exception(taf::Int32 ret);

	virtual void callback_auditCircle(taf::Int32 ret);
	virtual void callback_auditCircle_exception(taf::Int32 ret);

	virtual void callback_modifyCircleLocation(taf::Int32 ret);
	virtual void callback_modifyCircleLocation_exception(taf::Int32 ret);

	void cleanCircleCache(Int64 lCircleId);

	virtual void callback_createCircleByAdmin(taf::Int32 ret, const QUAN::CreateCircleRsp& tRsp);
	virtual void callback_createCircleByAdmin_exception(taf::Int32 ret);

public:

    void setCallerLog(const string&sLog)
    {   
        _sLog       = sLog;
    }

	void setSetCircleReq(SetCircleReq const& tReq)
	{
		_tSetCircleReq = tReq;
	}

	void setCircleId(Int64 lCircleId)
	{
		_lCircleId = lCircleId;
	}

	void setUserId(UserId const& tUserId)
	{
		_tUserId = tUserId;
	}

	void setReason(string const& sReason)
	{
		_sReason = sReason;
	}

	void setAuditState(Int32 iAuditState)
	{
		_iAuditState = iAuditState;
	}

	void setCreateCircleReq(CreateCircleReq const& tReq)
	{
		_tCreateCircleReq = tReq;
	}

private:
    string      _sLog;
    taf::JceCurrentPtr  _tCurrent;
	CirclePrx           _pCircleProxy;
	MsgPrx              _pMsgProxy;
	SetCircleReq        _tSetCircleReq;
	Int64               _lCircleId;
	UserId              _tUserId;
	string              _sReason;
	Int32               _iAuditState;
	CreateCircleReq     _tCreateCircleReq;
};
typedef taf::TC_AutoPtr<CircleCallback> CircleCallbackPtr;


#endif
