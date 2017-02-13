#include "GetSecretImp.h"
#include "servant/Application.h"
#include <algorithm>
#include <functional>
#include "ConstValue.h"
#include <boost/bind.hpp>
// #include <boost/function.hpp>
// #include "GetSecretCallback.h"

using namespace std;


//////////////////////////////////////////////////////
void GetSecretImp::initialize()
{
    //initialize servant here:
    //...
    // 用户信息句柄
    _pUserProxy = Application::getCommunicator()->stringToProxy<UserInfoPrx>("QUAN.UserInfoServer.UserInfoObj");

    // 帖子内容列表
    _pPostProxy = Application::getCommunicator()->stringToProxy<PostPrx>("QUAN.PostServer.PostObj");

    // Lbs列表
    _pLbsProxy = Application::getCommunicator()->stringToProxy<LbsPrx>("QUAN.LbsServer.LbsObj");

    // Lbs列表
    _pCircleProxy = Application::getCommunicator()->stringToProxy<CirclePrx>("QUAN.CircleServer.CircleObj");

    // 获取配置信息
    TC_Config tConf;
    __TRY__
    tConf.parseFile(ServerConfig::BasePath + "GetSecretServer.conf");
    __CATCH__

    //最大缓存帖子数
    _uMaxSecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<MaxSecretsNum>", "500"));

    // 24小时系统推荐帖子个数
    _u24SecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<24SecretsNum>", "1"));

    // 72小时系统推荐帖子个数
    _u72SecretsNum = TC_Common::strto<size_t>(tConf.get("/main/<72SecretsNum>", "5"));

    // 获取判断好友数量
    _uLockFriendsNum = TC_Common::strto<size_t>(tConf.get("/main/<LockFriendsNum>", "3"));
    // 获取好友点赞数
    _uMaxFrdsFvrNum = TC_Common::strto<size_t>(tConf.get("/main/<MaxFrdsFvrNum>", "10"));
    // 获取好友点赞数
    _uMinFrdsFvrNum = TC_Common::strto<size_t>(tConf.get("/main/<MinFrdsFvrNum>", "10"));
    // 获取系统引导帖子位置
    _uSysScrtLoc = TC_Common::strto<size_t>(tConf.get("/main/<SysScrtLoc>", "3"));
    // 获取活跃用户数量
    _uActiveFriends = TC_Common::strto<size_t>(tConf.get("/main/<ActiveFriends>", "20"));
    // 获取系统引导帖子位置
    _uTopN = TC_Common::strto<size_t>(tConf.get("/main/<topn>", "25"));
}

//////////////////////////////////////////////////////
void GetSecretImp::destroy()
{
    //destroy servant here:
    //...
}

//////////////////////////////////////////////////////
// 获取系统推荐 好友推荐
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getRecommendSecrets(SecretListReq const& tReq, Int32 iFrdsNum, FrdsVector const& vFrdsFavor, ScrtVector& vSecrets)
{
    int iRet;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;
    
    ScrtVector    vRmdScrts;        // 系统推荐帖子
    ScrtVector    vFvrScrts;        // 好友赞过帖子

    // 获取系统推荐帖子        
    iRet = _tDbHandle.getRecommendSecrets(tReq.tUserId, _u24SecretsNum, _u72SecretsNum, vRmdScrts);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Sys RecommendSecrets failure! "<<endl;
        return iRet;
    }

    LOG->info() << __FUNCTION__<< " get Sys RecommendSecrets success! "<<endl;

    // 判断好友数量
    if (size_t(iFrdsNum) < _uLockFriendsNum)
    {
        //获取1个好友赞过的帖子列表 1度关系 最多20条
        iRet = this->getBatchUserFavorSecrets(tReq.tUserId.lUId, tReq.iCount, _uMinFrdsFvrNum, vFrdsFavor, vFvrScrts); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 1favor get Friends Favor Secrets failure! "<<endl;
            return iRet;
        }
        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        // 获取3个好友赞过的帖子列表 1度关系 最多20条 
        iRet = this->getBatchUserFavorSecrets(tReq.tUserId.lUId, tReq.iCount, _uMaxFrdsFvrNum, vFrdsFavor, vFvrScrts); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 3favor get Friends Favor Secrets failure! "<<endl;
            return iRet;
        }
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
    }

    display(" Friends Favor Vector Secrets : ", vFvrScrts);

    if (tReq.tRange.lBeginId != SECRET_ID_BEGIN)
    {
        vFvrScrts.erase(remove_if(vFvrScrts.begin(),  vFvrScrts.end(),  bind2nd(greater<Int64>(), tReq.tRange.lBeginId)), vFvrScrts.end()); 
        vRmdScrts.erase(remove_if(vRmdScrts.begin(),  vRmdScrts.end(),  bind2nd(greater<Int64>(), tReq.tRange.lBeginId)), vRmdScrts.end()); 
    }

    // 复制
    copy(vRmdScrts.begin(), vRmdScrts.end(), back_inserter(vSecrets));
    copy(vFvrScrts.begin(), vFvrScrts.end(), back_inserter(vSecrets));

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 获取好友帖子列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getFriendsSecrets(SecretListReq const& tReq, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, Int64& lNextBeginId, Int32& iLockCount, ScrtVector& vSecrets, Int32& iTimeSpan)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    ScrtVector    vSecretsFriends;     // 好友帖子列表

	// 检查耗时
	Int64 iBeginTime = TC_TimeProvider::getInstance()->getNowMs();

    // 如果返回朋友列表大于20个需按beginID取
    size_t iR   = 0;
    size_t iC   = 0;
    size_t iSum = 0;
    size_t iFrdsNum = tFrdsPair.first.size();
    FrdsVector  vFriends;

    LOG->debug() << __FUNCTION__<< " Friends Num "<<iFrdsNum<<" _uLockFriendsNum: "<<_uLockFriendsNum<<endl;

    if (size_t(iFrdsNum) >= _uLockFriendsNum)
    {
        iR = vFrdsAll.size() / _uTopN;
        iC = vFrdsAll.size() % _uTopN;
        iSum = iR + (iC > 0 ? 1 : 0);
        vFriends = vFrdsAll;
    }
    else
    {
        iR = tFrdsPair.second.size() / _uTopN;
        iC = tFrdsPair.second.size() % _uTopN;
        iSum = iR + (iC > 0 ? 1 : 0);
        vFriends = tFrdsPair.second;
    }

    LOG->debug() << __FUNCTION__<< " iR "<<iR<<" iC "<<iC<<" iSum "<<iSum<<endl;
	
    for (size_t i = 0; i < iSum; i++)
    {
        FrdsVector   vTmp;
        if (vFriends.size() < _uTopN * (i+1)) 
        { 
            copy(vFriends.begin() + i * _uTopN, vFriends.end(), back_inserter(vTmp));
        }
        else
        {
            copy(vFriends.begin() + i * _uTopN, vFriends.begin() + (i+1) * _uTopN, back_inserter(vTmp));
        }

        // 获取好友，好友的好友的帖子列表; 
		// vSecretsFriends.clear();
        iRet = _tDbHandle.getAllUserSecrets(vTmp, tReq.iCount, tReq.tRange.lBeginId, lNextBeginId, vSecretsFriends);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " get Friends User Secrets  failure! "<<endl;
            break;
        }
        display(" Friends Vector Secrets : ", vSecretsFriends);
		LOG->debug() << __FUNCTION__ << " get Friends User Secrets Time: "<< TC_TimeProvider::getInstance()->getNowMs() - iBeginTime<<endl;
        if (vFriends.size() > (i+1)*_uTopN)
        {
			iTimeSpan = vFriends[(i + 1)*_uTopN + 1].iTimespan;
			size_t iFrdsSize = vSecretsFriends.size();
			LOG->debug() << __FUNCTION__<<" i: "<<i<<" the time Span: "<<vFriends[(i+1)*_uTopN+1].iTimespan<< " Intev: "<< TC_TimeProvider::getInstance()->getNowMs() - iBeginTime<<endl;
            if (vFriends[(i+1)*_uTopN+1].iTimespan > 0)
            {
				iFrdsSize -= count_if(vSecretsFriends.begin(), vSecretsFriends.end(), LessTime(vFriends[(i + 1)*_uTopN + 1].iTimespan));
            }
            if (iFrdsSize >= size_t(tReq.iCount * 2))
            {
                break;
            }
        }
		else
		{
			iTimeSpan = 0;
		}
    }

    // 判断好友数量
    if (size_t(iFrdsNum) < _uLockFriendsNum)
    {
        //查询朋友的帖子数;
        iLockCount = vSecretsFriends.size();
 
        if (iFrdsNum == 0 || iLockCount == 0)
        {
            // 如果好友数为0则默认设置为1
            iLockCount = 1;
        }

        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
        // 解锁
        iLockCount = 0;
    }

    // 复制好友帖子列表
    if (!vSecretsFriends.empty())
    {
        copy(vSecretsFriends.begin(), vSecretsFriends.end(), back_inserter(vSecrets));
    }

    display(" All Friends Vector Secrets : ", vSecretsFriends);
    LOG->debug() << __FUNCTION__<< endl;
    display(" After Friends Vector Secrets : ", vSecrets);

    iRet = SUCCESS; 
    LOG->debug() << __FUNCTION__<< " FriendsTime: "<<TC_TimeProvider::getInstance()->getNowMs() - iBeginTime<<endl;
    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 取好友列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getFriendsList(UserId const& tUserId, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, FrdsVector& vFrdsAll, Int32& iRegTime)
{
    Int32 iRet;

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
    FrdsRelat(tFrdsRsp.vPostFriends);
    FrdsRelat(tFrdsRsp.vFavorFriends);

    vFrdsAll.swap(tFrdsRsp.vPostFriends);
    vFrdsFavor.swap(tFrdsRsp.vFavorFriends);

    if (!vFrdsAll.empty())
    {
        FrdsIterator iBegin = vFrdsAll.begin();
        FrdsIterator iEnd   = vFrdsAll.end();
        for (FrdsIterator i = iBegin; i != iEnd; ++i)
        {
            if (i->iRelat == FRIEND)
            {
                tFrdsPair.first.push_back(*i);
            }
			else
			{
				tFrdsPair.second.push_back(*i);
			}
        }
    }

	// 用户注册时间
	iRegTime = tFrdsRsp.iRegistTime;

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
// 取树洞列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getCirclesList(SecretListReq const& tReq, FrdsVector& vFrdsCircle, map<Int64, CircleInfo>& mCircles)
{
    Int32 iRet;

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    GetCircleReq   tCircleReq;
    GetCircleRsp   tCircleRsp;

	tCircleReq.tUserId = tReq.tUserId;
	tCircleReq.lCircleType = EConcern | ENearBy;
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
				FriendsList tTmp;
				tTmp.iRelat = CIRCLE;
				tTmp.lUId = j->lCircleId;
				vFrdsCircle.push_back(tTmp);
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
// 取好友列表 重载
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getFriendsList(SecretListReq const& tReq, UserId const& tUserId, taf::JceCurrentPtr current)
{
#if 0
    Int32 iRet;

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    GetActiveFriendsReq   tFrdsReq;
    tFrdsReq.lId = tUserId.lUId;

	GetSecretCallback  tGetSecret(*this);
	TC_AutoPtr< SharedCallback< UserInfoPrxCallback > > ptr = new SharedCallback< UserInfoPrxCallback >(current);
	ptr->setBindgetActiveFriends(boost::bind(&GetSecretCallback::getFriendsListCallback, tGetSecret, tReq, _1, _2));
    _pUserProxy->taf_hash(tUserId.lUId)->async_getActiveFriends(ptr, tFrdsReq);
	current->setResponse(false);

    iRet = SUCCESS;

    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    LOG->debug() <<endl;

    return iRet;
#endif
    return SUCCESS;
}

//////////////////////////////////////////////////////
// 对外接口: 获取帖子列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current)
{
    Int32 iRet;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    FrdsVector              vFrdsAll;              // 所有好友
    FrdsVector              vFrdsFavor;            // 点赞活跃好友
    FrdsVectorPair          tFrdsPair;             // 1 + 2 度好友
	Int32                   iRegTime;              // 注册时间
    FrdsVector              vFrdsCircle;           // 树洞列表
	map<Int64, CircleInfo>  mCircles;              // 树洞map 

	// 异步调用代码暂时不用
	// getFriendsList(tReq, tReq.tUserId, current);
	// return SUCCESS;

    PROC_BEGIN
    __TRY__

    ScrtVector    vSecrets;         // 帖子ID

    // 获取好友列表 
    iRet = this->getFriendsList(tReq.tUserId, vFrdsFavor, tFrdsPair, vFrdsAll, iRegTime);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Friends List failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Friends List success! "<<endl;

	// 获取树洞列表
	iRet = this->getCirclesList(tReq, vFrdsCircle, mCircles);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Circle List failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Circle List success! "<<endl;

    // 查询收帖箱是否有帖子
    iRet = _tDbHandle.getUserSecretListFromCache(tReq.tUserId.lUId, tReq.tRange.lBeginId, tReq.iCount, vSecrets, tRsp.iLockCount);
    if (iRet == SUCCESS)
    {
        LOG->debug() << __FUNCTION__<< " get UserSecretList From Cache SUCCESS! "<<endl;
        tRsp.lNextBeginId = vSecrets.back();
        display(" Cache Secret Accept: ", vSecrets);
    }
    else if (iRet == TIMEOUT)
    {
        LOG->debug() << __FUNCTION__<< " get Pull User Secret List From DB ! "<<endl;

		Int32 iEraseTime = 0;

        // 收帖箱取不到数据从(拉cache)或者DB拉数据
        iRet = this->getPullSecretList(tReq, tRsp, tFrdsPair, vFrdsAll, vFrdsFavor, vSecrets, vFrdsCircle, iEraseTime);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " get Pull Secret List FAILURE! "<<endl;
            break;
        }

        display(" Pull List: ", vSecrets);
        LOG->info() << __FUNCTION__<< " get Pull Secret List SUCCESS! "<<endl;

        // 将拉出来的帖子列表放入收帖箱
		if (tReq.iType == GET_SECRET_ALL)
		{
            iRet = _tDbHandle.setBatchSecretsToCache(vSecrets, tReq.tUserId.lUId, tRsp.iLockCount);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " Set Batch Secrets ToCache failure! "<<endl;
                break;
            }
            LOG->info() << __FUNCTION__<< " Set Batch Secrets ToCache success! "<<endl;
		}

		// 擦除不在此时间范围帖子
		vSecrets.erase(remove_if(vSecrets.begin(),  vSecrets.end(),  LessTime(iEraseTime)), vSecrets.end());
    }
    else
    {
        LOG->debug() << __FUNCTION__<< " get User Secret List From Cache failure! "<<endl;
        break;
    }

    // 重新排序分页
    iRet = this->uniqueSecretList(tReq, tRsp, tFrdsPair, vFrdsAll, mCircles, vSecrets, iRegTime);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " unique Secret List FAILURE! "<<endl;
        break;
    }

    // 限制长度
    if (tRsp.vSecrets.size() >= size_t(tReq.iCount))
    {
        tRsp.vSecrets.resize(tReq.iCount);
        if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }

	// stable_sort(tRsp.vSecrets.begin(), tRsp.vSecrets.end(), GreatShareId());

    // 编码混淆UserId
    // encodeResponse(tRsp.vSecrets);

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
taf::Int32 GetSecretImp::getPullSecretList(const QUAN::SecretListReq & tReq,
                                           QUAN::SecretListRsp &tRsp, 
                                           FrdsVectorPair const& tFrdsPair, 
                                           FrdsVector const& vFrdsAll, 
                                           FrdsVector const& vFrdsFavor, 
                                           ScrtVector& vSecrets,
										   FrdsVector const& vFrdsCircle,
										   Int32& iEraseTime)
{
    Int32 iRet;

    // 输出参数
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__
#if 0
    // 将用户加入好友列表
    FrdsVector vMes;
    FriendsList tMe;
    tMe.lUId = tReq.tUserId.lUId;
    tMe.iRelat  = ME;
    vMes.push_back(tMe);

    // 获取自己的帖子列表;  
    iRet = _tDbHandle.getAllUserSecrets(vMes, tReq.iCount, tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets MySelf failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Secrets MySelf success! "<<endl;

    display(" MySelf Vector Secrets : ", vSecrets);
#endif

    FriendsList tMe;
    tMe.lUId = tReq.tUserId.lUId;
    tMe.iRelat  = ME;
	FrdsVector vFrdsAllTmp;
	vFrdsAllTmp = vFrdsAll;
	vFrdsAllTmp.insert(vFrdsAllTmp.begin(), tMe);

    // 获取好友帖子列表
	Int32 iTimeSpan = 0;
    Int32 iFrdsNum = tFrdsPair.first.size();
    iRet = this->getFriendsSecrets(tReq, tFrdsPair, vFrdsAllTmp, tRsp.lNextBeginId, tRsp.iLockCount, vSecrets, iTimeSpan);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets Friends failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Friends Secrets success! "<<endl;

    // 获取树洞帖子列表
    iRet = _tDbHandle.getAllUserSecrets(vFrdsCircle, tReq.iCount, tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets Circle failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Secrets Circle success! "<<endl;

    display(" Circles Vector Secrets : ", vSecrets);

    // 获取系统,好友推荐
    iRet = this->getRecommendSecrets(tReq, iFrdsNum, vFrdsFavor, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Recommend Secrets failure! "<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " get Recommend Secrets success! "<<endl;


    // 城市帖子 
    iRet = this->getCitySecrets(tReq, vSecrets);

    LOG->debug() << __FUNCTION__<< " get City Secrets success! "<<endl;


	vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), LessTime(iTimeSpan)), vSecrets.end());

	iEraseTime = iTimeSpan;

    // 排序 
    stable_sort(vSecrets.rbegin(),  vSecrets.rend());
    // 去重
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());
  
    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 重新排列帖子列表分页
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::uniqueSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, map<Int64, CircleInfo> const& mCircles, ScrtVector& vSecrets, Int32 iRegTime)
{
    int iRet = FAILURE;

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

	// 设置朋友数
	tRsp.iFriends = tFrdsPair.first.size();

	// 记录用户活跃情况
	if (tReq.tRange.lBeginId == SECRET_ID_BEGIN)
		_tDbHandle.setActiveUser(tReq.tUserId);

	// 解锁1个朋友操作
	if (tFrdsPair.first.size() >= 1 && tRsp.iLockCount != 0)
	{
        // 2个小时后自动解锁 
		time_t iTime = TC_TimeProvider::getInstance()->getNow();
        LOG->debug() << __FUNCTION__<< " now Time :"<<iTime<<" RegTime: "<<iRegTime<<endl;
		if (iTime - iRegTime > 7200)
		{
			tRsp.iLockCount = 0;
		}
	}

	// 获取附近帖子列表
   	ScrtMap    mScrtMaps;
   	iRet = this->getNearSecrets(tReq, mScrtMaps, vSecrets);
   	if (iRet == FAILURE)
   	{
       	LOG->debug() << __FUNCTION__<< " get Near Secrets failure! "<<endl;
		iRet = SUCCESS;
   	}

    LOG->info() << __FUNCTION__<<  "tran SecretsId SUCCESS !"<<endl;

    // 排序 
    stable_sort(vSecrets.rbegin(), vSecrets.rend());
    // 去重
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());
  
    // 限制条数
    if (!vSecrets.empty())
    {
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN || vSecrets[0] < tReq.tRange.lBeginId)
        {
        }
        else
        {
            vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), bind2nd(greater_equal<Int64>(), tReq.tRange.lBeginId)), vSecrets.end());
        }
		if (!vSecrets.empty())
		{
			tRsp.lNextBeginId = vSecrets.back();
		}
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    if (!vSecrets.empty())
    {
        display(" the SECRET ID: ", vSecrets);

        FrdsMap mFrds = Vector2Map(vFrdsAll);
        FrdsMap mFrdsFirst = Vector2Map(tFrdsPair.first);

        // 帖子ID转换内容
        iRet = this->tranSecretsId(tReq, tRsp, vSecrets, mFrdsFirst);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " tran SecretsId failure! "<<endl;
            break;
        }

        vector<SecretInfo> vTmp;
        vector<SecretInfo> vFriends;
		display(" the Friends List ", mFrds);
        for (vector<SecretInfo>::iterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end(); ++i)
        {
            if (i->lUId == tReq.tUserId.lUId)
            {
                i->iType = ME;
				i->sLocal = Near_Address;
                vTmp.push_back(*i);
            }
            else
            {
                FrdsMapIterator pos = mFrds.find(i->lUId);
                if (pos != mFrds.end())
                {
					LOG->info() << __FUNCTION__ << "Friends : " << i->lUId << " relation "<< pos->second.iRelat << endl;
                    if (tRsp.iLockCount == 0 && pos->second.iRelat == FRIEND)
                    {
                        i->iType = FRIEND;
						i->sLocal = Near_Address;
                        vTmp.push_back(*i);
                        vFriends.push_back(*i);
                    }
                    else if (pos->second.iRelat == FFRIEND)
                    {
                        i->iType = FFRIEND;
						i->sLocal = Near_Address;
                    }
                }
                else
                {
					FrdsMapIterator pos1 = mFrds.find(i->lShareId);
					if (pos1 != mFrds.end())
					{
                        i->iType = FFRIEND;
						i->sLocal = Near_Address;
					}
					else
					{
						ScrtMapIterator mpos = mScrtMaps.find(i->lSId);
                    	if (mpos != mScrtMaps.end())
                    	{
							LOG->info() << __FUNCTION__<<" Near  "<<mpos->first<<endl;
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

			// 版本区分色块id;
			if (i->iColorId > 15 && iVersion < 200)
			{
				i->sBitUrl = "http://s1.dwstatic.com/quan/secrets/l" + TC_Common::tostr(i->iColorId) + ".jpg";
			}
        }

		// 推荐贴删除空地理位置
        tRsp.vSecrets.erase(remove_if(tRsp.vSecrets.begin(),  tRsp.vSecrets.end(),  NullLocation()), tRsp.vSecrets.end()); 

        // 类型为朋友则只取朋友
        if (tReq.iType == GET_SECRET_FRIENDS)
        {
            tRsp.vSecrets.swap(vTmp);
        }

		if (!tRsp.vSecrets.empty())
		{
			tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
		}
		else
		{
			tRsp.lNextBeginId = SECRET_ID_END;
		}

		// 设置解锁数
		if (tRsp.iLockCount != 0 && tFrdsPair.first.size() < _uLockFriendsNum)
		{
			tRsp.iLockCount = vFriends.size();
			if (tRsp.iLockCount == 0)
				tRsp.iLockCount = 1;
		}

        // 获取系统引导帖子        
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN && tRsp.iLockCount == 0 && tReq.iType != GET_SECRET_FRIENDS)
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
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 获取用户赞帖列表
//////////////////////////////////////////////////////
Int32 GetSecretImp::getBatchUserFavorSecrets(Int64 lUId, Int32 iSum, Int32 iFrdsFvrNum, FrdsVector const& vFrds, ScrtVector& vSecrets)
{
    int iRet;

    PROC_BEGIN
    __TRY__

    vector<map<Int64, Int32> > vOp;
    iRet = _pPostProxy->taf_hash(lUId)->getUserOpBatch(lUId, vFrds, vOp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "get UserOpBatch  failure !"<<endl;
        break;
    }

    map<Int64, Int32> mOp;
    for (vector<map<Int64, Int32> >::iterator i = vOp.begin(); i != vOp.end(); ++i)
    {
        for (map<Int64, Int32>::iterator j = i->begin(); j != i->end(); ++j)
        {
            if (j->second == 0)
            {
                map<Int64, Int32>::iterator pos = mOp.find(j->first);
                if (pos != mOp.end())
                {
                    mOp[j->first] += 1;
                }
                else
                {
                    mOp.insert(make_pair(j->first, 1));
                }
				LOG->info() << __FUNCTION__<< "UId: "<<lUId<<" Friends Favor SId: "<<j->first<<" num: "<<mOp[j->first]<<endl;
            }
        }
    }

    for (map<Int64, Int32>::iterator i = mOp.begin(); i != mOp.end(); ++i)
    {
        if (i->second >= iFrdsFvrNum) 
        {
            vSecrets.push_back(i->first);
        }
    }

    LOG->info() << __FUNCTION__<<  "get UserOpBatch SUCCESS !"<<endl;

    iRet = SUCCESS;

    __CATCH__
    PROC_END

    return iRet;
}

//////////////////////////////////////////////////////
// 对外接口: 设置收帖箱cache
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq,taf::JceCurrentPtr current)
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
    for (FrdsVector::const_iterator i = tReq.vUIds.begin(); i != tReq.vUIds.end(); ++i)
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

    FriendsList tMe;
    tMe.lUId = tReq.lUId;
    tMe.iRelat = ME;
    iRet = _tDbHandle.setSecretToCache(tReq.tPostReq, tReq.lSId, tMe, tReq.iType);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " setSecretToCache failure!"<<endl;
        break;
    }

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

taf::Int32 GetSecretImp::cleanCircleIdCache(taf::Int64 lCircleId, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    // 输出参数
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<lCircleId<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<lCircleId<<endl;

    PROC_BEGIN
    __TRY__

    // 根据帖子ID单独获取帖子内容
    iRet = _tDbHandle.cleanCircleIdCache(lCircleId);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " clean Circle Id Cache failure!"<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " clean Circle Id Cache success!"<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;

    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<lCircleId<<endl;

    return iRet;
}

taf::Int32 GetSecretImp::test(taf::Int64 lId,taf::JceCurrentPtr current)
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

	vector<string>  vString;
	QUAN::split("android", "&", vString);
	for (vector<string>::iterator i = vString.begin(); i != vString.end(); ++i)
	{
		LOG->debug() <<"split string: "<<*i<<endl;
	}

	vString.clear();
	QUAN::split("adr&110&duowan", "&", vString);
	for (vector<string>::iterator i = vString.begin(); i != vString.end(); ++i)
	{
		LOG->debug() <<"split string: "<<*i<<endl;
	}

    long lEncodeId = encodeUId(lId);
    LOG->debug() <<"encode UId: "<<lEncodeId<<" MAX "<<LONG_MAX<<endl;
    LOG->debug() <<"decode UId: "<<decodeUId(lEncodeId)<<endl;

    return SUCCESS;
}

//////////////////////////////////////////////////////
// 对外接口: 单独获取帖子详情
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current)
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
taf::Int32 GetSecretImp::setUserSecretShare(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp,taf::JceCurrentPtr current)
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
taf::Int32 GetSecretImp::feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current)
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
// 对外接口: 获取系统引导帖
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getSystemList(const QUAN::SystemListReq & tReq,QUAN::SystemListRsp &tRsp,taf::JceCurrentPtr current)
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

	vector<string> vTmp;
	vector<SecretInfo> vSecrets;

    // 获取系统引导对应的帖子ID
	iRet = _tDbHandle.getSystemList(vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " getSystemList failure!"<<endl;
        break;
    }

	// 打日志
	string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(vSecrets);
	LOG->debug() << __FUNCTION__ << " before: " << sLog << endl;

	split(tReq.tUserId.sUA, "&", vTmp);

	for (auto i = vSecrets.begin(); i != vSecrets.end(); ++i)
	{
		if (vTmp.size() >= 2 && i->sLocal == "0")
		{
			tRsp.vSystems.push_back(*i);
		}
		else if (i->lUId == 0)
		{
			tRsp.vSystems.push_back(*i);
		}
		else if (vTmp.size() >= 2 && i->sLocal == vTmp[0] && i->lUId >= TC_Common::strto<Int64>(vTmp[1]))
		{
			tRsp.vSystems.push_back(*i);
		}
		else if (!vTmp.empty() && i->sLocal == vTmp[0])
		{
			tRsp.vSystems.push_back(*i);
		}
	}

	sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp.vSystems);
	LOG->debug() << __FUNCTION__ << " after: " << sLog << endl;

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
// 获取附近列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getNearSecrets(SecretListReq const& tReq, ScrtMap& vMaps, ScrtVector& vSecrets)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

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
        LOG->debug() << __FUNCTION__ << "coords :" <<tLbsReq.dLat<<" | "<<tLbsReq.dLng<< endl;
    }
    else
    {
        LOG->debug() << __FUNCTION__ << "no coords !" << endl;
        return SUCCESS;
    }

    tLbsReq.lId = tReq.tUserId.lUId;

    LOG->debug() <<__FUNCTION__<< " Lbs Request :" << tReq.tUserId.lUId << endl;
    iRet = _pLbsProxy->taf_hash(tReq.tUserId.lUId)->getNearbyUser(tLbsReq, tLbsRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__ << "get Lbs Request failure !" << endl;
        iRet = FAILURE;
        break;
    }

    for (vector<LbsUser>::iterator i = tLbsRsp.vId.begin(); i != tLbsRsp.vId.end(); ++i)
    {
		// LOG->info() << __FUNCTION__ << "lSId: " <<i->lId<<" Distance: "<<i->iDistance<< endl;

		i->iDistance = i->iDistance * 1000;

		if (i->iDistance < 500)
		{
			vMaps.insert(make_pair(i->lId, i->iDistance));
			vSecrets.push_back(i->lId);
		}
    }

    iRet = SUCCESS; 
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end iRet:"<<iRet<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 获取城市帖子列表
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::getCitySecrets(SecretListReq const& tReq, ScrtVector& vSecrets)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

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
        LOG->debug() << __FUNCTION__ << "coords :" <<tLbsReq.dLat<<" | "<<tLbsReq.dLng<< endl;
    }
    else
    {
        LOG->debug() << __FUNCTION__ << "no coords !" << endl;
        return SUCCESS;
    }

    tLbsReq.lId = tReq.tUserId.lUId;

    LOG->debug() <<__FUNCTION__<< " Lbs Request :" << tReq.tUserId.lUId << endl;
    iRet = _pLbsProxy->taf_hash(tReq.tUserId.lUId)->getNearbyUser(tLbsReq, tLbsRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__ << "get Lbs Request failure !" << endl;
        iRet = FAILURE;
        break;
    }

    for (vector<LbsUser>::iterator i = tLbsRsp.vId.begin(); i != tLbsRsp.vId.end(); ++i)
    {
		i->iDistance = i->iDistance * 1000;

		if (i->iDistance < 10000)
		{
			vSecrets.push_back(i->lId);
		}
    }

    iRet = SUCCESS; 
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end iRet:"<<iRet<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// 帖子ID转换帖子内容
//////////////////////////////////////////////////////
taf::Int32 GetSecretImp::tranSecretsId(SecretListReq const& tReq, SecretListRsp& tRsp, ScrtVector& vSecrets, FrdsMap const& mFrds)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

        LOG->debug()<< " Batch Secrets From Post :"<<tReq.tUserId.lUId<<endl;

        if (vSecrets.empty())
        {
            LOG->debug()<< " tranSecretsId is empty"<<tReq.tUserId.lUId<<endl;
            return SUCCESS;
        }

        Int32    iStop = vSecrets.size();
        Int32    iIdsSum = 0;
        Int32    iSize = tReq.iCount * 2;
        Int32    i = 0;
        do
        {
            LOG->debug()<< " iStop: "<<iStop<<" iIdsSum: "<<iIdsSum<<" iSize: "<<iSize<<" i: "<<i<<endl;
            vector<SecretInfo> vTmpSecrets;
            ScrtVector         vTmpIds;

            if (iStop >= iSize)
            {
                copy(vSecrets.begin() + i * iSize , vSecrets.begin() + (i+1) * iSize, back_inserter(vTmpIds));
            }
            else
            {
                copy(vSecrets.begin() + i * iSize , vSecrets.end(), back_inserter(vTmpIds));
            }

            // 获取帖子内容列表
            iRet = _pPostProxy->taf_hash(tReq.tUserId.lUId)->getBatchSecrets(tReq.tUserId.lUId, vTmpIds, vTmpSecrets);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< "get  Batch Secrets failure !"<<endl;
                break;
            }
			
			if (tReq.iType == GET_SECRET_FRIENDS)
			{
				for (vector<SecretInfo>::iterator pos = vTmpSecrets.begin(); pos != vTmpSecrets.end(); ++pos)
				{
					if (tReq.tUserId.lUId == pos->lUId || mFrds.find(pos->lUId) != mFrds.end())
					{
						iIdsSum += 1;
					}
				}
			}
			else
			{
				iIdsSum += vTmpSecrets.size();
			}

            iStop   -= iSize;
            i++;

            copy(vTmpSecrets.begin(), vTmpSecrets.end(), back_inserter(tRsp.vSecrets));

            LOG->debug()<< " iStop: "<<iStop<<" iIdsSum: "<<iIdsSum<<" iSize: "<<iSize<<" i: "<<i<<endl;

        } while (iIdsSum < tReq.iCount && iStop > 0);

    iRet = SUCCESS; 
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

taf::Int32 GetSecretImp::getAllSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current)
{
    Int32 iRet;

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

    ScrtVector vSecrets;

    // 获取所有帖子
    iRet = _tDbHandle.getAllSecretsFromDb(tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get All Secrets failure! "<<endl;
        iRet = SUCCESS;
    }

    // 获取附近帖子列表
    ScrtMap    mScrtMaps;
    iRet = this->getNearSecrets(tReq, mScrtMaps, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Near Secrets failure! "<<endl;
        iRet = SUCCESS;
    }

    LOG->info() << __FUNCTION__<<  "tran SecretsId SUCCESS !"<<endl;

    // 排序 
    stable_sort(vSecrets.rbegin(), vSecrets.rend());
    // 去重
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());
  
    // 限制条数
    if (!vSecrets.empty())
    {
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN || vSecrets[0] < tReq.tRange.lBeginId)
        {
        }
        else
        {
            vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), bind2nd(greater_equal<Int64>(), tReq.tRange.lBeginId)), vSecrets.end());
        }
		if (!vSecrets.empty())
		{
			tRsp.lNextBeginId = vSecrets.back();
		}
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    if (!vSecrets.empty())
    {
        display(" the SECRET ID: ", vSecrets);

        FrdsMap mFrdsFirst;

        // 帖子ID转换内容
        iRet = this->tranSecretsId(tReq, tRsp, vSecrets, mFrdsFirst);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " tran SecretsId failure! "<<endl;
            break;
        }

        for (vector<SecretInfo>::iterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end(); ++i)
        {
            ScrtMapIterator mpos = mScrtMaps.find(i->lSId);
            if (mpos != mScrtMaps.end())
            {
	        LOG->info() << __FUNCTION__<<" Near  "<<mpos->first<<endl;
                if (mpos->second < 500)
                {
	            i->sLocal = Near_500;
                }
                if (mpos->second < 100)
                {
                    i->sLocal = Near_100;
                }
            }
            if (i->sLocal.empty())
            {
                i->sLocal = Near_World;
            }
            i->iType = RECMD;
        }

	if (!tRsp.vSecrets.empty())
	{
	    tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
	}
	else
	{
	    tRsp.lNextBeginId = SECRET_ID_END;
	}
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    // 限制长度
    if (tRsp.vSecrets.size() >= size_t(tReq.iCount))
    {
        tRsp.vSecrets.resize(tReq.iCount);
        if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }

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

taf::Int32 GetSecretImp::getCircleList(const QUAN::GetCircleListReq & tReq,QUAN::GetCircleListRsp &tRsp,taf::JceCurrentPtr current)
{
    Int32 iRet;

    // 输出参数
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

    FrdsVector              vFrdsAll;              // 所有好友
    FrdsVector              vFrdsFavor;            // 点赞活跃好友
    FrdsVectorPair          tFrdsPair;             // 1 + 2 度好友
	Int32                   iRegTime;              // 注册时间
    FrdsVector              vFrdsCircle;           // 树洞列表

	SecretListReq tSReq;
	SecretListRsp tSRsp;
	tSReq.tUserId = tReq.tUserId;
	tSReq.tRange = tReq.tRange;
	tSReq.iCount = tReq.iCount;

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

    ScrtVector    vSecrets;         // 帖子ID

	// 记录用户活跃情况
	if (tReq.tRange.lBeginId == SECRET_ID_BEGIN)
	{
		_tDbHandle.setActiveUser(tReq.tUserId, tReq.lCircleId);
	}

    // 获取好友列表 
    iRet = this->getFriendsList(tReq.tUserId, vFrdsFavor, tFrdsPair, vFrdsAll, iRegTime);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Friends List failure! "<<endl;
        break;
    }

	// 设置朋友数量
	tRsp.iFriends = tFrdsPair.first.size();

    LOG->info() << __FUNCTION__<< " get Friends List success! "<<endl;

    // 查询收帖箱是否有帖子
	if (tReq.iType == 0)
	{
		iRet = _tDbHandle.getCircleListFromCache(tReq, vSecrets);
   		if (iRet == SUCCESS)
   		{
       		LOG->debug() << __FUNCTION__<< " get UserSecretList From Cache SUCCESS! "<<endl;
       		tRsp.lNextBeginId = vSecrets.back();
       		display(" Circle List Cache Secret Accept: ", vSecrets);
   		}
   		else if (iRet == TIMEOUT)
   		{
			FriendsList tMe;
    		tMe.lUId = tReq.tUserId.lUId;
    		tMe.iRelat  = ME;
			FrdsVector vFrdsAllTmp;
			vFrdsAllTmp = vFrdsAll;
			vFrdsAllTmp.insert(vFrdsAllTmp.begin(), tMe);
	
    		// 获取好友帖子列表
			Int32 iEraseTime = 0;
			Int32 iTmp = 0;
    		iRet = this->getFriendsSecrets(tSReq, tFrdsPair, vFrdsAllTmp, tRsp.lNextBeginId, iTmp, vSecrets, iEraseTime);
    		if (iRet == FAILURE)
    		{
        		LOG->debug() << __FUNCTION__<< " get Secrets Friends failure! "<<endl;
        		break;
    		}
			// 擦除不在此时间范围帖子
			vSecrets.erase(remove_if(vSecrets.begin(),  vSecrets.end(),  LessTime(iEraseTime)), vSecrets.end());

    		LOG->info() << __FUNCTION__<< " get Friends Secrets success! "<<endl;

			iRet = _tDbHandle.setCircleListToCache(tReq, vSecrets);
       		if (iRet == FAILURE)
       		{
           		LOG->debug() << __FUNCTION__<< " Set Batch Secrets ToCache failure! "<<endl;
           		break;
       		}

       		LOG->info() << __FUNCTION__<< " Set Batch Secrets ToCache success! "<<endl;
   		}
		else
		{
       		LOG->info() << __FUNCTION__<< " Get Circle List ToCache success! "<<endl;
			break;
		}
	}
	else
	{
		// 获取circleinfo
		iRet = _pCircleProxy->taf_hash(tReq.tUserId.lUId)->getCircleInfo(tReq.tUserId, tReq.lCircleId, tReq.sCoord, tRsp.tCircleInfo);
		if (iRet != SUCCESS)
		{
   			LOG->debug() << __FUNCTION__<< " get Circle Info failure! "<<endl;
			break;
		}

		iRet = _tDbHandle.getBatchUserSecrets(tReq.lCircleId, tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecrets, CIRCLE);
		if (iRet == FAILURE)
		{
   			LOG->debug() << __FUNCTION__<< " get Circle List failure! "<<endl;
   			break;
		}
	}

    // 排序 
    stable_sort(vSecrets.rbegin(), vSecrets.rend());
    // 去重
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());
  
    // 限制条数
    if (!vSecrets.empty())
    {
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN || vSecrets[0] < tReq.tRange.lBeginId)
        {
        }
        else
        {
            vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), bind2nd(greater_equal<Int64>(), tReq.tRange.lBeginId)), vSecrets.end());
        }
		if (!vSecrets.empty())
		{
			tRsp.lNextBeginId = vSecrets.back();
		}
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    if (!vSecrets.empty())
    {
        display(" the SECRET ID: ", vSecrets);

		FrdsMap mFrdsFirst;
        // 帖子ID转换内容
        iRet = this->tranSecretsId(tSReq, tSRsp, vSecrets, mFrdsFirst);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " tran SecretsId failure! "<<endl;
            break;
        }

		tRsp.vSecrets = tSRsp.vSecrets;

        for (vector<SecretInfo>::iterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end(); ++i)
        {
            if (i->lUId == tReq.tUserId.lUId)
                i->iType = ME;
            else
                i->iType = FFRIEND;
			/*
			if (tReq.iType == 0)
				i->sLocal = Near_Address;
			else 
				i->sLocal = tRsp.tCircleInfo.sName;
			*/

			// 版本区分色块id;
			if (i->iColorId > 15 && iVersion < 200)
			{
				i->sBitUrl = "http://s1.dwstatic.com/quan/secrets/l" + TC_Common::tostr(i->iColorId) + ".jpg";
			}
        }

		if (!tRsp.vSecrets.empty())
			tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
		else
			tRsp.lNextBeginId = SECRET_ID_END;
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    // 限制长度
    if (tRsp.vSecrets.size() >= size_t(tReq.iCount))
    {
        tRsp.vSecrets.resize(tReq.iCount);
        if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }
	else
	{
        tRsp.lNextBeginId = SECRET_ID_END;
	}


    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " NextBeginId : "<<tRsp.lNextBeginId<<endl;
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

