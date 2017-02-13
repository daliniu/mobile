#ifndef __KEYWORDCB_H__
#define __KEYWORDCB_H__

#include "Keyword.h"
#include "DbHandle.h"
#include "ErrorCode.h"
#include "PostImp.h"
#include "PostServer.h"
#include "AutoTimeLog.h"

struct AutoTimeLogFuncObj
{
    void operator()(const std::string& str) const
    {
        FDLOG()<<str<<endl;
    }
};

typedef tafAsync::AutoTimeLogObj<AutoTimeLogFuncObj> AutoTimeLog;

inline taf::Int64 getId(long lUId)
{
    time_t now = TC_TimeProvider::getInstance()->getNow();

    Int64 lTmp = now << 32;
    Int64 lSId = lTmp + ((lUId & 0x00000000FFFFFFFF) ^ gUIdXorNum);

    if(lSId<=0) 
    {
        throw std::runtime_error("Invaild SID"+TC_Common::tostr(lSId));
    }
    return lSId;
}


class PostKeywordCallback:public Comm::KeywordPrxCallback
{
    public:
        PostKeywordCallback(AutoTimeLog &log,const QUAN::PostSecretReq & tReq,taf::JceCurrentPtr current)
            :log(log),tReq(tReq),current(current)
        {}
        virtual void callback_keywordFilter(taf::Int32 ret, taf::Int32 level)
        {
            QUAN::PostSecretRsp tRsp;
            if(level<=g_app.getSenstiveLevel())
            {
                int iRet=-1;

                try
                {
                    // 生成CId 
                    taf::Int64 lSId=getId(tReq.tUserId.lUId);

                    if(!g_app.spanCheck(tReq.tUserId.lUId,tReq.sContent))
                    {
                        tRsp.iStatus=SUCCESS;
                        tRsp.lSId=lSId;
                    }
                    else
                    {
                        //dbaccess
                        QUAN::SecretInfo tSecret;
                        tSecret.lSId=lSId;
                        tSecret.lUId=tReq.tUserId.lUId;
                        tSecret.sContent=tReq.sContent;
                        tSecret.iColorId=tReq.iColorId;
                        tSecret.sBitUrl=tReq.sBitUrl;
                        tSecret.sLocal=tReq.sLocal;
                        tSecret.lCircleId=tReq.lCircleId;

                        SecretExtra tExtra;
                        tExtra.sCoord=tReq.sCoord;

                        taf::TC_ThreadLock::Lock lock(*LockPostCache::getInstance());
                        if(LockPostCache::getInstance()->postSecret(tSecret,tExtra)>=0)
                        {
                            tRsp.iStatus=SUCCESS;
                            tRsp.lSId=lSId;
                            iRet=0;
                        }
                    }
                }
                catch(exception &ex)
                {
                    LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
                }

                iRet<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
                PostImp::async_response_postSecret(current,iRet,tRsp);
                
            }
            else
            {
                tRsp.iStatus=ERR_SENSITIVEWORDS;
                ERR_SENSITIVEWORDS<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
                PostImp::async_response_postSecret(current,ERR_SENSITIVEWORDS,tRsp);
            }
        }
        virtual void callback_keywordFilter_exception(taf::Int32 ret)
        {
            QUAN::PostSecretRsp tRsp;
            ERR_UNKNOWN<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
            PostImp::async_response_postSecret(current,ERR_UNKNOWN,tRsp);
        }
    private:
        AutoTimeLog log;
        QUAN::PostSecretReq tReq;
        taf::JceCurrentPtr current;
};

class CommentKeywordCallback:public Comm::KeywordPrxCallback
{
    public:
        CommentKeywordCallback(AutoTimeLog &log,const QUAN::PostContentReq & tReq,taf::JceCurrentPtr current)
            :log(log),tReq(tReq),current(current)
        {}
        virtual void callback_keywordFilter(taf::Int32 ret, taf::Int32 level)
        {
            QUAN::PostContentRsp tRsp;
            vector<taf::Int64> vRelatUId;
            if(level<=g_app.getSenstiveLevel())
            {
                int iRet=-1;

                try
                {
                    taf::Int64 lCId=getId(tReq.tUserId.lUId);

                    vector<ContentInfo> vContents;
                    taf::TC_ThreadLock::Lock lock(*LockPostCache::getInstance());
                    if(LockPostCache::getInstance()->getComments(tReq.lSId,vContents)>=0)
                    {
                        QUAN::ContentInfo tContent;
                        tContent.lSId=tReq.lSId;
                        tContent.lCId=lCId;
                        tContent.lUId=tReq.tUserId.lUId;
                        tContent.sContent=tReq.sContent;
                        tContent.Time=TC_Common::now2str("%Y-%m-%d %H:%M:%S");
                        tContent.isFirst=tReq.lFirstId==tReq.tUserId.lUId;
                        if(tContent.isFirst) //群主头像0
                        {
                            tContent.iPortrait=0;
                        }
                        else    //生成头像ID
                        {
                            set<taf::Int64> mpPortrait;
                            int iPortrait=1;
                            if(vContents.size()>0)
                            {
                                for(vector<ContentInfo>::iterator it=vContents.begin();it!=vContents.end();++it)
                                {
                                    if(it->lUId!=tReq.lFirstId)
                                    {
                                        if(it->lUId==tReq.tUserId.lUId)
                                        {
                                            break;
                                        }
                                        set<taf::Int64>::iterator pit=mpPortrait.find(it->lUId);
                                        if(pit==mpPortrait.end())
                                        {
                                            mpPortrait.insert(pit,it->lUId);
                                            iPortrait++;
                                            if((iPortrait+1)%31==0)  //CLIENT BUG FIX
                                            {
                                                ++iPortrait;
                                            }
                                        }
                                    }
                                }

                            }
                            tContent.iPortrait=iPortrait;
                        }
                        if(!g_app.spanCheck(tReq.tUserId.lUId,tReq.sContent))
                        {
                            tRsp.iStatus=SUCCESS;
                            tContent.bIsMe=true;
                            tRsp.tContent=tContent;
                            LOG->error()<<__FUNCTION__<<"|span|"<<tReq.tUserId.lUId<<":"<<tReq.sContent<<endl;
                            iRet=0;
                        }
                        else
                        {
                            LockPostCache::getInstance()->getPostRelation(tReq.lSId,vRelatUId);
                            if(LockPostCache::getInstance()->postContent(tContent)>=0)
                            {
                                tRsp.iStatus=SUCCESS;
                                tContent.bIsMe=true;
                                tRsp.tContent=tContent;
                                LockPostCache::getInstance()->clearPostCache(tReq.lSId);
                                iRet=0;
                            }
                            else
                            {
                                LOG->error()<<__FUNCTION__<<" postContent fail!"<<endl;
                            }
                        }
                    }
                    else
                    {
                        LOG->error()<<__FUNCTION__<<"|"<<"Get Comment Failed Post ID:"<<tReq.lSId<<endl;
                    }
                }
                catch(exception &ex)
                {
                    LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
                }

                iRet<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
                PostImp::async_response_postContent(current,iRet,tRsp,vRelatUId);
            }
            else
            {
                tRsp.iStatus=ERR_SENSITIVEWORDS;
                ERR_SENSITIVEWORDS<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
                PostImp::async_response_postContent(current,ERR_SENSITIVEWORDS,tRsp,vRelatUId);
            }
        }
        virtual void callback_keywordFilter_exception(taf::Int32 ret)
        {
            QUAN::PostContentRsp tRsp;
            vector<taf::Int64> vRelatUId;
            ERR_UNKNOWN<<("|"<<log<<"|"<<JCETOSTR(tReq)<<"|"<<JCETOSTR(tRsp));
            PostImp::async_response_postContent(current,ERR_UNKNOWN,tRsp,vRelatUId);
        }
    private:
        AutoTimeLog log;
        QUAN::PostContentReq tReq;
        taf::JceCurrentPtr current;
};

#endif
