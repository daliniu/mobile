#include <algorithm>
#include <sstream>
#include "PostImp.h"
#include "servant/Application.h"
#include "KeywordCb.h"

using namespace std;



//////////////////////////////////////////////////////
void PostImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void PostImp::destroy()
{
    //destroy servant here:
    //...
}


taf::Int32 PostImp::getSecretContent(const QUAN::ContentListReq & tReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        iRet=tPostCache.getUserComments(tReq.tUserId.lUId,tReq.lSId,tRsp.tSecret,tRsp.iFavorNum,tRsp.iCmtNum,tRsp.vContents);
        tRsp.iStatus=iRet;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}


taf::Int32 PostImp::postSecret(const QUAN::PostSecretReq & tReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        if(!tReq.sContent.empty())
        {
            AutoTimeLog log(__FUNCTION__);
            TC_AutoPtr<PostKeywordCallback> cb=new PostKeywordCallback(log,tReq,current);
            g_app.getKeywordPrx()->async_keywordFilter(cb,"secret",tReq.sContent);
            current->setResponse(false);
            return 0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp)<<endl;
    return iRet;
}


taf::Int32 PostImp::postContent(const QUAN::PostContentReq & tReq,QUAN::PostContentRsp &tRsp,vector<taf::Int64> &vRelatUId,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        if(!tReq.sContent.empty())
        {
            AutoTimeLog log(__FUNCTION__);
            TC_AutoPtr<CommentKeywordCallback> cb=new CommentKeywordCallback(log,tReq,current);
            g_app.getKeywordPrx()->async_keywordFilter(cb,"secret",tReq.sContent);
            current->setResponse(false);
            return 0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp)<<endl;
    return iRet;
}


taf::Int32 PostImp::postFavor(const QUAN::PostFavorReq & tReq,QUAN::PostFavorRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        if(tPostCache.postSecretOp(tReq.iType,tReq.tUserId.lUId,tReq.lFavoredId,tReq.lId,tReq.iOp)>=0)
        {
            tRsp.iStatus=SUCCESS;
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    
    log<<iRet<<"|"<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}


taf::Int32 PostImp::postReport(const QUAN::PostReportReq & tReq,QUAN::PostReportRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        if(tPostCache.postSecretOp(tReq.iType,tReq.tUserId.lUId,tReq.lReportedId,tReq.lSId,REPORTY)>=0)
        {
            tRsp.iStatus=SUCCESS;
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    
    log<<iRet<<"|"<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}

taf::Int32 PostImp::removeSecret(const QUAN::RemoveSecretReq & tReq,QUAN::RemoveSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        SecretInfo tSecretInfo;
        if(tPostCache.getPost(tReq.lSId,tSecretInfo)>=0)
        {
            if(tReq.tUserId.lUId==tSecretInfo.lUId)
            {
                if(tPostCache.postSecretOp(SECRET,tReq.tUserId.lUId,tReq.lUId,tReq.lSId,CLEANY)>=0)
                {
                    tRsp.iStatus=SUCCESS;
                    iRet=0;
                }
            }
            else
            {
                if(tPostCache.postSecretOp(SECRET,tReq.tUserId.lUId,tReq.lUId,tReq.lSId,REMOVEY)>=0)
                {
                    tRsp.iStatus=SUCCESS;
                    iRet=0;
                }
            }
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    
    log<<iRet<<"|"<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}

taf::Int32 PostImp::removeContent(const QUAN::RemoveContentReq & tReq,QUAN::RemoveContentRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        vector<ContentInfo> vContents;
        if(tPostCache.getComments(tReq.lSId,vContents)>=0)
        {
            for(vector<ContentInfo>::iterator it=vContents.begin();;++it) //## 性能考查:这里可以考虑用equal_range查
            {
                if(it!=vContents.end())
                {
                    if(it->lCId==tReq.lCId)
                    {
                        if(tReq.tUserId.lUId==it->lUId)
                        {
                            if(tPostCache.postSecretOp(CONTENT,it->lUId,tReq.tUserId.lUId,tReq.lCId,CLEANY)>=0)
                            {
                                tRsp.iStatus=SUCCESS;
                                iRet=0;
                            }
                        }
                        else
                        {
                            if(tPostCache.postSecretOp(CONTENT,it->lUId,tReq.tUserId.lUId,tReq.lCId,REMOVEY)>=0)
                            {
                                tRsp.iStatus=SUCCESS;
                                iRet=0;
                            }
                        }
                        break;
                    }
                }
                else
                {
                    LOG->error()<<__FUNCTION__<<"| COMMENTID:"<<tReq.lCId<<" NO FOUND IN POST ID:"<<tReq.lSId<<endl;
                    break;
                }
            }
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"| POST ID:"<<tReq.lSId<<" NO FOUND"<<endl;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    
    log<<iRet<<"|"<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}

taf::Int32 PostImp::cleanTrail(const QUAN::CleanTrailReq & tReq,QUAN::CleanTrailRsp &tRsp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        if(tPostCache.cleanTrail(tReq.tUserId.lUId)>=0)
        {
            tRsp.iStatus=SUCCESS;
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp);
    return iRet;
}

taf::Int32 PostImp::getBatchSecrets(taf::Int64 lUId,const vector<taf::Int64> & vInSecret,vector<QUAN::SecretInfo> &vOutSecrets,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        if(vInSecret.size()==0 || tPostCache.getUserPostBatch(lUId,vInSecret,vOutSecrets)>=0)
        {
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId<<"|"<<vInSecret.size()<<"/"<<vOutSecrets.size();
    return iRet;
}

taf::Int32 PostImp::clearPostCache(taf::Int64 lSId,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        iRet=tPostCache.clearPostCache(lSId);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lSId;
    return iRet;
}

taf::Int32 PostImp::clearCommentCache(taf::Int64 lCId,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        iRet=tPostCache.clearCommentCache(lCId);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lCId;
    return iRet;
}

taf::Int32 PostImp::clearUserOpCache(taf::Int64 lUId,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        iRet=tPostCache.clearUserOpCache(lUId);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId;
    return iRet;
}

taf::Int32 PostImp::clearUserCleanTrailCache(taf::Int64 lUId,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        iRet=tPostCache.clearUserCleanTrailCache(lUId);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId;
    return iRet;
}

taf::Int32 PostImp::getUserOpBatch(taf::Int64 lUId,const vector<QUAN::FriendsList> & vUId,vector<map<taf::Int64, taf::Int32> > &vOp,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    ostringstream ss;
    try
    {
        for(vector<QUAN::FriendsList>::const_iterator it=vUId.begin();it!=vUId.end();++it)
        {
            map<taf::Int64,taf::Int32> mpUserOp,mpTemp;
            tPostCache.getUserFavor(it->lUId,mpUserOp);
            for(map<taf::Int64,taf::Int32>::iterator mit=mpUserOp.begin();mit!=mpUserOp.end();++mit)
            {
                if(mit->second==0)
                {
                    mpTemp[mit->first]=mit->second;
                }
            }
            vOp.push_back(mpTemp);
            ss<<it->lUId<<"=>["<<mpTemp.size()<<"];";
        }
        LOG->debug()<<__FUNCTION__<<"|"<<lUId<<"|"<<ss.str()<<endl;
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId<<"|"<<vOp.size();
    return iRet;
}


taf::Int32 PostImp::getBatchNotifySecrets(taf::Int64 lUId,const vector<QUAN::NotifyPost> & vInSecret,vector<QUAN::SecretInfo> &vOutSecrets,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        if(vInSecret.size()==0 || tPostCache.getUserNotifyBatch(lUId,vInSecret,vOutSecrets)>=0)
        {
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId<<"|"<<vInSecret.size()<<"/"<<vOutSecrets.size();
    return iRet;
}


taf::Int32 PostImp::shareSecret(taf::Int64 lSId,taf::Int64 lShareId,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        tPostCache.shareSecret(lSId,lShareId);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<lSId<<"|"<<lShareId;
    return iRet;
}

taf::Int32 PostImp::auditSecret(const vector<taf::Int64> & vSId,taf::Int32 iLevel,taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        for(auto lSId:vSId)
        {
            tPostCache.auditSecret(lSId,iLevel);
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<TC_Common::tostr(vSId)<<"|"<<iLevel;
    return iRet;
}

taf::Int32 PostImp::getSecretById(const GetSecretByIdReq& tReq, GetSecretByIdRsp& tRsp, taf::JceCurrentPtr current)
{
    AutoTimeLog log;
    int iRet=-1;
    try
    {
        tPostCache.getPost(tReq.tSecret.lSId, tRsp.tSecret);

        if (tReq.tContent.lUId != 0)
        {
            vector<ContentInfo> vContents;
            tPostCache.getComments(tReq.tSecret.lSId, vContents);
            if (!vContents.empty())
            {
                for (auto tContent : vContents)
                {
                    LOG->debug()<<__FUNCTION__<<" "<<JCETOSTR(tContent)<<" "<<tContent.lCId<<" "<<tReq.tContent.lCId<<endl;
                    if (tContent.lCId == tReq.tContent.lCId) 
                    {
                        tRsp.tReadContent = tContent;
                        break;
                    }
                }
                for (auto tContent : vContents)
                {
                    if (tContent.lUId == tReq.tContent.lUId) 
                    {
                        tRsp.tContent = tContent;
                        break;
                    }
                }
            }
        }

        LOG->debug()<<__FUNCTION__<<" 111111111"<<endl;
/*
        if (tRsp.tSecret.lSId != 0 && tRsp.tContent.lCId == 0)
        {
            vector<Int64> vRelatUId;
            PostContentReq tPReq;
            PostContentRsp tPRsp;
            tPReq.tUserId = tReq.tUserId;
            tPReq.lSId = tRsp.tSecret.lSId;
            tPReq.lFirstId = tRsp.tSecret.lUId;
            tPReq.sContent = "$[position]";
            this->postContent(tPReq, tPRsp, vRelatUId, current);
            current->setResponse(false);
            tRsp.tContent = tPRsp.tContent;
            LOG->debug()<<__FUNCTION__<<" 111111111"<<endl;
        }
*/
        iRet = 0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    log<<iRet<<"|"<<__FUNCTION__<<"|"<<tRsp.tSecret.lSId<<"|"<<tRsp.tContent.lCId;
    return iRet;
}
