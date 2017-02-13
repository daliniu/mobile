#include "SecretCallback.h"
#include "MsgCallback.h"
#include "UserInfoCallback.h"

void GetSecretCallback::callback_getCircleList(taf::Int32 ret, const QUAN::GetCircleListRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    GetSecret::async_response_getCircleList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void GetSecretCallback::callback_getCircleList_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    GetCircleListRsp tRsp;
	tRsp.iStatus = -1;
	ret = -1;
    GetSecret::async_response_getCircleList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}
void GetSecretCallback::callback_getAllSecretList(taf::Int32 ret,  const QUAN::SecretListRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    GetSecret::async_response_getAllSecretList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void GetSecretCallback::callback_getAllSecretList_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    SecretListRsp tRsp;
	tRsp.iStatus = -1;
	ret = -1;
    GetSecret::async_response_getAllSecretList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}
void GetSecretCallback::callback_getSecretList(taf::Int32 ret,  const QUAN::SecretListRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    GetSecret::async_response_getSecretList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void GetSecretCallback::callback_getSecretList_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    SecretListRsp tRsp;
	tRsp.iStatus = -1;
	ret = -1;
    GetSecret::async_response_getSecretList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}
void GetSecretCallback::callback_getSystemList(taf::Int32 ret,  const QUAN::SystemListRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    GetSecret::async_response_getSystemList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void GetSecretCallback::callback_getSystemList_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    SystemListRsp tRsp;
    GetSecret::async_response_getSystemList(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}

void PostCallback::callback_getSecretContent(taf::Int32 ret,  const QUAN::ContentListRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<JCETOSTR(tRsp)<<endl;

    if (_pMsgProxy)
    {
        // 更新消息提醒关系
        ChangeMsgReq   tMsgReq;
        tMsgReq.tUserId = _tContentList.tUserId;
        tMsgReq.lSId = _tContentList.lSId;
        tMsgReq.iOp = READED;           // 修改状态为已读
        tMsgReq.iType = CONT_FAVOR;     // 评论和点赞
        tMsgReq.lFirstId = _tContentList.lFirstId;     //楼主ID

        LOG->debug() <<" changeMsgStatus begin ..................."<<_tContentList.tUserId.lUId<<endl;
		FDLOG() << " changeMsgStatus begin ..................." << _tContentList.tUserId.lUId << endl;

		MsgCallbackPtr pCb = new MsgCallback(_tCurrent, _pMsgProxy);
		pCb->setChangeMsgReq(tMsgReq);
		pCb->setCallerLog(_sLog);
        _pMsgProxy->taf_hash(_tContentList.tUserId.lUId)->async_changeMsgStatus(pCb, tMsgReq);

    }

    Post::async_response_getSecretContent(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}
void PostCallback::callback_getSecretContent_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::ContentListRsp tRsp;
    Post::async_response_getSecretContent(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_postSecret(taf::Int32 ret,  const QUAN::PostSecretRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    Post::async_response_postSecret(_tCurrent,ret,tRsp);

	if (_pCircleProxy && _tPostSecret.lCircleId != 0)
	{
		_pCircleProxy->taf_hash(_tPostSecret.tUserId.lUId)->async_postInCircle(NULL, _tPostSecret.tUserId, _tPostSecret.lCircleId, _tPostSecret.sCoord, false);
	}

	if (_pSecretProxy)
	{
		if (_tPostSecret.lCircleId == 0)
		{
			_pSecretProxy->taf_hash(_tPostSecret.tUserId.lUId)->async_cleanCircleIdCache(NULL, _tPostSecret.tUserId.lUId);
		}
		else
		{
			_pSecretProxy->taf_hash(_tPostSecret.tUserId.lUId)->async_cleanCircleIdCache(NULL, _tPostSecret.lCircleId);
		}
	}

    //异步push帖子信息到粉丝cache
    if(_pUserProxy)
    {
        GetActiveFollowersReq tFollowersReq;
        tFollowersReq.lId = _tPostSecret.tUserId.lUId;
    
        PushSecretInfoPtr pPushSecretInfoPtr = new PushSecretInfo();
    
        pPushSecretInfoPtr->tUserSecretCacheReq.tPostReq     = _tPostSecret;
        pPushSecretInfoPtr->tUserSecretCacheReq.lSId         = tRsp.lSId;
        pPushSecretInfoPtr->tUserSecretCacheReq.iType        = USER_POST;
    
        UserInfoCallbackPtr pCbUser = new UserInfoCallback(_tCurrent,
                                                           _tPostSecret.tUserId,
                                                           _pUserProxy,
                                                           _pSecretProxy);

        _sLog +="|"+string(__FUNCTION__);
        pCbUser->setCallerLog(_sLog);
        pCbUser->setPushSecretInfo(pPushSecretInfoPtr);
       _pUserProxy->taf_hash(_tPostSecret.tUserId.lUId)->async_getActiveFollowers(pCbUser,tFollowersReq);
    }

	// 更新Lbs信息
	this->updateLbsMap(_tPostSecret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}


void PostCallback::callback_postSecret_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::PostSecretRsp tRsp;
    Post::async_response_postSecret(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_postContent(taf::Int32 ret,  const QUAN::PostContentRsp& tRsp, const vector<Int64>& tRelatId)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<"|"<<simpledisplay(tRelatId)<<endl;
    FDLOG()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<"|"<<simpledisplay(tRelatId)<<endl;

	// 特殊接口tGetSecretByIdRsp返回
	if (_tGetSecretByIdRsp.tSecret.lSId != 0)
	{
		this->cleanPostCache(_tPostContent.lSId);
    	this->cleanCommentCache(_tPostContent.lSId);
		_tGetSecretByIdRsp.tContent = tRsp.tContent;
		_tGetSecretByIdRsp.tContent.sContent = "";
		LOG->debug() << ret << "|" << string(__FUNCTION__) << "|" << _sLog << JCETOSTR(_tGetSecretByIdRsp) << endl;
		Post::async_response_getSecretById(_tCurrent, ret, _tGetSecretByIdRsp);
		return;
	}

    if (_pMsgProxy)
    {
        AddMsgReq tAddMsgReq;
        tAddMsgReq.tUserId = _tPostContent.tUserId;          // 用户ID
        tAddMsgReq.lSId = _tPostContent.lSId;                // 帖子ID
        tAddMsgReq.iOp = CONT;                               // 评论
        tAddMsgReq.lFirstId = _tPostContent.lFirstId;        // 用户ID
        tAddMsgReq.vUserIds = tRelatId;

        tAddMsgReq.vUserIds.erase(remove(tAddMsgReq.vUserIds.begin(), tAddMsgReq.vUserIds.end(), _tPostContent.lFirstId), tAddMsgReq.vUserIds.end());
		tAddMsgReq.vUserIds.erase(remove(tAddMsgReq.vUserIds.begin(), tAddMsgReq.vUserIds.end(), _tPostContent.tUserId.lUId), tAddMsgReq.vUserIds.end());
        if (_tPostContent.tUserId.lUId != _tPostContent.lFirstId)
            tAddMsgReq.vUserIds.push_back(_tPostContent.lFirstId);

        LOG->debug() <<" addMsg begin ..................."<<_tPostContent.tUserId.lUId<<endl;
		FDLOG() << " addMsg begin ..................." << _tPostContent.tUserId.lUId << endl;
		MsgCallbackPtr pCb = new MsgCallback(_tCurrent, _pMsgProxy);
		pCb->setAddMsgReq(tAddMsgReq);
		pCb->setCallerLog(_sLog);
        _pMsgProxy->taf_hash(_tPostContent.lFirstId)->async_addMsg(pCb, tAddMsgReq);

    }

    this->cleanPostCache(_tPostContent.lSId);
    this->cleanCommentCache(_tPostContent.lSId);

    Post::async_response_postContent(_tCurrent,ret, tRsp, tRelatId);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;

}

void PostCallback::cleanPostCache(Int64 lSId)
{
    if (_pPostProxy)
    {
        LOG->debug()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<lSId<<endl;
        size_t iNum = _pPostProxy->getEndpoint4All().size();
        for (size_t i = 0; i < iNum; i++)
        {
            _pPostProxy->taf_hash(i)->async_clearPostCache(NULL, lSId);
        }
    }
}

void PostCallback::updateLbsMap(PostSecretReq const& tReq, PostSecretRsp const& tRsp)
{
	__TRY__
    if (_pLbsProxy)
    {
		LbsReq  tLbsReq;
		if (tReq.sCoord.find(",") != string::npos)
		{
		    string sLat = tReq.sCoord.substr(0, tReq.sCoord.find(",")+1);
		    string sLnt = tReq.sCoord.substr(tReq.sCoord.find(",")+1);
			tLbsReq.dLat = TC_Common::strto<Double>(sLat);
			tLbsReq.dLng = TC_Common::strto<Double>(sLnt);
			tLbsReq.lId = tRsp.lSId;
			LOG->debug() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << tReq.tUserId.lUId << "|" << JCETOSTR(tLbsReq) << endl;
			FDLOG() << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << tReq.tUserId.lUId << "|" << JCETOSTR(tLbsReq) << endl;
        	size_t iNum = _pLbsProxy->getEndpoint4All().size();
        	for (size_t i = 0; i < iNum; i++)
        	{
            	_pLbsProxy->taf_hash(i)->async_updateLbsMap(NULL, tLbsReq);
        	}
		}
    }
	__CATCH__
}

void PostCallback::cleanCommentCache(Int64 lSId)
{
    if (_pPostProxy)
    {
        LOG->debug()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<lSId<<endl;
        FDLOG()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<lSId<<endl;
        size_t iNum = _pPostProxy->getEndpoint4All().size();
        for (size_t i = 0; i < iNum; i++)
        {
            _pPostProxy->taf_hash(i)->async_clearCommentCache(NULL, lSId);
        }
    }
}

void PostCallback::cleanMsgCache(vector<Int64> const& vUserId)
{
    if (_pMsgProxy)
    {
        LOG->debug()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<simpledisplay(vUserId)<<endl;
        FDLOG()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<simpledisplay(vUserId)<<endl;
        size_t iNum = _pMsgProxy->getEndpoint4All().size();
        for (size_t i = 0; i < iNum; i++)
        {
            _pMsgProxy->taf_hash(i)->async_cleanCache(NULL, vUserId);
        }
    }
}

void PostCallback::cleanUserTrailCache(Int64 lUId)
{
    if (_pPostProxy)
    {
        LOG->debug()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<lUId<<endl;
        FDLOG()<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<"|"<<lUId<<endl;
        size_t iNum = _pPostProxy->getEndpoint4All().size();
        for (size_t i = 0; i < iNum; i++)
        {
            _pPostProxy->taf_hash(i)->async_clearUserCleanTrailCache (NULL, lUId);
        }
    }
}

void PostCallback::callback_postContent_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::PostContentRsp tRsp;
    vector<Int64> tRelatId;
    Post::async_response_postContent(_tCurrent,ret,tRsp, tRelatId);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_postFavor(taf::Int32 ret,  const QUAN::PostFavorRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    if (_tPostFavor.iType == SECRET)
    {
        // 清除cache
        this->cleanPostCache(_tPostFavor.lId);
    }
    else
    {
        // 清除cache
        this->cleanCommentCache(_tPostFavor.lSId);
    }

    Post::async_response_postFavor(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void PostCallback::callback_postFavor_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::PostFavorRsp tRsp;
    Post::async_response_postFavor(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_postReport(taf::Int32 ret,  const QUAN::PostReportRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

	this->cleanPostCache(_tPostReportReq.lSId);

    Post::async_response_postReport(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void PostCallback::callback_postReport_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::PostReportRsp tRsp;
    Post::async_response_postReport(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_cleanTrail(taf::Int32 ret,  const QUAN::CleanTrailRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

    if (_pMsgProxy)
    {
        // 更新消息提醒关系
        ChangeMsgReq   tMsgReq;
        tMsgReq.tUserId = _tCleanTrail.tUserId;
        tMsgReq.lSId = _tCleanTrail.lSId;
        tMsgReq.iOp = READED;           // 修改状态为已读
        tMsgReq.iType = CLEAN_MSG;      // 清除
        tMsgReq.lFirstId = 0;
    
        // 输出参数
        ostringstream os2; tMsgReq.displaySimple(os2);
        LOG->debug()<<__FUNCTION__<<" changeMsgStatus begin ..................."<<os2.str()<<endl;
        FDLOG()<<__FUNCTION__<<" changeMsgStatus begin ..................."<<os2.str()<<endl;

		MsgCallbackPtr pCb = new MsgCallback(_tCurrent, _pMsgProxy);
		pCb->setChangeMsgReq(tMsgReq);
		pCb->setCallerLog(_sLog);
        _pMsgProxy->taf_hash(_tContentList.tUserId.lUId)->async_changeMsgStatus(pCb, tMsgReq);
    }

    this->cleanUserTrailCache(_tCleanTrail.tUserId.lUId);

    Post::async_response_cleanTrail(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void PostCallback::callback_cleanTrail_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::CleanTrailRsp tRsp;
    Post::async_response_cleanTrail(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_removeSecret(taf::Int32 ret,  const QUAN::RemoveSecretRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

	if (_tRemoveSecret.tUserId.lUId == _tRemoveSecret.lUId)
	{
		this->cleanPostCache(_tRemoveSecret.lSId);
	}

    Post::async_response_removeSecret(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void PostCallback::callback_removeSecret_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;

    QUAN::RemoveSecretRsp tRsp;
    Post::async_response_removeSecret(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_removeContent(taf::Int32 ret,  const QUAN::RemoveContentRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog <<JCETOSTR(tRsp)<<endl;

	this->cleanCommentCache(_tCleanTrail.lSId);

    Post::async_response_removeContent(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}
void PostCallback::callback_removeContent_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;
    
    QUAN::RemoveContentRsp tRsp;
    Post::async_response_removeContent(_tCurrent,ret,tRsp);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<<JCETOSTR(tRsp)<< endl;
}

void PostCallback::callback_shareSecret(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"| ShareId "<<_lShareId<<"|" <<_sLog<<endl;

	this->cleanPostCache(_lSId);
	
	if (_pUserProxy)
	{
		UpdatePostTimeReq tPostTimeReq;
		tPostTimeReq.lId = _lShareId;
		size_t iNum = _pUserProxy->getEndpoint4All().size();
		for (size_t i = 0; i < iNum; i++)
		{
			_pUserProxy->taf_hash(i)->async_updatePostTime(NULL, tPostTimeReq);
		}
	}

    Post::async_response_shareSecret(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<< endl;
}

void PostCallback::callback_shareSecret_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;
    
    Post::async_response_shareSecret(_tCurrent,ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void PostCallback::callback_auditSecret(taf::Int32 ret)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"| lSId "<<_lSId<<"|" <<_sLog<<endl;

	for (vector<Int64>::iterator i = _vSId.begin(); i != _vSId.end(); ++i)
	{
		this->cleanPostCache(*i);
	}
	
    Post::async_response_auditSecret(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<<"|"<< endl;

}

void PostCallback::callback_auditSecret_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;
    
    Post::async_response_auditSecret(_tCurrent, ret);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}

void PostCallback::callback_getSecretById(taf::Int32 ret, const QUAN::GetSecretByIdRsp& tRsp)
{
    LOG->debug()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<JCETOSTR(tRsp)<<endl;

	CircleInfo tInfo;
	GetSecretByIdRsp tGRsp;
	PostContentReq tPReq;
	tGRsp = tRsp;

	if (tRsp.tSecret.lSId != 0)
	{
		if (_pCircleProxy && tRsp.tSecret.lCircleId !=0)
		{
			_pCircleProxy->taf_hash(_tUserId.lUId)->getCircleInfo(_tUserId, tRsp.tSecret.lCircleId, "", tInfo);
			tGRsp.tSecret.sLocal = tInfo.sName;
		}
		else
		{
			tGRsp.tSecret.sLocal = "通讯录";
		}
	}

	if (tRsp.tSecret.lSId != 0 && tRsp.tContent.lCId == 0)
	{
		tPReq.tUserId = _tUserId;
		tPReq.lSId = tRsp.tSecret.lSId;
		tPReq.lFirstId = tRsp.tSecret.lUId;
		tPReq.sContent = "$[position]";

		PostCallbackPtr pCb = new PostCallback(_tCurrent, _pUserProxy, _pSecretProxy, _pMsgProxy, _pPostProxy);
    	pCb->setCallerLog(_sLog);
    	pCb->setPostContent(tPReq);
		pCb->setGetSecretByIdRsp(tGRsp);
    	_pPostProxy->taf_hash(_tUserId.lUId)->async_postContent(pCb, tPReq);    
    	_tCurrent->setResponse(false);
	}
	else
	{
		Post::async_response_getSecretById(_tCurrent, ret, tGRsp);
	}

    FDLOG()<<ret<<"|" <<_sLog<<"|"<< endl;

}

void PostCallback::callback_getSecretById_exception(taf::Int32 ret)
{
    LOG->error()<<ret<<"|"<< string(__FUNCTION__)<<"|" <<_sLog<<endl;
   
	ret = -1;
	GetSecretByIdRsp tRsp;
	Post::async_response_getSecretById(_tCurrent, ret, tRsp);

    FDLOG()<<ret<<"|" <<_sLog<< endl;
}