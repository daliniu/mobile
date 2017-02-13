#ifndef __POSTCACHE_H__
#define __POSTCACHE_H__

#include "Post.h"
#include "DbHandle.h"
#include "CommHashMap.h"

#define CACHE_TIMEOUT (600)

typedef JceHashMap<PostCacheKey,PostCacheValue,ThreadLockPolicy,FileStorePolicy> PostCacheType;
typedef JceHashMap<PostCacheKey,CommentCacheValue,ThreadLockPolicy,FileStorePolicy> CommentCacheType;
typedef JceHashMap<PostCacheKey,UserFavorCacheValue,ThreadLockPolicy,FileStorePolicy> UserFavorCacheType;
typedef JceHashMap<PostCacheKey,UserCleanTrailCacheValue,ThreadLockPolicy,FileStorePolicy> UserCleanTrailCacheType;


extern PostCacheType gPostCache;
extern CommentCacheType gCommentCache;
extern UserFavorCacheType gUserFavorCache;
extern UserCleanTrailCacheType gUserCleanTrailCache;


class PostCache
{
    public:
        PostCache()
        {
        }

        static void init()
        {
            TC_Config tConf;
            tConf.parseFile(ServerConfig::BasePath + "PostServer.conf");

            CommHashMap::getInstance()->initHashMap(tConf,"postCache",gPostCache);
            CommHashMap::getInstance()->initHashMap(tConf,"commentCache",gCommentCache);
            CommHashMap::getInstance()->initHashMap(tConf,"userFavorCache",gUserFavorCache);
            CommHashMap::getInstance()->initHashMap(tConf,"userCleanTrailCache",gUserCleanTrailCache);
        }

        int shareSecret(taf::Int64 lSId,taf::Int64 lShareId)
        {
            return tDbHandle.shareSecret(lSId,lShareId);
        }
        int postSecret(SecretInfo &tSecret,SecretExtra &tExtra)
        {
            return tDbHandle.postSecret(tSecret,tExtra);
        }
        int postContent(ContentInfo &tContent)
        {
            return tDbHandle.postContent(tContent);
        }
        int auditSecret(taf::Int64 lSId,taf::Int32 iLevel)
        {
            return tDbHandle.auditSecret(lSId,iLevel);
        }
        int postSecretOp(taf::Int32 iType,taf::Int64 lUId,taf::Int64 lTargetUId,taf::Int64 lSId,taf::Int32 iOp)
        {
            int iRet=tDbHandle.postSecretOp(iType,lUId,lTargetUId,lSId,iOp);
            if(iRet>=0)
            {
                return clearUserOpCache(lUId);
            }
            return iRet;
        }
        int cleanTrail(taf::Int64 lUId)
        {
            int iRet=tDbHandle.cleanTrail(lUId);
            if(iRet>=0)
            {
                return clearUserCleanTrailCache(lUId);
            }
            return iRet;
        }
        int getUserPostBatch(taf::Int64 lUId,const vector<taf::Int64> &vInSecret,vector<SecretInfo> &vOutSecret);
        int getUserNotifyBatch(taf::Int64 lUId,const vector<QUAN::NotifyPost> &vInSecret,vector<SecretInfo> &vOutSecret);
        int getUserComments(taf::Int64 lUId,taf::Int64 lSId,QUAN::SecretInfo& tSecret,taf::Int32 &lFavorNum,taf::Int32 &lCmtNum,vector<ContentInfo> &vContents);
        int getUserFavor(taf::Int64 lUId,map<taf::Int64,taf::Int32> &mpUserFavor);

        int getPostRelation(taf::Int64 lSId,vector<taf::Int64> &vUId);

        int clearPostCache(taf::Int64 lSId);
        int clearCommentCache(taf::Int64 lCId);
        int clearUserOpCache(taf::Int64 lUId);
        int clearUserCleanTrailCache(taf::Int64 lUId);

    public:
        int getPost(taf::Int64 lSId,SecretInfo &tSecret);
        int updatePost(taf::Int64 lSId,SecretInfo &tSecret);

        int getComments(taf::Int64 lSId,vector<ContentInfo> &vContents);

        int getCleanTrailTime(taf::Int64 lUId,taf::Int32 &iCleanTrailTime);

        bool checkPostRelation(taf::Int64 lUId,SecretInfo &tSecretInfo);


    private:
        DbHandle tDbHandle;
};

class LockPostCache:public PostCache,public TC_ThreadLock,public TC_Singleton<LockPostCache>
{
};

#endif
