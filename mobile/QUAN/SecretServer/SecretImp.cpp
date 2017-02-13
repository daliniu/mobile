#include "SecretImp.h"
#include "servant/Application.h"
#include <algorithm>

using namespace std;

//////////////////////////////////////////////////////
void SecretImp::initialize()
{
	//initialize servant here:
	//...

    // 用户信息句柄
    _pUserProxy = Application::getCommunicator()->stringToProxy<UserInfoPrx>("QUAN.UserInfoServer.UserInfoObj");

    // 信息提醒句柄
    _pMsgProxy = Application::getCommunicator()->stringToProxy<MsgPrx>("QUAN.MsgServer.MsgObj");

    // 获取配置信息
    TC_Config tConf;
    __TRY__
    tConf.parseFile(ServerConfig::BasePath + "SecretServer.conf");
    __CATCH__

    //最大缓存帖子数
    _uMaxSecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<MaxSecretsNum>", "500"));

    // 24小时系统推荐帖子个数
    _u24SecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<24SecretsNum>", "3"));

    // 72小时系统推荐帖子个数
    _u72SecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<72SecretsNum>", "60"));

    // 获取判断好友数量
    _uMaxFriendsNum = TC_Common::strto<size_t>(tConf.get("/main/<MaxFriendsNum>", "3"));

    // 获取好友点赞数
    _uMaxFrdsFvrNum = TC_Common::strto<size_t>(tConf.get("/main/<MaxFrdsFvrNum>", "3"));

    // 获取好友点赞数
    _uMinFrdsFvrNum = TC_Common::strto<size_t>(tConf.get("/main/<MinFrdsFvrNum>", "1"));

    // 获取系统引导帖子位置
    _uSysScrtLoc = TC_Common::strto<size_t>(tConf.get("/main/<SysScrtLoc>", "3"));

    // 获取活跃用户数量
    _uActiveFriends = TC_Common::strto<size_t>(tConf.get("/main/<ActiveFriends>", "20"));

    // 获取系统引导帖子位置
    _uTopN = TC_Common::strto<size_t>(tConf.get("/main/<topn>", "20"));

}

int SecretImp::checkUserId(const QUAN::UserId &tId,int *platform)
{
    if(tId.sUA.find("ios")!=string::npos)
    {   
        if(platform) *platform=1;
    }   
    else if(tId.sUA.find("adr")!=string::npos)
    {   
        if(platform) *platform=2;
    }   
    else
    {   
        if(platform) *platform=2;
    }   
    return SUCCESS;
}


//////////////////////////////////////////////////////
void SecretImp::destroy()
{
	//destroy servant here:
	//...
}

//////////////////////////////////////////////////////
// 处理点赞关系移除关系
//////////////////////////////////////////////////////
taf::Int32 SecretImp::dealWithFavorAndRemove(UserId const& tUserId, size_t iFriends, Int32& iLockCount, vector<SecretInfo>& vSecrets)
{
    int iRet = FAILURE;

    LOG->debug() <<endl;


    // 获取用户点赞关系
    SecretRelatMap   tFavorScrts;
    iRet = _tDbHandle.getUserFavorSecrets(tUserId.lUId, tFavorScrts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " | "<<tUserId.lUId<<" get User Favor Secrets failure! "<<endl;
        return iRet;
    }
    LOG->info() << __FUNCTION__<< "get User Favor Secrets success! "<<endl;

    // 获取用户移除关系
    SecretRelatMap   tRemoveScrts;
    iRet = _tDbHandle.getUserRemoveSecrets(tUserId.lUId, tRemoveScrts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "get User Remove Secrets failure! "<<endl;
        return iRet;
    }
    LOG->info() << __FUNCTION__<< " get User Remove Secrets success! "<<endl;

    for (ScrtIterator i = vSecrets.begin(); i != vSecrets.end(); ++i)
    {
        RelatIterator pos = tRemoveScrts.mSId.find(i->lSId);
        if (pos != tRemoveScrts.mSId.end())
        {
            i->isRemove = 1;
            continue;
        } 
        else
        {
            i->isRemove = 0;
        }

        pos = tFavorScrts.mSId.find(i->lSId);
        if (pos != tFavorScrts.mSId.end() && i->iFavorNum > 0)    // 点赞负数还有
        {
            i->isFavor = true;
        } 
        else
        {
            i->isFavor = false;
        }
    }

    vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), EqualIsRemove()), vSecrets.end());
   
    LOG->debug() <<endl;

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 合并帖子列表
//////////////////////////////////////////////////////
taf::Int32 SecretImp::mergeSecrets(UserId const& tUserId, int iLockCount, vector<SecretInfo>& vRmdScrts, vector<SecretInfo>& vFvrScrts, vector<SecretInfo>& vSecrets)
{
    LOG->debug() <<endl;

    // 复制
    copy(vFvrScrts.begin(), vFvrScrts.end(), back_inserter(vSecrets));
    copy(vRmdScrts.begin(), vRmdScrts.end(), back_inserter(vSecrets));
   

    LOG->debug() <<endl;

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 获取推荐
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getRecommendSecrets(UserId const& tUserId, vector<SecretInfo>& vSecrets)
{
    int iRet = FAILURE;
    LOG->debug() <<endl;

    LOG->debug() << __FUNCTION__<< " begin "<<endl;
    
    iRet = _tDbHandle.getRecommendSecrets(tUserId, _u24SecretsNum, _u72SecretsNum, vSecrets);

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    LOG->debug() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 取好友列表
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getFriendsList(UserId const& tUserId, Int64 lBeginId, vector<FriendsList>& vFrdsFavor, vector<FriendsList>& vFrds1, vector<FriendsList>& vFrdsAll)
{
    int iRet = FAILURE;

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    GetActiveFriendsReq   tFrdsReq;
    GetActiveFriendsRsp   tFrdsRsp;
    tFrdsReq.lId = tUserId.lUId;

    iRet = _pUserProxy->taf_hash(tUserId.lUId)->getActiveFriends(tFrdsReq, tFrdsRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "get Active FriendsList  1 + 2 failure !"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<  "get Active FriendsList  1 + 2 SUCCESS !"<<endl;

    // 转义好友度数
    for (vector<FriendsList>::iterator i = tFrdsRsp.vPostFriends.begin(); i!= tFrdsRsp.vPostFriends.end(); ++i)
    {
        if (i->iRelat > ONE)
            i->iRelat--;
    }

    // 转义好友度数
    for (vector<FriendsList>::iterator i = tFrdsRsp.vFavorFriends.begin(); i!= tFrdsRsp.vFavorFriends.end(); ++i)
    {
        if (i->iRelat > ONE)
            i->iRelat--;
    }

    vFrdsAll = tFrdsRsp.vPostFriends;
    vFrdsFavor = tFrdsRsp.vFavorFriends;

/*
    // 转义时间
    time_t iTime = decodeSecretId(lBeginId);
    // 如果返回朋友列表大于20个需按beginID取
    if (lBeginId != -1 && vFrdsAll.size() > _uActiveFriends)
    {
        vector<FriendsList>  vTmp;
        size_t               iCount = 0;
        for (vector<FriendsList>::iterator i = vFrdsAll.begin(); i != vFrdsAll.end(); ++i)
        {
            if ((i->iTimespan < iTime) && (iCount++ < _uActiveFriends))
            {
                vTmp.push_back(*i);
            } 
        }
        vFrdsAll.swap(vTmp); 

        vTmp.clear();
        iCount = 0;
        for (vector<FriendsList>::iterator i = vFrdsFavor.begin(); i != vFrdsFavor.end(); ++i)
        {
            if ((i->iTimespan < iTime) && (iCount++ < _uActiveFriends))
            {
                vTmp.push_back(*i);
            } 
        }
        vFrdsFavor.swap(vTmp); 
    }
*/
    if (vFrdsAll.size() != 0)
    {
        vector<FriendsList>::iterator iBegin = vFrdsAll.begin();
        vector<FriendsList>::iterator iEnd   = vFrdsAll.end();
 
        for (vector<FriendsList>::iterator i = iBegin; i != iEnd; ++i)
        {
            if (i->iRelat == FRIEND)
                vFrds1.push_back(*i);
        }
    }

    display(" Active Post  Friends List: ", vFrdsAll);
    display(" Active Favor Friends List: ", vFrdsFavor);

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    LOG->debug() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 获取帖子列表
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;
    int iMemRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    vector<FriendsList> vFrds1;              //一度好友
    vector<FriendsList> vFrdsAll;            // 所有好友
    vector<FriendsList> vFrdsFavor;          // 点赞活跃好友

    PROC_BEGIN
    __TRY__

    // 获取好友列表 
    iRet = getFriendsList(tReq.tUserId, tReq.tRange.lBeginId, vFrdsFavor, vFrds1, vFrdsAll);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getFriendsList failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " getFriendsList success! "<<endl;

    // 查询收帖箱是否有帖子
    iRet = _tDbHandle.getUserSecretListFromCache(tReq.tUserId.lUId, tReq.tRange.lBeginId, tReq.iCount, tRsp.vSecrets);
    if (iRet == SUCCESS)
    {
        LOG->debug() << __FUNCTION__<< " get UserSecretList From Cache SUCCESS! "<<endl;
        iMemRet = SUCCESS;
        /*
        if (tRsp.vSecrets.size() > (size_t)tReq.iCount)
            tRsp.lNextBeginId = tRsp.vSecrets[tReq.iCount - 1].lSId;
        else
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
        */
        tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
        display(" Cache return Accept: ", tRsp.vSecrets);

    }
    else if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get UserSecretList From Cache failure! "<<endl;
        break;
    }
    else
    {
        LOG->debug() << __FUNCTION__<< " get UserSecretList From DB ! "<<endl;

        // 收帖箱取不到数据从(拉cache)或者DB拉数据
        iRet = getPullSecretList(tReq, tRsp, vFrds1, vFrdsAll, vFrdsFavor);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " get Pull Secret List FAILURE! "<<endl;
            break;
        }

        display(" Pull List: ", tRsp.vSecrets);
        LOG->info() << __FUNCTION__<< " get Pull Secret List SUCCESS! "<<endl;

        // 将拉出来的帖子列表放入收帖箱
        if (iMemRet != SUCCESS)
        {
            iRet = _tDbHandle.setBatchSecretsToCache(tRsp.vSecrets, tReq.tUserId.lUId);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " Set Batch Secrets ToCache failure! "<<endl;
                break;
            }
            LOG->info() << __FUNCTION__<< " Set Batch Secrets ToCache success! "<<endl;
        }
    }

    // 重新排序分页
    iRet = uniqueSecretList(tReq, tRsp, vFrds1, vFrdsAll, vFrdsFavor);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " unique Secret List FAILURE! "<<endl;
        break;
    }

    // 限制长度
    if (tRsp.vSecrets.size() >= (size_t)tReq.iCount)
    {
        tRsp.vSecrets.resize(tReq.iCount);
        if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }

    // 编码混淆UserId
    encodeResponse(tRsp.vSecrets);

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " the Lock Count : "<<tRsp.iLockCount<<" NextBeginId : "<<tRsp.lNextBeginId<<endl;
    display(" Vector Secrets ", tRsp.vSecrets);
    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 获取帖子列表 -- 拉数据
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getPullSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, vector<FriendsList> const& vFrds1, vector<FriendsList> const& vFrdsAll, vector<FriendsList> const& vFrdsFavor)
{
    int iRet = FAILURE;

    // 输出参数
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    // 将用户加入好友列表
    vector<FriendsList> vMes;
    FriendsList tMe;
    tMe.lUId = tReq.tUserId.lUId;
    tMe.iRelat  = ME;
    vMes.push_back(tMe);
    ScrtVector   vSecretsFriends;

    // 获取帖子列表;  
    iRet = _tDbHandle.getAllUserSecrets(vMes, tReq.iCount, tReq.tRange.lBeginId, tRsp.lNextBeginId, tRsp.vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets MySelf failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Secrets MySelf success! "<<endl;

    display(" MySelf Vector Secrets : ", tRsp.vSecrets);

    // 如果返回朋友列表大于20个需按beginID取
    // 新逻辑
    LOG->debug() << __FUNCTION__<< " Friends Num "<<vFrdsAll.size()<<" _uActiveFriends: "<<_uActiveFriends<<endl;
    if (vFrdsAll.size() > _uMaxFriendsNum)
    {
        size_t iSum = vFrdsAll.size() / _uTopN + 1;
        
        LOG->debug() << __FUNCTION__<< " Friends Num "<<vFrdsAll.size()<<" _uActiveFriends: "<<_uActiveFriends<<" Sum "<<iSum<<" "<<_uTopN<<endl;
        for (size_t i = 0; i < iSum; i++)
        {
            vector<FriendsList>   vTmp;
            if (vFrdsAll.size() < _uTopN) 
            copy(vFrdsAll.begin(), vFrdsAll.end(), back_inserter(vTmp));
            else
            copy(vFrdsAll.begin(), vFrdsAll.begin() + (i+1) * _uTopN, back_inserter(vTmp));

            // 获取好友，好友的好友的帖子列表; 
            iRet = _tDbHandle.getAllUserSecrets(vTmp, tReq.iCount, tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecretsFriends);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " get Friends User Secrets  failure! "<<endl;
                break;
            }
            display(" Friends Vector Secrets : ", tRsp.vSecrets);
            LOG->info() << __FUNCTION__<< " get Friends User Secrets success! "<<endl;
            if (vFrdsAll.size() >= (i+1)*_uTopN)
            {
                if (vFrdsAll[(i+1)*_uTopN+1].iTimespan > 0)
                    vSecretsFriends.erase(remove_if(vSecretsFriends.begin(), vSecretsFriends.end(), LessTime(vFrdsAll[(i+1)*_uTopN+1].iTimespan)), vSecretsFriends.end());
                if (vSecretsFriends.size() >= (size_t)tReq.iCount)
                    break;
            }
        }
    }

    // 判断好友数量
    size_t iFrdsNum = vFrds1.size();
    if (iFrdsNum < _uMaxFriendsNum)
    {
        //查询朋友的帖子数;
        tRsp.iLockCount = vSecretsFriends.size();
        vSecretsFriends.clear();
 
        if (iFrdsNum == 0 || tRsp.iLockCount == 0)
        {
            // 如果好友数为0则默认设置为1
            tRsp.iLockCount = 1;
        }

        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
        // 解锁
        tRsp.iLockCount = 0;

        if (vSecretsFriends.size() != 0)
            copy(vSecretsFriends.begin(), vSecretsFriends.end(), back_inserter(tRsp.vSecrets));
    }

    // 排序 
    stable_sort(tRsp.vSecrets.begin(),  tRsp.vSecrets.end(),  GreatSecret());

    ScrtVector    vRmdScrts;        // 系统推荐帖子
    ScrtVector    vFvrScrts;        // 好友赞过帖子

    // 获取系统推荐帖子        
    iRet = this->getRecommendSecrets(tReq.tUserId, vRmdScrts);  
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getRecommendSecrets failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " getRecommendSecrets success! "<<endl;


    // 判断好友数量
    // size_t iFrdsNum = vFrds1.size();
    if (iFrdsNum < _uMaxFriendsNum)
    {
        //获取1个好友赞过的帖子列表 1度关系 最多20条
        iRet = _tDbHandle.getBatchUserFavorSecrets(tReq.tUserId.lUId, tReq.iCount, _uMinFrdsFvrNum, vFrdsFavor, vFvrScrts); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 1favor get Friends Favor Secrets failure! "<<endl;
            break;

        }
        display(" Friends Favor Vector Secrets : ", vFvrScrts);
        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        // 获取3个好友赞过的帖子列表 1度关系 最多20条 
        iRet = _tDbHandle.getBatchUserFavorSecrets(tReq.tUserId.lUId, tReq.iCount, _uMaxFrdsFvrNum, vFrdsFavor, vFvrScrts); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 3favor get Friends Favor Secrets failure! "<<endl;
            break;
        }
        display(" Friends Favor Vector Secrets : ", vFvrScrts);
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
    }

    if (tReq.tRange.lBeginId != -1)
    {
        vFvrScrts.erase(remove_if(vFvrScrts.begin(),  vFvrScrts.end(),  GreatSId(tReq.tRange.lBeginId)), vFvrScrts.end()); 
        vRmdScrts.erase(remove_if(vRmdScrts.begin(),  vRmdScrts.end(),  GreatSId(tReq.tRange.lBeginId)), vRmdScrts.end()); 
    }

    // 汇总后帖子列表
    iRet = this->mergeSecrets(tReq.tUserId, tRsp.iLockCount, vRmdScrts, vFvrScrts, tRsp.vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " mergeSecrets failure! "<<endl;
        break;
    }

    // 排序 
    stable_sort(tRsp.vSecrets.begin(),  tRsp.vSecrets.end(),  GreatSecret());
    // 去重
    tRsp.vSecrets.erase(unique(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), EqualSecret()), tRsp.vSecrets.end());
  
    LOG->info() << __FUNCTION__<< " mergeSecrets success! "<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// uniqueSecretList 
//////////////////////////////////////////////////////
taf::Int32 SecretImp::uniqueSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, vector<FriendsList> const& vFrds1, vector<FriendsList> const& vFrdsAll, vector<FriendsList> const& vFrdsFavor)
{
    int iRet = FAILURE;

    // 输出参数
    LOG->debug() <<endl;

    PROC_BEGIN
    __TRY__

    // 设置解锁数
    size_t iFriends = vFrds1.size();
    if (iFriends >= _uMaxFriendsNum)
    {
        tRsp.iLockCount = 0;
    }
    else
    {
        tRsp.iLockCount = count_if(tRsp.vSecrets.begin(), tRsp.vSecrets.end(),  EqualFriends());
        if (tRsp.iLockCount == 0)
            tRsp.iLockCount = 1;
    }

    // 排序 
    stable_sort(tRsp.vSecrets.begin(),  tRsp.vSecrets.end(),  GreatSecret());
    // 去重
    tRsp.vSecrets.erase(unique(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), EqualSecret()), tRsp.vSecrets.end());
  
    LOG->info() << __FUNCTION__<< " mergeSecrets success! "<<endl;

    // 限制条数
    if (tRsp.vSecrets.size() != 0)
    {
        if (tReq.tRange.lBeginId == -1 || tRsp.vSecrets[0].lSId < tReq.tRange.lBeginId)
        {
        }
        else
        {
            tRsp.vSecrets.erase(remove_if(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), GreatSId(tReq.tRange.lBeginId)), tRsp.vSecrets.end());
        }

        tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }
    else
    {
        tRsp.lNextBeginId = -1;
    }

    //display("..................Vector Secrets", tRsp.vSecrets);
    // 限制条数
    if (tRsp.vSecrets.size() != 0)
    {

        LOG->info() << __FUNCTION__<< " deal With Favor And Remove SUCCESS! "<<endl;

        // 修正关系
        for (ScrtIterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end();)
        {
            for (vector<FriendsList>::const_iterator j = vFrdsAll.begin(); j != vFrdsAll.end(); ++j)
            {
                if (tRsp.iLockCount == 0 && i->lUId == j->lUId && j->iRelat == FRIEND)
                {
                    i->iType = FRIEND;
                }
                else if (tRsp.iLockCount == 0 && i->lUId == j->lUId && j->iRelat == FFRIEND)
                {
                    i->iType = FFRIEND;
                }
                else if (i->lUId == j->lUId)
                {
                    i->iType = RECMD;
                }
            }

            if (i->iFavorNum < 0)
                i->iFavorNum = 0;

            if (i->iCmtNum < 0)
                i->iCmtNum = 0;

            if (i->lUId == tReq.tUserId.lUId)
            {
                i->iType = ME;
            }

            if (i->sLocal.size() == 0 && i->iType == RECMD)
                tRsp.vSecrets.erase(i++);
            else 
                ++i;
        }

        // 处理点赞关系
        iRet = dealWithFavorAndRemove(tReq.tUserId, vFrds1.size(), tRsp.iLockCount, tRsp.vSecrets);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " deal With Favor And Remove failure! "<<endl;
            break;
        }

        // 获取系统引导帖子        
        if (tReq.tRange.lBeginId == -1 && tRsp.iLockCount == 0)
        {
            SecretInfo            tSysScrt;    
            iRet = _tDbHandle.getSystemSecret(tSysScrt);  
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " get SystemSecret failure! "<<endl;
                break;
            }

            tSysScrt.isFavor = false;

            LOG->info() << __FUNCTION__<< " get SystemSecret success! "<<endl;
        
            // 插入系统引导帖
            if (tRsp.vSecrets.size() < _uSysScrtLoc)
            {
                tRsp.vSecrets.push_back(tSysScrt);
            }
            else
            {
                tRsp.vSecrets.insert(tRsp.vSecrets.begin()+ _uSysScrtLoc - 1, tSysScrt);
            }
        }
    }
    else
    {
        tRsp.lNextBeginId = -1;
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 获取帖子详情
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getSecretContent(const QUAN::ContentListReq & tReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    // 解码ID
    LOG->debug() << __FUNCTION__<< " before decode "<<tReq.lFirstId<<endl;
    ContentListReq& tTmp = const_cast<ContentListReq&>(tReq);
    tTmp.lFirstId = decodeUId(tTmp.lFirstId);
    LOG->debug() << __FUNCTION__<< " after decode "<<tReq.lFirstId<<endl;
 
    PROC_BEGIN
    __TRY__

    // 获取帖子详情 
    iRet = _tDbHandle.getContents(tReq.tUserId, tReq.lFirstId, tReq.lSId, tRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" getContents failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<" getContents success! "<<endl;

    // 获取用户点赞关系
    ContentRelatMap   tFavorCnts;
    iRet = _tDbHandle.getUserContentSecrets(tReq.tUserId.lUId, tReq.lSId, tFavorCnts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "getUserContentSecrets failure! "<<endl;
        return iRet;
    }
    LOG->info() << __FUNCTION__<< " getUserContentSecrets success! "<<endl;

    display(" Favor Content RELATION: ", tFavorCnts.mCId);

    for (CntIterator i = tRsp.vContents.begin(); i != tRsp.vContents.end(); ++i)
    {
        RelatIterator pos = tFavorCnts.mCId.find(i->lCId);
        if (pos != tFavorCnts.mCId.end() && i->iFavorNum > 0)
        {
            i->isFavor = true;
        } 
        else
            i->isFavor = false;
        if (i->lUId == tReq.tUserId.lUId)
        {
            i->bIsMe = true;
        }
        else
        {
            i->bIsMe = false;
        }
    }

    // 更新消息提醒关系
    ChangeMsgReq   tMsgReq;
    tMsgReq.tUserId = tReq.tUserId;
    tMsgReq.lSId = tReq.lSId;
    tMsgReq.iOp = READED;           // 修改状态为已读
    tMsgReq.iType = CONT_FAVOR;     // 评论和点赞
    tMsgReq.lFirstId = tReq.lFirstId;     //楼主ID

    // 输出参数
    ostringstream os2; tMsgReq.displaySimple(os2);
    LOG->debug() <<" changeMsgStatus begin ..................."<<os2.str()<<endl;
        
    size_t iNum = _pMsgProxy->getEndpoint4All().size();
    for (size_t i = 0; i < iNum; i++)
    { 
        _pMsgProxy->taf_hash(i)->async_changeMsgStatus(NULL, tMsgReq);
    }
/*
    iRet = _pMsgProxy->taf_hash(tReq.tUserId.lUId)->async_changeMsgStatus(NULL, tMsgReq);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" changeMsgStatus failure! "<<endl;
        break;
    }
*/
    display(" vector content : ", tRsp.vContents);

    // 编码混淆UserId
    encodeResponse(tRsp.vContents);

    LOG->debug() << __FUNCTION__<<" success! end "<<endl;
 
    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 发表帖子
//////////////////////////////////////////////////////
taf::Int32 SecretImp::postSecret(const QUAN::PostSecretReq & tReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    string sId;
    getId(tReq.tUserId.lUId, sId);
    Int64 lSId = TC_Common::strto<Int64>(sId);

    // 返回帖子ID
    tRsp.lSId = lSId;

    // 发表帖子
    iRet = _tDbHandle.setSecret(tReq, lSId);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" set Secret failure! "<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<<" success! end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 发表评论
//////////////////////////////////////////////////////
taf::Int32 SecretImp::postContent(const QUAN::PostContentReq& tReq, QUAN::PostContentRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PostContentReq& tTmp = const_cast<PostContentReq&>(tReq); 
    tTmp.lFirstId = decodeUId(tTmp.lFirstId);
    LOG->debug() << __FUNCTION__<< " after decode "<<tReq.lFirstId<<endl;

    PROC_BEGIN
    __TRY__

    // 生成CId 
    string sId;
    getId(tReq.tUserId.lUId, sId);
 
    // 获取头像ID
    int iPortraitId = 0;
    iRet = _tDbHandle.getPortraitId(tReq, iPortraitId); 
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" getPortraitId failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<" getPortraitId success! "<<endl;
       

    // 发表评论
    iRet = _tDbHandle.setContent(tReq, sId, iPortraitId, tRsp.tContent);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" setContent failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<" setContent success! "<<endl;

    // 更新帖子评论数
    LOG->debug() << __FUNCTION__<<" updScrtContentNum before "<<CLEANN<<endl;
    iRet = _tDbHandle.updScrtContentNum(tReq.tUserId, tReq.lFirstId, tReq.lSId, CLEANN);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<<" updScrtContentNum failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<" updScrtContentNum success! "<<endl;

    // 插入消息提醒
    AddMsgReq tAddMsgReq;
    tAddMsgReq.tUserId = tReq.tUserId;         // 用户ID
    tAddMsgReq.lSId = tReq.lSId;               // 帖子ID
    tAddMsgReq.iOp = CONT;                     // 评论
    tAddMsgReq.lFirstId = tReq.lFirstId;       // 用户ID

    // 获取评论用户ID集合
    iRet = _tDbHandle.getContentUserIds(tReq.tUserId.lUId, tReq.lSId, tAddMsgReq.vUserIds);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getContentUserIds failure!"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Content UserIds success!"<<endl;

    LOG->debug() << __FUNCTION__<< " ....... UserIds : "<<simpledisplay(tAddMsgReq.vUserIds)<<endl;

    // 如果这条评论不是楼主发的需要把楼主加入
    if (tReq.tUserId.lUId != tReq.lFirstId)
        tAddMsgReq.vUserIds.push_back(tReq.lFirstId);

/*
    iRet = _pMsgProxy->addMsg(tAddMsgReq);    
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " _pMsgProxy->addMsg failure!"<<endl;
        break;
    }
*/

    LOG->debug()<< "|-Msg--------------------------------access|"<< endl;
    size_t iNum = _pMsgProxy->getEndpoint4All().size();
    for (size_t i = 0; i < iNum; i++)
    {
        _pMsgProxy->taf_hash(i)->async_addMsg(NULL, tAddMsgReq);
    }
    // current->setResponse(false);

    // 编码UID
    tRsp.tContent.lUId = encodeUId(tRsp.tContent.lUId);

    LOG->debug() << __FUNCTION__<< " postContent! end"<<endl;

    iRet = SUCCESS;
    tRsp.iStatus = SUCCESS;

    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 点赞取消赞
//////////////////////////////////////////////////////
taf::Int32 SecretImp::postFavor(const QUAN::PostFavorReq& tReq, QUAN::PostFavorRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PostFavorReq& tTmp = const_cast<PostFavorReq&>(tReq); 
    tTmp.lFavoredId = decodeUId(tTmp.lFavoredId);
    LOG->debug() << __FUNCTION__<< " after decode "<<tReq.lFavoredId<<endl;

    PROC_BEGIN
    __TRY__

    if (tReq.iType == SECRET)
    {
        // 对帖子进行点赞操作
        iRet = _tDbHandle.setSecretOp(tReq.tUserId, tReq.lFavoredId, tReq.lId, tReq.iOp);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " set Secret favor failure !"<<endl;
            break;
        }

        LOG->info() << __FUNCTION__<< " set Secret favar success!"<<endl;

        if (tReq.iOp == FAVORY && tReq.lFavoredId != tReq.tUserId.lUId)
        {
            // 插入消息提醒
            AddMsgReq tAddMsgReq;
            tAddMsgReq.tUserId.lUId = tReq.lFavoredId;   // 用户ID
            tAddMsgReq.lSId = tReq.lId;                  // 帖子ID
            tAddMsgReq.iOp = FAVOR;                      // 点赞
            tAddMsgReq.lFirstId = tReq.lFavoredId;       // 用户ID
/*
            iRet = _pMsgProxy->addMsg(tAddMsgReq);    
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<<" SId: "<<tReq.lId<<" FavoredId: "<<tReq.lFavoredId<<" _pMsgProxy->addMsg failure! Op: "<<tReq.iOp<<endl;
                break;
            }
*/
            LOG->debug()<< "|-Msg--------------------------------access|"<< endl;
            size_t iNum = _pMsgProxy->getEndpoint4All().size();
            for (size_t i = 0; i < iNum; i++)
            {
                _pMsgProxy->taf_hash(i)->async_addMsg(NULL, tAddMsgReq);
            }
            // current->setResponse(false);

            LOG->debug() << __FUNCTION__<<" SId: "<<tReq.lId<<" FavoredId: "<<tReq.lFavoredId<<" _pMsgProxy->addMsg sucesss! Op: "<<tReq.iOp<<endl;
       } 
       else
       {
            LOG->debug() << __FUNCTION__<< " Cancel Favor Don't Need Msg! op: "<<tReq.iOp<<endl;
       }
    }
    else if (tReq.iType == CONTENT)
    {
        // 对评论进行点赞操作
        iRet = _tDbHandle.setContentOp(tReq.tUserId, tReq.lFavoredId, tReq.lId, tReq.lSId, tReq.iOp);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " set Content Favor failure!"<<endl;
            break;
        }

        LOG->info() << __FUNCTION__<< " set Content Favor success!"<<endl;
    }    

    LOG->debug() << __FUNCTION__<< " success! end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}
        
//////////////////////////////////////////////////////
// 对外接口: 举报
//////////////////////////////////////////////////////
taf::Int32 SecretImp::postReport(const QUAN::PostReportReq& tReq, QUAN::PostReportRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PostReportReq& tTmp = const_cast<PostReportReq&>(tReq);
    tTmp.lReportedId = decodeUId(tTmp.lReportedId);
    LOG->debug() << __FUNCTION__<< " after decode "<<tReq.lReportedId<<endl;

    PROC_BEGIN
    __TRY__

    if (tReq.iType == SECRET)
    { 
        // 对帖子进行举报操作
        iRet = _tDbHandle.setSecretOp(tReq.tUserId, tReq.lReportedId, tReq.lSId, REPORTY);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " setSecretOp failure!"<<endl;
            break;
        }
    }
    else
    {
        // 对评论进行举报操作
        iRet = _tDbHandle.setContentOp(tReq.tUserId, tReq.lReportedId, tReq.lSId, 0, REPORTY);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " setSecretOp failure!"<<endl;
            break;
        }
    }

    LOG->debug() << __FUNCTION__<< " success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 清除痕迹
//////////////////////////////////////////////////////
taf::Int32 SecretImp::cleanTrail(const QUAN::CleanTrailReq& tReq, QUAN::CleanTrailRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 清除发帖痕迹

    if (tReq.lCId == 0 && tReq.lSId == 0) 
    {
        iRet = _tDbHandle.setSecretOp(tReq.tUserId, tReq.tUserId.lUId, 0, CLEANY);  // 0代表对所有的帖子进行操作
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " setSecretOp failure!"<<endl;
            break;
        }

        // 更新消息提醒关系
        ChangeMsgReq   tMsgReq;
        tMsgReq.tUserId = tReq.tUserId;
        tMsgReq.lSId = tReq.lSId;
        tMsgReq.iOp = READED;           // 修改状态为已读
        tMsgReq.iType = CLEAN_MSG;      // 清除
        tMsgReq.lFirstId = 0;
    
        // 输出参数
        ostringstream os2; tMsgReq.displaySimple(os2);
        LOG->debug() <<" changeMsgStatus begin ..................."<<os2.str()<<endl;
        size_t iNum = _pMsgProxy->getEndpoint4All().size();
        for (size_t i = 0; i < iNum; i++)
        { 
            _pMsgProxy->taf_hash(i)->async_changeMsgStatus(NULL, tMsgReq);
        }
    }
    else 
    {
        LOG->info() << __FUNCTION__<< " clean Secret Comment  ........!"<<endl;
    }

    LOG->info() << __FUNCTION__<< " setSecretOp success!"<<endl;

    if (tReq.lCId != 0)
    {
        // 清除发评论痕迹
        iRet = _tDbHandle.setContentOp(tReq.tUserId, tReq.tUserId.lUId, tReq.lCId, tReq.lSId, CLEANY);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " setContentOp failure!"<<endl;
            break;
        }
    }

    LOG->info() << __FUNCTION__<< " setContentOp success!"<<endl;

    LOG->debug() << __FUNCTION__<< " success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}
        
//////////////////////////////////////////////////////
// 对外接口: 移除帖子
//////////////////////////////////////////////////////
taf::Int32 SecretImp::removeSecret(const QUAN::RemoveSecretReq& tReq, QUAN::RemoveSecretRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    if (tReq.tUserId.lUId == tReq.lUId)
    {
        // 移除用户id 与用户id相等 清除痕迹
        iRet = _tDbHandle.setSecretOp(tReq.tUserId, tReq.tUserId.lUId, tReq.lSId, CLEANY);  // 0代表对所有的帖子进行操作
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " remove self setSecretOp failure!"<<endl;
            break;
        }

        LOG->info() << __FUNCTION__<< " remove self setSecretOp success!"<<endl;
    }
    else
    {
        // 移除用户帖子
        // 插入用户帖子关系 
        iRet = _tDbHandle.setSecretOp(tReq.tUserId, tReq.lUId, tReq.lSId, REMOVEY); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " remove other setSecretOp failure!"<<endl;
            break;
        }

        LOG->info() << __FUNCTION__<< " remove other setSecretOp success!"<<endl;
    }

    LOG->debug() << __FUNCTION__<< " success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 获取点赞关系
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getUserFavorRelat(taf::Int64 lUId,QUAN::SecretRelatMap &tFavorScrts,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = "   |   " + TC_Common::tostr(lUId) + "  |  ";
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    iRet = _tDbHandle.getUserFavorSecrets(lUId, tFavorScrts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "lUId: "<<lUId<<" getUserFavorSecrets RELATION failure!"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< "lUId: "<<lUId<<" getUserFavorSecrets RELATION success!"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    return iRet;

}

//////////////////////////////////////////////////////
// 对外接口: 获取移除关系
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getUserRemoveRelat(taf::Int64 lUId,QUAN::SecretRelatMap &tRemoveScrts,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = "   |   " + TC_Common::tostr(lUId) + "  |  ";
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    iRet = _tDbHandle.getUserRemoveSecrets(lUId, tRemoveScrts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "lUId: "<<lUId<<" getUserRemoveSecrets RELATION failure!"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< "lUId: "<<lUId<<" getUserRemoveSecrets RELATION success!"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;

    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: MsgServer调用返回帖子列表
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getUserBatchSecrets(taf::Int64 lUId,const map<taf::Int64, QUAN::MsgRmd> & mSIds,vector<QUAN::SecretInfo> &vSecrets,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = "   |   " + TC_Common::tostr(lUId) + "  |  ";
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    iRet = _tDbHandle.getUserBatchSecrets(lUId, mSIds, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "lUId: "<<lUId<<" getUserBatchSecrets failure!"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< "lUId: "<<lUId<<" getUserBatchSecrets RELATION success!"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 单独获取帖子详情
//////////////////////////////////////////////////////
taf::Int32 SecretImp::getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 根据帖子ID单独获取帖子内容
    iRet = _tDbHandle.getUserSingleSecretFromDb(tReq, tRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getUserSingleSecretsFromDb failure!"<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " getUserSingleSecretsFromDb success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;

    sLog = JCETOSTR(tRsp);
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;

    tRsp.iStatus = iRet;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 设置分享
//////////////////////////////////////////////////////
taf::Int32 SecretImp::setUserSecretShare(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 设置帖子为分享状态
    iRet = _tDbHandle.setUserSecretShareToDb(tReq, tRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getUserSingleSecretsFromDb failure!"<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " getUserSingleSecretsFromDb success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;

    sLog = JCETOSTR(tRsp);
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;

    tRsp.iStatus = iRet;

    return iRet;

}

//////////////////////////////////////////////////////
// 对外接口: 反馈
//////////////////////////////////////////////////////
taf::Int32 SecretImp::feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    iRet = _tDbHandle.setFeedBack(tReq, tRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " Feed Back failure!"<<endl;
        break;
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;

    sLog = JCETOSTR(tRsp);
    FDLOG() << __FUNCTION__<< " iRet: "<<0<<" | "<<sLog<<endl;

    tRsp.iStatus = SUCCESS;

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 设置收帖箱cache
//////////////////////////////////////////////////////
taf::Int32 SecretImp::setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 设置帖子到收帖箱
    for (vector<FriendsList>::const_iterator i = tReq.vUIds.begin(); i != tReq.vUIds.end(); ++i)
    {
        FriendsList& tTmp = const_cast<FriendsList&>(*i);
        if (tTmp.iRelat > ONE)
            tTmp.iRelat--;
        if (tReq.lUId != tTmp.lUId)
        {
            iRet = _tDbHandle.setSecretToCache(tReq.tPostReq, tReq.lSId, tTmp, tReq.iType);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " setSecretToCache failure!"<<endl;
                break;
            }
       }
    }

    //if (iType != USER_REMOVEY_CONT)
    //{
        FriendsList tMe;
        tMe.lUId = tReq.lUId;
        tMe.iRelat = ME;
        iRet = _tDbHandle.setSecretToCache(tReq.tPostReq, tReq.lSId, tMe, tReq.iType);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " setSecretToCache failure!"<<endl;
            break;
        }
    //}

    LOG->info() << __FUNCTION__<< " setSecretToCache success!end"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;

    return iRet;

}

taf::Int32 SecretImp::test(taf::Int64 lId,taf::JceCurrentPtr current)
{
    string sId;
    getId(2, sId); 
    LOG->debug() <<"---------------------------------------------------------"<<endl;
    LOG->debug() <<"the id is: "<<sId<<endl;

    time_t now = TC_TimeProvider::getInstance()->getNow();

    Int64 lTmp = (now & 0x00000000FFFFFFFF) << 32;

    Int64 llSId = lTmp + ((2 & 0x00000000FFFFFFFF) ^ gUIdXorNum);
    LOG->debug() <<"the id is: "<<now<<endl;
    LOG->debug() <<"the id is: "<<(now & 0x00000000FFFFFFFF)<<endl;
    LOG->debug() <<"the id is: "<<lTmp<<endl;
    LOG->debug() <<"the id is: "<<((2 & 0x00000000FFFFFFFF) ^ gUIdXorNum)<<endl;
    LOG->debug() <<"the id is: "<<llSId<<endl;
    LOG->debug() <<"the de id is: "<<decodeSecretId(llSId)<<endl;
    LOG->debug() <<"the de id is: "<<decodeSecretIdUId(llSId)<<endl;

    LOG->debug() <<"---------------------------------------------------------"<<endl;

    
    long lEncodeId = encodeUId(lId);
    LOG->debug() <<"encode UId: "<<lEncodeId<<" MAX "<<LONG_MAX<<endl;
    LOG->debug() <<"decode UId: "<<decodeUId(lEncodeId)<<endl;

    return SUCCESS;
}



