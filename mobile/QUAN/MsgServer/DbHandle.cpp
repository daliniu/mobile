#include "DbHandle.h"

RemindHashMap  g_remind_hashmap;
SysMsgHashMap  g_sysmsg_hashmap;
PushMsgHashMap  g_pushmsg_hashmap;

int DbHandle::init()
{
    __TRY__

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "MsgServer.conf");

    TC_DBConf tcDBConf;
    
    tcDBConf.loadFromMap(tConf.getDomainMap("/main/db"));
        
    string sHost        =  tConf.get("/main/db/<dbhost>");
    string sUser        =  tConf.get("/main/db/<dbuser>");
    string sPassword    =  tConf.get("/main/db/<dbpass>");
    string sCharset     =  tConf.get("/main/db/<charset>");
    int sPort           =  TC_Common::strto<int>(tConf.get("/main/db/<dbport>"));
    
    _sDbPre             =  tConf.get("/main/db/<dbpre>","secret");
    _sTbSecretPre       =  tConf.get("/main/db/<tbpre>","posts");
    _sTbContentPre      =  tConf.get("/main/db/<tbpre>","posts_comment");
    _sTbRelatPre        =  tConf.get("/main/db/<tbpre>","user_relation");
    _uDbNum             =  TC_Common::strto<int>(tConf.get("/main/db/<dbnum>","1"));
    _uTbNum             =  TC_Common::strto<int>(tConf.get("/main/db/<tbnum>","1"));
    _uMaxMsgNum         =  TC_Common::strto<int>(tConf.get("/main/db/<maxmsgnum>","100"));
    _uCacheTimeOut      = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","300"));
    
    _uDbNum = _uDbNum<1?1:_uDbNum;   //至少配置一个db、tb
    _uTbNum = _uTbNum<1?1:_uTbNum;

    LOG->debug()<<__FUNCTION__<<  "dbnum:" << _uDbNum <<" tbnum: "<<_uTbNum <<"charset: "<<sCharset<< endl;
    
    for (unsigned short i =0;i< _uDbNum; i++)
    {
        _mMysql[i] = new TC_Mysql();
        _mMysql[i]->init(sHost,sUser,sPassword,_sDbPre+TC_Common::tostr(i),sCharset,sPort,0);
    }
    
    
    return 0;
    
    __CATCH__       

    exit(0);
    return FAILURE;
}

int DbHandle::setMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp, bool bUseCache)
{
    int iDbRet  = FAILURE;
    int iMemRet = FAILURE;

    MsgRemindKey   tKey;
    MsgRemindValue tValue;
    tKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_remind_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    int iIntev = tTimeNow - tValue.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iType == CONT_FAVOR)
    {
		iDbRet = updMsgRead(lUId, lId);
        if (iDbRet == FAILURE)
        {
			LOG->debug() << " update change don't need cache " << endl;
            return SUCCESS;
        }
		LOG->debug() << " update change cache " << endl;
    }
    else if (iType == CLEAN_MSG)
    {
        // 清除评论
        iDbRet = updCleanMsgToUserId(lUId, lId, lFirstId, CLEAN_MSG, iOp);
        if (iDbRet == FAILURE)
        {
            return iDbRet;
        }
        if (iMemRet == 0)
        {
            tValue.mSIds.clear();
            CommHashMap::getInstance()->addHashMap(g_remind_hashmap, tKey, tValue);
        }
        return iDbRet;
    }
    else 
    {
        iDbRet = setMsgToUserIdToDb(lUId, lId, lFirstId, iType, iOp);
    } 

    LOG->debug()<<"Set User Msg Remind to db ret:"<<iDbRet<<"| UId: "<<lUId<<"| type: "<<iType<<endl;

    if (iDbRet == SUCCESS && lUId != 0)
    {
        LOG->debug()<<"...........Set Mesg to Cache : "<<lId<<" op "<<iOp<<endl;
        MsgRmd   tTmp;
		tTmp.iOp = iType;
        if (iOp == NOTRD) 
            tTmp.isRead = false;
        else if (iOp == READED)
            tTmp.isRead = true;
        else
            tTmp.isRead = true;
        tValue.mSIds[lId] = tTmp; 
        tValue.iTime = tTimeNow;
        CommHashMap::getInstance()->addHashMap(g_remind_hashmap, tKey, tValue);
        return iDbRet;
    }

    return FAILURE;
}

int DbHandle::getMsgsRemind(PushMsgReq const& tReq, map<Int64, MsgRmd>& mSIds, bool bUseCache)
{
    int iRet    = FAILURE;
    int iDbRet  = FAILURE;
    int iMemRet = FAILURE;

    MsgRemindKey   tKey;
    MsgRemindValue tValue;
    tKey.lUId = tReq.tUserId.lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_remind_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    int iIntev = tTimeNow - tValue.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<tReq.tUserId.lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != SUCCESS  || iIntev > _uCacheTimeOut)
    {
        iDbRet = this->getMsgsRemindFromDb(tReq, mSIds);

		LOG->debug() << "get User Msg Remind  from db ret:" << iDbRet << "| UId: " << tReq.tUserId.lUId << endl;

        if (iDbRet == SUCCESS && tReq.tUserId.lUId != 0)
        {
            tValue.mSIds = mSIds;
            tValue.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_remind_hashmap, tKey, tValue);
        }
    }
    else
    {
        LOG->debug()<<"get User Msg Remind from cache ret:"<<iMemRet<<"| UId: "<<tReq.tUserId.lUId<<endl;
    }

    mSIds = tValue.mSIds;

    if (iDbRet == SUCCESS)
    {
        iRet = TIMEOUT; //Cahce命中超时

    }
    else if (iMemRet == SUCCESS && iIntev<=60*10)
    {
        iRet = SUCCESS; //CacheOK
    }

    return iRet;
}

int DbHandle::getMsgsRemindFromDb(PushMsgReq const& tReq, map<Int64, MsgRmd>& mSIds)
{
    __TRY__

	// 已读消息
    string sSql = " select pid, operation, isread "
                  " from posts_remind pr where isread = 1 and pr.uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
                  " order by pr.isread asc, pr.update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);


    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	MsgRmd tTmp;
        	tTmp.iOp = TC_Common::strto<Int64>(tRes[i]["operation"]);
        	tTmp.isRead   = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	mSIds[TC_Common::strto<Int64>(tRes[i]["pid"])] =  tTmp;
    	}
	}

	// 点赞未读消息
    sSql = " select pid, operation, isread "
                  " from posts_remind pr where operation = 1 and isread = 0 and pr.uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
                  " order by pr.isread asc, pr.update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);


    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	MsgRmd tTmp;
        	tTmp.iOp = TC_Common::strto<Int64>(tRes[i]["operation"]);
        	tTmp.isRead   = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	mSIds[TC_Common::strto<Int64>(tRes[i]["pid"])] =  tTmp;
    	}
	}

	// 评论未读消息
    sSql = " select pid, operation, isread "
                  " from posts_remind pr where operation = 0 and isread = 0 and pr.uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
                  " order by pr.isread asc, pr.update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);


    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	MsgRmd tTmp;
        	tTmp.iOp = TC_Common::strto<Int64>(tRes[i]["operation"]);
        	tTmp.isRead   = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	mSIds[TC_Common::strto<Int64>(tRes[i]["pid"])] =  tTmp;
    	}
	}
	// @消息未读消息
    sSql = " select pid, operation, isread "
                  " from posts_remind pr where operation = 5 and isread = 0 and pr.uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
                  " order by pr.isread asc, pr.update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);


    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	MsgRmd tTmp;
        	tTmp.iOp = TC_Common::strto<Int64>(tRes[i]["operation"]);
        	tTmp.isRead   = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	mSIds[TC_Common::strto<Int64>(tRes[i]["pid"])] =  tTmp;
    	}
	}

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

int DbHandle::setMsgToUserIdToDb(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp)
{
    int iRet = FAILURE;
    __TRY__

    string sSql = "INSERT INTO `posts_remind` (`uid`, `pid`, `firstid`, `operation`, `isread`, `update_time`) VALUES ("
                  + TC_Common::tostr(lUId) + ", " + TC_Common::tostr(lId) + ", " + TC_Common::tostr(lFirstId) + ", " + TC_Common::tostr(iType) +", " + TC_Common::tostr(iOp) 
                  + ", now())  on duplicate key update isread = " + TC_Common::tostr(iOp) + ", update_time = now()";

    LOG->debug() << __FUNCTION__ << "|sql: " << sSql << endl;

    getMysql(0)->execute(sSql);

    iRet = SUCCESS;
    __CATCH__

    return iRet;
}

int DbHandle::updMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp)
{
     int iRet = FAILURE;
    __TRY__

    string sSql = "UPDATE `posts_remind` set isread= " + TC_Common::tostr(iOp) + " , update_time=now() where uid="
                  + TC_Common::tostr(lUId) + " and operation= " + TC_Common::tostr(iType) + " and pid= " + TC_Common::tostr(lId);

    LOG->debug() << __FUNCTION__ << "|sql: " << sSql << endl;

    getMysql(0)->execute(sSql);

    if (getMysql(0)->getAffectedRows() == 0)
    {
        iRet = FAILURE;
    }
    else
    {
        iRet = SUCCESS;
    }
    __CATCH__

    return iRet;
}

int DbHandle::updMsgRead(Int64 lUId, Int64 lId)
{
	int iRet = FAILURE;
	__TRY__

    string sSql = "UPDATE `posts_remind` set isread= 1, update_time=now() where uid="
                  + TC_Common::tostr(lUId) + " and pid= " + TC_Common::tostr(lId);

	LOG->debug() << __FUNCTION__ << "|sql: " << sSql << endl;

	getMysql(0)->execute(sSql);

	if (getMysql(0)->getAffectedRows() == 0)
	{
		iRet = FAILURE;
	}
	else
	{
		iRet = SUCCESS;
	};

	__CATCH__

	return iRet;
}

int DbHandle::updCleanMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp)
{
     int iRet = FAILURE;
    __TRY__

    string sSql = "UPDATE `posts_remind` set isread=3 , update_time=now() where uid=" + TC_Common::tostr(lUId);

    LOG->debug() << __FUNCTION__ << "|sql: " << sSql << endl;

    getMysql(0)->execute(sSql);

    iRet = SUCCESS;
    __CATCH__

    return iRet;
}

int DbHandle::cleanCache(const vector<Int64> & vUserId)
{
	for (vector<Int64>::const_iterator i = vUserId.begin(); i != vUserId.end(); ++i)
	{
		MsgRemindKey   tKey;
		tKey.lUId = *i;
		CommHashMap::getInstance()->eraseHashMap(g_remind_hashmap, tKey);
	}

	return SUCCESS;
}

int DbHandle::cleanSysCache(Int64 lUserId)
{
	SysMsgKey   tKey;
	tKey.lUId = lUserId;
	CommHashMap::getInstance()->eraseHashMap(g_sysmsg_hashmap, tKey);

	return SUCCESS;
}

int DbHandle::getSysMsgList(GetSysMsgReq const& tReq, vector<SysMsgInfo>& vSysMsgs, bool bUseCache)
{
    int iRet    = FAILURE;
    int iDbRet  = FAILURE;
    int iMemRet = FAILURE;

    SysMsgKey   tKey;
    SysMsgValue tValue;
    tKey.lUId = tReq.tUserId.lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_sysmsg_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    int iIntev = tTimeNow - tValue.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<tReq.tUserId.lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != SUCCESS  || iIntev > _uCacheTimeOut)
    {
        iDbRet = this->getSysMsgListFromDb(tReq, vSysMsgs);

		LOG->debug() << "get Sys Msg  from db ret:" << iDbRet << "| UId: " << tReq.tUserId.lUId << endl;

        if (iDbRet == SUCCESS && tReq.tUserId.lUId != 0)
        {
            tValue.vSysMsgs = vSysMsgs;
            tValue.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_sysmsg_hashmap, tKey, tValue);
        }
    }
    else
    {
        LOG->debug()<<"get Sys Msg from cache ret:"<<iMemRet<<"| UId: "<<tReq.tUserId.lUId<<endl;
    }

    vSysMsgs = tValue.vSysMsgs;

    if (iDbRet == SUCCESS)
    {
        iRet = TIMEOUT; //Cahce命中超时
    }

    else if (iMemRet == SUCCESS)
    {
        iRet = SUCCESS; //CacheOK
    }

    return iRet;
}

int DbHandle::getSysMsgListFromDb(GetSysMsgReq const& tReq, vector<SysMsgInfo>& vSysMsgs)
{
    __TRY__

	// 已读消息
    string sSql = " select id, content, update_time, isread, circleid, ispass "
                  " from system_user_msg where isread = 1 and uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
                  " order by isread asc, update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);


    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	SysMsgInfo tTmp;
        	tTmp.lId = TC_Common::strto<Int64>(tRes[i]["id"]);
        	tTmp.sContent   = tRes[i]["content"];
        	tTmp.sTime   = tRes[i]["update_time"];
        	tTmp.bIsRead = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	tTmp.lCircleId = TC_Common::strto<Int64>(tRes[i]["circleid"]);
        	tTmp.bIsPass = TC_Common::strto<Bool>(tRes[i]["ispass"]);
			vSysMsgs.push_back(tTmp);
    	}
	}

	// 未读消息
    sSql = " select id, content, update_time, isread, circleid , ispass"
           " from system_user_msg where isread = 0 and uid=" + TC_Common::tostr(tReq.tUserId.lUId) + 
           " order by isread asc, update_time desc limit 0, " + TC_Common::tostr(_uMaxMsgNum);

    LOG->debug()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
    }
	else
	{
		for (size_t i = 0; i < tRes.size(); i++)
    	{
        	SysMsgInfo tTmp;
        	tTmp.lId = TC_Common::strto<Int64>(tRes[i]["id"]);
        	tTmp.sContent   = tRes[i]["content"];
        	tTmp.sTime   = tRes[i]["update_time"];
        	tTmp.bIsRead = TC_Common::strto<Bool>(tRes[i]["isread"]);
        	tTmp.lCircleId = TC_Common::strto<Int64>(tRes[i]["circleid"]);
        	tTmp.bIsPass = TC_Common::strto<Bool>(tRes[i]["ispass"]);
			vSysMsgs.push_back(tTmp);
    	}
	}

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

int DbHandle::setSysMsg(Int64 lUId, SysMsgInfo& tSysMsg, bool bUseCache)
{
    int iDbRet  = FAILURE;

    SysMsgKey   tKey;
    SysMsgValue tValue;
    tKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_sysmsg_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

	iDbRet = this->setSysMsgToDb(lUId, tSysMsg);

	if (iDbRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " Set Sys Msg To DB FAILURE!" << endl;
		return FAILURE;
	}

	tValue.vSysMsgs.insert(tValue.vSysMsgs.begin(), tSysMsg);
	tValue.iTime = tTimeNow;

    CommHashMap::getInstance()->addHashMap(g_sysmsg_hashmap, tKey, tValue);

    return SUCCESS;
}

int DbHandle::setSysMsgReaded(Int64 lUId, bool bUseCache)
{
    int iDbRet  = FAILURE;

    SysMsgKey   tKey;
    SysMsgValue tValue;
    tKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_sysmsg_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

	iDbRet = this->setSysMsgReadedToDb(lUId);

	if (iDbRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " Set Sys Msg To DB FAILURE!" << endl;
		return FAILURE;
	}

	for (auto i = tValue.vSysMsgs.begin(); i != tValue.vSysMsgs.end(); ++i)
	{
		i->bIsRead = true;
	}

	tValue.iTime = tTimeNow;

    CommHashMap::getInstance()->addHashMap(g_sysmsg_hashmap, tKey, tValue);

    return SUCCESS;
}

int DbHandle::setSysMsgReadedToDb(Int64 lUId)
{
    Int32 iRet= FAILURE;

    try 
    {   
        TC_Mysql *mysql = this->getMysql(0);
		string sCondition = " where uid=" + TC_Common::tostr(lUId);

        TC_Mysql::RECORD_DATA rd; 
		rd["isread"] = DBINT(1);

		mysql->updateRecord("system_user_msg", rd, sCondition);

		iRet = SUCCESS;
    }   
    catch(exception &ex)
    {   
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<endl;
    }   

    return iRet;
}

int DbHandle::setSysMsgToDb(Int64 lUId, SysMsgInfo& tSysMsg)
{
    Int32 iRet= FAILURE;

    try 
    {   
        TC_Mysql *mysql = this->getMysql(0);

		/*
        int now = TC_TimeProvider::getInstance()->getNow();
        TC_Mysql::RECORD_DATA rd; 
		rd["uid"] = DBINT(lUId);
        rd["content"]= DBSTR(tSysMsg.sContent);
        rd["isread"]= DBINT(tSysMsg.bIsRead);
        rd["update_time"]= DBDAT(now);
        rd["circleid"]= DBINT(tSysMsg.lCircleId);
		rd["ispass"] = DBINT(tSysMsg.bIsPass);

        if(mysql->insertRecord("system_user_msg", rd))
        {   
            iRet = SUCCESS;
        }
*/
		string sSql = "INSERT INTO system_user_msg(uid, content, isread, update_time, circleid, ispass) values(" 
			+ TC_Common::tostr(lUId) + ", '" 
			+ tSysMsg.sContent + "', 0, now(), " 
			+ TC_Common::tostr(tSysMsg.lCircleId) + ", " 
			+ TC_Common::tostr(int(tSysMsg.bIsPass)) + ")";
		LOG->debug() << __FUNCTION__ << "|sql: " << sSql << endl;
		mysql->execute(sSql);
		tSysMsg.lId = mysql->lastInsertID();
		iRet = SUCCESS;
    }   
    catch(exception &ex)
    {   
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<JCETOSTR(tSysMsg)<<endl;
    }   

    return iRet;
}

int DbHandle::checkPushMsg(UserId const& tUserId, PushMsgValue& tValue)
{
	PushMsgKey tKey;

	tKey.lUId = tUserId.lUId;

    CommHashMap::getInstance()->getHashMap(g_pushmsg_hashmap, tKey, tValue);

	// 检查
	if (tValue.iVersion == 22)
	{
		return tValue.iOnline;
	}

	return -1;
}

int DbHandle::setPushMsg(UserId const& tUserId, Int32 iOnline, string const& sToken)
{
	PushMsgKey tKey;
	PushMsgValue tValue;

	tKey.lUId = tUserId.lUId;

    CommHashMap::getInstance()->getHashMap(g_pushmsg_hashmap, tKey, tValue);

	// 赋值
	vector<string> vTmp;
	long lVersion = 0;
	QUAN::split(tUserId.sUA, "&", vTmp);
	if (vTmp.size() >= 3)
	{
		lVersion = TC_Common::strto<Int64>(vTmp[1]);
	}
	if (lVersion >= 220)
	{
		tValue.iVersion = 22;
	}
	else
	{
		tValue.iVersion = 21;
	}

	if (tUserId.sUA.find("ios") != string::npos)
	{
		tValue.sOs = "ios";
	}
	else
	{
		tValue.sOs = "adr";
	}
	
	if (iOnline != 0)
	{
		tValue.iOnline = iOnline;
	}
	
	tValue.sToken = sToken;

    CommHashMap::getInstance()->addHashMap(g_pushmsg_hashmap, tKey, tValue);

	return SUCCESS;
}

TC_Mysql* DbHandle::getMysql(Int64 iId)
{
    unsigned short uIndex = ((unsigned long long)iId%10) %_uDbNum;
    return _mMysql[uIndex];
}

