#ifndef __GETSECRET_DBHANDLE_H_
#define __GETSECRET_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "GetSecret.h"
#include "Base.h"
#include "jmem/jmem_hashmap.h"
#include "../SecretServer/CommHashMap.h"
#include "UserInfo.h" 

using namespace  QUAN;

typedef pair<vector<FriendsList>, vector<FriendsList> >        FrdsVectorPair;
typedef Int64                                      ScrtId;
typedef map<Int64, FriendsList>                    FrdsMap;
typedef map<Int64, FriendsList>::iterator          FrdsMapIterator;
typedef map<Int64, FriendsList>::const_iterator    FrdsMapCIterator;
typedef vector<FriendsList>::iterator              FrdsIterator;
typedef vector<FriendsList>                        FrdsVector;
typedef vector<FriendsList>::iterator              FrdsIterator;
typedef vector<FriendsList>::const_iterator        FrdsCIterator;
typedef vector<Int64>                              ScrtVector;
typedef map<Int64, Int64>                          ScrtMap;
typedef map<Int64, Int64>::iterator                ScrtMapIterator;
typedef map<Int64, Int64>::const_iterator          ScrtMapCIterator;
typedef vector<Int64>::iterator                    ScrtIterator;
typedef vector<Int64>::const_iterator              ScrtCIterator;

/* callback of async proxy for client */


typedef map<unsigned short, TC_Mysql*>  DwDbMap;
typedef JceHashMap<SecretListKey, SecretListValue, ThreadLockPolicy, FileStorePolicy> SecretHashMap;
typedef JceHashMap<SecretListKey, SecretListValue, ThreadLockPolicy, FileStorePolicy> CircleHashMap;

inline void FrdsRelat(FrdsVector& vFrds)
{
    // 转义好友度数
    for (FrdsIterator i = vFrds.begin(); i!= vFrds.end(); ++i)
    {
        if (i->iRelat > ONE)
            i->iRelat--;
    }
}

inline void FrdsRelat(FrdsIterator& i)
{
    // 转义好友度数
    if (i->iRelat > ONE)
        i->iRelat--;
}


struct NullLocation
{
    bool operator()(SecretInfo const& e) const
    {
        return e.iType == RECMD && e.sLocal.empty();
    }
};

struct GreatShareId
{
    bool operator()(SecretInfo const& e1, SecretInfo const& e2) const
    {
		return e1.lShareId > e2.lShareId;
    }
};

struct LessTime
{
    LessTime (Int64 lId) : lTime(lId) { }

    bool operator()(Int64 lSId) const
    {
        return decodeSecretId(lSId) < lTime;
    }
 
    Int64 lTime;
};

struct EqualFriend
{
    bool operator()(SecretInfo const& e) const
    {
        return e.iType == FRIEND;
    }
};

class DbHandle
{

public:
    DbHandle()          
    {
        init();
    }

    Int32 init();

    // 获取系统推荐帖
    Int32 getRecommendSecrets(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets, bool bUseCache = true);
    Int32 getRecommendSecretsFromDb(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets);

    // 获取系统引导帖
    Int32 getSystemSecret(SecretInfo& tSecret, bool bUseCache = true);
    Int32 getSystemSecretFromDb(SecretInfo& tSecret);

    // 从cache获取用户的帖子列表
    Int32 getUserSecretListFromCache(Int64 lUId, Int64 lBeginId, Int32 iScrtsNum, ScrtVector& vSecrets, Int32& iLockCount);

    // 批量获取用户，用户好友，好友好友发帖子列表
    Int32 getAllUserSecrets(FrdsVector const& vFrdsAll, size_t iScrtsNum, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets);

    // 批量获取用户帖子列表
    Int32 getBatchUserSecrets(Int64 lUId, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, Int32 iCircle, bool bUseCache = true);
    Int32 getBatchUserSecretsFromDb(Int64 lUId, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, Int32 iCircle);

    // 单独获取用户帖帖子
    Int32 getUserSecrets(Int64 lUId, Int64 lSId, ScrtId& tSecret);

    // 缓存粉丝收帖箱
    Int32 setSecretToCache(PostSecretReq const& tReq, Int64 lSId, FriendsList const& tFriendsList, Int32 iType, bool bUseCache = true);

    Int32 setBatchSecretsToCache(ScrtVector const& vSecrets, Int64 lUId, Int32 iLockCount, bool bUseCache = true);

    // 单独提供帖子内容
    Int32 getUserSingleSecretFromDb(const QUAN::SglSecretReq & tReq, QUAN::SglSecretRsp &tRsp);
    // 设置帖子为分享状态
    Int32 setUserSecretShareToDb(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp);
    // 设置反馈
    Int32 setFeedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp);

    // 获取系统引导帖批量接口
    Int32 getSystemList(vector<SecretInfo>& vSecrets, bool bUseCache = true);
    Int32 getSystemListFromDb(vector<SecretInfo>& vSecrets);

	// 获取树洞列表
	Int32 getCircleList(GetCircleListReq const& tReq, ScrtVector& vCircleLists, bool bUseCache);
	Int32 getCircleListFromCache(GetCircleListReq const& tReq, ScrtVector& vCircleLists);
	Int32 setCircleListToCache(GetCircleListReq const& tReq, ScrtVector const& vCircleLists);

	// 更新活跃用户
	Int32 setActiveUser(const UserId& tReq, Int64 lCircleId = 0);

    // 获取所有帖子列表
    Int32 getAllSecretsFromDb(Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets);

	Int32 cleanCircleIdCache(Int64 lCircleId);

private:

    TC_Mysql* getMysql(Int64 yyuid);
    string getTbName(Int64 yyuid);

private:

    DwDbMap         _mMysql;
    unsigned short  _uDbNum;
    unsigned short  _uTbNum;
    unsigned short  _uUserRemoveNum;
    unsigned short  _uUserFavorNum;
    unsigned short  _uCacheTimeOut;
    unsigned short  _uUserScrtCacheTimeOut;
    unsigned short  _uRecmdTimeOut;
    unsigned short  _uSysTimeOut;
    string          _sDbPre;
    string          _sTbSecretPre;
    string          _sTbContentPre;
    string          _sTbRelatPre;
	TC_ThreadMutex   tRmdLock;
	TC_ThreadMutex   tSysListLock;
	TC_ThreadMutex   tSysLock;
};

class LockDbHandle:public DbHandle,public TC_ThreadLock,public TC_Singleton<LockDbHandle>
{
};

#endif                                             
