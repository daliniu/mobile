#ifndef __GETSECRET_CALLBACK_H_
#define __GETSECRET_CALLBACK_H_
#include "Lbs.h"
#include "GetSecret.h"
#include "Post.h"
#include "UserInfo.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"
#include "Base.h"
#include "GetSecretImp.h"
#include "DbHandle.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>


using namespace  QUAN;

#define CLASS_NAME   UserInfo
#define UserInfo_SEQ    1
#define UserInfo_MEMBER_FUNCTION_SIZE    1
#define UserInfo_MEMBER_FUNCTION_COLUMNS ( (taf::Int32, getActiveFriends, const QUAN::GetActiveFriendsRsp&) )
#include "SharedCallback.h"

#define CLASS_NAME   Post
#define Post_SEQ    2
#define Post_MEMBER_FUNCTION_SIZE    2
typedef vector<map<Int64, Int32> > const&  PostRspType;
typedef vector<SecretInfo> const&  SecretsRspType;
#define Post_MEMBER_FUNCTION_COLUMNS ( (taf::Int32, getUserOpBatch,  PostRspType),        \
                                       (taf::Int32, getBatchSecrets, SecretsRspType)      \
                                     )
#include "SharedCallback.h"

#define CLASS_NAME   Lbs
#define Lbs_SEQ    3
#define Lbs_MEMBER_FUNCTION_SIZE    1
#define Lbs_MEMBER_FUNCTION_COLUMNS ( (taf::Int32, getNearbyUser, const QUAN::LbsRsp&) )
#include "SharedCallback.h"

class GetSecretCallback
{
public:

	GetSecretCallback(GetSecretImp const& tImp)
	{
		_uMaxSecretsNum = tImp._uMaxSecretsNum;
		_u24SecretsNum	= tImp._u24SecretsNum;
       _u72SecretsNum	= tImp._u72SecretsNum;
     _uLockFriendsNum	= tImp._uLockFriendsNum;
      _uMaxFrdsFvrNum   = tImp._uMaxFrdsFvrNum;
      _uMinFrdsFvrNum	= tImp._uMinFrdsFvrNum;
         _uSysScrtLoc   = tImp._uSysScrtLoc;
      _uActiveFriends	= tImp._uActiveFriends;
               _uTopN	= tImp._uTopN;
          _pUserProxy	= tImp._pUserProxy;
          _pPostProxy	= tImp._pPostProxy;
           _pLbsProxy	= tImp._pLbsProxy;
	}

	GetSecretCallback(GetSecretCallback const& tImp)
	{
		_uMaxSecretsNum = tImp._uMaxSecretsNum;
		_u24SecretsNum	= tImp._u24SecretsNum;
       _u72SecretsNum	= tImp._u72SecretsNum;
     _uLockFriendsNum	= tImp._uLockFriendsNum;
      _uMaxFrdsFvrNum   = tImp._uMaxFrdsFvrNum;
      _uMinFrdsFvrNum	= tImp._uMinFrdsFvrNum;
         _uSysScrtLoc   = tImp._uSysScrtLoc;
      _uActiveFriends	= tImp._uActiveFriends;
               _uTopN	= tImp._uTopN;
          _pUserProxy	= tImp._pUserProxy;
          _pPostProxy	= tImp._pPostProxy;
           _pLbsProxy	= tImp._pLbsProxy;
	}

	taf::Int32 getFriendsListCallback(SecretListReq tReq, taf::JceCurrentPtr current, GetActiveFriendsRsp const& tRsp);

	taf::Int32 test(taf::Int64 lId, taf::JceCurrentPtr current);
	// 获取帖子列表
	taf::Int32 getSecretList(const QUAN::SecretListReq & tReq, FrdsVector& vFrdsAll, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, Int32 iRegTime, taf::JceCurrentPtr current);
	// 推送帖子
	taf::Int32 setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq, taf::JceCurrentPtr current);

	Int32 getBatchUserFavorSecretsCallback(SecretListReq tReq,
		SecretListRsp tRsp,
		Int32 iFrdsFvrNum,
		FrdsVectorPair tFrdsPair,
		Int32 iRegTime,
		ScrtVector vSecrets,
		taf::JceCurrentPtr current,
		vector<map<Int64, Int32> > const& vOp);

	void returnStatus(Int32 iRet, taf::JceCurrentPtr current)
	{
		LOG->debug() << __FUNCTION__ << " iRet: "<<iRet<<" end " << endl;
		SecretListRsp tRsp;
	    tRsp.iStatus = iRet;
		tRsp.lNextBeginId = 9999999999999;
		GetSecret::async_response_getSecretList(current, iRet, tRsp);
	}

private:

	// begin  获取帖子列表调用函数
	taf::Int32 getRecommendSecrets(SecretListReq const& tReq, 
								   SecretListRsp& tRsp, 
								   Int32 iFrdsNum, 
								   FrdsVectorPair const& tFrdsPair, 
								   FrdsVector const& vFrdsAll, 
								   FrdsVector const& vFrdsFavor, 
								   Int32 iRegTime,
								   ScrtVector& vSecrets,
								   taf::JceCurrentPtr current);

	// 获取好友帖子列表 
	taf::Int32 getFriendsSecrets(SecretListReq const& tReq, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, Int64& lNextBeginId, Int32& iLockCount, ScrtVector& vSecrets);
	// 获取好友列表
	taf::Int32 getFriendsList(UserId const& tUserId, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, FrdsVector& vFrdsAll, Int32& iRegTime);

	Int32 getBatchUserFavorSecrets(SecretListReq const& tReq, 
								   SecretListRsp& tRsp, 
								   Int32 iFrdsFvrNum, 
								   FrdsVectorPair const& tFrdsPair, 
								   FrdsVector const& vFrdsAll, 
								   FrdsVector const& vFrds, 
								   Int32 iRegTime,
								   ScrtVector& vSecrets,
								   taf::JceCurrentPtr current);

	// 从cache或者数据库拉数据
	taf::Int32 getPullSecretList(const QUAN::SecretListReq & tReq,
		QUAN::SecretListRsp &tRsp,
		FrdsVectorPair const& tFrdsPair,
		FrdsVector const& vFrdsAll,
		FrdsVector const& vFrdsFavor,
		Int32 iRegTime,
		ScrtVector& vSecrets,
		taf::JceCurrentPtr current);
	// 重新排列
	taf::Int32 uniqueSecretList(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, Int32 iRegTime, taf::JceCurrentPtr current);
	// 获取附近列表
	taf::Int32 getNearSecrets(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, Int32 iRegTime, taf::JceCurrentPtr current);
	taf::Int32 getNearSecretsCallback(const QUAN::SecretListReq & tReq, 
									  QUAN::SecretListRsp &tRsp, 
									  FrdsVectorPair const& tFrdsPair, 
									  ScrtVector& vSecrets, 
									  Int32 iRegTime, 
									  taf::JceCurrentPtr current, 
									  LbsRsp const& tLbsRsp);

	// 帖子ID转换内容
	taf::Int32 tranSecretsId(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, ScrtMap const& vMaps, taf::JceCurrentPtr current);

	taf::Int32 tranSecretsIdCallback(QUAN::SecretListReq tReq, 
								     QUAN::SecretListRsp tRsp, 
									 FrdsVectorPair tFrdsPair, 
									 ScrtVector vSecrets, 
									 Int32 iStop, 
									 ScrtMap vMaps, 
									 taf::JceCurrentPtr current, 
									 vector<SecretInfo> const& vSecretsInfo);

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
};
#endif
