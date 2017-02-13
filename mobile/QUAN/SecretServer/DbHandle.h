#ifndef __SECRET_DBHANDLE_H_
#define __SECRET_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "Secret.h"
#include "Base.h"
#include "jmem/jmem_hashmap.h"
#include "CommHashMap.h"
#include "UserInfo.h" 

using namespace  QUAN;

typedef vector<SecretInfo>             ScrtVector;
typedef vector<SecretInfo>::iterator   ScrtIterator;
typedef vector<ContentInfo>            CntVector;
typedef vector<ContentInfo>::iterator  CntIterator;
typedef map<Int64, IdNum>               RelatList;
typedef map<Int64, IdNum>::iterator     RelatIterator;

/* callback of async proxy for client */


typedef map<unsigned short, TC_Mysql*>  DwDbMap;
typedef JceHashMap<SecretListKey, SecretListValue, ThreadLockPolicy, FileStorePolicy> SecretHashMap;
typedef JceHashMap<ContentListKey, ContentListValue, ThreadLockPolicy, FileStorePolicy> ContentHashMap;


struct GreatSecret
{
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.lSId > e2.lSId;
    }
};

struct LessRemove
{
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.isRemove < e2.isRemove;
    }
};

struct EqualIsRemove
{
    bool operator()(const SecretInfo &e1)
    {
        return e1.isRemove == 1;
    }
};

struct EqualFriends
{
    bool operator()(const SecretInfo &e1)
    {
        return e1.iType == FRIEND;
    }
};
struct GreatSId
{
    GreatSId (Int64 lId) : lSId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.lSId >= lSId;
    }
 
    Int64 lSId;
};

struct LessTime
{
    LessTime (Int64 lId) : lSId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return decodeSecretId(e1.lSId) < lSId;
    }
 
    Int64 lSId;
};

struct EqualCntUId
{
    EqualCntUId (Int64 lId) : lUId(lId) { }

    bool operator()(const ContentInfo &e1)
    {
        return e1.lUId == lUId;
    }
 
    Int64 lUId;
};

struct EqualCId
{
    EqualCId (Int64 lId) : lCId(lId) { }

    bool operator()(const ContentInfo &e1)
    {
        return e1.lCId == lCId;
    }
 
    Int64 lCId;
};

struct EqualSecret
{
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.lSId == e2.lSId;
    }
 
    Int64 lSId;
};

struct EqualSId
{
    EqualSId (Int64 lId) : lSId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.lSId == lSId;
    }
 
    Int64 lSId;
};

struct EqualUId
{
    EqualUId (Int64 lId) : lUId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.lUId == lUId;
    }
 
    Int64 lUId;
};

struct LessSId
{
    LessSId (Int64 lId) : lSId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.lSId < lSId;
    }
 
    Int64 lSId;
};

struct LessRead
{
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.isRead == false ? true : false;
    }
};

class DbHandle : public TC_Singleton<DbHandle>
{

public:
    DbHandle()          
    {
        init();
    }

    Int32 init();

    // 获取帖子列表
    Int32 getRecommendSecrets(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, vector<SecretInfo>& vSecrets, bool bUseCache = true);
    Int32 getSystemSecret(SecretInfo& tSecret, bool bUseCache = true);

    Int32 getRecommendSecretsFromDb(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, vector<SecretInfo>& vSecrets);
    Int32 getSystemSecretFromDb(SecretInfo& tSecret);

    // 发表帖子
    Int32 setSecret(PostSecretReq const& tReq, Int64 lSId, bool bUseCache = true);
    Int32 setSecretToDb(PostSecretReq const& tReq, Int64 lSId);

    // 发表评论
    Int32 setContent(PostContentReq const& tReq, string const& sCId, Int32 iAvatarId, ContentInfo& tReturnCnt, bool bUseCache = true);
    Int32 setContentToDb(PostContentReq const& tReq, string const& sCId, Int32 iAvatarId);
    Int32 getPortraitId(PostContentReq const& tReq, Int32& iPortraitId, bool bUseCache = true);
    Int32 getPortraitIdFromDb(PostContentReq const& tReq, Int32& iPortraitId);

    // 获取帖子详情 -- 评论列表
    Int32 getContents(UserId const& tUserId, Int64 lFirstId, Int64 lSId, ContentListRsp& tRsp, bool bUseCache = true);
    Int32 getContentsFromDb(UserId const& tUserId, Int64 lSId, ContentListRsp& tRsp);

    // 更新操作
    Int32 setSecretOp(UserId const& tUserId, Int64 lUId, Int64 lId, Int32 iOp, bool bUseCache = true);
    Int32 setSecretOpToDb(UserId const& tUserId, Int64 lUId, Int64 lId, Int32 iOp);
    Int32 setContentOp(UserId const& tUserId, Int64 lUId, Int64 lId, Int64 lSId, Int32 iOp, bool bUseCache = true);
    Int32 setContentOpToDb(UserId const& tUserId, Int64 lUId, Int64 lId, Int64 lSId, Int32 iOp);

    // 用户点赞帖子的关系
    Int32 getUserFavorSecrets(Int64 lUId, SecretRelatMap& tFavorScrts, bool bUseCache = true);
    Int32 getUserFavorSecretsFromDb(Int64 lUId, SecretRelatMap& tFavorScrts);
    // 用户移除帖子的关系
    Int32 getUserRemoveSecrets(Int64 lUId, SecretRelatMap& tRemoveScrts, bool bUseCache = true);
    Int32 getUserRemoveSecretsFromDb(Int64 lUId, SecretRelatMap& tRemoveScrts);
    // 用户点赞评论的关系
    Int32 getUserContentSecrets(Int64 lUId, Int64 lSId, ContentRelatMap& tFavorCnts, bool bUseCache = true);
    Int32 getUserFavorContentsFromDb(Int64 lSId, ContentRelatMap& tFavorCnts);
    // 从cache获取用户的帖子列表
    Int32 getUserSecretListFromCache(Int64 lUId, Int64 lBeginId, Int32 iScrtsNum, ScrtVector& vSecrets);

    // 获取用户赞帖列表
    Int32 getBatchUserFavorSecrets(Int64 lUId, Int32 iSum, Int32 iFrdsFvrNum, vector<FriendsList> const& vFrds, ScrtVector& vSecrets);
    // 批量获取用户，用户好友，好友好友发帖子列表
    Int32 getAllUserSecrets(vector<FriendsList> const& vFrdsAll, size_t iScrtsNum, Int64 lBeginId, Int64& lNextBeginId, vector<SecretInfo>& vSecrets);
    // 批量获取用户帖子列表
    Int32 getBatchUserSecrets(Int64 lUId, Int32 iType, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, bool bUseCache = true);
    Int32 getBatchUserSecretsFromDb(Int64 lUId, Int32 iType, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets);
    // 单独获取用户帖帖子
    Int32 getUserSecrets(Int64 lUId, Int64 lSId, Int32 iType, SecretInfo& tSecret);
    // 批量获取用户帖帖子
    Int32 getUserBatchSecrets(Int64 lUId, map<Int64,MsgRmd> const& vSIds, ScrtVector& Secrets);

    // 更新帖子评论数
    Int32 updScrtContentNum(UserId const& tUserId, Int64 lFirstId, Int64 lSId, Int32 iOp, bool bUseCache = true);
    Int32 updScrtContentNumToDb(UserId const& tUserId, Int64 lSId, Int32 iOp);

    // 取评论用户ID集合
    Int32 getContentUserIds(Int64 lUId, Int64 lSId, vector<Int64>& vUserIds, bool bUseCache = true);
    Int32 getContentUserIdsFromDb(Int64 lUId, Int64 lSId, vector<Int64>& vUserIds);

    // 缓存粉丝收帖箱
    Int32 setSecretToCache(PostSecretReq const& tReq, Int64 lSId, FriendsList const& tFriendsList, Int32 iType, bool bUseCache = true);
    Int32 setBatchSecretsToCache(vector<SecretInfo>& vSecrets, Int64 lUId, bool bUseCache = true);

    // 单独提供帖子内容
    Int32 getUserSingleSecretFromDb(const QUAN::SglSecretReq & tReq, QUAN::SglSecretRsp &tRsp);
    // 设置帖子为分享状态
    Int32 setUserSecretShareToDb(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp);

    Int32 setFeedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp);

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
    TC_ThreadMutex  _tSecretLock;
    TC_ThreadMutex  _tContentLock;
    TC_ThreadMutex  _tFavorNumLock;
    TC_ThreadMutex  _tCmtNumLock;
};

#endif                                             
