#include "UserInfoCallback.h"
#include "SecretCallback.h"


void UserInfoCallback::setPushSecretInfo(const PushSecretInfoPtr &pPushSecretInfoPtr)
{   
    _pPushSecretInfoPtr = pPushSecretInfoPtr;
}

//单向通知粉丝有新的操作
void UserInfoCallback::updatePostTime(const vector<FriendsList> &vFollowers)
{   
    if(_tCurrent->getFuncName() != "postSecret" && _tCurrent->getFuncName() != "postFavor")
    {
        return;
    }

    LOG->debug()<< string(__FUNCTION__)<<"|" <<_sLog <<"|follow |" <<vFollowers.size()<<endl;

    UpdatePostTimeReq tReq;
    tReq.lId = _tUserId.lUId;   
    for(unsigned i=0;i< vFollowers.size();i++)
    {
        tReq.vFollowerId.push_back(vFollowers[i].lUId);
    }

    LOG->debug()<< string(__FUNCTION__)<<"|" <<_sLog <<"|push content|"<<JCETOSTR(tReq)<<endl;
    unsigned uNum = _pUserProxy->getEndpoint4All().size();
    for(unsigned i =0; i< uNum; i++ )
    {  
        _pUserProxy->taf_hash(i)->async_updatePostTime(NULL,tReq);
    }
}

void UserInfoCallback::pushPostSecret(const vector<FriendsList> &vFollowers)
{   
    if(!_pPushSecretInfoPtr)
    {
        return;
    }

    if (_pPushSecretInfoPtr->tUserSecretCacheReq.iType == USER_FAVORY 
        || _pPushSecretInfoPtr->tUserSecretCacheReq.iType == USER_CONTENT
        || _pPushSecretInfoPtr->tUserSecretCacheReq.iType == USER_FAVORN)
    {
        return;
    }

    LOG->debug()<< string(__FUNCTION__)<<"|" <<_sLog <<"|follow |" <<vFollowers.size()<<endl;

    for(unsigned i=0;i<vFollowers.size();i++)
    {
        if(vFollowers[i].lUId != _tUserId.lUId)  //防止二度好友是自己的bug
        {
            _pPushSecretInfoPtr->tUserSecretCacheReq.vUIds.push_back(vFollowers[i]);
        }
    }
    _pPushSecretInfoPtr->tUserSecretCacheReq.lUId  = _tUserId.lUId; //加上用户自己

    LOG->debug()<< string(__FUNCTION__)<<"|" <<_sLog <<"|push content|"<<JCETOSTR(_pPushSecretInfoPtr->tUserSecretCacheReq)<<endl;

    unsigned uNum = _pSecretProxy->getEndpoint4All().size();

    for(unsigned i =0; i< uNum; i++ )
    {   
        _pSecretProxy->taf_hash(i)->async_setUserSecretToCache(NULL,_pPushSecretInfoPtr->tUserSecretCacheReq);
    }
}

void UserInfoCallback::callback_getActiveFollowers(taf::Int32 ret,  const QUAN::GetActiveFollowersRsp& tRsp)
{
    ostringstream os;

    tRsp.display(os);
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    const vector<FriendsList> &vFollowers  = tRsp.vFollowers;

    //异步更新粉丝中此用户活跃时间cache
    updatePostTime(vFollowers);

    //异步更新粉丝数据cache
    pushPostSecret(vFollowers);

}
void UserInfoCallback::callback_getActiveFollowers_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    //无须回包
    ostringstream os;
    os<< ret<<"|" <<_sLog<< endl;
    LOG->error()<<os.str();

    //FDLOG() <<os.str();
}

