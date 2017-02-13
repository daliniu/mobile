#ifndef _SecretImp_H_
#define _SecretImp_H_

#include "servant/Application.h"
#include "Base.h"
#include "Secret.h"
#include "UserInfo.h"
#include "Msg.h"
#include "DbHandle.h"

using namespace QUAN;

/**
 *
 *
 */
class SecretImp : public QUAN::Secret
{
public:
	/**
	 *
	 */
	virtual ~SecretImp() {}

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
        virtual taf::Int32 test(taf::Int64 lId,taf::JceCurrentPtr current);

        virtual taf::Int32 getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 getSecretContent(const QUAN::ContentListReq & tReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 postSecret(const QUAN::PostSecretReq & tReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 postContent(const QUAN::PostContentReq& tReq, QUAN::PostContentRsp& tRsp, taf::JceCurrentPtr current);

        virtual taf::Int32 postFavor(const QUAN::PostFavorReq& tReq, QUAN::PostFavorRsp& tRsp, taf::JceCurrentPtr current);

        virtual taf::Int32 postReport(const QUAN::PostReportReq& tReq, QUAN::PostReportRsp& tRsp, taf::JceCurrentPtr current);

        virtual taf::Int32 cleanTrail(const QUAN::CleanTrailReq& tReq, QUAN::CleanTrailRsp& tRsp, taf::JceCurrentPtr current);

        virtual taf::Int32 removeSecret(const QUAN::RemoveSecretReq& tReq, QUAN::RemoveSecretRsp& tRsp, taf::JceCurrentPtr current);

        virtual taf::Int32 getUserFavorRelat(taf::Int64 lUId,QUAN::SecretRelatMap &tFavorScrts,taf::JceCurrentPtr current);

        virtual taf::Int32 getUserRemoveRelat(taf::Int64 lUId,QUAN::SecretRelatMap &tRemoveScrts,taf::JceCurrentPtr current);

        virtual taf::Int32 getUserBatchSecrets(taf::Int64 lUId,const map<taf::Int64, QUAN::MsgRmd> & mSIds,vector<QUAN::SecretInfo> &vSecrets,taf::JceCurrentPtr current);

        virtual taf::Int32 getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 setUserSecretShare(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq,taf::JceCurrentPtr current);
        virtual taf::Int32 feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current);
private:
        int checkUserId(const QUAN::UserId &tId,int *platform);

        // begin  获取帖子列表调用函数
        taf::Int32 getRecommendSecrets(UserId const& tUserId, vector<SecretInfo>& vSecrets);
        
        taf::Int32 mergeSecrets(UserId const& tUserId, int iLockCount, vector<SecretInfo>& vRmdScrts, vector<SecretInfo>& vFvrScrts, vector<SecretInfo>& vSecrets);

        taf::Int32 getFriendsList(UserId const& tUserId, Int64 lBeginId, vector<FriendsList>& vFrdsFavor, vector<FriendsList>& vFrds1, vector<FriendsList>& vFrdsAll);

        taf::Int32 dealWithFavorAndRemove(UserId const& tUserId, size_t iFriends, Int32& iLockCount, vector<SecretInfo>& vSecrets);
        // 从cache或者数据库拉数据
        taf::Int32 getPullSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, vector<FriendsList> const& vFrds1, vector<FriendsList> const& vFrdsAll, vector<FriendsList> const& vFrdsFavor);
        taf::Int32 uniqueSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, vector<FriendsList> const& vFrds1, vector<FriendsList> const& vFrdsAll, vector<FriendsList> const& vFrdsFavor);
private:
        size_t              _uMaxSecretsNum;
        size_t              _u24SecretsNum;
        size_t              _u72SecretsNum;
        size_t              _uMaxFriendsNum;
        size_t              _uMaxFrdsFvrNum;
        size_t              _uMinFrdsFvrNum;
        size_t              _uSysScrtLoc;
        size_t              _uActiveFriends;
        size_t              _uTopN;
        UserInfoPrx         _pUserProxy;
        MsgPrx              _pMsgProxy;
        DbHandle            _tDbHandle;
};
/////////////////////////////////////////////////////
#endif
