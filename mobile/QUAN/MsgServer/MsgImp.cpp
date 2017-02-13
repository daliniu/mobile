#include "BatchHttp.h"
#include "MsgImp.h"
#include "MsgServer.h"
#include "servant/Application.h"
#include "ConstValue.h"
#include "util/tc_base64.h"

using namespace std;

class AsyncHttpCallback : public TC_HttpAsync::RequestCallback
{
public:
	AsyncHttpCallback(const string &sUrl) : _sUrl(sUrl)
	{
	}
	virtual void onException(const string &ex)
	{
		FDLOG() << "onException:" << _sUrl << ":" << ex << endl;
	}
	virtual void onResponse(bool bClose, TC_HttpResponse &stHttpResponse)
	{
		FDLOG() << "onResponse:" << _sUrl << endl;
	}
	virtual void onTimeout()
	{
		FDLOG() << "onTimeout:" << _sUrl << endl;
	}
	virtual void onClose()
	{
		FDLOG() << "onClose:" << _sUrl << endl;
	}
protected:
	string _sUrl;
};

struct LessRemove
{   
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.isRemove < e2.isRemove;
    }
};

string urlencode(const string &sUrl)
{
	static char HEX_TABLE[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	string result;
	for (size_t i = 0; i < sUrl.length(); i++)
	{
		char c = sUrl[i];
		if (c == ' ')
			result.append(1, '+');
		else if ((c >= '0' && c <= '9')
			|| (c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c == '-')
			|| (c == '_')
			|| (c == '.'))
			result.append(1, c);
		else
		{
			result.append(1, '%');
			result.append(1, HEX_TABLE[(c >> 4) & 0x0f]);
			result.append(1, HEX_TABLE[c & 0x0f]);
		}
	}

	return result;
}
struct LessIsRead
{   
    bool operator()(const SecretInfo &e1, const SecretInfo &e2)
    {
        return e1.isRead< e2.isRead;
    }
};

struct EqualSId
{
    EqualSId (Int64 lId) : lSId(lId) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.lSId == lSId;
    }

    Int64 lSId;
};

struct EqualisRead
{
    EqualisRead (bool is) : isRead(is) { }

    bool operator()(const SecretInfo &e1)
    {
        return e1.isRead == isRead;
    }

    bool isRead;
};
//////////////////////////////////////////////////////
void MsgImp::initialize()
{
	//initialize servant here:
	//...
    // 用户信息句柄
    _pUserProxy  = Application::getCommunicator()->stringToProxy<UserInfoPrx>("QUAN.UserInfoServer.UserInfoObj");
    _pPostProxy = Application::getCommunicator()->stringToProxy<PostPrx>("QUAN.PostServer.PostObj");
	_pPushProxy = Application::getCommunicator()->stringToProxy<PushProxyPrx>("MDW.PushProxyServer.PushProxyObj");
	_pLbsProxy = Application::getCommunicator()->stringToProxy<LbsPrx>("QUAN.LbsServer.LbsObj");
	_pCircleProxy = Application::getCommunicator()->stringToProxy<CirclePrx>("QUAN.CircleServer.CircleObj");

	TC_Config tConf;
	tConf.parseFile(ServerConfig::BasePath + "MsgServer.conf");
	// tokenServerHost = tConf.get("/main/<tokenserverhost>", "183.61.6.99");
	tokenServerHost = tConf.get("/main/<tokenserverhost>", "10.21.32.243");
	tokenServerPort = TC_Common::strto<int>(tConf.get("/main/<tokenserverport>", "6379"));
}

//////////////////////////////////////////////////////
void MsgImp::destroy()
{
	//destroy servant here:
	//...
}

//////////////////////////////////////////////////////
// 获取推送消息提醒
//////////////////////////////////////////////////////
taf::Int32 MsgImp::getPushMsg(const QUAN::PushMsgReq& tReq, QUAN::PushMsgRsp& tRsp, taf::JceCurrentPtr current)
{
    int iRet = -1;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    map<Int64, MsgRmd> mSIds;

    // 查询消息提醒
    iRet = _tDbHandle.getMsgsRemind(tReq, mSIds);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getMsgsRemindFromDb failure!"<<endl;
        break;
    }
    LOG->debug() << __FUNCTION__<< " getMsgsRemindFromDb success!"<<endl;
    display(" the Msg RELATION ", mSIds);

    // 查询好友列表
    // 生成对应关系
    GetActiveFriendsReq tFrdsReq;
    GetActiveFriendsRsp tFrdsRspAll;         // 1+2度关系
    tFrdsReq.lId = tReq.tUserId.lUId;

    LOG->debug()<<__FUNCTION__<<" UserProxy->getActiveFriends UId:"<<tReq.tUserId.lUId<<endl;
    FDLOG()<<__FUNCTION__<<" UserProxy->getActiveFriends UId:"<<tReq.tUserId.lUId<<endl;
    iRet = _pUserProxy->taf_hash(tReq.tUserId.lUId)->getActiveFriends(tFrdsReq, tFrdsRspAll);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " Active getFriendsList  1 + 2 failure !"<<endl;
        break;
    }

    display(" the FriendsList : ", tFrdsRspAll.vPostFriends);

    // 生成好友关系map
    map<Int64, FriendsList> tFrdsMap = FrdsVector2Map(tFrdsRspAll.vPostFriends);
  
    // 获取消息提醒内容
    iRet = this->getMsgContent(tReq, tRsp, mSIds);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Msg Content failure !"<<endl;
        break;
    }

    // LOG->debug()<<__FUNCTION__<<" msg list: "<<JCETOSTR(tRsp.vSecrets)<<endl;

    // 修正好友关系
    iRet = this->addFrdsRelatForMsg(tReq, tRsp, mSIds, tFrdsMap);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " add Friends Relation Msg failure !"<<endl;
        break;
    }


    // 编码混淆UserId
    //encodeResponse(tRsp.vSecrets);

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = JCETOSTR(tRsp);
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
// 添加消息提醒
//////////////////////////////////////////////////////
taf::Int32 MsgImp::addMsg(const QUAN::AddMsgReq& tReq, taf::JceCurrentPtr current)
{
    int iRet = -1;
    //int platform = -1;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 评论消息提醒 
	if (tReq.iOp == CONT || tReq.iOp == FRD_RECMD)
    {
        if (tReq.vUserIds.size() != 0)
        {
            vector<Int64>::const_iterator iBegin = tReq.vUserIds.begin();
			vector<Int64>::const_iterator iEnd = tReq.vUserIds.end();
			for (vector<Int64>::const_iterator i = iBegin; i != iEnd; ++i)
            {
                // 将消息推送给评论者
				iRet = _tDbHandle.setMsgToUserId(*i, tReq.lSId, tReq.lFirstId, tReq.iOp, NOTRD);    // 评论操作， 未读
                if (iRet)
                {
                    LOG->debug() << __FUNCTION__<<" UserId: "<<*i<<" | "<<"setMsgToUserId content failure"<<endl;
                }
            }
			// 清理缓存
			this->cleanCache(tReq.vUserIds, current);
			// 云推送
			if (tReq.iOp == CONT && tReq.tUserId.lUId != tReq.lFirstId)
			{
				UserId tUserId;
				tUserId = tReq.tUserId;
				tUserId.lUId = tReq.lFirstId;
				this->pushMsg(tUserId, tReq.lSId, tReq.lFirstId, tReq.iOp, "false", current);
			}
			else
			{
				for (vector<Int64>::const_iterator i = iBegin; i != iEnd; ++i)
				{
					UserId tUserId;
					tUserId = tReq.tUserId;
					tUserId.lUId = *i;
					this->pushMsg(tUserId, tReq.lSId, tReq.lFirstId, tReq.iOp, "false", current);
				}
			}
        }
        else
        {
            LOG->debug() << __FUNCTION__<<" the size is 0 set Msg To UserId don't Need "<<endl;
        }
    }  
    // 点赞消息提醒
    else if (tReq.iOp == FAVOR)
    {
        // 将消息推送给帖子作者
        iRet = _tDbHandle.setMsgToUserId(tReq.lFirstId, tReq.lSId, tReq.lFirstId, FAVOR, NOTRD);    // 点赞操作， 未读
        if (iRet)
        {
            LOG->debug() << __FUNCTION__<<" UserId: "<<tReq.tUserId.lUId<<" | "<<"setMsgToUserId secret failure"<<endl;
        }

        LOG->debug() << __FUNCTION__<<" UserId: "<<tReq.tUserId.lUId<<" | "<<"setMsgToUserId secret success"<<endl;

		// 云推送
		UserId tUserId;
		tUserId = tReq.tUserId;
		tUserId.lUId = tReq.lFirstId;
		this->pushMsg(tUserId, tReq.lSId, tReq.lFirstId, tReq.iOp, "false", current);
    }
    else
    {
        LOG->debug() << __FUNCTION__<<" OP: "<<tReq.iOp<<" | "<<"other failure!"<<endl;
        iRet = FAILURE;
        break;
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 改变消息提醒状态
//////////////////////////////////////////////////////
taf::Int32 MsgImp::changeMsgStatus(const QUAN::ChangeMsgReq & tReq,taf::JceCurrentPtr current)
{
    int iRet = -1;
    //int platform = -1;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    // 修改消息提醒状态 已读 或者 未读
    iRet = _tDbHandle.setMsgToUserId(tReq.tUserId.lUId, tReq.lSId, tReq.lFirstId, tReq.iType, tReq.iOp);  
    if (iRet)
    {
        LOG->debug() << __FUNCTION__<<" UserId: "<<tReq.tUserId.lUId<<" | "<<"setMsgToUserId change read failure"<<endl;
    }

    LOG->debug() << __FUNCTION__<<" UserId: "<<tReq.tUserId.lUId<<" | "<<"setMsgToUserId change read success"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;

    return iRet;
}


//////////////////////////////////////////////////////
//  查询消息提醒数量
//////////////////////////////////////////////////////
taf::Int32 MsgImp::getMsgNum(const QUAN::GetMsgNumReq & tReq,QUAN::GetMsgNumRsp &tRsp,taf::JceCurrentPtr current)
{
    Int32 iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    PROC_BEGIN
    __TRY__

    map<Int64, MsgRmd> mSIds;

    // 查询消息提醒
    PushMsgReq   tTmp;
    tTmp.tUserId = tReq.tUserId;
    iRet = _tDbHandle.getMsgsRemind(tTmp, mSIds);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getMsgsRemindFromDb failure!"<<endl;
        break;
    }

    display(" the Remind RELATION: ", mSIds);

    LOG->debug() << __FUNCTION__<< " getMsgsRemindFromDb success!"<<endl;

    // 修正未读结果
    vector<NotifyPost> vSecretsId;
    vector<SecretInfo> vSecrets;
    for (map<Int64, MsgRmd>::iterator i = mSIds.begin(); i != mSIds.end(); ++i)
    {
		if (i->second.isRead == false)
		{
			NotifyPost tTmp;
			tTmp.lSId = i->first;
			tTmp.iNotifyType = i->second.iOp;
			vSecretsId.push_back(tTmp);
		}
    }

    iRet = _pPostProxy->taf_hash(tReq.tUserId.lUId)->getBatchNotifySecrets(tReq.tUserId.lUId, vSecretsId, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " _pPostProxy->getBatchSecrets failure!"<<endl;
        break;
    }

	display(" mSIds :", vSecretsId);

    tRsp.iCount = vSecrets.size();

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " the..............MsgsRemind NUM is: "<<tRsp.iCount<<endl;
    FDLOG() << __FUNCTION__<< " the..............MsgsRemind NUM is: "<<tRsp.iCount<<endl;
   
    tRsp.iStatus = iRet;
    return iRet;
}

//////////////////////////////////////////////////////
//  查询消息提醒内容
//////////////////////////////////////////////////////
taf::Int32 MsgImp::getMsgContent(const QUAN::PushMsgReq& tReq,QUAN::PushMsgRsp &tRsp, map<Int64, MsgRmd> const& mSIds)
{
    Int32 iRet = FAILURE;

    PROC_BEGIN
    __TRY__

	vector<NotifyPost>  vNotifyPost;

    for (map<Int64, MsgRmd>::const_iterator i = mSIds.begin(); i != mSIds.end(); ++i)
    {
		if (i->second.isRead == false)
		{
			NotifyPost  tTmp;
			tTmp.lSId = i->first;
			tTmp.iNotifyType = i->second.iOp;
            vNotifyPost.insert(vNotifyPost.begin(), tTmp); 
		}
		else
		{
			NotifyPost  tTmp;
			tTmp.lSId = i->first;
			tTmp.iNotifyType = i->second.iOp;
			vNotifyPost.push_back(tTmp);
		}
    }

    LOG->debug()<<__FUNCTION__<<" getBatchNotifySecrets UId:"<<tReq.tUserId.lUId<<endl;
    FDLOG()<<__FUNCTION__<<" getBatchNotifySecrets UId:"<<tReq.tUserId.lUId<<endl;
    iRet = _pPostProxy->taf_hash(tReq.tUserId.lUId)->getBatchNotifySecrets(tReq.tUserId.lUId, vNotifyPost, tRsp.vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " _pPostProxy->getBatch Notify Secrets failure! NOT READ "<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " _pPostProxy->getBatchSecrets success!"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    return iRet;
}

//////////////////////////////////////////////////////
// 取树洞列表
//////////////////////////////////////////////////////
taf::Int32 MsgImp::getCirclesList(PushMsgReq const& tReq, map<Int64, CircleInfo>& mCircles)
{
    Int32 iRet;

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    GetCircleReq   tCircleReq;
    GetCircleRsp   tCircleRsp;

	tCircleReq.tUserId = tReq.tUserId;
	tCircleReq.lCircleType = ENearBy | EConcern;
	tCircleReq.sCoord = tReq.sCoord;

    iRet = _pCircleProxy->taf_hash(tReq.tUserId.lUId)->getCircle(tCircleReq, tCircleRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "get Circle failure !"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<  "get Circle SUCCESS !"<<endl;

    if (!tCircleRsp.vCircleGroup.empty())
    {
		for (vector<CircleGroup>::iterator i = tCircleRsp.vCircleGroup.begin(); i != tCircleRsp.vCircleGroup.end(); ++i)
		{
			for (vector<CircleInfo>::iterator j = i->vCircleInfo.begin(); j != i->vCircleInfo.end(); ++j)
			{
				mCircles.insert(make_pair(j->lCircleId, *j));
			}
			LOG->debug() << __FUNCTION__ << " Concern Circle: " << JCETOSTR(*i) << endl;
		}
    }


    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    LOG->debug() <<endl;

    //return iRet;
    return SUCCESS;
}

//////////////////////////////////////////////////////
//  添加好友关系
//////////////////////////////////////////////////////
taf::Int32 MsgImp::addFrdsRelatForMsg(const QUAN::PushMsgReq& tReq,QUAN::PushMsgRsp &tRsp, map<Int64, MsgRmd> const& mSIds, map<Int64, FriendsList> const& tFrdsMap)
{
    Int32 iRet = FAILURE;

	PROC_BEGIN
	__TRY__


	// 版本区分，区分色块
	vector<string> vUaTmp;
	split(tReq.tUserId.sUA, "&", vUaTmp);
	Int32  iVersion = 0;
	if (vUaTmp.size() >= 2)
	{
		iVersion = TC_Common::strto<Int32>(vUaTmp[1]);
	}

    // 获取附近帖子列表
    map<Int64, Int64>    mScrtMaps;
	iRet = this->getNearSecrets(tReq, mScrtMaps);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Near Secrets failure! "<<endl;
    }

	map<Int64, CircleInfo> mCircles;
	iRet = this->getCirclesList(tReq, mCircles);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Circles failure! "<<endl;
    }

	Int32 iCount = 0;
    if (tRsp.vSecrets.size() > 0)
    {
		sort(tRsp.vSecrets.rbegin(), tRsp.vSecrets.rend());

        for (vector<SecretInfo>::iterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end(); ++i)
        {
            if (i->lUId == tReq.tUserId.lUId)
            {
                i->iType = ME;
				i->sLocal = Near_Address;
            }
            else
            {
                map<Int64, FriendsList>::const_iterator pos = tFrdsMap.find(i->lUId);
                if (pos != tFrdsMap.end())
                {
                    i->iType = pos->second.iRelat;
					i->sLocal = Near_Address;
                }
				else
				{
					map<Int64, FriendsList>::const_iterator pos1 = tFrdsMap.find(i->lShareId);
					if (pos1 != tFrdsMap.end())
					{
                        i->iType = FFRIEND;
						i->sLocal = Near_Address;
					}
					else
					{
						map<Int64, Int64>::iterator mpos = mScrtMaps.find(i->lSId);
						if (mpos != mScrtMaps.end())
						{
							if (mpos->second < 500)
							{
								i->sLocal = Near_500;
							}
							if (mpos->second < 100)
							{
								i->sLocal = Near_100;
							}
						}
						i->iType = RECMD;
					}
				}
			}

			// 树洞权限设置
			if (i->lCircleId != 0)
			{
				map<Int64, CircleInfo>::const_iterator itCrl = mCircles.find(i->lCircleId);
				if (itCrl != mCircles.end())
				{
					// 关注并且可以发布 但是是推荐来的
					if (itCrl->second.lCircleType == (EConcern | EAccessable) && (i->iType == RECMD))
					{
						i->iType = CIRCLE;
						i->sLocal = itCrl->second.sName;
					}
					// 关注并且可以发布 但是是推荐来的
					else if (itCrl->second.lCircleType == (EConcern | ENearBy) && (i->iType == RECMD))
					{
						i->iType = CIRCLE;
						i->sLocal = itCrl->second.sName;
					}
					// 关注不可以发布 但是是推荐来的
					else if (itCrl->second.lCircleType == EConcern && (i->iType == RECMD))
					{
						i->iType = CRCLCK;
						i->sLocal = itCrl->second.sName;
					}
					// 推荐来的 但是没有关注 理论上不会有这种情况
					else if (i->iType == RECMD)
					{
					}
					// 有关系， 自己发的，朋友发的
					else
					{
						i->sLocal = string(Near_Address) + "," + itCrl->second.sName;
					}
				}
			}

			// 版本区分色块id
			if (i->iColorId > 15 && iVersion < 200)
			{
				i->sBitUrl = "http://s4.dwstatic.com/quan/14/5E/145e34e2bbf8d8ef307a0e8bb0d192ce5264.jpg";
			}

            map<Int64, MsgRmd>::const_iterator sit= mSIds.find(i->lSId);
            if (sit != mSIds.end())
            {
                i->isRead=sit->second.isRead;
				if (sit->second.iOp == FRD_RECMD)
				{
					i->iNtfyType = AT_MSG;
				}
				else if (sit->second.iOp == FAVOR)
				{
					i->iNtfyType = AT_FVR;
				}
				else
				{
					i->iNtfyType = AT_CMT;
				}
            }
			else
			{
				i->isRead = true;
				i->iNtfyType = AT_CMT;
			}

			if (i->isRead == false)
			{
				iCount++;
			}
        }

        // 把已读信息移到后面
        stable_sort(tRsp.vSecrets.begin(),tRsp.vSecrets.end(),LessIsRead());

		if (iCount == 0)
		{
			sort(tRsp.vSecrets.rbegin(), tRsp.vSecrets.rend());
		}

        if (tRsp.vSecrets.size() > size_t(tReq.iCount))
            tRsp.vSecrets.resize(tReq.iCount);
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    return iRet;
}

//////////////////////////////////////////////////////
// @消息
//////////////////////////////////////////////////////
taf::Int32 MsgImp::atMsg(const QUAN::AtMsgReq & tReq,taf::JceCurrentPtr current)
{
	// 输出参数
	string sLog = JCETOSTR(tReq);
	LOG->debug() << endl;
	LOG->debug() << "--------------------now  access-------------------------------------" << endl;
	LOG->debug() << __FUNCTION__ << " begin " << sLog << endl;
	FDLOG() << endl;
	FDLOG() << "--------------------now  access-------------------------------------" << endl;
	FDLOG() << __FUNCTION__ << " begin " << sLog << endl;
    
    // 添加消息提醒 
	AddMsgReq tAddMsgReq;
	tAddMsgReq.tUserId = tReq.tUserId;          // 用户ID
	tAddMsgReq.lSId = tReq.lSId;                // 帖子ID
	tAddMsgReq.iOp = FRD_RECMD;                 // 朋友推荐
	tAddMsgReq.vUserIds = tReq.vUserIds;
	tAddMsgReq.lFirstId = 0;
	this->addMsg(tAddMsgReq, current);

	LOG->debug() << __FUNCTION__ << " ----------------------exit------------------- "<< endl;
	FDLOG() << __FUNCTION__ << " ----------------------exit------------------- " << endl;
    
    return SUCCESS;
}

//////////////////////////////////////////////////////
// 绑定云推送
//////////////////////////////////////////////////////
taf::Int32 MsgImp::bindMsg(const QUAN::BindMsgReq & tReq,taf::JceCurrentPtr current)
{
	Int32 iRet = FAILURE;

	// 输出参数
	string sLog = JCETOSTR(tReq);
	LOG->debug() << endl;
	LOG->debug() << "--------------------now  access-------------------------------------" << endl;
	LOG->debug() << __FUNCTION__ << " begin " << sLog << endl;
	FDLOG() << endl;
	FDLOG() << "--------------------now  access-------------------------------------" << endl;
	FDLOG() << __FUNCTION__ << " begin " << sLog << endl;

	PROC_BEGIN
	__TRY__

	// 绑定云推送
	UidKey tUidKey;
	UidValue tUidValue;
	tUidKey.uid = tReq.tUserId.lUId;
	if (tReq.tUserId.iAppId != 0)
		tUidKey.appid = 3;
	else
		tUidKey.appid = E_SECRET;
	tUidValue.channelid = tReq.channelid;
	string::size_type iIdx = tReq.tUserId.sUA.find("ios");
	if (iIdx == string::npos)
	{
		tUidValue.ePlatform = E_ANDROID;
	}
	else
	{
		tUidValue.ePlatform = E_IOS;
	}
	tUidValue.userid = tReq.sId;
	
	size_t iNum = _pPushProxy->getEndpoint4All().size();
	for (size_t i = 0; i < iNum; i++)
	{
		if (tReq.iType == MSG_BIND)
		{
#if 0
			// 推送消息
			PushMsgValue tValue;
			Int32 iChkRet = _tDbHandle.checkPushMsg(tReq.tUserId, tValue);
			if (iChkRet == 1 && tValue.sOs == "ios" && !tValue.sToken.empty() && tReq.sId != tValue.sToken)
			{
				this->pushMsg(tReq.tUserId, 0, 0, 0, "relogin", current);
			}
#endif
			_tDbHandle.setPushMsg(tReq.tUserId, 1, tReq.sId);
			_pPushProxy->taf_hash(i)->async_bind(NULL, tUidKey, tUidValue);
		}
		else
		{
			_tDbHandle.setPushMsg(tReq.tUserId, 2, tReq.sId);
			_pPushProxy->taf_hash(i)->async_unbind(NULL, tUidKey);
		}
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

	LOG->debug() << __FUNCTION__ << " ----------------------exit------------------- "<< endl;
	FDLOG() << __FUNCTION__ << " ----------------------exit------------------- " << endl;

	return iRet;
}

//////////////////////////////////////////////////////
// 云推送 
//////////////////////////////////////////////////////
taf::Int32 MsgImp::pushMsg(UserId const& tUserId, Int64 lSId, Int64 lFirstId, Int32 iOp, string const& sIsSys, taf::JceCurrentPtr current, Int64 senderId, Int64 postId, Int64 commentId, char const* sContent, char const* sSource, char const* sBadge, char const* sSmallpicurl)
{
	Int32 iRet = FAILURE;

	PROC_BEGIN
	__TRY__

	// 云推送
	UidKey tUidKey;
	tUidKey.uid = tUserId.lUId;
	if (tUserId.iAppId != 0)
		tUidKey.appid = 3;
	else
		tUidKey.appid = E_SECRET;
	MDW::Msg    tMsg;
	PushMsgReq tMsgReq;
	PushMsgRsp tRsp;
	tMsgReq.tUserId = tUserId;
	tMsgReq.iCount = 20;
	iRet = this->getPushMsg(tMsgReq, tRsp, current);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Msg Num failure!"<<endl;
        break;
    }

	vector<SecretInfo>::iterator pos = find_if(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), EqualSId(lSId));
	if (sIsSys == "false" && pos == tRsp.vSecrets.end())
	{
		LOG->debug() << __FUNCTION__<< " Not Exist async push Msg UId: "<<tUserId.lUId<<" lSId: "<<lSId<<endl;
		return SUCCESS;
	}

	vector<SecretInfo>::difference_type iCount = count_if(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), EqualisRead(false));

	string sNum;
	if (sIsSys == "true")
	{
		sNum = TC_Common::tostr(0) + " " + TC_Common::tostr(0) + " " + TC_Common::tostr(0) + " " + TC_Common::tostr(0) + " 0 " + sIsSys;
	}
	else if (sIsSys == "IM")
	{
		sNum = TC_Common::tostr(0) + " " + "ABC" + " " + TC_Common::tostr(0) + " " + TC_Common::tostr(0) + " 0 " + "true" + " " + "IM " + TC_Common::tostr(senderId) + " " + TC_Common::tostr(postId) + " " + TC_Common::tostr(commentId) + " " + sSource;
	}
	else if (sIsSys == "relogin")
	{
		sNum = TC_Common::tostr(0) + " " + "ABC" + " " + TC_Common::tostr(0) + " " + TC_Common::tostr(0) + " 0 " + "true" + " " + "relogin";
	}
	else
	{
		if (pos->sLocal.empty())
		{
			pos->sLocal = "中国";
		}
		sNum = TC_Common::tostr(iCount) + " " + TC_Common::tostr(lSId) + " " + TC_Common::tostr(encodeUId(pos->lUId)) + " " + TC_Common::tostr(pos->iType) + " " + pos->sLocal + " " + sIsSys;
	}

	tMsg.msg.assign(sNum.begin(), sNum.end());
	tMsg.expires = 5 * 3600;
	if (iOp == FRD_RECMD)
	{
		tMsg.desc = "有一个好友提醒你查看秘密！";
	}
	else
	{
		tMsg.desc = "您收到一条新评论！";
	}

	if (sIsSys == "true")
	{
		tMsg.desc = "您收到一条系统通知！";
	}
	else if (sIsSys == "relogin")
	{
		tMsg.desc = "您的账号在其他设备上登录！";
	}
	else if (sIsSys == "IM")
	{
		tMsg.desc = sContent;
	}

    LOG->debug() << __FUNCTION__<< " async push Msg UId: "<<tUserId.lUId<<" iCount: "<<iCount<<" content: "<<sNum<<endl;
	if (tRsp.iStatus == SUCCESS && (iCount != 0 || sIsSys == "true" || sIsSys == "IM" || sIsSys == "relogin"))
	{
		FDLOG() << __FUNCTION__<< " async push Msg UId: "<<tUserId.lUId<<" iCount: "<<iCount<<" content: "<<sNum<<endl;

		// new push
		PushMsgValue tValue;
		Int32 iChkRet = _tDbHandle.checkPushMsg(tUserId, tValue);
		if (iChkRet == 1)
		{
			Int32 iNum = 0;
			getImNum(tUserId, iNum);
			string sB = sBadge;
			if (strlen(sBadge) == 0)
			{
				sB = TC_Common::tostr(iNum);
			}
			string sUrl = g_app._sPushUrl + "appId=quan&deviceToken=" + tValue.sToken + "&osType=" + tValue.sOs + "&content=" + urlencode(tMsg.desc) + "&badge=" + sB + "&custom=" + urlencode("{\"content\":\"" + TC_Base64::encode(sNum) + "\", \"smallpicurl\":\"" + TC_Base64::encode(sSmallpicurl) + "\"}");
			TC_HttpAsync::RequestCallbackPtr p = new AsyncHttpCallback(sUrl);
			g_app.getMsgHttp().doAsyncRequest(sUrl, p);
			LOG->debug() << " URL: " << sUrl << endl;
			FDLOG() << " URL: " << sUrl << endl;
		}
		else if (iChkRet == 2)
		{
			FDLOG() << __FUNCTION__ << " don't async push Msg " << endl;
			LOG->debug() << __FUNCTION__ << " don't async push Msg " << endl;
		}
		else
		{
			FDLOG() << __FUNCTION__ << " old async push Msg " << endl;
			LOG->debug() << __FUNCTION__ << " old async push Msg " << endl;
			_pPushProxy->taf_hash(tUserId.lUId)->async_pushMsg(NULL, tUidKey, tMsg);
		}
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

	return iRet;
}

taf::Int32 MsgImp::cleanCache(const vector<Int64> & vUserId,taf::JceCurrentPtr current)
{
	Int32 iRet = FAILURE;

    // 输出参数
    string sLog = simpledisplay(vUserId);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	PROC_BEGIN
	__TRY__

	iRet = _tDbHandle.cleanCache(vUserId);
	if (iRet == FAILURE)
	{
		LOG->debug() <<__FUNCTION__<<" Msg clean Cache FALURE"<<endl;
		break;
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

	return iRet;
}

//////////////////////////////////////////////////////
// 获取附近列表
//////////////////////////////////////////////////////
taf::Int32 MsgImp::getNearSecrets(PushMsgReq const& tReq, map<Int64, Int64>& vMaps)
{
	int iRet = FAILURE;
	LOG->debug() << __FUNCTION__ << " begin " << endl;

	PROC_BEGIN
		__TRY__

	if (tReq.sCoord.empty())
	{
		return SUCCESS;
	}

	LbsReq tLbsReq;
	LbsRsp tLbsRsp;
	if (tReq.sCoord.find(",") != string::npos)
	{
		string sLat = tReq.sCoord.substr(0, tReq.sCoord.find(",") + 1);
		string sLnt = tReq.sCoord.substr(tReq.sCoord.find(",") + 1);
		tLbsReq.dLat = TC_Common::strto<Double>(sLat);
		tLbsReq.dLng = TC_Common::strto<Double>(sLnt);
		LOG->debug() << __FUNCTION__ << "coords :" << tLbsReq.dLat << " | " << tLbsReq.dLng << endl;
	}
	else
	{
		LOG->debug() << __FUNCTION__ << "no coords !" << endl;
		return SUCCESS;
	}

	tLbsReq.lId = tReq.tUserId.lUId;

	LOG->debug() << __FUNCTION__ << " Lbs Request :" << tReq.tUserId.lUId << endl;
	iRet = _pLbsProxy->taf_hash(tReq.tUserId.lUId)->getNearbyUser(tLbsReq, tLbsRsp);
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << "get Lbs Request failure !" << endl;
		iRet = FAILURE;
		break;
	}

	for (vector<LbsUser>::iterator i = tLbsRsp.vId.begin(); i != tLbsRsp.vId.end(); ++i)
	{
		vMaps.insert(make_pair(i->lId, i->iDistance * 1000));
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

	LOG->debug() << __FUNCTION__ << " end iRet:" << iRet << endl;

	return iRet;
}

taf::Int32 MsgImp::getSysMsg(const QUAN::GetSysMsgReq & tReq,QUAN::GetSysMsgRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	PROC_BEGIN
	__TRY__

	// 获取系统消息
	iRet = _tDbHandle.getSysMsgList(tReq, tRsp.vSysMsgInfo);
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " get System Message failure!" << endl;
		break;
	}


	// 更新系统消息状态
	iRet = _tDbHandle.setSysMsgReaded(tReq.tUserId.lUId);
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " set Sys Msg Readed failure!" << endl;
		break;
	}
/*
	for (vector<SysMsgInfo>::iterator i = tRsp.vSysMsgInfo.begin(); i != tRsp.vSysMsgInfo.end(); ++i)
	{
		CircleInfo tTmp;
		_pCircleProxy->taf_hash(tReq.tUserId.lUId)->getCircleInfo(tReq.tUserId, i->lCircleId, "0,0", tTmp);
		LOG->debug() << " name " <<tTmp.sName<<endl;
		i->sContent = tTmp.sName;
	}
*/

	sort(tRsp.vSysMsgInfo.begin(), tRsp.vSysMsgInfo.end(), [](SysMsgInfo const& e1, SysMsgInfo const& e2) { return e1.lId > e2.lId; });

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    return iRet;
}

taf::Int32 MsgImp::pushSysMsg(const QUAN::PushSysMsgReq & tReq,taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	PROC_BEGIN
	__TRY__

	SysMsgInfo tSysMsg = tReq.tSysMsgInfo;

	// tSysMsg.sContent = TC_Base64::decode(tSysMsg.sContent);

	// 插入系统消息
	iRet = _tDbHandle.setSysMsg(tReq.tUserId.lUId, tSysMsg);
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " set System Message failure!" << endl;
		break;
	}
 
	// 推送消息
	this->pushMsg(tReq.tUserId, 0, 0, 0, "true", current);
#if 0
	// 推送消息
	UserId tUserId;
	tUserId = tReq.tUserId;
	tUserId.iAppId = 3;
	this->pushMsg(tUserId, 0, 0, 0, "true", current);
#endif

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    return iRet;
}

taf::Int32 MsgImp::cleanSysCache(taf::Int64 tUserId, taf::JceCurrentPtr current)
{
	Int32 iRet = FAILURE;

    // 输出参数
    string sLog = TC_Common::tostr(tUserId);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	PROC_BEGIN
	__TRY__

	iRet = _tDbHandle.cleanSysCache(tUserId);
	if (iRet == FAILURE)
	{
		LOG->debug() <<__FUNCTION__<<" Msg clean Cache FALURE"<<endl;
		break;
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

	return iRet;

}

taf::Int32 MsgImp::getNewSysMsg(const QUAN::GetNewSysMsgReq & tReq, QUAN::GetNewSysMsgRsp &tRsp, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    string sLog = JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	PROC_BEGIN
	__TRY__

	// 获取系统消息
	GetSysMsgReq tSReq;
	tSReq.tUserId = tReq.tUserId;
	vector<SysMsgInfo> vSysMsgInfo;
	iRet = _tDbHandle.getSysMsgList(tSReq, vSysMsgInfo);
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " get System Message failure!" << endl;
		break;
	}

	tRsp.iCount = 0;
	
	for (auto i = vSysMsgInfo.begin(); i != vSysMsgInfo.end(); ++i)
	{
		if (i->bIsRead == false)
			tRsp.iCount++;
	}
	
    iRet = SUCCESS;
    __CATCH__
    PROC_END

    sLog = JCETOSTR(tRsp);
    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    FDLOG() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    return iRet;
}

taf::Int32 MsgImp::pushImMsg(const PushImMsgReq& tReq, taf::JceCurrentPtr current)
{
	int iRet = FAILURE;

	// 输出参数
	string sLog = JCETOSTR(tReq);
	LOG->debug() << endl;
	LOG->debug() << "--------------------now  access-------------------------------------" << endl;
	LOG->debug() << __FUNCTION__ << " begin " << sLog << endl;
	FDLOG() << endl;
	FDLOG() << "--------------------now  access-------------------------------------" << endl;
	FDLOG() << __FUNCTION__ << " begin " << sLog << endl;

	PROC_BEGIN
	__TRY__

	// 推送消息
	this->pushMsg(tReq.tUserId, 0, 0, 0, "IM", current, tReq.senderId, tReq.postId, tReq.commentId, tReq.sContent.c_str(), tReq.source.c_str(), tReq.sBadge.c_str(), TC_Common::tostr(tReq.msgId).c_str());
	// 推送消息
	//UserId tUserId;
	//tUserId = tReq.tUserId;
	//tUserId.iAppId = 3;
	//this->pushMsg(tUserId, 0, 0, 0, "IM", current);

	iRet = SUCCESS;
	__CATCH__
	PROC_END

	sLog = JCETOSTR(tReq);
	LOG->debug() << endl;
	LOG->debug() << __FUNCTION__ << " iRet: " << iRet << " | " << sLog << endl;
	LOG->debug() << "---------------------- now  exit -----------------------------------" << endl;
	FDLOG() << endl;
	FDLOG() << __FUNCTION__ << " iRet: " << iRet << " | " << sLog << endl;
	FDLOG() << "---------------------- now  exit -----------------------------------" << endl;

	return iRet;
}

taf::Int32 MsgImp::pushBindMsg(const QUAN::UserId & tUserId, Int32 iType, string const& sId, taf::JceCurrentPtr current)
{
	Int32 iRet = FAILURE;
	// 输出参数
	string sLog = JCETOSTR(tUserId) + "|" + TC_Common::tostr(iType) + "|" + sId;
	LOG->debug() << endl;
	LOG->debug() << "--------------------now  access-------------------------------------" << endl;
	LOG->debug() << __FUNCTION__ << " begin " << sLog << endl;
	FDLOG() << endl;
	FDLOG() << "--------------------now  access-------------------------------------" << endl;
	FDLOG() << __FUNCTION__ << " begin " << sLog << endl;

	PROC_BEGIN
	__TRY__
	if (iType == MSG_BIND)
	{
		// 推送消息
		PushMsgValue tValue;
		Int32 iChkRet = _tDbHandle.checkPushMsg(tUserId, tValue);
		LOG->debug() << __FUNCTION__<<"|"<<JCETOSTR(tValue) << endl;
		if (iChkRet == 1 && tValue.sOs == "ios" && !tValue.sToken.empty() && sId != tValue.sToken)
		{
			this->pushMsg(tUserId, 0, 0, 0, "relogin", current);
		}
		_tDbHandle.setPushMsg(tUserId, 1, sId);
	}
	iRet = SUCCESS;
	__CATCH__
	PROC_END

    return iRet;
}

int MsgImp::connectReddis()
{
	if (!tokenServerHost.empty())
	{
		try
		{
		delete rclient;
		rclient = new redis::client(tokenServerHost, tokenServerPort);
		return 0;
		}
		catch (exception& e)
		{
		LOG->error() << "RedisConnectError:" << e.what() << endl;
		rclient = NULL;
		return -1;
		}
	}
	else
	{
		LOG->error() << "RedisConnectError:" << "Server Host Empty" << endl;
		return -1;
	}
}

int MsgImp::getImNum(UserId const& tUserId, Int32& iNum)
{
	Int32 iRet = -1;

	try
	{
		if (rclient || connectReddis() >= 0)
		{
			string ret = rclient->get("nrc_" + TC_Common::tostr(tUserId.lUId));
			iRet = ret.empty() ? -1 : 0;
			iNum = TC_Common::strto<Int32>(ret);
		}
		else
		{
			LOG->error() << __FUNCTION__ << "|RedisError:" << "Connection Error" << endl;
		}
	}
	catch (exception &e)
	{
		if (connectReddis()<0) return -1;
		try
		{
			string ret = rclient->get("nrc_" + TC_Common::tostr(tUserId.lUId));
			iRet = ret.empty() ? -1 : 0;
			iNum = TC_Common::strto<Int32>(ret);
		}
		catch (exception &e)
		{
			LOG->error() << __FUNCTION__ << "|RedisError:" << e.what() << endl;
		}
	}

	return iRet;
}
