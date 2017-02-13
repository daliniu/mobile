#include "BatchHttp.h"
#include "SecretUiImp.h"
#include "SecretUiServer.h"
#include "servant/Application.h"
#include "SecretCallback.h"
#include "MsgCallback.h"
#include "CircleCallback.h"

using namespace std;

class AsyncHttpCallback : public TC_HttpAsync::RequestCallback
{
public:
    AsyncHttpCallback(const string &sUrl) : _sUrl(sUrl)
    {
    }
    virtual void onException(const string &ex) 
    {
        FDLOG()<< "onException:" << _sUrl << ":" << ex << endl;
    }
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpResponse) 
    {
        FDLOG()<< "onResponse:" << _sUrl << endl;
    }
    virtual void onTimeout() 
    {
        FDLOG()<< "onTimeout:" << _sUrl << endl;
    }
    virtual void onClose()
    {
        FDLOG()<< "onClose:" << _sUrl << endl;
    }
protected:
    string _sUrl;
};

//////////////////////////////////////////////////////
void SecretUiImp::initialize()
{
    //initialize servant here:
    //...
    TC_Config conf;

    try 
    {   
        conf.parseFile(ServerConfig::BasePath + "/" + ServerConfig::ServerName+".conf");
    }   
    catch(...)
    {   
        exit(-2);
    }   

    _sActiveUrl =conf.get("/main/<ActiveUrl>");

    // 帖子列表服务
    _pSecretProxy = Application::getCommunicator()->stringToProxy<GetSecretPrx>("QUAN.GetSecretServer.GetSecretObj");

    // 帖子服务句柄
    _pPostProxy = Application::getCommunicator()->stringToProxy<PostPrx>("QUAN.PostServer.PostObj");

    // 用户信息句柄
    _pUserProxy = Application::getCommunicator()->stringToProxy<UserInfoPrx>("QUAN.UserInfoServer.UserInfoObj");

    // 信息提醒句柄
    _pMsgProxy = Application::getCommunicator()->stringToProxy<MsgPrx>("QUAN.MsgServer.MsgObj");

	// Lbs句柄
    _pLbsProxy = Application::getCommunicator()->stringToProxy<LbsPrx>("QUAN.LbsServer.LbsObj");

	// 圈子句柄
    _pCircleProxy = Application::getCommunicator()->stringToProxy<CirclePrx>("QUAN.CircleServer.CircleObj");
}

//////////////////////////////////////////////////////
void SecretUiImp::destroy()
{
    //destroy servant here:
    //...
}

bool SecretUiImp::isValidToken(QUAN::UserId &tId)
{
    long lUId   = 0;
    int iRet    = -1;

    PROC_BEGIN 
    __TRY__

	if (tId.sUA == "ios&100&store")
	{
		return true;
	}

	iRet = _pUserProxy->verifyToken(tId,lUId);

    if(iRet != 0)
    {
        break;
    }
    if(lUId <= 0)
    {
        break;
    }
    //id不一致
    if(tId.lUId > 0 && tId.lUId != lUId  && decodeUId(tId.lUId) != lUId)
    {
        break;
    }
	/*
	*/
	if (iRet == 0 && lUId > 0)
	{
		tId.lUId = lUId;
	}
	LOG->debug() << "verifyToken ok  ret:" << iRet << "|" << JCETOSTR(tId) << "|" << lUId << endl;

    SetActiveReq tReq;
	tReq.lId = tId.lUId;
    _pUserProxy->async_setActive(NULL,tReq);
    return true;

    __CATCH__
    PROC_END
    LOG->debug()<<"verifyToken fail ret:"<<iRet<<"|"<<JCETOSTR(tId)<<"|"<<lUId<< endl;
    return true;
    // return true;
}


void SecretUiImp::notifyFollowers(taf::JceCurrentPtr current,const QUAN::UserId &tUserId,const PushSecretInfoPtr &pPushSecretInfoPtr)
{
    GetActiveFollowersReq tFollowersReq;
    tFollowersReq.lId = tUserId.lUId;

    UserInfoCallbackPtr pCbUser = new UserInfoCallback(current,tUserId,_pUserProxy,_pSecretProxy);
    pCbUser->setCallerLog(_sLog);
    if(pPushSecretInfoPtr)
    {

        pCbUser->setPushSecretInfo(pPushSecretInfoPtr);
    }
   _pUserProxy->taf_hash(tUserId.lUId)->async_getActiveFollowers(pCbUser,tFollowersReq);
}



taf::Int32 SecretUiImp::getSecretList(const QUAN::SecretListReq & tOrigReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet            = -1;

    SecretListReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tUserId) + "|" + JCETOSTR(tReq.tRange) + "|" + TC_Common::tostr(tReq.iCount) + " | " + tReq.sCoord;
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    GetSecretCallbackPtr pCb = new GetSecretCallback(current);
    pCb->setCallerLog(_sLog);
	pCb->setSecretListReq(tReq);
    _pSecretProxy->taf_hash(tUserId.lUId)->async_getSecretList(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::getSecretContent(const QUAN::ContentListReq & tOrigReq,QUAN::ContentListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = -1;

    ContentListReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lFirstId           = decodeUId(tReq.lFirstId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+TC_Common::tostr(tReq.lFirstId) + "|"+ TC_Common::tostr(tReq.lSId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    PostCallbackPtr pCb = new PostCallback(current,_pUserProxy, _pSecretProxy, _pMsgProxy);
    pCb->setCallerLog(_sLog);
    pCb->setContentList(tReq);
    _pPostProxy->taf_hash(tUserId.lUId)->async_getSecretContent(pCb, tReq);    
    current->setResponse(false);
    iRet = 0;

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::postSecret(const QUAN::PostSecretReq & tOrigReq,QUAN::PostSecretRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = -1;

    PostSecretReq tReq      = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);        
    UserId  &tUserId        = tReq.tUserId;


    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+tReq.sBitUrl + "|"+ TC_Common::tostr(tReq.iColorId)+"|"+
        tReq.sContent+"|"+tReq.sLocal+"|"+tReq.sCoord + "|circleid:" + TC_Common::tostr(tReq.lCircleId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;
    FDLOG()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    //发表帖子
    PostCallbackPtr pCb = new PostCallback(current,_pUserProxy,_pSecretProxy, _pMsgProxy, _pPostProxy, _pLbsProxy, _pCircleProxy);
    pCb->setCallerLog(_sLog);
    pCb->setPostSecret(tReq);
    _pPostProxy->taf_hash(tUserId.lUId)->async_postSecret(pCb,tReq);    
    current->setResponse(false);
    iRet = 0;

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::postContent(const QUAN::PostContentReq& tOrigReq, QUAN::PostContentRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    PostContentReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lFirstId           = decodeUId(tReq.lFirstId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+ TC_Common::tostr(tReq.lSId) + "|"+ TC_Common::tostr(tReq.lFirstId)+"|"+tReq.sContent;
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
    pCb->setPostContent(tReq);
    _pPostProxy->taf_hash(tUserId.lUId)->async_postContent(pCb,tReq);    
    current->setResponse(false);

    PushSecretInfoPtr pPushSecretInfoPtr = new PushSecretInfo();
    pPushSecretInfoPtr->tUserSecretCacheReq.lSId  = tReq.lSId;
    pPushSecretInfoPtr->tUserSecretCacheReq.iType = USER_CONTENT;

    notifyFollowers(current,tUserId,pPushSecretInfoPtr);

	// 活动接口
	if (tReq.lFirstId != tReq.tUserId.lUId)
	{
    	string sUrl = _sActiveUrl + "comment&uid=" + TC_Common::tostr(tReq.tUserId.lUId);
    	TC_HttpAsync::RequestCallbackPtr p = new AsyncHttpCallback(sUrl);
    	g_app.getActiveHttp().doAsyncRequest(sUrl, p);
    	LOG->debug()<< " URL: "<<sUrl<<endl;
    	FDLOG()<< " URL: "<<sUrl<<endl;
	}

    iRet = 0;

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::postFavor(const QUAN::PostFavorReq& tOrigReq, QUAN::PostFavorRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    PostFavorReq tReq       = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lFavoredId         = decodeUId(tReq.lFavoredId);          
    UserId  &tUserId        = tReq.tUserId;
    
     
    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+ TC_Common::tostr(tReq.lId) + "|"+ TC_Common::tostr(tReq.lSId)
            +"|"+ TC_Common::tostr(tReq.iType)+"|"+TC_Common::tostr(tReq.iOp)+"|"+TC_Common::tostr(tReq.lFavoredId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    //点赞操作
    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
    pCb->setPostFavor(tReq);
    _pPostProxy->taf_hash(decodeUId(tUserId.lUId))->async_postFavor(pCb,tReq);    
    current->setResponse(false);
    iRet = 0;

    //异步通知粉丝有最新操作
    //只有帖子赞需要通知
    if(tReq.iType == 0)
    {
        PushSecretInfoPtr pPushSecretInfoPtr = new PushSecretInfo();
        pPushSecretInfoPtr->tUserSecretCacheReq.lSId = tReq.lId;
        if(tReq.iOp == 0) 
        {
            pPushSecretInfoPtr->tUserSecretCacheReq.iType = USER_FAVORY;
        }
        else if (tReq.iOp == 1) 
        {
            pPushSecretInfoPtr->tUserSecretCacheReq.iType = USER_FAVORN;
        }          
        else
        {
            break;
        }

        notifyFollowers(current,tUserId,pPushSecretInfoPtr);
    }

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::postReport(const QUAN::PostReportReq& tOrigReq, QUAN::PostReportRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    PostReportReq tReq      = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lReportedId        = decodeUId(tReq.lReportedId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+ TC_Common::tostr(tReq.lSId) + "|"+ TC_Common::tostr(tReq.lReportedId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
	pCb->setPostReportReq(tReq);
    _pPostProxy->taf_hash(tUserId.lUId)->async_postReport(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::cleanTrail(const QUAN::CleanTrailReq& tOrigReq, QUAN::CleanTrailRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    CleanTrailReq tReq      = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);       
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId)+"|"+TC_Common::tostr(tReq.lSId) + "|"+ TC_Common::tostr(tReq.lCId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
    pCb->setCleanTrail(tReq);
    if (tReq.lCId != 0)
    {
        RemoveContentReq tReReq;
        tReReq.tUserId = tReq.tUserId;
        tReReq.lSId = tReq.lSId;
        tReReq.lCId = tReq.lCId;
        _pPostProxy->taf_hash(tUserId.lUId)->async_removeContent(pCb,tReReq);    
    }
    else
    {
        _pPostProxy->taf_hash(tUserId.lUId)->async_cleanTrail(pCb,tReq);    
        //异步通知粉丝有最新操作
        PushSecretInfoPtr pPushSecretInfoPtr = new PushSecretInfo();
        pPushSecretInfoPtr->tUserSecretCacheReq.lSId  = tReq.lSId;
        pPushSecretInfoPtr->tUserSecretCacheReq.iType = USER_CLEAN;
        notifyFollowers(current,tUserId,pPushSecretInfoPtr);
    }
    current->setResponse(false);
    iRet = 0;

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}
taf::Int32 SecretUiImp::removeSecret(const QUAN::RemoveSecretReq& tOrigReq, QUAN::RemoveSecretRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    RemoveSecretReq tReq    = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lUId               = decodeUId(tReq.lUId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+ TC_Common::tostr(tReq.lSId) + "|"+ TC_Common::tostr(tReq.lUId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
	pCb->setRemoveSecret(tReq);
    _pPostProxy->taf_hash(tUserId.lUId)->async_removeSecret(pCb,tReq);    
    current->setResponse(false);
    iRet = 0;

    //移除自己的帖子，需要异步通知粉丝有最新操作
    if(tUserId.lUId == tReq.lUId)
    {
        PushSecretInfoPtr pPushSecretInfoPtr = new PushSecretInfo();
        pPushSecretInfoPtr->tUserSecretCacheReq.lSId  = tReq.lSId;
        pPushSecretInfoPtr->tUserSecretCacheReq.iType = USER_REMOVEY;
        notifyFollowers(current,tUserId,pPushSecretInfoPtr);
    }

    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::getPushMsg(const QUAN::PushMsgReq& tOrigReq, QUAN::PushMsgRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    PushMsgReq tReq         = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);         
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId)+"|"+JCETOSTR(tReq.tRange) + "|"+ TC_Common::tostr(tReq.iCount);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current);
    pCb->setCallerLog(_sLog);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_getPushMsg(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}
    
taf::Int32 SecretUiImp::addMsg(const QUAN::AddMsgReq& tOrigReq, taf::JceCurrentPtr current)
{
    int iRet = -1;

    AddMsgReq tReq          = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lFirstId           = decodeUId(tReq.lFirstId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId)+"|"+TC_Common::tostr(tReq.lSId)+"|"+TC_Common::tostr(tReq.iOp)+"|"+TC_Common::tostr(tReq.lFirstId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current);
    pCb->setCallerLog(_sLog);
	pCb->setAddMsgReq(tReq);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_addMsg(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<< endl;
    }

    return  iRet;
}
    
taf::Int32 SecretUiImp::changeMsgStatus(const QUAN::ChangeMsgReq & tOrigReq,taf::JceCurrentPtr current)
{
    int iRet = -1;

    ChangeMsgReq tReq       = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);
    tReq.lFirstId           = decodeUId(tReq.lFirstId);          
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId)+"|"+TC_Common::tostr(tReq.lSId)+"|"+TC_Common::tostr(tReq.iOp)+"|"+TC_Common::tostr(tReq.lFirstId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current);
    pCb->setCallerLog(_sLog);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_changeMsgStatus(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::getMsgNum(const QUAN::GetMsgNumReq &tOrigReq,QUAN::GetMsgNumRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = -1;

    GetMsgNumReq tReq       = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);       
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId);
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current);
    pCb->setCallerLog(_sLog);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_getMsgNum(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}


taf::Int32 SecretUiImp::feedBack(const QUAN::FeedBackReq & tOrigReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current)
{
   int iRet = -1;

    FeedBackReq tReq       = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);       
    UserId  &tUserId        = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|"+tReq.sContent;
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    _pSecretProxy->taf_hash(tUserId.lUId)->feedBack(tReq,tRsp);    

    iRet = 0;

    __CATCH__
    PROC_END

    LOG->debug()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
    FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;

    return  iRet;
}

taf::Int32 SecretUiImp::getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current)
{
   int iRet = -1;


    _sLog = string(__FUNCTION__) +"|"+TC_Common::tostr(tReq.lSId) ;
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    __TRY__

    _pSecretProxy->taf_hash(tReq.lSId)->getUserSingleSecret(tReq,tRsp);    

    iRet = 0;

    __CATCH__
    PROC_END

    LOG->debug()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
    FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;

    return  iRet;
}

taf::Int32 SecretUiImp::clearUserCache(taf::Int64 lUId,taf::JceCurrentPtr current)
{
   int iRet = -1;

    _sLog = string(__FUNCTION__) +"|"+TC_Common::tostr(lUId)+"|decodeUId|"+TC_Common::tostr(decodeUId(lUId));
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    __TRY__

    unsigned uNum = _pUserProxy->getEndpoint4All().size();
    for(unsigned i =0; i< uNum; i++ )
    {  
        _pUserProxy->taf_hash(i)->async_clearUserCache(NULL,decodeUId(lUId));
    } 

    iRet = 0;

    __CATCH__
    PROC_END

    LOG->debug()<<iRet<<"|"<<_sLog<< endl;
    FDLOG()<<iRet<<"|"<<_sLog<< endl;

    return  iRet;
}

taf::Int32 SecretUiImp::getSystemList(const QUAN::SystemListReq & tOrigReq, QUAN::SystemListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet            = -1;

    SystemListReq tReq     = tOrigReq;
    tReq.tUserId.lUId      = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId       = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    GetSecretCallbackPtr pCb = new GetSecretCallback(current);
    pCb->setCallerLog(_sLog);
    _pSecretProxy->taf_hash(tUserId.lUId)->async_getSystemList(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::bindMsg(const QUAN::BindMsgReq & tOrigReq,taf::JceCurrentPtr current)
{
    int iRet            = -1;

    BindMsgReq tReq     = tOrigReq;
    tReq.tUserId.lUId   = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId    = tReq.tUserId;

    _sLog = string(__FUNCTION__) +"|"+JCETOSTR(tUserId) +"|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

	MsgCallbackPtr pCb = new MsgCallback(current, _pMsgProxy);
	pCb->setBindMsgReq(tReq);
	pCb->setCallerLog(_sLog);
	_pMsgProxy->taf_hash(tReq.tUserId.lUId)->async_pushBindMsg(pCb, tReq.tUserId, tReq.iType, tReq.sId);
	current->setResponse(false);

#if 0
	size_t iNum = _pMsgProxy->getEndpoint4All().size();
	for (size_t i = 0; i < iNum; i++)
	{
		_pMsgProxy->taf_hash(i)->async_bindMsg(NULL, tReq);
	}
#endif
    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<"------------"<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<"-----------"<< endl;
    }

    return SUCCESS;
}

taf::Int32 SecretUiImp::atMsg(const QUAN::AtMsgReq & tOrigReq,taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	AtMsgReq tReq = tOrigReq;
	tReq.tUserId.lUId = decodeUId(tReq.tUserId.lUId);
	UserId  &tUserId = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;


	PROC_BEGIN
	__TRY__

	MsgCallbackPtr pCb = new MsgCallback(current, _pMsgProxy);
	pCb->setAtMsgReq(tReq);
	pCb->setCallerLog(_sLog);
	_pMsgProxy->taf_hash(tUserId.lUId)->async_atMsg(pCb, tReq);
	current->setResponse(false);

	iRet = 0;
	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << "------------" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << "-----------" << endl;
	}

	return SUCCESS;;
}

taf::Int32 SecretUiImp::shareSecret(taf::Int64 lSId, taf::Int64 lShareId, taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	_sLog = string(__FUNCTION__) + "|" + TC_Common::tostr<Int64>(lSId) + "|" + TC_Common::tostr<Int64>(lShareId) + "|";
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;

	PROC_BEGIN
	__TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
    pCb->setSId(lSId);
	pCb->setShareId(lShareId);
    _pPostProxy->taf_hash(decodeUId(lShareId))->async_shareSecret(pCb, lSId, lShareId);    
    current->setResponse(false);
    iRet = 0;

	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << "------------" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << "-----------" << endl;
	}

	return SUCCESS;;

}

taf::Int32 SecretUiImp::getAllSecretList(const QUAN::SecretListReq & tOrigReq, QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet            = -1;

    SecretListReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tUserId) + "|" + JCETOSTR(tReq.tRange) + "|" + TC_Common::tostr(tReq.iCount) + " | " + tReq.sCoord;
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
		tRsp.iStatus = -119;
        break;
    }

    __TRY__

    GetSecretCallbackPtr pCb = new GetSecretCallback(current);
    pCb->setCallerLog(_sLog);
	pCb->setSecretListReq(tReq);
    _pSecretProxy->taf_hash(tUserId.lUId)->async_getAllSecretList(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::createCircle(const QUAN::CreateCircleReq& tOrigReq, QUAN::CreateCircleRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet            = -1;

    CreateCircleReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    CircleCallbackPtr pCb = new CircleCallback(current, _pCircleProxy);
    pCb->setCallerLog(_sLog);
	pCb->setCreateCircleReq(tReq);
    _pCircleProxy->taf_hash(tUserId.lUId)->async_createCircle(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::searchCircle(const QUAN::SearchCircleReq& tOrigReq, QUAN::SearchCircleRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet            = -1;

    SearchCircleReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    CircleCallbackPtr pCb = new CircleCallback(current);
    pCb->setCallerLog(_sLog);
    _pCircleProxy->taf_hash(tUserId.lUId)->async_searchCircle(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::getCircle(const QUAN::GetCircleReq& tOrigReq, QUAN::GetCircleRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet            = -1;

    GetCircleReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    CircleCallbackPtr pCb = new CircleCallback(current);
    pCb->setCallerLog(_sLog);
    _pCircleProxy->taf_hash(tUserId.lUId)->async_getCircle(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::setCircleConcern(const QUAN::SetCircleReq& tOrigReq, taf::JceCurrentPtr current)
{
    int iRet            = -1;

    SetCircleReq tReq     = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);      
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    CircleCallbackPtr pCb = new CircleCallback(current, _pCircleProxy);
    pCb->setCallerLog(_sLog);
	pCb->setSetCircleReq(tReq);
    _pCircleProxy->taf_hash(tUserId.lUId)->async_setCircleConcern(pCb,tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tReq)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tReq)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::auditCircle(taf::Int64 lCircleId, Int32 iAuditState, string const& sReason, taf::JceCurrentPtr current)
{
    int iRet            = -1;

	_sLog = string(__FUNCTION__) + "|" + TC_Common::tostr(lCircleId) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    __TRY__

    CircleCallbackPtr pCb = new CircleCallback(current, _pCircleProxy);
    pCb->setCallerLog(_sLog);
	pCb->setCircleId(lCircleId);
	// pCb->setUserId(tUserId);
	// pCb->setReason(sReason);
	// pCb->setAuditState(iAuditState);
    _pCircleProxy->taf_hash(lCircleId)->async_auditCircle(pCb, lCircleId, iAuditState, sReason);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    return  iRet;
}

taf::Int32 SecretUiImp::recommCircle(taf::Int64 lCircleId, taf::Int32 iRecommValue, taf::JceCurrentPtr current)
{
    int iRet            = -1;

	_sLog = string(__FUNCTION__) + "|" + TC_Common::tostr(lCircleId) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    __TRY__

    _pCircleProxy->taf_hash(lCircleId)->async_recommCircle(NULL, lCircleId, iRecommValue);    

    iRet = 0;
    __CATCH__
    PROC_END

    return  iRet;
}

taf::Int32 SecretUiImp::getCircleList(const QUAN::GetCircleListReq& tOrigReq, QUAN::GetCircleListRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    GetCircleListReq tReq         = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);         
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    GetSecretCallbackPtr pCb = new GetSecretCallback(current);
    pCb->setCallerLog(_sLog);
    _pSecretProxy->taf_hash(tUserId.lUId)->async_getCircleList(pCb, tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::getSysMsg(const QUAN::GetSysMsgReq& tOrigReq, QUAN::GetSysMsgRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    GetSysMsgReq tReq         = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);         
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;


    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current);
    pCb->setCallerLog(_sLog);
	pCb->setUserId(tReq.tUserId.lUId);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_getSysMsg(pCb, tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tRsp)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::pushSysMsg(const QUAN::PushSysMsgReq& tOrigReq, taf::JceCurrentPtr current)
{
    int iRet = -1;

    PushSysMsgReq tReq         = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);         
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN


    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current, _pMsgProxy);
    pCb->setCallerLog(_sLog);
	pCb->setUserId(tReq.tUserId.lUId);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_pushSysMsg(pCb, tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tReq)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tReq)<< endl;
    }

    return  iRet;
}

taf::Int32 SecretUiImp::auditSecret(vector<taf::Int64> const& vSId,taf::Int32 iLevel,taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	_sLog = string(__FUNCTION__) +  "|" + TC_Common::tostr<Int32>(iLevel) + "|";
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;

	PROC_BEGIN
	__TRY__

    PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    pCb->setCallerLog(_sLog);
    pCb->setVectorSId(vSId);
    _pPostProxy->taf_hash(decodeUId(iLevel))->async_auditSecret(pCb, vSId, iLevel);    
    current->setResponse(false);
    iRet = 0;

	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << "------------" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << "-----------" << endl;
	}

	return SUCCESS;;
}

taf::Int32 SecretUiImp::getNewSysMsg(const QUAN::GetNewSysMsgReq & tOrigReq, QUAN::GetNewSysMsgRsp &tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    GetNewSysMsgReq tReq         = tOrigReq;
    tReq.tUserId.lUId       = decodeUId(tReq.tUserId.lUId);         
    UserId  &tUserId        = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    if (!isValidToken(tUserId))
    {
        iRet = -119;
        break;
    }

    __TRY__

    MsgCallbackPtr pCb = new MsgCallback(current, _pMsgProxy);
    pCb->setCallerLog(_sLog);
	pCb->setUserId(tReq.tUserId.lUId);
    _pMsgProxy->taf_hash(tUserId.lUId)->async_getNewSysMsg(pCb, tReq);    
    current->setResponse(false);

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<<JCETOSTR(tReq)<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<<SIMPLEJCETOSTR(tReq)<< endl;
    }

    return  iRet;

}

taf::Int32 SecretUiImp::addCircleLocation(taf::Int64 lCircleId, const QUAN::CircleLocation & tCircleLocation, taf::JceCurrentPtr current)
{
    int iRet = -1;

	_sLog = string(__FUNCTION__) + "|" + TC_Common::tostr(lCircleId) + "|" + JCETOSTR(tCircleLocation) + "|";
    LOG->debug()<< _sLog << "|---------------------------------access|"<< endl;

    PROC_BEGIN
    __TRY__

    _pCircleProxy->taf_hash(lCircleId)->async_addCircleLocation(NULL, lCircleId, tCircleLocation);    

    iRet = 0;
    __CATCH__
    PROC_END

    if (iRet)
    {
        LOG->error()<<iRet<<"|"<<_sLog<<"|"<< endl;
        FDLOG()<<iRet<<"|"<<_sLog<<"|"<< endl;
    }

    return  iRet;

}

taf::Int32 SecretUiImp::modifyCircleLocation(taf::Int64 lCircleId,taf::Int64 lCircleLocationId,const QUAN::CircleLocation & tCircleLocation,taf::JceCurrentPtr current)
{
	int iRet = -1;

	_sLog = string(__FUNCTION__) + "|" + TC_Common::tostr(lCircleId) + "|";
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;

	PROC_BEGIN
	__TRY__

	CircleCallbackPtr pCb = new CircleCallback(current, _pCircleProxy);
	pCb->setCallerLog(_sLog);
	pCb->setCircleId(lCircleId);
	_pCircleProxy->taf_hash(lCircleId)->async_modifyCircleLocation(pCb, lCircleLocationId, tCircleLocation);
	current->setResponse(false);

	iRet = 0;
	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << endl;
	}

	return  iRet;
}

taf::Int32 SecretUiImp::getSecretById(const GetSecretByIdReq& tOrigReq, GetSecretByIdRsp& tRsp, taf::JceCurrentPtr current)
{
	int iRet = -1;

	GetSecretByIdReq tReq = tOrigReq;
	tReq.tUserId.lUId = decodeUId(tReq.tUserId.lUId);
	UserId  &tUserId = tReq.tUserId;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tUserId) + "|" + JCETOSTR(tReq);
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;


	PROC_BEGIN
	if (!isValidToken(tUserId))
	{
		iRet = -119;
		break;
	}

	__TRY__

	PostCallbackPtr pCb = new PostCallback(current, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy, _pLbsProxy, _pCircleProxy);
	pCb->setCallerLog(_sLog);
	tUserId.lUId = tReq.tContent.lUId;
	pCb->setUserId(tUserId);
	_pPostProxy->taf_hash(tReq.tUserId.lUId)->async_getSecretById(pCb, tReq);
	current->setResponse(false);
	iRet = 0;

	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << JCETOSTR(tRsp) << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << SIMPLEJCETOSTR(tRsp) << endl;
	}

	return  iRet;
}

taf::Int32 SecretUiImp::pushImMsg(const PushImMsgReq& tReq, taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|";
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;

	PROC_BEGIN
	__TRY__

	_pMsgProxy->taf_hash(tReq.tUserId.lUId)->async_pushImMsg(NULL, tReq);

	iRet = 0;
	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << "------------" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << "-----------" << endl;
	}

	return SUCCESS;;
}

taf::Int32 SecretUiImp::createCircleByAdmin(const QUAN::CreateCircleReq& tReq, const std::vector<QUAN::CircleLocation>& vCircleLocation, QUAN::CreateCircleRsp& tRsp, taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	_sLog = string(__FUNCTION__) + "|" + JCETOSTR(tReq) + "|" + JCETOSTR(vCircleLocation);
	LOG->debug() << _sLog << "|---------------------------------access|" << endl;

	PROC_BEGIN
	__TRY__

	CircleCallbackPtr pCb = new CircleCallback(current, _pCircleProxy);
	pCb->setCallerLog(_sLog);
	_pCircleProxy->taf_hash(tReq.tUserId.lUId)->async_createCircleByAdmin(pCb, tReq, vCircleLocation);
	current->setResponse(false);

	iRet = 0;
	__CATCH__
	PROC_END

	if (iRet)
	{
		LOG->error() << iRet << "|" << _sLog << "|" << "------------" << endl;
		FDLOG() << iRet << "|" << _sLog << "|" << "-----------" << endl;
	}

	return SUCCESS;;
}
