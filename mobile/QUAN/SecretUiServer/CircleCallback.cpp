#include "CircleCallback.h"

void CircleCallback::callback_createCircle(taf::Int32 ret, const QUAN::CreateCircleRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;
	
	if (_pCircleProxy)
	{
		_pCircleProxy->async_clearLbsCache(_tCreateCircleReq.sCoord);
	}

    Circle::async_response_createCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_createCircle_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	CreateCircleRsp tRsp;
    Circle::async_response_createCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_searchCircle(taf::Int32 ret, const QUAN::SearchCircleRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    Circle::async_response_searchCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<JCETOSTR(tRsp)<<endl;

}
void CircleCallback::callback_searchCircle_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	SearchCircleRsp tRsp;
    Circle::async_response_searchCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_getCircle(taf::Int32 ret, const QUAN::GetCircleRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    Circle::async_response_getCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<JCETOSTR(tRsp)<< endl;

}

void CircleCallback::callback_getCircle_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	GetCircleRsp tRsp;
    Circle::async_response_getCircle(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;

}

void CircleCallback::callback_setCircleConcern(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	this->cleanCircleCache(_tSetCircleReq.lCircleId);

    Circle::async_response_setCircleConcern(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_setCircleConcern_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Circle::async_response_setCircleConcern(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_auditCircle(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	this->cleanCircleCache(_lCircleId);
	/*
	if (_pMsgProxy)
	{
		PushSysMsgReq tReq;
		tReq.tUserId = _tUserId;
		tReq.tSysMsgInfo.bIsRead = false;
		tReq.tSysMsgInfo.lCircleId = _lCircleId;
		tReq.tSysMsgInfo.sContent = _sReason;
		tReq.tSysMsgInfo.bIsPass = _iAuditState == 2 ? true : false;
		LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tReq)<<endl;
		_pMsgProxy->taf_hash(_tUserId.lUId)->async_pushSysMsg(NULL, tReq);
	}
    */
    Circle::async_response_auditCircle(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_auditCircle_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

    Circle::async_response_auditCircle(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::cleanCircleCache(Int64 lCircleId)
{
	if (_pCircleProxy)
	{
		LOG->debug() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << lCircleId << endl;
		FDLOG() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << lCircleId << endl;
		size_t iNum = _pCircleProxy->getEndpoint4All().size();
		for (size_t i = 0; i < iNum; i++)
		{
			_pCircleProxy->taf_hash(i)->async_clearCircleCache(NULL, lCircleId);
		}
	}
}

void CircleCallback::callback_modifyCircleLocation(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	this->cleanCircleCache(_lCircleId);
	Circle::async_response_modifyCircleLocation(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_modifyCircleLocation_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	Circle::async_response_modifyCircleLocation(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_createCircleByAdmin(taf::Int32 ret, const QUAN::CreateCircleRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;
	
    SecretUi::async_response_createCircleByAdmin(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void CircleCallback::callback_createCircleByAdmin_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<endl;

	CreateCircleRsp tRsp;
    SecretUi::async_response_createCircleByAdmin(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}