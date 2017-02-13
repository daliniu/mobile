#ifndef _SecretUiImp_H_
#define _SecretUiImp_H_

#include "servant/Application.h"
#include "SecretUi.h"
#include "UserInfo.h"
#include "GetSecret.h"
#include "Lbs.h"
#include "Post.h"
#include "Msg.h"
#include "Circle.h"
#include "UserInfoCallback.h"

using namespace  QUAN;
/**
 *
 *
 */
class SecretUiImp : public QUAN::SecretUi
{
public:
    /**
     *
     */
    virtual ~SecretUiImp() {}

    /**
     *
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroy();

    /**
     *
     */
    virtual int test(taf::JceCurrentPtr current) { return 0;};


    virtual taf::Int32 getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 getSecretContent(const QUAN::ContentListReq & tReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 postSecret(const QUAN::PostSecretReq & tReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 postContent(const QUAN::PostContentReq& tReq, QUAN::PostContentRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 postFavor(const QUAN::PostFavorReq& tReq, QUAN::PostFavorRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 postReport(const QUAN::PostReportReq& tReq, QUAN::PostReportRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 cleanTrail(const QUAN::CleanTrailReq& tReq, QUAN::CleanTrailRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 removeSecret(const QUAN::RemoveSecretReq& tReq, QUAN::RemoveSecretRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 clearUserCache(taf::Int64 lUId,taf::JceCurrentPtr current);

    virtual taf::Int32 getSystemList(const QUAN::SystemListReq & tReq,QUAN::SystemListRsp &tRsp,taf::JceCurrentPtr current);

	virtual taf::Int32 addCircleLocation(taf::Int64 lCircleId, const QUAN::CircleLocation & tCircleLocation, taf::JceCurrentPtr current);
public:

    virtual taf::Int32 getPushMsg(const QUAN::PushMsgReq& tReq, QUAN::PushMsgRsp& tRsp, taf::JceCurrentPtr current);
    
    virtual taf::Int32 addMsg(const QUAN::AddMsgReq& tReq, taf::JceCurrentPtr current);
    
    virtual taf::Int32 changeMsgStatus(const QUAN::ChangeMsgReq & tReq,taf::JceCurrentPtr current);

    virtual taf::Int32 getMsgNum(const QUAN::GetMsgNumReq & tReq,QUAN::GetMsgNumRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 bindMsg(const QUAN::BindMsgReq & tReq,taf::JceCurrentPtr current);

    virtual taf::Int32 atMsg(const QUAN::AtMsgReq & tReq,taf::JceCurrentPtr current);

    virtual taf::Int32 shareSecret(taf::Int64 lSId,taf::Int64 lShareId,taf::JceCurrentPtr current);

    virtual taf::Int32 auditSecret(vector<taf::Int64> const& vSId,taf::Int32 iLevel,taf::JceCurrentPtr current);

    // IOS过渡版本
    virtual taf::Int32 getAllSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);

    // 2.0圈子新功能 
    virtual taf::Int32 createCircle(const QUAN::CreateCircleReq& tReq, QUAN::CreateCircleRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 searchCircle(const QUAN::SearchCircleReq& tReq, QUAN::SearchCircleRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 getCircle(const QUAN::GetCircleReq& tReq, QUAN::GetCircleRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 setCircleConcern(const QUAN::SetCircleReq& tReq, taf::JceCurrentPtr current);

	virtual taf::Int32 auditCircle(taf::Int64 lCircleId, Int32 iAuditState, string const& sReason, taf::JceCurrentPtr current);

    virtual taf::Int32 recommCircle(taf::Int64 lCircleId, taf::Int32 iRecommValue, taf::JceCurrentPtr current);

    virtual taf::Int32 getCircleList(const QUAN::GetCircleListReq& tReq, QUAN::GetCircleListRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 getSysMsg(const QUAN::GetSysMsgReq& tReq, QUAN::GetSysMsgRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 pushSysMsg(const QUAN::PushSysMsgReq& tReq, taf::JceCurrentPtr current);

	virtual taf::Int32 getNewSysMsg(const QUAN::GetNewSysMsgReq & tReq, QUAN::GetNewSysMsgRsp &tRsp, taf::JceCurrentPtr current);

	virtual taf::Int32 modifyCircleLocation(taf::Int64 lCircleId, taf::Int64 lCircleLocationId, const QUAN::CircleLocation & tCircleLocation, taf::JceCurrentPtr current);

    virtual taf::Int32 getSecretById(const GetSecretByIdReq& tReq, GetSecretByIdRsp& tRsp, taf::JceCurrentPtr current);

    virtual taf::Int32 pushImMsg(const PushImMsgReq& tReq, taf::JceCurrentPtr current);

	virtual taf::Int32 createCircleByAdmin(const QUAN::CreateCircleReq& tReq, const std::vector<QUAN::CircleLocation>& vCircleLocation, QUAN::CreateCircleRsp& tRsp, taf::JceCurrentPtr current);

private:

    bool isValidToken(QUAN::UserId &tId);

    void notifyFollowers(taf::JceCurrentPtr current,const QUAN::UserId &tUserId,const PushSecretInfoPtr &pPushSecretInfoPtr);

private:

    string              _sActiveUrl;

    GetSecretPrx        _pSecretProxy;
    PostPrx             _pPostProxy;
    UserInfoPrx         _pUserProxy;
    MsgPrx              _pMsgProxy;
    LbsPrx              _pLbsProxy;
    CirclePrx           _pCircleProxy;
    string              _sLog;
};
/////////////////////////////////////////////////////
#endif
