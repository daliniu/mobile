#include "DbHandle.h"
#include <functional>

#define SECRET_MANAGER_SIZE    10000
SecretHashMap   g_secret_hashmap;
CircleHashMap   g_circle_hashmap;
SecretInfo  tSysScrt;
time_t      iSysTime = 0;
vector<SecretInfo>  vSysListScrts;
time_t      iSysListTime = 0;
ScrtVector  vRcmdCaches;
time_t      iRcmdTime = 0;

Int32 DbHandle::init()
{
    __TRY__

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "GetSecretServer.conf");

    TC_DBConf tcDBConf;
    
    tcDBConf.loadFromMap(tConf.getDomainMap("/main/db"));
        
    string sHost        =  tConf.get("/main/db/<dbhost>");
    string sUser        =  tConf.get("/main/db/<dbuser>");
    string sPassword    =  tConf.get("/main/db/<dbpass>");
    string sCharset     =  tConf.get("/main/db/<charset>");
    Int32 sPort           =  TC_Common::strto<Int32>(tConf.get("/main/db/<dbport>"));
    
    _sDbPre             =  tConf.get("/main/db/<dbpre>","secret");
    _sTbSecretPre       =  tConf.get("/main/db/<tbpre>","posts");
    _sTbContentPre      =  tConf.get("/main/db/<tbpre>","posts_comment");
    _sTbRelatPre        =  tConf.get("/main/db/<tbpre>","user_relation");
    _uDbNum             =  TC_Common::strto<Int32>(tConf.get("/main/db/<dbnum>","1"));
    _uTbNum             =  TC_Common::strto<Int32>(tConf.get("/main/db/<tbnum>","1"));

    _uUserRemoveNum     =  TC_Common::strto<Int32>(tConf.get("/main/db/<UserRemoveNum>", "100"));
    _uUserFavorNum      =  TC_Common::strto<Int32>(tConf.get("/main/db/<UserFavorNum>",  "100"));
    _uCacheTimeOut      = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","600"));
    _uRecmdTimeOut      = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","600"));
    _uSysTimeOut        = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","600"));

    _uUserScrtCacheTimeOut = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<userscrttimeout>","600"));
    
    _uDbNum = _uDbNum<1?1:_uDbNum;   //至少配置一个db、tb
    _uTbNum = _uTbNum<1?1:_uTbNum;

    LOG->debug()<<__FUNCTION__<<  "dbnum:" << _uDbNum <<" tbnum: "<<_uTbNum <<"charset: sCharset"<< endl;
    
    for (unsigned short i =0;i< _uDbNum; i++)
    {
        _mMysql[i] = new TC_Mysql();
        _mMysql[i]->init(sHost,sUser,sPassword,_sDbPre+TC_Common::tostr(i),sCharset,sPort,0);
    }
    
    return SUCCESS; 
    __CATCH__       

    exit(0);
    return FAILURE;
}

Int32 DbHandle::getSystemList(vector<SecretInfo>& vSecrets, bool bUseCache)
{
    Int32 iRet = FAILURE;

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();
    Int32 iIntev = tTimeNow - iSysListTime;
    
    LOG->info()<<"cache value |time Intev :|"<<iIntev<<"|UseCache|"<<bUseCache<<endl;

    if (vSysListScrts.empty() || iIntev > _uSysTimeOut)
    {
        iRet = this->getSystemListFromDb(vSecrets);
        if (iRet != FAILURE)
        {
			tSysListLock.lock();
            vSysListScrts = vSecrets;
            iSysListTime = tTimeNow;
			tSysListLock.unlock();
        }

        LOG->debug()<<" get System Secrets From Db " <<endl;
    }
    else
    {
		tSysListLock.lock();
        vSecrets = vSysListScrts;
		tSysListLock.unlock();
		// 随机重排
		random_shuffle(vSecrets.begin(), vSecrets.end());
        LOG->debug()<<" get System Secrets From cache " <<endl;
        iRet = SUCCESS;
    }

    return iRet;
}

Int32 DbHandle::getSystemListFromDb(vector<SecretInfo>& vSecrets)
{
    Int32 iRet = FAILURE;

    __TRY__

    string sSql = " select id, biturl, content,os, version "
                  " from posts_system where isremove = 0 and expired_time > now() and  publish_time < now() order by sequence desc, publish_time desc limit 0,100";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }

    for (size_t i = 0; i < tRes.size(); i++)
    {
		SecretInfo tSecret;
		tSecret.lSId      = TC_Common::strto<Int64>(tRes[i]["id"]); 
		tSecret.lUId      = TC_Common::strto<Int64>(tRes[i]["version"]);
		tSecret.iColorId  = 0;
    	tSecret.sBitUrl   = tRes[i]["biturl"]; 
    	tSecret.sContent  = tRes[i]["content"]; 
    	tSecret.iType     = SYS;
		tSecret.sLocal    = tRes[i]["os"];
		tSecret.iFavorNum = 0;
		tSecret.iCmtNum   = 0;
    	tSecret.isFavor   = false;
    	tSecret.isRead    = false;
        vSecrets.push_back(tSecret); 
    }

    iRet = SUCCESS; 

    __CATCH__

    return iRet;
}

Int32 DbHandle::getSystemSecret(SecretInfo& tSecret, bool bUseCache)
{
    Int32 iRet = FAILURE;

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();
    Int32 iIntev = tTimeNow - iSysTime;
    
    LOG->info()<<"cache value |time Intev :|"<<iIntev<<"|UseCache|"<<bUseCache<<endl;

    if (tSysScrt.lSId == 0 || iIntev > _uSysTimeOut)
    {
        iRet = this->getSystemSecretFromDb(tSecret);
        if (iRet != FAILURE)
        {
			tSysLock.lock();
            tSysScrt = tSecret;
            iSysTime = tTimeNow;
			tSysLock.unlock();
        }

        LOG->debug()<<" get System Secrets From Db " <<endl;
    }
    else
    {
		tSysLock.lock();
        tSecret = tSysScrt;
		tSysLock.unlock();
        LOG->debug()<<" get System Secrets From cache " <<endl;
        return SUCCESS;
    }

    return iRet;
}

Int32 DbHandle::getSystemSecretFromDb(SecretInfo& tSecret)
{
    Int32 iRet = FAILURE;

    __TRY__

    string sSql = " select id, uid, colorid, biturl, content, location, favor_num, cmt_num "  
                  " from posts p where source=1 and p.isremove=0 order by update_time desc limit 0,3";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }

    tSecret.lSId      = TC_Common::strto<Int64>(tRes[0]["id"]); 
    tSecret.lUId      = TC_Common::strto<Int64>(tRes[0]["uid"]); 
    tSecret.iColorId  = TC_Common::strto<Int32>(tRes[0]["colorid"]); 
    tSecret.sBitUrl   = tRes[0]["biturl"]; 
    tSecret.sContent  = tRes[0]["content"]; 
    tSecret.iType     = SYS;
    tSecret.sLocal    = tRes[0]["location"]; 
    tSecret.iFavorNum = TC_Common::strto<Int32>(tRes[0]["favor_num"]); 
    tSecret.iCmtNum   = TC_Common::strto<Int32>(tRes[0]["cmt_num"]); 
    tSecret.isFavor   = false;
    tSecret.isRead    = false;

    iRet = SUCCESS; 

    __CATCH__

    return iRet;
}


// 系统推荐帖子
Int32 DbHandle::getRecommendSecrets(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets, bool bUseCache)
{
    Int32 iRet  = FAILURE;

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();
    Int32 iIntev = tTimeNow - iRcmdTime;
    
    LOG->info()<<"cache value |time Intev :|"<<iIntev<<"| UId: "<<tUserId.lUId<<"|UseCache|"<<bUseCache<<endl;

    if (vRcmdCaches.empty() || iIntev > _uRecmdTimeOut)
    {
        iRet = this->getRecommendSecretsFromDb(tUserId, _u24SecretsNum, _u72SecretsNum, vSecrets);
        if (iRet != FAILURE)
        {
			tRmdLock.lock();
            vRcmdCaches = vSecrets;
            iRcmdTime = tTimeNow;
			tRmdLock.unlock();
        }

        LOG->debug()<<" get Recommend Secrets From DB " <<endl;
    }
    else
    {
		tRmdLock.lock();
        vSecrets = vRcmdCaches;
		tRmdLock.unlock();
        LOG->debug()<<" get Recommend Secrets From cache " <<endl;
        iRet = SUCCESS;
    }

    return iRet;
}


// 系统推荐帖子
Int32 DbHandle::getRecommendSecretsFromDb(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets)
{
    Int32 iRet = FAILURE;

	__TRY__

	string sSql = " select id "
		" from posts p where source=0 and isremove=0 and uid < 10000 and update_time > DATE_SUB(now(), INTERVAL 1 HOUR)  "
		" order by favor_num desc ";

    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
    else 
    {
       for (size_t i = 0; i < tRes.size(); i++)
       {
		   if (i >= size_t(4))
			   break;
           vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"])); 
       }
    }

	sSql = " select id "
		" from posts p where source=0 and isremove=0 and uid < 10000 and datediff(now(), update_time)<=3 "
		" order by favor_num desc ";

    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
    else 
    {
       for (size_t i = 0; i < tRes.size(); i++)
       {
		   if (i >= size_t(6))
			   break;
           vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"])); 
       }
    }
#if 0
    // 测试的时候应该做屏蔽
	sSql = " select id "
		" from posts p where source=0 and isremove=0 and location !='' and datediff(now(), update_time)<=3 "
		" order by favor_num desc ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql);

    if (tRes1.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes1.size() << ") ne 1  sql:" <<sSql<< endl;
    }

    for (size_t i = 0; i < tRes1.size(); i++)
    {
		if (i >= size_t(_u72SecretsNum))
			break;
        vSecrets.push_back(TC_Common::strto<Int64>(tRes1[i]["id"])); 
    }
#endif

    iRet = SUCCESS;

    __CATCH__

    return iRet;
}

// 批量获取用户，用户好友，好友好友发帖子列表
Int32 DbHandle::getAllUserSecrets(FrdsVector const& vFrdsAll, size_t iScrtsNum, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets)
{
    Int32 iRet = FAILURE;
    Int64 lMinNextId = LONG_MAX;

    LOG->info() << __FUNCTION__<< " begin "<<endl;

    if (vFrdsAll.empty())
    {
        LOG->info() << __FUNCTION__<<" Don't Have Friends getAllUserSecrets SUCCESS!"<<endl;
        return SUCCESS;
    }

    FrdsCIterator iBegin = vFrdsAll.begin();
    FrdsCIterator iEnd   = vFrdsAll.end();

    for (FrdsCIterator i = iBegin; i != iEnd; ++i)
    {
        ScrtVector            vTmp;
        ScrtId                lTmp;
        iRet = this->getBatchUserSecrets(i->lUId, lBeginId, lTmp, vTmp, i->iRelat);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<<" FriendsId: "<<i->lUId<<" | "<<" getBatchUserSecrets failure!"<<endl;
        }

        LOG->info() << __FUNCTION__<<" UserId: "<<i->lUId<<" | "<<" getBatchUserSecrets success!"<<endl;

        if (lTmp < lMinNextId && lTmp != SECRET_ID_END)
        {
            lMinNextId = lTmp;
        }

        if (lBeginId != SECRET_ID_BEGIN)
        { 
            ScrtIterator pos = find_if(vTmp.begin(), vTmp.end(), bind2nd(less<Int64>(), lBeginId));
            if (pos == vTmp.end())
            {
                continue;
            }
            if (distance(pos, vTmp.end()) >= ScrtVector::difference_type(iScrtsNum))
            {
                copy(pos, pos + iScrtsNum, back_inserter(vSecrets));
            }
            else
            {
                copy(pos, vTmp.end(), back_inserter(vSecrets));
            }
        }
        else if (vTmp.size() >= ScrtVector::size_type(iScrtsNum) && lBeginId == SECRET_ID_BEGIN)
        { 
            copy(vTmp.begin(), vTmp.begin() + iScrtsNum, back_inserter(vSecrets));
        }
        else
        {
            copy(vTmp.begin(), vTmp.end(), back_inserter(vSecrets));
        }
    }

    lNextBeginId = lMinNextId;

    if (lNextBeginId == LONG_MAX)
    {
        lNextBeginId = SECRET_ID_END;
    }

    LOG->info() << __FUNCTION__<< " end "<<endl;

    return SUCCESS;
}

// 批量获取用户帖子列表
Int32 DbHandle::getBatchUserSecrets(Int64 lUId, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, Int32 iCircle, bool bUseCache)
{

    Int32 iRet    = FAILURE;
    Int32 iDbRet  = FAILURE;
    Int32 iMemRet = FAILURE;

    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = lUId;

	//Int64 lMs = TC_TimeProvider::getInstance()->getNowMs();

    __TRY__

    if(bUseCache)
    {
		if (iCircle == CIRCLE)
			iMemRet = CommHashMap::getInstance()->getHashMap(g_circle_hashmap,tKey,tValue);
		else
			iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tValue.iPullTime;

    LOG->info()<<" cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;
  
    if (iMemRet != SUCCESS || lBeginId == SECRET_ID_BEGIN || lBeginId < tValue.lNextBeginId || iIntev > _uCacheTimeOut)
    {
        iDbRet =  this->getBatchUserSecretsFromDb(lUId, lBeginId, lNextBeginId, vSecrets, iCircle);

        LOG->info()<<" get User Batch Secrets from db ret:"<<iDbRet<<"| UId: "<<lUId<<endl;

        if (iDbRet == SUCCESS && lUId != 0)
        {
			if (lBeginId == SECRET_ID_BEGIN)
			{
				tValue.vSecrets     = vSecrets;
			}
			else
			{
				copy(vSecrets.begin(), vSecrets.end(), back_inserter(tValue.vSecrets));
				tValue.vSecrets.erase(unique(tValue.vSecrets.begin(), tValue.vSecrets.end()), tValue.vSecrets.end());
			}

            tValue.lNextBeginId = lNextBeginId;
            tValue.iPullTime    = tTimeNow;
			if (iCircle == CIRCLE)
				CommHashMap::getInstance()->addHashMap(g_circle_hashmap, tKey, tValue);
			else
				CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);
        }
    }
    else
    {
        LOG->info()<<" get Batch User Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    vSecrets = tValue.vSecrets;
    lNextBeginId = tValue.lNextBeginId;

    if (iDbRet == SUCCESS)
    {
        iRet = TIMEOUT; //Cahce命中超时

    }
    else if (iMemRet== SUCCESS)
    {
        iRet = SUCCESS; //CacheOK
    }

	//LOG->debug() << __FUNCTION__ << " time: " << TC_TimeProvider::getInstance()->getNowMs() - lMs << " uid: " <<lUId<<" circlie: "<< (iCircle == CIRCLE) << endl;

    return iRet;
}

// 批量获取用户帖子列表
Int32 DbHandle::getBatchUserSecretsFromDb(Int64 lUId, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, Int32 iCircle)
{
    Int32 iRet = FAILURE;
	// Int64 lMs = TC_TimeProvider::getInstance()->getNowMs();
    __TRY__

    string sPage;

    if (lBeginId == SECRET_ID_BEGIN)
    {
        sPage = "";
    }
    else 
    {
        sPage = " and id < " + TC_Common::tostr(lBeginId);
    }

	if (iCircle == CIRCLE)
	{
		string sCircleId;

		if (lUId == 0)
		{
			sCircleId = "";
		}
		else
		{
			sCircleId = " circleid=" + TC_Common::tostr(lUId) + " and ";
		}

		// 获取树洞帖子
		string sSql = " select id "
			" from posts "
					" where " + sCircleId +
                  	" isremove=0 "  +
                  	" and islock=0 "+ sPage + " order by update_time desc limit 0,1000; ";
	
    	LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;
	
    	taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);
	
    	if (tRes.size() == 0)
    	{
        	LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    	}
		else
		{
			for (size_t i = 0; i < tRes.size(); i++)
    		{
        		vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"])); 
    		}
		}
		// 下一个起始号
		if (vSecrets.empty())
		{
        	lNextBeginId = SECRET_ID_END;
		}
		else
		{
			lNextBeginId = vSecrets.back();
		}
		return SUCCESS;
	}

	if (lUId < SECRET_MANAGER_SIZE)
	{
		// 添加普通帖变运营帖功能
    	string sSql = " select id "
                  	" from posts "
                  	" where shareid="  + TC_Common::tostr(lUId) +  
                  	" and isremove=0 "  +
                  	" and islock=0 "+ sPage + " order by update_time desc limit 0,500; ";
	
    	LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;
	
    	taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);
	
    	if (tRes.size() == 0)
    	{
        	LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    	}
		else
		{
			for (size_t i = 0; i < tRes.size(); i++)
    		{
        		vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"])); 
    		}
		}
	}

	// 正常取普通帖
    string sSql = " select id "
                  " from posts "
                  " where uid="  + TC_Common::tostr(lUId) +  
                  " and isremove=0 " +
                  " and islock=0 "+ sPage + " order by update_time desc limit 0,500; ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
		{
			vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"]));
		}
	}

	sort(vSecrets.rbegin(), vSecrets.rend());

    // 下一个起始号
	if (vSecrets.empty())
	{
        lNextBeginId = SECRET_ID_END;
	}
	else
	{
		lNextBeginId = vSecrets.back();
	}

    iRet = SUCCESS;

	__CATCH__

	//LOG->debug() << __FUNCTION__ << " time: " << TC_TimeProvider::getInstance()->getNowMs() - lMs << " uid: " <<lUId<<" circlie: "<< (iCircle == CIRCLE) << endl;

    return iRet;
}

// 获取用户帖帖子
Int32 DbHandle::getUserSecrets(Int64 lUId, Int64 lSId, ScrtId& tSecret)
{
    Int32        iRet;
    Int64        lBeginId = -1;
    Int64        lNextBeginId = 0;
    ScrtVector   vSecrets;
   
    iRet = this->getBatchUserSecrets(lUId, lBeginId, lNextBeginId, vSecrets, 0);
    if (iRet != FAILURE)
    {
        LOG->info()<<__FUNCTION__<< " get Batch User Secrets find the lSId success"<<endl;
        iRet = FAILURE;
        for (ScrtIterator i = vSecrets.begin(); i != vSecrets.end(); ++i)
        {
            if (*i == lSId)
            {
                tSecret = *i;
                iRet = SUCCESS;
                break;
            }
        }
    } 
    else
    {
        LOG->info()<<__FUNCTION__<< " get Batch User Secrets find the lSId failure"<<endl; 
    }

    return iRet;
}

Int32 DbHandle::getUserSecretListFromCache(Int64 lUId, Int64 lBeginId, Int32 iScrtsNum, ScrtVector& vSecrets, Int32& iLockCount)
{
    Int32 iRet    = FAILURE;
    Int32 iMemRet = FAILURE;

    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = lUId;

    __TRY__

    iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tValue.iAcceptTime;

    LOG->debug()<<__FUNCTION__<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<" Accepts size :"<<tValue.vAccepts.size()<<endl;

    if (iMemRet != 0 || tValue.vAccepts.empty() || iIntev > _uUserScrtCacheTimeOut)
    {
        iLockCount = 0;
        LOG->debug()<<__FUNCTION__<<" No Data Or Timeout "<<lUId<<endl;
        return TIMEOUT;
    }
    else
    {
        // 获取好友秘密数
        iLockCount = tValue.iLockCount;
        LOG->debug()<<__FUNCTION__<<"get User Accept Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<"| LockCount: "<<iLockCount<<endl;
    }

    if (lBeginId != -1)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), bind2nd(less<Int64>(), lBeginId));

        if (pos == tValue.vAccepts.end())
        {
            // 收帖箱找不到需要的帖子，找DB
            LOG->debug()<<__FUNCTION__<<" User Accept can't find Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
            vSecrets = tValue.vAccepts;
            return TIMEOUT;
        }
        if (distance(pos, tValue.vAccepts.end()) >= ScrtVector::difference_type(iScrtsNum))
        {
            // 找到,返回
			// copy(pos, tValue.vAccepts.end(), back_inserter(vSecrets));
            vSecrets = tValue.vAccepts;
            LOG->debug()<<__FUNCTION__<<" User Accept has finded Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
        }
        else
        {
            // 找到，但不够需求
			// copy(pos, tValue.vAccepts.end(), back_inserter(vSecrets));
            vSecrets = tValue.vAccepts;
            LOG->debug()<<__FUNCTION__<<" User Accept has finded Secrets from cache but .......ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
            return TIMEOUT;
        }
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<" User Accept find All Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
        // 返回整个收件箱
        if (tValue.vAccepts.size() >= size_t(iScrtsNum))
        {
            vSecrets = tValue.vAccepts;
        }
        else
        {
            vSecrets = tValue.vAccepts;
            return TIMEOUT;
        }
    }

    iRet = SUCCESS; //CacheOK

    return iRet;
}

// 发表帖子 缓存粉丝收帖箱
Int32 DbHandle::setBatchSecretsToCache(ScrtVector const& vSecrets, Int64 lUId, Int32 iLockCount, bool bUseCache)
{
    __TRY__ 

    LOG->debug()<<__FUNCTION__<<" the UId is : "<<lUId<<endl;

    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = lUId;

	if (vSecrets.empty())
		return SUCCESS;

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }

    tValue.vAccepts = vSecrets;
    tValue.iLockCount = iLockCount;
    tValue.iAcceptTime = TC_TimeProvider::getInstance()->getNow();

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    __CATCH__

    return SUCCESS;
}

// 发表帖子 缓存粉丝收帖箱
Int32 DbHandle::setSecretToCache(PostSecretReq const& tReq, Int64 lSId, FriendsList const& tFriendsList, Int32 iType, bool bUseCache)
{
    __TRY__ 

    int iMemRet = FAILURE;
    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = tFriendsList.lUId;

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }


    if (iType == USER_POST)
    {
        if (tValue.vAccepts.size() != 0)
        {
            tValue.vAccepts.insert(tValue.vAccepts.begin(), lSId);
        }
        else
        {
            tValue.vAccepts.push_back(lSId);
        }

		if (!tValue.vSecrets.empty() && tFriendsList.iRelat == ME)
		{
			tValue.vSecrets.insert(tValue.vSecrets.begin(), lSId);
			tValue.iPullTime = TC_TimeProvider::getInstance()->getNow();
		}

        tValue.iAcceptTime = TC_TimeProvider::getInstance()->getNow();
        LOG->debug()<<__FUNCTION__<<" post success "<<endl;
    }
    else if (iType == USER_REMOVEY && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), bind2nd(equal_to<Int64>(), lSId));
        if (pos != tValue.vAccepts.end())
        {
            tValue.vAccepts.erase(pos);
            LOG->info()<<__FUNCTION__<<" remove success "<<endl;
        } 
        else        
            LOG->debug()<<__FUNCTION__<<" remove failure "<<endl;
    }
    else if (iType == USER_CLEAN && iMemRet == 0) 
    {
        return CommHashMap::getInstance()->eraseHashMap(g_secret_hashmap, tKey);
    }

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    __CATCH__

    return SUCCESS;
}

Int32 DbHandle::getUserSingleSecretFromDb(const QUAN::SglSecretReq & tReq, QUAN::SglSecretRsp &tRsp)
{
    __TRY__

    string sSql = " select id, uid, colorid, biturl, content, location, favor_num, cmt_num, circleid "  
                  " from posts p where source=0 and isremove=0 and id = "  + TC_Common::tostr(tReq.lSId);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return FAILURE;
    }

    tRsp.tSecret.lSId      = TC_Common::strto<Int64>(tRes[0]["id"]); 
    tRsp.tSecret.lUId      = TC_Common::strto<Int64>(tRes[0]["uid"]); 
    tRsp.tSecret.iColorId  = TC_Common::strto<Int32>(tRes[0]["colorid"]); 
    tRsp.tSecret.sBitUrl   = tRes[0]["biturl"]; 
    tRsp.tSecret.sContent  = tRes[0]["content"]; 
    tRsp.tSecret.iType     = NONE;
    tRsp.tSecret.sLocal    = tRes[0]["location"]; 
    tRsp.tSecret.iFavorNum = TC_Common::strto<Int32>(tRes[0]["favor_num"]); 
    tRsp.tSecret.iCmtNum   = TC_Common::strto<Int32>(tRes[0]["cmt_num"]); 
    tRsp.tSecret.isFavor   = false;
    tRsp.tSecret.isRead    = false;
    tRsp.tSecret.lCircleId   = TC_Common::strto<Int64>(tRes[0]["circleid"]); 

    string sSql1 = " select id, pid, uid, update_time, content, favor_num , avatarid "  
                  " from posts_comment pc where pc.pid=" + TC_Common::tostr(tReq.lSId) + " and pc.isremove=0 order by pc.update_time asc ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql1 << endl;

    taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql1);

    if (tRes1.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }

	// 前端调用直接返回
	if (tReq.tUserId.lUId != 0)
	{
		return SUCCESS;
	}

    for (size_t i = 0; i < tRes1.size(); i++)
    {
        ContentInfo    tTmp;

        tTmp.lCId      = TC_Common::strto<Int64>(tRes1[i]["id"]);   // 评论ID
        tTmp.lSId      = TC_Common::strto<Int64>(tRes1[i]["pid"]);   // 父ID
        tTmp.lUId      = TC_Common::strto<Int64>(tRes1[i]["uid"]);   // 评论者ID
        tTmp.isFirst   = false;
        tTmp.Time      = tRes1[i]["update_time"];                            // 时间 (YYYY-MM-DD HH:MM)
        tTmp.sContent  = tRes1[i]["content"];                            // 内容
        tTmp.iFavorNum = TC_Common::strto<Int32>(tRes1[i]["favor_num"]);   // 赞数
        tTmp.isFavor   = false;
        tTmp.iPortrait = TC_Common::strto<Int32>(tRes1[i]["avatarid"]);   // 头像ID

        tRsp.vContents.push_back(tTmp);            
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;

}

Int32 DbHandle::setUserSecretShareToDb(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp)
{
    __TRY__

    string sSql = "update posts set isshare=1 where id="+ TC_Common::tostr(tReq.lSId);
    LOG->debug()<<__FUNCTION__<<" SQL:" << sSql << endl;
    getMysql(0)->execute(sSql);

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

Int32 DbHandle::setFeedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp)
{
    __TRY__

    string sSql = "INSERT INTO feedback (plat, version, content, system, uid, time) VALUES("
                  " '" + tReq.sPlatform + "', "
                  " '" + tReq.sVersion + "', "
                  " '" + tReq.sContent + "', "
                  " '" + tReq.sSystem + "', "
                  " '" + TC_Common::tostr(tReq.tUserId.lUId) + "', now())";
                 
    LOG->debug()<<__FUNCTION__<<" SQL:" << sSql << endl;
    getMysql(0)->execute(sSql);

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

Int32 DbHandle::setActiveUser(UserId const& tUserId, Int64 lCircleId)
{
	__TRY__

	vector<string> sUserId;
	string sVersion;
	string sOs;

	QUAN::split(tUserId.sUA, "&", sUserId);

	if (sUserId.size() == 1)
	{
		sOs = sUserId[0];
		sVersion = "0";
	}
	else if (sUserId.size() >= 2)
	{
		sOs = sUserId[0];
		sVersion = sUserId[1];
	}

	string sSql = "INSERT INTO active_user (uid, num, update_date, version, os, circleid) VALUES("
		" '" + TC_Common::tostr(tUserId.lUId) + "', 1, now(), '" + sVersion + "', '" + sOs + "','" + TC_Common::tostr(lCircleId) + "') on duplicate key update num = num + 1, version ='" + sVersion + "', os='" + sOs + "'";
                 
    LOG->debug()<<__FUNCTION__<<" SQL:" << sSql << endl;
    getMysql(0)->execute(sSql);

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 批量获取用户帖子列表
Int32 DbHandle::getAllSecretsFromDb(Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets)
{
    Int32 iRet = FAILURE;

    __TRY__

    string sPage;

    if (lBeginId == SECRET_ID_BEGIN)
    {
        sPage = "";
    }
    else 
    {
        sPage = " and id < " + TC_Common::tostr(lBeginId);
    }

	// 正常取普通帖
    string sSql = " select id "
                  " from posts "
                  " where " 
                  " isremove=0 " 
                  " and islock=0 "+ sPage + " order by update_time desc limit 0,500; ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
		{
			vSecrets.push_back(TC_Common::strto<Int64>(tRes[i]["id"]));
		}
	}

    // 下一个起始号
	if (vSecrets.empty())
	{
        lNextBeginId = SECRET_ID_END;
	}
	else
	{
		lNextBeginId = vSecrets.back();
	}

    iRet = SUCCESS;

    __CATCH__

    return iRet;
}

Int32 DbHandle::getCircleListFromCache(GetCircleListReq const& tReq, ScrtVector& vCircleLists)
{
    Int32 iRet    = FAILURE;
    Int32 iMemRet = FAILURE;

    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = (tReq.tUserId.lUId << 32);

    __TRY__

    iMemRet = CommHashMap::getInstance()->getHashMap(g_circle_hashmap, tKey, tValue);

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

	Int32 iIntev = tTimeNow - tValue.iPullTime;

    LOG->info()<<" Circle List cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| CircleId: "<<tReq.lCircleId<<"|UseCache|"<<endl;
  
    if (iMemRet != SUCCESS || iIntev > _uCacheTimeOut)
    {
		return TIMEOUT;
    }
    else
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), bind2nd(less<Int64>(), tReq.tRange.lBeginId));

        if (pos == tValue.vAccepts.end())
        {
            // 收帖箱找不到需要的帖子，找DB
            LOG->debug()<<__FUNCTION__<<" User Accept can't find Secrets from cache ret:"<<iMemRet<<"| CircleId: "<<tReq.lCircleId<<endl;
			vCircleLists = tValue.vSecrets;
            return TIMEOUT;
        }

		vCircleLists = tValue.vSecrets;
		return SUCCESS;
    }

    return iRet;
}

Int32 DbHandle::setCircleListToCache(GetCircleListReq const& tReq, ScrtVector const& vCircleLists)
{
    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = (tReq.tUserId.lUId << 32);

    __TRY__

    CommHashMap::getInstance()->getHashMap(g_circle_hashmap, tKey, tValue);

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

	tValue.vSecrets     = vCircleLists;
    tValue.iPullTime    = tTimeNow;
    CommHashMap::getInstance()->addHashMap(g_circle_hashmap, tKey, tValue);

    return SUCCESS;
}

Int32 DbHandle::cleanCircleIdCache(Int64 lCircleId)
{
	SecretListKey tKey;
	tKey.lUId = (lCircleId << 32);
	return CommHashMap::getInstance()->eraseHashMap(g_circle_hashmap, tKey);
}

TC_Mysql* DbHandle::getMysql(Int64 iId)
{
    unsigned short uIndex = ((unsigned Int64)iId%10) %_uDbNum;
    return _mMysql[uIndex];
}
