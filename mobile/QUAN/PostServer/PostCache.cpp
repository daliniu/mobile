#include "PostCache.h"
#include "util/tc_timeprovider.h"
#include "ErrorCode.h"

PostCacheType gPostCache;
CommentCacheType gCommentCache;
UserFavorCacheType gUserFavorCache;
UserCleanTrailCacheType gUserCleanTrailCache;

const int SKIP_REASON_REMOVE=1;
const int SKIP_REASON_CLEAN=2;
const int SKIP_REASON_NOEXISTS=3;
const int SKIP_REASON_REPORT=4;
const int SKIP_REASON_NORELAT=5;

inline int getTimespanFromID(long lId)
{
    return (lId)>>32;
}

const char* DELETE_COMMENT_HINT="[该评论已被作者删除]";

int PostCache::getPost(taf::Int64 lSId,SecretInfo &tSecret)
{
    int iRet=-1;

    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();
        PostCacheKey key;
        PostCacheValue value;
        key.lSId=lSId;
        if(CommHashMap::getInstance()->getHashMap(gPostCache,key,value)>=0 && now-value.iTimespan<CACHE_TIMEOUT)
        {
            tSecret=value.tSecretInfo;
            iRet=0;
        }
        else if(tDbHandle.getSecret(lSId,tSecret)>=0)
        {
            value.tSecretInfo=tSecret;
            value.iTimespan=now;
            CommHashMap::getInstance()->addHashMap(gPostCache,key,value);
            iRet=0;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|"<<"DB GET ID:"<<lSId<<" FAILED"<<endl;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int PostCache::updatePost(taf::Int64 lSId,SecretInfo &tSecret)
{
    int iRet=-1;
    
    try
    {
        PostCacheKey key;
        PostCacheValue value;
        key.lSId=lSId;
        value.tSecretInfo=tSecret;
        value.iTimespan=TC_TimeProvider::getInstance()->getNow();
        if(CommHashMap::getInstance()->addHashMap(gPostCache,key,value)==0)
        {
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int PostCache::getComments(taf::Int64 lSId,vector<ContentInfo> &vContents)
{
    int iRet=-1;

    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();
        PostCacheKey key;
        CommentCacheValue value;
        key.lSId=lSId;
        if(CommHashMap::getInstance()->getHashMap(gCommentCache,key,value)>=0 && now-value.iTimespan<CACHE_TIMEOUT)
        {
            vContents.swap(value.vContentInfo);
            iRet=0;
        }
        else if(tDbHandle.getComments(lSId,value.vContentInfo)>=0)
        {
            value.iTimespan=now;
            CommHashMap::getInstance()->addHashMap(gCommentCache,key,value);
            vContents.swap(value.vContentInfo);
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int PostCache::getCleanTrailTime(taf::Int64 lUId,taf::Int32 &iCleanTrailTime)
{
    int iRet=-1;

    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();
        PostCacheKey key;
        UserCleanTrailCacheValue value;
        key.lSId=lUId;
        if(CommHashMap::getInstance()->getHashMap(gUserCleanTrailCache,key,value)>=0 && now-value.iTimespan<CACHE_TIMEOUT)
        {
            iCleanTrailTime=value.iCleanTrailTime;
            iRet=0;
        }
        else if(tDbHandle.getCleanTrailTime(lUId,iCleanTrailTime)>=0)
        {
            value.iTimespan=now;
            value.iCleanTrailTime=iCleanTrailTime;
            CommHashMap::getInstance()->addHashMap(gUserCleanTrailCache,key,value);
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;

}

int PostCache::getUserFavor(taf::Int64 lUId,map<taf::Int64,taf::Int32> &mpUserFavor)
{
    int iRet=-1;

    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();
        PostCacheKey key;
        UserFavorCacheValue value;
        key.lSId=lUId;
        if(CommHashMap::getInstance()->getHashMap(gUserFavorCache,key,value)>=0 && now-value.iTimespan<CACHE_TIMEOUT)
        {
            mpUserFavor=std::move(value.mpFavorList);
            iRet=0;
        }
        else if(tDbHandle.getUserFavor(lUId,value.mpFavorList)>=0)
        {
            value.iTimespan=now;
            CommHashMap::getInstance()->addHashMap(gUserFavorCache,key,value);
            mpUserFavor=std::move(value.mpFavorList);
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int PostCache::getUserPostBatch(taf::Int64 lUId,const vector<taf::Int64> &vInSecret,vector<SecretInfo> &vOutSecret)
{
    int iRet=-1;
    map<taf::Int64,taf::Int32> mpUserFavor;
    if(getUserFavor(lUId,mpUserFavor)>=0)
    {
        vOutSecret.reserve(vInSecret.size());
        ostringstream ss;
        for(vector<taf::Int64>::const_iterator it=vInSecret.begin();it!=vInSecret.end();++it)
        {
            int skipReason=0;
            SecretInfo tSecretInfo;
            if(getPost(*it,tSecretInfo)>=0)
            {
                if(tSecretInfo.iReportNum>=3 && tSecretInfo.lUId!=lUId)
                {
                    skipReason=SKIP_REASON_REPORT;
                }
                else if(tSecretInfo.isRemove)
                {
                    skipReason=SKIP_REASON_REMOVE;
                }
                else
                {
                    int iCleanTrailTime;
                    if(getCleanTrailTime(tSecretInfo.lUId,iCleanTrailTime)>=0 && iCleanTrailTime<getTimespanFromID(tSecretInfo.lSId))
                    {
                        tSecretInfo.iType=QUAN::RECMD;
                        map<taf::Int64,taf::Int32>::iterator opit=mpUserFavor.find(tSecretInfo.lSId);
                        if(opit!=mpUserFavor.end())
                        {
                            if(opit->second==0)
                            {
                                tSecretInfo.isFavor=true;
                            }
                            else
                            {
                                skipReason=SKIP_REASON_REMOVE;
                            }
                        }
                    }
                    else
                    {
                        skipReason=SKIP_REASON_CLEAN;
                    }
                }
            }
            else
            {
                skipReason=SKIP_REASON_NOEXISTS;
            }

            if(skipReason)
            {
                ss<<"{"<<*it<<"=># "<<skipReason<<" #}";
            }
            else
            {
                vOutSecret.push_back(tSecretInfo);
                ss<<"{"<<*it<<"=>"<<JCETOSTR(tSecretInfo)<<"}";
            }
        }
        LOG->debug()<<__FUNCTION__<<"|"<<lUId<<"|"<<ss.str()<<endl;
        iRet=0;
    }
    return iRet;
}

int PostCache::getUserNotifyBatch(taf::Int64 lUId,const vector<QUAN::NotifyPost> &vInSecret,vector<SecretInfo> &vOutSecret)
{
    int iRet=-1;
    map<taf::Int64,taf::Int32> mpUserFavor;
    if(getUserFavor(lUId,mpUserFavor)>=0)
    {
        vOutSecret.reserve(vInSecret.size());
        ostringstream ss;
        for(vector<NotifyPost>::const_iterator it=vInSecret.begin();it!=vInSecret.end();++it)
        {
            int skipReason=0;
            SecretInfo tSecretInfo;
            if(getPost(it->lSId,tSecretInfo)>=0)
            {
                if(tSecretInfo.iReportNum>3 && tSecretInfo.lUId!=lUId)
                {
                    skipReason=SKIP_REASON_REPORT;
                }
                else if(tSecretInfo.isRemove)
                {
                    skipReason=SKIP_REASON_REMOVE;
                }
                else
                {
                    int iCleanTrailTime;
                    if(getCleanTrailTime(tSecretInfo.lUId,iCleanTrailTime)>=0 && iCleanTrailTime<getTimespanFromID(tSecretInfo.lSId))
                    {
                        map<taf::Int64,taf::Int32>::iterator opit=mpUserFavor.find(tSecretInfo.lSId);
                        if(opit!=mpUserFavor.end())
                        {
                            if(opit->second==0)
                            {
                                tSecretInfo.isFavor=true;
                            }
                            else if(opit->second==2)
                            {
                                skipReason=SKIP_REASON_REMOVE;
                            }
                        }
                        tSecretInfo.iType=QUAN::RECMD;
                        if(skipReason==0 && it->iNotifyType!= FRD_RECMD && !checkPostRelation(lUId,tSecretInfo))
                        {
                            skipReason=SKIP_REASON_NORELAT;
                        }
                    }
                    else
                    {
                        skipReason=SKIP_REASON_CLEAN;
                    }
                }

                if(skipReason)
                {
                    ss<<"{"<<it->lSId<<"["<<it->iNotifyType<<"]"<<"=># "<<skipReason<<" #}";
                }
                else
                {
                    vOutSecret.push_back(tSecretInfo);
                    ss<<"{"<<it->lSId<<"["<<it->iNotifyType<<"]"<<"=>"<<JCETOSTR(tSecretInfo)<<"}";
                }
            }
        }
        LOG->debug()<<__FUNCTION__<<"|"<<lUId<<"|"<<ss.str()<<endl;
        iRet=0;
    }
    return iRet;
}

int PostCache::getUserComments(taf::Int64 lUId,taf::Int64 lSId,QUAN::SecretInfo& tSecret,taf::Int32 &lFavorNum,taf::Int32 &lCmtNum,vector<ContentInfo> &vContents)
{
    int iRet=-1;
    map<taf::Int64,taf::Int32> mpUserFavor;
    if(getUserFavor(lUId,mpUserFavor)>=0)
    {
        vector<ContentInfo> vContentInfo;
        if(getPost(lSId,tSecret)>=0 && tSecret.isRemove==0 && getComments(lSId,vContentInfo)>=0 )
        {
            int iPortrait=1;
            map<taf::Int64,int> mpPortrait;
            lFavorNum=tSecret.iFavorNum;
            lCmtNum=tSecret.iCmtNum;
            if(tSecret.lUId==lUId)
            {
                tSecret.iType=ME;
            }

            map<taf::Int64,taf::Int32>::iterator opit= mpUserFavor.find(lSId);
            if(opit!=mpUserFavor.end())
            {
                if(opit->second==0)
                {
                    tSecret.isFavor=true;
                }
                else
                {
                    tSecret.isRemove=true;
                }
            }
            // 2.1
            if(!tSecret.isRemove)
            {
                vContents.reserve(vContents.size());
                for(vector<ContentInfo>::iterator it=vContentInfo.begin();it!=vContentInfo.end();++it)
                {

                    map<taf::Int64,taf::Int32>::iterator opit=mpUserFavor.find(it->lCId);
                    if(opit!=mpUserFavor.end())
                    {
                        if(opit->second==0)
                        {
                            it->isFavor=true;
                        }
                        else
                        {
                            it->isRemove=true;
                        }
                    }
                    // 2.1
                    if(it->isRemove == 1)
                    {
                        int lCId=it->lCId;
                        int lUId=it->lUId;
                        string Time=it->Time;
                        it->resetDefautlt();
                        it->lSId=lSId;
                        it->lCId=lCId;
                        it->lUId=lUId;
                        it->Time=Time;
                        it->sContent=DELETE_COMMENT_HINT;
                        it->isRemove=true;
                    }
                    it->bIsMe=(it->lUId==lUId);
                    if(it->lUId==tSecret.lUId)
                    {
                        it->isFirst=true;
                        it->iPortrait=0;
                    }
                    else
                    {
                        map<taf::Int64,int>::iterator pit=mpPortrait.find(it->lUId);
                        if(pit==mpPortrait.end())
                        {
                            it->iPortrait=iPortrait++;
                            mpPortrait.insert(pit,make_pair(it->lUId,it->iPortrait));
                            if((iPortrait+1)%31==0) //CLIENT BUG FIX 
                            {
                                ++iPortrait;
                            }
                        }
                        else
                        {
                            it->iPortrait=pit->second;
                        }
                    }

                    // 2.1
                    if (it->isRemove != 2)
                    {
                        vContents.push_back(*it);
                    }
                }

                if(vContentInfo.size()!=(size_t)tSecret.iCmtNum)    //自动修正回贴数
                {
                    LOG->error()<<__FUNCTION__<<"|AutoFix CmtNum:"<<tSecret.iCmtNum<<"=>"<<vContents.size()<<endl;
                    tSecret.iCmtNum=vContents.size();
                    tDbHandle.updatePosts(lSId,"cmt_num",TC_Common::tostr(vContents.size()));
                    updatePost(lSId,tSecret);
                }

                iRet=0;
            }
            else
            {
                iRet=ERR_NOEXISTS;
            }
        }
        else
        {
            iRet=ERR_NOEXISTS;
            if(tSecret.isRemove==false)
            {
                LOG->error()<<__FUNCTION__<<"|"<<tSecret.isRemove<<"| GET COMMENT FAILED"<<endl; 
            }
        }
    }
    return iRet;
}

int PostCache::getPostRelation(taf::Int64 lSId,vector<taf::Int64> &vUId)
{
    int iRet=-1;
    SecretInfo tSecretInfo;
    vector<ContentInfo> vContents;
    if(getPost(lSId,tSecretInfo)>=0 && tSecretInfo.isRemove==0)
    {
        if(getComments(lSId,vContents)>=0)
        {
            vUId.push_back(tSecretInfo.lUId);
            for(vector<ContentInfo>::iterator it=vContents.begin();it!=vContents.end();++it)
            {
                if(it->isRemove==true)
                {
                    continue;
                }
                int iCleanTrailTime;
                if(getCleanTrailTime(it->lUId,iCleanTrailTime)>=0 && iCleanTrailTime>=getTimespanFromID(it->lCId))
                {
                    continue;
                }
                vUId.push_back(it->lUId);
            }
            sort(vUId.begin(),vUId.end());
            vUId.erase(unique(vUId.begin(),vUId.end()),vUId.end());
            iRet=0;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|"<<"Get Comment Failed"<<endl;
        }
    }
    else
    {
        LOG->error()<<__FUNCTION__<<"|"<<"Get Post Failed"<<endl;
    }
    return iRet;
}

bool PostCache::checkPostRelation(taf::Int64 lUId,SecretInfo &tSecretInfo)
{
    if(lUId==tSecretInfo.lUId)
    {
        return true;
    }
    vector<ContentInfo> vContents;
    int iCleanTrailTime;
    if(getComments(tSecretInfo.lSId,vContents)>=0 && getCleanTrailTime(lUId,iCleanTrailTime)>=0)
    {
        for(vector<ContentInfo>::iterator it=vContents.begin();it!=vContents.end();++it)
        {
            if(it->lUId==lUId && iCleanTrailTime<getTimespanFromID(it->lCId) && it->isRemove==false)
            {
                return true;
            }
        }
    }
    return false;
}

int PostCache::clearPostCache(taf::Int64 lSId)
{
    PostCacheKey key;
    key.lSId=lSId;
    return CommHashMap::getInstance()->eraseHashMap(gPostCache,key);
}
int PostCache::clearCommentCache(taf::Int64 lCId)
{
    PostCacheKey key;
    key.lSId=lCId;
    return CommHashMap::getInstance()->eraseHashMap(gCommentCache,key);
}
int PostCache::clearUserOpCache(taf::Int64 lUId)
{
    PostCacheKey key;
    key.lSId=lUId;
    return CommHashMap::getInstance()->eraseHashMap(gUserFavorCache,key);
}
int PostCache::clearUserCleanTrailCache(taf::Int64 lUId)
{
    PostCacheKey key;
    key.lSId=lUId;
    return CommHashMap::getInstance()->eraseHashMap(gUserCleanTrailCache,key);
}
