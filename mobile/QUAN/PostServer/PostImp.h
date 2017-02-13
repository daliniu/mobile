#ifndef _PostImp_H_
#define _PostImp_H_

#include "servant/Application.h"
#include "Post.h"
#include "PostCache.h"

/**
 *
 *
 */
class PostImp : public QUAN::Post
{
    public:
        /**
         *
         */
        virtual ~PostImp() {}

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
        virtual taf::Int32 getSecretContent(const QUAN::ContentListReq & tReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 postSecret(const QUAN::PostSecretReq & tReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 postContent(const QUAN::PostContentReq & tReq,QUAN::PostContentRsp &tRsp,vector<taf::Int64> &vRelatUId,taf::JceCurrentPtr current);

        virtual taf::Int32 postFavor(const QUAN::PostFavorReq & tReq,QUAN::PostFavorRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 postReport(const QUAN::PostReportReq & tReq,QUAN::PostReportRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 removeSecret(const QUAN::RemoveSecretReq & tReq,QUAN::RemoveSecretRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 removeContent(const QUAN::RemoveContentReq & tReq,QUAN::RemoveContentRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 cleanTrail(const QUAN::CleanTrailReq & tReq,QUAN::CleanTrailRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 getBatchSecrets(taf::Int64 lUId,const vector<taf::Int64> & vInSecret,vector<QUAN::SecretInfo> &vOutSecrets,taf::JceCurrentPtr current);

        virtual taf::Int32 clearPostCache(taf::Int64 lSId,taf::JceCurrentPtr current);

        virtual taf::Int32 clearCommentCache(taf::Int64 lCId,taf::JceCurrentPtr current);

        virtual taf::Int32 clearUserOpCache(taf::Int64 lUId,taf::JceCurrentPtr current);

        virtual taf::Int32 clearUserCleanTrailCache(taf::Int64 lUId,taf::JceCurrentPtr current);

        virtual taf::Int32 getUserOpBatch(taf::Int64 lUId,const vector<QUAN::FriendsList> & vUId,vector<map<taf::Int64, taf::Int32> > &vOp,taf::JceCurrentPtr current);

        virtual taf::Int32 getBatchNotifySecrets(taf::Int64 lUId,const vector<QUAN::NotifyPost> & vInSecret,vector<QUAN::SecretInfo> &vOutSecrets,taf::JceCurrentPtr current);

        virtual taf::Int32 shareSecret(taf::Int64 lSId,taf::Int64 lShareId,taf::JceCurrentPtr current);

        virtual taf::Int32 auditSecret(const vector<taf::Int64> & vSId,taf::Int32 iLevel,taf::JceCurrentPtr current);
        virtual taf::Int32 getSecretById(const GetSecretByIdReq& tReq, GetSecretByIdRsp& tRsp, taf::JceCurrentPtr current);
    protected:
        PostCache tPostCache;

};
/////////////////////////////////////////////////////
#endif
