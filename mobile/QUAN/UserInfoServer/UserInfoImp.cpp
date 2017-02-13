#include <iterator>
#include "CommHashMap.h"
#include "UserInfoImp.h"
#include "servant/Application.h"
#include "UserInfoServer.h"

using namespace std;


#define CACHE_TIMEOUT (60)



class UpdatePostTimeAsync:public AsyncWork
{
    public:
        UpdatePostTimeAsync(taf::Int64 lId,int iTimespan,const vector<taf::Int64> &vFollowers):lId(lId),iTimespan(iTimespan),vFollowers(vFollowers)
        {
        }
        void run(DbHandle &tDbHandle)
        {
            try
            {
                CacheKey key; key.lId=lId;
                UserInfoCacheValue userInfoValue;
                if(CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,key,userInfoValue)!=0) throw std::runtime_error("CACHE ERROR");
                userInfoValue.iPostTime=iTimespan;
                if(CommHashMap::getInstance()->addHashMap(g_app.mUserInfoHashMap,key,userInfoValue)!=0) throw std::runtime_error("CACHE ERROR");

                for(vector<taf::Int64>::iterator it=vFollowers.begin();it!=vFollowers.end();++it)
                {
                    CacheKey followerKey;
                    FriendListCacheValue friendList;
                    followerKey.lId=*it;
                    if(CommHashMap::getInstance()->getHashMap(g_app.mActiveFriendsHashMap,followerKey,friendList)!=0) throw std::runtime_error("CACHE ERROR"); 
                    for(vector<FriendsList>::iterator fit=friendList.vPostFriends.begin();fit!=friendList.vPostFriends.end();++fit)
                    {
                        if(fit->lUId==*it)
                        {
                            fit->iTimespan=iTimespan;
                        }
                    }
                }
                int iRet=tDbHandle.updatePostTime(lId,iTimespan);
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|"<<iTimespan<<"|"<<iRet<<endl;
                return;
            }
            catch(exception &ex)
            {
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|"<<iTimespan<<"|ERROR"<<endl;
            }
            
        }
    private:
        taf::Int64 lId;
        int iTimespan;
        vector<taf::Int64> vFollowers;

};

class LoadActiveFollowerAsync:public AsyncWork
{
    public:
        LoadActiveFollowerAsync(taf::Int64 lId,int  MaxFollowerCount)
            :lId(lId),MaxFollowerCount(MaxFollowerCount)
        {
        }
    private:
        void run(DbHandle &tDbHandle)
        {
            try
            {
                CacheKey key;
                key.lId=lId;
                FollowerListCacheValue value;
                vector<FriendsList> vFriends;
                if(tDbHandle.getFollowersListFromDb(lId,QUAN::ALL,vFriends)>=0)
                {
                    value.vFollowers.reserve(vFriends.size());
                    vector<FriendsList> followerInDegree;
                    for(vector<FriendsList>::iterator it=vFriends.begin();it!=vFriends.end() ; ++it)
                    {
                        FriendsList followerValue;
                        followerValue=*it;
                        {
                            CacheKey followerKey;
                            followerKey.lId=it->lUId;
                            UserInfoCacheValue userCache;
                            CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,followerKey,userCache);
                            if(userCache.iVisitTime>followerValue.iTimespan) followerValue.iTimespan=userCache.iVisitTime;
                        }
                        if(it->iRelat==1)
                        {
                            value.vFollowers.push_back(followerValue);
                        }
                        else
                        {
                            followerInDegree.push_back(followerValue);
                        }
                    }
                    if(value.vFollowers.size()<(size_t)MaxFollowerCount)
                    {
                        sort(followerInDegree.rbegin(),followerInDegree.rend());
                        int degree2Count=MaxFollowerCount-value.vFollowers.size();
                        copy(followerInDegree.begin(),min(followerInDegree.end(),followerInDegree.begin()+degree2Count),back_inserter(value.vFollowers));
                    }


                    value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                    CommHashMap::getInstance()->addHashMap(g_app.mActiveFollowersHashMap,key,value);
                    FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<endl;
                }
                else
                {
                    FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|FAIL"<<endl;
                }
            }
            catch(exception &ex)
            {
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|ERROR"<<endl;
            }
        }
        taf::Int64 lId;
        int MaxFollowerCount;
};

class LoadActiveFriendAsync:public AsyncWork
{
    public:
        LoadActiveFriendAsync (taf::Int64 lId,int  MaxFriendCount)
            :lId(lId),MaxFriendCount(MaxFriendCount)
        {
        }
    private:
        void run(DbHandle &tDbHandle)
        {
            try
            {
                CacheKey key;
                key.lId=lId;
                FriendListCacheValue value;
                vector<FriendsListDb> vFriends;
                if(tDbHandle.getFriendsListFromDb(lId,QUAN::ALL,vFriends)>=0)
                {
                    value.vPostFriends.reserve(vFriends.size());
                    value.vFavorFriends.reserve(vFriends.size());
                    vector<FriendsList> postfriendInDegree;
                    vector<FriendsList> favorfriendInDegree;
                    for(vector<FriendsListDb>::iterator it=vFriends.begin();it!=vFriends.end(); ++it)
                    {
                        FriendsList postfriendValue,favorfriendValue;
                        favorfriendValue.lUId=postfriendValue.lUId=it->lUId;
                        favorfriendValue.iRelat=postfriendValue.iRelat=it->iRelat;
                        favorfriendValue.iTimespan=it->iFavorTime;
                        postfriendValue.iTimespan=it->iPostTime;
                        {
                            CacheKey friendKey;
                            friendKey.lId=it->lUId;
                            UserInfoCacheValue userCache;
                            CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,friendKey,userCache);
                            if(userCache.iPostTime>postfriendValue.iTimespan) postfriendValue.iTimespan=userCache.iPostTime;
                            if(userCache.iFavorTime>favorfriendValue.iTimespan) favorfriendValue.iTimespan=userCache.iFavorTime;
                        }
                        if(it->iRelat==1)
                        {
                            value.vPostFriends.push_back(postfriendValue);
                            value.vFavorFriends.push_back(favorfriendValue);
                        }
                        else
                        {
                            postfriendInDegree.push_back(postfriendValue);
                            favorfriendInDegree.push_back(favorfriendValue);
                        }
                    }
                    if(value.vPostFriends.size()<(size_t)MaxFriendCount)
                    {
                        sort(postfriendInDegree.rbegin(),postfriendInDegree.rend());
                        sort(favorfriendInDegree.rbegin(),favorfriendInDegree.rend());
                        int degree2Count=MaxFriendCount-value.vPostFriends.size();
                        copy(postfriendInDegree.begin(),min(postfriendInDegree.end(),postfriendInDegree.begin()+degree2Count),back_inserter(value.vPostFriends));
                        copy(favorfriendInDegree.begin(),min(favorfriendInDegree.end(),favorfriendInDegree.begin()+degree2Count),back_inserter(value.vFavorFriends));
                    }
                    sort(value.vPostFriends.rbegin(),value.vPostFriends.rend());
                    sort(value.vFavorFriends.rbegin(),value.vFavorFriends.rend());

                    value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                    CommHashMap::getInstance()->addHashMap(g_app.mActiveFriendsHashMap,key,value);
                    FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<endl;
                }
                else
                {
                    FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|FAIL"<<endl;
                }
            }
            catch(exception &ex)
            {
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|ERROR"<<endl;
            }
        }
        taf::Int64 lId;
        int MaxFriendCount;
};

class SaveFriendListCacheAsync:public AsyncWork
{
    public:
        SaveFriendListCacheAsync(taf::Int64 lId,FriendListCacheValue& value,int MaxFriendCount):lId(lId),value(value),MaxFriendCount(MaxFriendCount)
        {
        }
        void run(DbHandle &tDbHandle)
        {
           tDbHandle.SaveFriendListCache(lId,value) ;
           TC_AutoPtr<LoadActiveFriendAsync> work=new LoadActiveFriendAsync(lId,MaxFriendCount);
           g_app.mAsyncMaster.doAsyncWork(work);
           FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<endl;
        }
    private:
        taf::Int64 lId;
        FriendListCacheValue value;
        int MaxFriendCount;
};

class UpdateFavorTimeAsync:public AsyncWork
{
    public:
        UpdateFavorTimeAsync(taf::Int64 lId,int iTimespan,const vector<taf::Int64> &vFollowers)
            :lId(lId),iTimespan(iTimespan),vFollowers(vFollowers)
        {
        }
        void run(DbHandle &tDbHandle)
        {
            try
            {
                CacheKey key; key.lId=lId;
                UserInfoCacheValue userInfoValue;
                if(CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,key,userInfoValue)!=0) throw std::runtime_error("CACHE ERROR");
                userInfoValue.iFavorTime=iTimespan;
                if(CommHashMap::getInstance()->addHashMap(g_app.mUserInfoHashMap,key,userInfoValue)!=0) throw std::runtime_error("CACHE ERROR");

                for(vector<taf::Int64>::iterator it=vFollowers.begin();it!=vFollowers.end();++it)
                {
                    CacheKey followerKey;
                    FriendListCacheValue friendList;
                    followerKey.lId=*it;
                    if(CommHashMap::getInstance()->getHashMap(g_app.mActiveFriendsHashMap,followerKey,friendList)!=0) throw std::runtime_error("CACHE ERROR"); 
                    for(vector<FriendsList>::iterator fit=friendList.vFavorFriends.begin();fit!=friendList.vFavorFriends.end();++fit)
                    {
                        if(fit->lUId==*it)
                        {
                            fit->iTimespan=iTimespan;
                        }
                    }
                }
                int iRet=tDbHandle.updateFavorTime(lId,iTimespan);
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|"<<iTimespan<<"|"<<iRet<<endl;
                return;
            }
            catch(exception &ex)
            {
                FDLOG("ASYNC_DB")<<__PRETTY_FUNCTION__<<"|"<<lId<<"|"<<iTimespan<<"|"<<"ERROR"<<endl;
            }
            
        }
    private:
        taf::Int64 lId;
        int iTimespan;
        vector<taf::Int64> vFollowers;
};
//////////////////////////////////////////////////////
void UserInfoImp::initialize()
{
    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "UserInfoServer.conf");
    MaxFollowerCount=TC_Common::strto<int>(tConf.get("/main/<MaxFollowerCount>","100"));
    MaxFriendCount=TC_Common::strto<int>(tConf.get("/main/<MaxFriendCount>","100"));
    tokenServerHost=tConf.get("/main/<tokenserverhost>");
    tokenServerPort=TC_Common::strto<int>(tConf.get("/main/<tokenserverport>","6379"));
    isVerifyToken=TC_Common::strto<int>(tConf.get("/main/<isverifytoken>","1"))>0;
	//initialize servant here:
	//...
}

//////////////////////////////////////////////////////
void UserInfoImp::destroy()
{
	//destroy servant here:
	//...
}

//////////////////////////////////////////////////////
// 对外接口: 注册
//////////////////////////////////////////////////////
taf::Int32 UserInfoImp::postUserEnroll(const QUAN::UserEnrollReq& tReq, QUAN::UserEnrollRsp& tRsp, taf::JceCurrentPtr current)
{

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 对外接口: 登陆
//////////////////////////////////////////////////////
taf::Int32 UserInfoImp::postUserLogin(const QUAN::UserLoginReq& tReq, QUAN::UserLoginRsp& tRsp, taf::JceCurrentPtr current)
{

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 对外接口: 联系人上传
//////////////////////////////////////////////////////
taf::Int32 UserInfoImp::postAddressList(const QUAN::PostAddressListReq& tReq, QUAN::PostAddressListRsp& tRsp, taf::JceCurrentPtr current)
{

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 对外接口: 获取好友列表
//////////////////////////////////////////////////////
taf::Int32 UserInfoImp::getFriendsList(const QUAN::FriendsListReq& tReq, QUAN::FriendsListRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;
    // 输出参数
    ostringstream os1; tReq.displaySimple(os1);
    LOG->debug() << __FUNCTION__<< " begin "<<os1.str()<<endl;

    PROC_BEGIN
    __TRY__

    iRet = _tDbHandle.getFriendsList(tReq.tUserId.lUId, tReq.iRelat, tRsp.vFriends);
    if (iRet)
    {
        LOG->debug() << __FUNCTION__<< " get friends failure "<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " get friends success "<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"###"<<JCETOSTR(tRsp)<<endl;

    return iRet; 
}

//////////////////////////////////////////////////////
// 对外接口: 验证Token
//////////////////////////////////////////////////////

int UserInfoImp::connectReddis()
{
    if(!tokenServerHost.empty())
    {
        try
        {
            delete rclient;
            rclient= new redis::client(tokenServerHost,tokenServerPort);
            return 0;
        }
        catch(exception& e)
        {
            LOG->error()<<"RedisConnectError:"<<e.what()<<endl;
            rclient=NULL;
            return -1;
        }
    }
    else
    {
        LOG->error()<<"RedisConnectError:"<<"Server Host Empty"<<endl;
        return -1;
    }
}


taf::Int32 UserInfoImp::verifyToken(const QUAN::UserId & tId,taf::Int64 &lUId,taf::JceCurrentPtr current)
{
    int iRet=-1;
    if(isVerifyToken)
    {
        try
        {
            if(rclient || connectReddis()>=0)
            {
                string ret=rclient->get(tId.sToken);
                if(!ret.empty())
                {
                    rclient->expire(tId.sToken,30*24*3600);
                }
                lUId=TC_Common::strto<long>(ret);
                iRet=lUId>0?0:-1;
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|RedisError:"<<"Connection Error"<<endl;
            }
        }
        catch(exception &e)
        {
            if(connectReddis()<0) return -1;
            try
            {
                string ret=rclient->get(tId.sToken);
                if(!ret.empty())
                {
                    rclient->expire(tId.sToken,30*24*3600);
                }
                lUId=TC_Common::strto<long>(ret);
                iRet=lUId>0?0:-1;
            }
            catch(exception &e)
            {
                LOG->error()<<__FUNCTION__<<"|RedisError:"<<e.what()<<endl;
            }
        }
    }
    else
    {
        iRet=0;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<JCETOSTR(tId)<<"=>"<<lUId<<"|"<<endl;
    return iRet;
}

taf::Int32 UserInfoImp::updatePostTime(const QUAN::UpdatePostTimeReq & tReq,taf::JceCurrentPtr current)
{
    int iRet=-1;
    int queueSize=-1;
    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();

        TC_AutoPtr<UpdatePostTimeAsync> work=new UpdatePostTimeAsync(tReq.lId,now,tReq.vFollowerId);
        queueSize=g_app.mAsyncMaster.doAsyncWork(work);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<tReq.lId<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.lId<<"|"<<JCETOSTR(tReq)<<"|queue:"<<queueSize<<endl;
    return iRet;
}


taf::Int32 UserInfoImp::updateFavorTime(const QUAN::UpdateFavorTimeReq & tReq,taf::JceCurrentPtr current)
{
    int iRet=-1;
    int queueSize=-1;
    try
    {
        int now=TC_TimeProvider::getInstance()->getNow();

#if 0
        CacheKey key;
        key.lId=tReq.lId;
        UserInfoCacheValue userInfoValue;
        CommHashMap::getInstance()->getInstance()->getHashMap(g_app.mUserInfoHashMap,key,userInfoValue);
        userInfoValue.iFavorTime=now;
        CommHashMap::getInstance()->getInstance()->addHashMap(g_app.mUserInfoHashMap,key,userInfoValue);

        FollowerListCacheValue value;

        CommHashMap::getInstance()->getInstance()->getHashMap(g_app.mActiveFollowersHashMap,key,value);
        for(vector<FriendsListCache>::iterator it=value.vFollowers.begin();it!=value.vFollowers.end();++it)
        {
            CacheKey followerKey;
            FriendListCacheValue friendList;
            followerKey.lId=it->tFriends.lUId;
            CommHashMap::getInstance()->getHashMap(g_app.mActiveFriendsHashMap,followerKey,friendList);
            for(vector<FriendsListCache>::iterator fit=friendList.vFavorFriends.begin();fit!=friendList.vFavorFriends.end();++fit)
            {
                if(fit->tFriends.lUId==it->tFriends.lUId)
                {
                    fit->iTimespan=now;
                }
            }
        }
        iRet=_tDbHandle.updateFavorTime(tReq.lId,now);
#endif
        TC_AutoPtr<UpdateFavorTimeAsync> work=new UpdateFavorTimeAsync(tReq.lId,now,tReq.vFollowerId);
        queueSize=g_app.mAsyncMaster.doAsyncWork(work);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<tReq.lId<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.lId<<"|"<<JCETOSTR(tReq)<<"|queue:"<<queueSize<<endl;
    return iRet;
}


taf::Int32 UserInfoImp::getActiveFriends(const QUAN::GetActiveFriendsReq & tReq,QUAN::GetActiveFriendsRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CacheKey key;
        key.lId=tReq.lId;
        {
            UserInfoCacheValue value;
            CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,key,value);
            if(value.iRegistTime<=0)
            {
                _tDbHandle.loadUserInfo(tReq.lId,value);
                if(value.iRegistTime>0)
                {
                    CommHashMap::getInstance()->addHashMap(g_app.mUserInfoHashMap,key,value);
                    tRsp.iRegistTime=value.iRegistTime;
                }
            }
            else
            {
                tRsp.iRegistTime=value.iRegistTime;
            }
        }

        FriendListCacheValue value;
        iRet=CommHashMap::getInstance()->getHashMap(g_app.mActiveFriendsHashMap,key,value);
        if(iRet==0)
        {
            int now = TC_TimeProvider::getInstance()->getNow();
            if(value.iTimespan<=0 || tReq.bLoadFromDb)
            {
                vector<FriendsListDb> vFriends;
                if(_tDbHandle.getFriendsListFromDb(tReq.lId,QUAN::ALL,vFriends)>=0)
                {
                    value.vPostFriends.reserve(vFriends.size());
                    value.vFavorFriends.reserve(vFriends.size());
                    vector<FriendsList> postfriendInDegree;
                    vector<FriendsList> favorfriendInDegree;
                    for(vector<FriendsListDb>::iterator it=vFriends.begin();it!=vFriends.end(); ++it)
                    {
                        FriendsList postfriendValue,favorfriendValue;
                        favorfriendValue.lUId=postfriendValue.lUId=it->lUId;
                        favorfriendValue.iRelat=postfriendValue.iRelat=it->iRelat;
                        favorfriendValue.iTimespan=it->iFavorTime;
                        postfriendValue.iTimespan=it->iPostTime;
                        {
                            CacheKey friendKey;
                            friendKey.lId=it->lUId;
                            UserInfoCacheValue userCache;
                            CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,friendKey,userCache);
                            if(userCache.iPostTime>postfriendValue.iTimespan) postfriendValue.iTimespan=userCache.iPostTime;
                            if(userCache.iFavorTime>favorfriendValue.iTimespan) favorfriendValue.iTimespan=userCache.iFavorTime;
                        }
                        if(it->iRelat==1)
                        {
                            value.vPostFriends.push_back(postfriendValue);
                            value.vFavorFriends.push_back(favorfriendValue);
                        }
                        else
                        {
                            postfriendInDegree.push_back(postfriendValue);
                            favorfriendInDegree.push_back(favorfriendValue);
                        }
                    }
                    if(value.vPostFriends.size()<(size_t)MaxFriendCount)
                    {
                        sort(postfriendInDegree.rbegin(),postfriendInDegree.rend());
                        sort(favorfriendInDegree.rbegin(),favorfriendInDegree.rend());
                        int degree2Count=MaxFriendCount-value.vPostFriends.size();

                        copy(postfriendInDegree.begin(),min(postfriendInDegree.end(),postfriendInDegree.begin()+degree2Count),back_inserter(value.vPostFriends));
                        copy(favorfriendInDegree.begin(),min(favorfriendInDegree.end(),favorfriendInDegree.begin()+degree2Count),back_inserter(value.vFavorFriends));
                    }
                    sort(value.vPostFriends.rbegin(),value.vPostFriends.rend());
                    sort(value.vFavorFriends.rbegin(),value.vFavorFriends.rend());

                    value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                    CommHashMap::getInstance()->addHashMap(g_app.mActiveFriendsHashMap,key,value);
                    LOG->debug()<<__FUNCTION__<<"|"<<"LoadFromDb|"<<tReq.lId<<endl;
                }
                else
                {
                    LOG->error()<<__FUNCTION__<<"|"<<"LoadFromDbFail|"<<tReq.lId<<endl;
                }

            }
            else if(now-value.iTimespan>CACHE_TIMEOUT)
            {
                TC_AutoPtr<SaveFriendListCacheAsync> work=new SaveFriendListCacheAsync(tReq.lId,value,MaxFriendCount);
                g_app.mAsyncMaster.doAsyncWork(work);
                value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                CommHashMap::getInstance()->addHashMap(g_app.mActiveFriendsHashMap,key,value);
                LOG->debug()<<__FUNCTION__<<"|"<<"SaveToDb|"<<tReq.lId<<endl;
            }
            else
            {
                LOG->debug()<<__FUNCTION__<<"|"<<"LoadFromCache|"<<tReq.lId<<endl;
            }
            if(value.iTimespan>0)
            {
                tRsp.vPostFriends=value.vPostFriends;
                tRsp.vFavorFriends=value.vFavorFriends;
            }
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<tReq.lId<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.lId<<"|"<<tReq.bLoadFromDb<<"|"<<JCETOSTR(tRsp)<<endl;
    return iRet;
}

taf::Int32 UserInfoImp::getActiveFollowers(const QUAN::GetActiveFollowersReq & tReq,QUAN::GetActiveFollowersRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CacheKey key;
        FollowerListCacheValue value;
        key.lId=tReq.lId;
        iRet=CommHashMap::getInstance()->getHashMap(g_app.mActiveFollowersHashMap,key,value);

        if(iRet==0)
        {
            int now = TC_TimeProvider::getInstance()->getNow();
            if(value.iTimespan<=0)
            {
                vector<FriendsList> vFriends;
                if(_tDbHandle.getFollowersListFromDb(tReq.lId,QUAN::ALL,vFriends)>=0)
                {
                    value.vFollowers.reserve(vFriends.size());
                    vector<FriendsList> followerInDegree;
                    for(vector<FriendsList>::iterator it=vFriends.begin();it!=vFriends.end() ; ++it)
                    {
                        FriendsList followerValue;
                        followerValue=*it;
                        {
                            CacheKey followerKey;
                            followerKey.lId=it->lUId;
                            UserInfoCacheValue userCache;
                            CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,followerKey,userCache);
                            if(userCache.iVisitTime>followerValue.iTimespan) followerValue.iTimespan=userCache.iVisitTime;
                        }
                        if(it->iRelat==1)
                        {
                            value.vFollowers.push_back(followerValue);
                        }
                        else
                        {
                            followerInDegree.push_back(followerValue);
                        }
                    }
                    if(value.vFollowers.size()<(size_t)MaxFollowerCount)
                    {
                        sort(followerInDegree.rbegin(),followerInDegree.rend());
                        int degree2Count=MaxFollowerCount-value.vFollowers.size();
                        copy(followerInDegree.begin(),min(followerInDegree.end(),followerInDegree.begin()+degree2Count),back_inserter(value.vFollowers));
                    }

                    sort(value.vFollowers.rbegin(),value.vFollowers.rend());

                    value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                    CommHashMap::getInstance()->addHashMap(g_app.mActiveFollowersHashMap,key,value);
                }
                else
                {
                    LOG->error()<<__FUNCTION__<<"|DBERROR|"<<tReq.lId<<endl;
                }

            }
            else if(now - value.iTimespan>CACHE_TIMEOUT)
            {
                value.iTimespan=TC_TimeProvider::getInstance()->getNow();
                CommHashMap::getInstance()->addHashMap(g_app.mActiveFollowersHashMap,key,value);
                TC_AutoPtr<LoadActiveFollowerAsync> work=new LoadActiveFollowerAsync(tReq.lId,MaxFollowerCount);
                g_app.mAsyncMaster.doAsyncWork(work);
            }
            tRsp.vFollowers=value.vFollowers;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<tReq.lId<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.lId<<"|"<<JCETOSTR(tRsp)<<endl;
    return iRet;
}

taf::Int32 UserInfoImp::setActive(const QUAN::SetActiveReq & tReq,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CacheKey key;
        key.lId=tReq.lId;
        UserInfoCacheValue value;
        int now=TC_TimeProvider::getInstance()->getNow();
        CommHashMap::getInstance()->getHashMap(g_app.mUserInfoHashMap,key,value);
        if(value.iRegistTime<=0)
        {
            _tDbHandle.loadUserInfo(tReq.lId,value);
        }
        if(value.iVisitTime<now) 
        {
            value.iVisitTime=now;
        }
        CommHashMap::getInstance()->addHashMap(g_app.mUserInfoHashMap,key,value);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<tReq.lId<<endl;
    }
    
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.lId<<endl;
    return iRet;
}

taf::Int32 UserInfoImp::clearUserCache(taf::Int64 lUId,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CacheKey key;
        key.lId=lUId;
        CommHashMap::getInstance()->eraseHashMap(g_app.mUserInfoHashMap,key);
        CommHashMap::getInstance()->eraseHashMap(g_app.mActiveFriendsHashMap,key);
        CommHashMap::getInstance()->eraseHashMap(g_app.mActiveFollowersHashMap,key);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<lUId<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lUId<<endl;
    return iRet;
}
