

#ifndef __SecretCallback_H_
#define __SecretCallback_H_
#include "GetSecret.h"
#include "Post.h"
#include "Msg.h"
#include "Lbs.h"
#include "Circle.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"
#include "Base.h"
#include "UserInfo.h"


using namespace  QUAN;

/* callback of async proxy for client */
class GetSecretCallback: public GetSecretPrxCallback
{

public:
    GetSecretCallback(taf::JceCurrentPtr current, UserInfoPrx pUserProxy=NULL, GetSecretPrx pSecretProxy=NULL)
    {
        _tCurrent           = current;
        _pUserProxy         = pUserProxy;
        _pSecretProxy       = pSecretProxy;
    }
    virtual void callback_getSecretList(taf::Int32 ret,  const QUAN::SecretListRsp& tRsp);
    virtual void callback_getSecretList_exception(taf::Int32 ret);

    virtual void callback_getSystemList(taf::Int32 ret,  const QUAN::SystemListRsp& tRsp);
    virtual void callback_getSystemList_exception(taf::Int32 ret);

    virtual void callback_getAllSecretList(taf::Int32 ret,  const QUAN::SecretListRsp& tRsp);
    virtual void callback_getAllSecretList_exception(taf::Int32 ret);

	virtual void callback_getCircleList(taf::Int32 ret, const QUAN::GetCircleListRsp& tRsp);
	virtual void callback_getCircleList_exception(taf::Int32 ret);

public:

    void setCallerLog(const string&sLog)
    {   
        _sLog       = sLog;
    }

	void setSecretListReq(SecretListReq const& tReq)
	{
		_tSecretListReq = tReq;
	}

private:
    string                      _sLog;


    taf::JceCurrentPtr          _tCurrent;
    UserInfoPrx                 _pUserProxy;
    GetSecretPrx                _pSecretProxy;
	SecretListReq               _tSecretListReq;
};

/* callback of async proxy for client */
class PostCallback: public PostPrxCallback
{

public:
    PostCallback(taf::JceCurrentPtr current, UserInfoPrx pUserProxy=NULL, GetSecretPrx pSecretProxy=NULL, MsgPrx pMsgProxy = NULL, PostPrx pPostProxy = NULL, LbsPrx pLbsProxy = NULL, CirclePrx pCircleProxy = NULL)
    {
        _tCurrent           = current;
        _pUserProxy         = pUserProxy;
        _pSecretProxy       = pSecretProxy;
        _pMsgProxy          = pMsgProxy;
        _pPostProxy         = pPostProxy;
        _pLbsProxy          = pLbsProxy;
        _pCircleProxy       = pCircleProxy;
    }

    virtual void callback_getSecretContent(taf::Int32 ret,  const QUAN::ContentListRsp& tRsp);
    virtual void callback_getSecretContent_exception(taf::Int32 ret);

    virtual void callback_postSecret(taf::Int32 ret,  const QUAN::PostSecretRsp& tRsp);
    virtual void callback_postSecret_exception(taf::Int32 ret);

    virtual void callback_postContent(taf::Int32 ret,  const QUAN::PostContentRsp& tRsp, const vector<Int64>& tRelatId);
    virtual void callback_postContent_exception(taf::Int32 ret);

    virtual void callback_postFavor(taf::Int32 ret,  const QUAN::PostFavorRsp& tRsp);
    virtual void callback_postFavor_exception(taf::Int32 ret);

    virtual void callback_postReport(taf::Int32 ret,  const QUAN::PostReportRsp& tRsp);
    virtual void callback_postReport_exception(taf::Int32 ret);

    virtual void callback_cleanTrail(taf::Int32 ret,  const QUAN::CleanTrailRsp& tRsp);
    virtual void callback_cleanTrail_exception(taf::Int32 ret);

    virtual void callback_removeSecret(taf::Int32 ret,  const QUAN::RemoveSecretRsp& tRsp);
    virtual void callback_removeSecret_exception(taf::Int32 ret);

    virtual void callback_removeContent(taf::Int32 ret,  const QUAN::RemoveContentRsp& tRsp);
    virtual void callback_removeContent_exception(taf::Int32 ret);

    virtual void callback_shareSecret(taf::Int32 ret);
    virtual void callback_shareSecret_exception(taf::Int32 ret);

	virtual void callback_auditSecret(taf::Int32 ret);
	virtual void callback_auditSecret_exception(taf::Int32 ret);

	virtual void callback_getSecretById(taf::Int32 ret, const QUAN::GetSecretByIdRsp& tRsp);
	virtual void callback_getSecretById_exception(taf::Int32 ret);
	
    void cleanUserTrailCache(Int64 lUId);
    void cleanCommentCache(Int64 lSId);
    void cleanPostCache(Int64 lSId);
	void cleanMsgCache(vector<Int64> const& vUserId);
	void updateLbsMap(PostSecretReq const& tReq, PostSecretRsp const& tRsp);

public:

    void setCallerLog(const string& sLog)
    {   
        _sLog       = sLog;
    }

    void setVectorSId(vector<Int64> const& vSId)
    {   
        _vSId = vSId;
    }

	void setUserId(UserId const& tUserId)
	{
		_tUserId = tUserId;
	}

    void setSId(Int64 lSId)
    {   
        _lSId = lSId;
    }

    void setShareId(Int64 lId)
    {   
        _lShareId = lId;
    }

    void setPostSecret(const QUAN::PostSecretReq &tPostSecret)
    {   
        _tPostSecret = tPostSecret;
    }

    void setPostContent(const QUAN::PostContentReq &tReq)
    {   
        _tPostContent = tReq;
    }

    void setPostFavor(const QUAN::PostFavorReq &tReq)
    {   
        _tPostFavor = tReq;
    }

    void setContentList(const QUAN::ContentListReq &tReq)
    {   
        _tContentList = tReq;
    }

    void setCleanTrail(const QUAN::CleanTrailReq &tReq)
    {   
        _tCleanTrail = tReq;
    }

    void setRemoveSecret(const QUAN::RemoveSecretReq &tReq)
    {   
        _tRemoveSecret = tReq;
    }

    void setPostReportReq(const QUAN::PostReportReq &tReq)
    {   
        _tPostReportReq = tReq;
    }

	void setGetSecretByIdRsp(GetSecretByIdRsp const& tGetSecretByIdRsp)
	{
		_tGetSecretByIdRsp = tGetSecretByIdRsp;
	}

private:
    string                      _sLog;

    taf::JceCurrentPtr          _tCurrent;
    UserInfoPrx                 _pUserProxy;
    GetSecretPrx                _pSecretProxy;
    MsgPrx                      _pMsgProxy;
    PostPrx                     _pPostProxy;
    LbsPrx                      _pLbsProxy;
    CirclePrx                   _pCircleProxy;
private:
    QUAN::PostSecretReq         _tPostSecret;
    QUAN::PostContentReq        _tPostContent;
    QUAN::PostFavorReq          _tPostFavor;
    QUAN::ContentListReq        _tContentList;
    QUAN::CleanTrailReq         _tCleanTrail;
    QUAN::RemoveSecretReq       _tRemoveSecret;
    QUAN::PostReportReq         _tPostReportReq;
	Int64						_lSId;
	Int64						_lShareId;
	vector<Int64>				_vSId;
	UserId						_tUserId;
	GetSecretByIdRsp            _tGetSecretByIdRsp;
};

typedef taf::TC_AutoPtr<GetSecretCallback> GetSecretCallbackPtr;
typedef taf::TC_AutoPtr<PostCallback> PostCallbackPtr;

#endif
