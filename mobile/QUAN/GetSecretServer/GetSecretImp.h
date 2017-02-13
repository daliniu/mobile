#ifndef _GetSecretImp_H_
#define _GetSecretImp_H_

#include "servant/Application.h"
#include "Base.h"
#include "SecretBase.h"
#include "GetSecret.h"
#include "UserInfo.h"
#include "Post.h"
#include "Lbs.h"
#include "Circle.h"
#include "DbHandle.h"

using namespace QUAN;

/**
 *
 *
 */
class GetSecretImp : public QUAN::GetSecret
{
public:
	friend class GetSecretCallback;
	/**
	 *
	 */
	virtual ~GetSecretImp() {}

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
        // 获取帖子列表
        virtual taf::Int32 getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);
        // 推送帖子
        virtual taf::Int32 setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq,taf::JceCurrentPtr current);
        // 单独获取帖子内容
        virtual taf::Int32 getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current);
        // 设置分享状态
        virtual taf::Int32 setUserSecretShare(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp,taf::JceCurrentPtr current);
        // 反馈
        virtual taf::Int32 feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current);
        // 获取系统引导帖
        virtual taf::Int32 getSystemList(const QUAN::SystemListReq & tReq,QUAN::SystemListRsp &tRsp,taf::JceCurrentPtr current);

		taf::Int32 getFriendsList(SecretListReq const& tReq, UserId const& tUserId, taf::JceCurrentPtr current);
   
        // 获取树洞帖子列表
        virtual taf::Int32 getCircleList(const QUAN::GetCircleListReq & tReq,QUAN::GetCircleListRsp &tRsp,taf::JceCurrentPtr current);
		virtual taf::Int32 getAllSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);

		virtual taf::Int32 cleanCircleIdCache(taf::Int64 lCircleId, taf::JceCurrentPtr current);
private:

        // begin  获取帖子列表调用函数
		taf::Int32 getRecommendSecrets(SecretListReq const& tReq, Int32 iFrdsNum, FrdsVector const& vFrdsFavor, ScrtVector& vSecrets);
        // 获取好友帖子列表 
        taf::Int32 getFriendsSecrets(SecretListReq const& tReq, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, Int64& lNextBeginId, Int32& iLockCount, ScrtVector& vSecrets, Int32& iTimeSpan);
        // 获取好友列表
        taf::Int32 getFriendsList(UserId const& tUserId, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, FrdsVector& vFrdsAll, Int32& iRegTime);
        Int32 getBatchUserFavorSecrets(Int64 lUId, Int32 iSum, Int32 iFrdsFvrNum, FrdsVector const& vFrds, ScrtVector& vSecrets);
        // 从cache或者数据库拉数据
        taf::Int32 getPullSecretList(const QUAN::SecretListReq & tReq,
                                     QUAN::SecretListRsp &tRsp, 
                                     FrdsVectorPair const& tFrdsPair, 
                                     FrdsVector const& vFrdsAll, 
                                     FrdsVector const& vFrdsFavor, 
                                     ScrtVector& vSecrets,
									 FrdsVector const& vFrdsCircle,
									 Int32& iEraseTime);
        // 重新排列
        taf::Int32 uniqueSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, map<Int64, CircleInfo> const& mCircles, ScrtVector& vSecrets, Int32 iRegTime);
		// 获取附近列表
		taf::Int32 getNearSecrets(SecretListReq const& tReq, ScrtMap& vMaps, ScrtVector& vSecrets);

		// 帖子ID转换内容
		taf::Int32 tranSecretsId(SecretListReq const& tReq, SecretListRsp& tRsp, ScrtVector& vSecrets, FrdsMap const& mFrds);

		// 获取树洞列表
		taf::Int32 getCirclesList(SecretListReq const& tReq, FrdsVector& vFrdsFavor, map<Int64, CircleInfo>& mCircles);

		taf::Int32 getCitySecrets(SecretListReq const& tReq, ScrtVector& vSecrets);

private:
        size_t              _uMaxSecretsNum;
        size_t              _u24SecretsNum;
        size_t              _u72SecretsNum;
        size_t              _uLockFriendsNum;
        size_t              _uMaxFrdsFvrNum;
        size_t              _uMinFrdsFvrNum;
        size_t              _uSysScrtLoc;
        size_t              _uActiveFriends;
        size_t              _uTopN;
        UserInfoPrx         _pUserProxy;
        PostPrx             _pPostProxy;
        LbsPrx              _pLbsProxy;
        CirclePrx           _pCircleProxy;
        DbHandle            _tDbHandle;
};
/////////////////////////////////////////////////////
#endif
