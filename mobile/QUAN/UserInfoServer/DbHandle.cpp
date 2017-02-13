#include "DbHandle.h"

int DbHandle::init()
{
    __TRY__

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "UserInfoServer.conf");

    string sHost        =  tConf.get("/main/db/<dbhost>");
    string sUser        =  tConf.get("/main/db/<dbuser>");
    string sPassword    =  tConf.get("/main/db/<dbpass>");
    string sCharset     =  tConf.get("/main/db/<charset>");
    int sPort           =  TC_Common::strto<int>(tConf.get("/main/db/<dbport>"));
    
    _sDbPre             =  tConf.get("/main/db/<dbpre>","secret");
    _sTbUserInfoPre       =  tConf.get("/main/db/<tbuserpre>","user_");
    _sTbUserRelatPre      =  tConf.get("/main/db/<tbuserrelationpre>","user_relation_");
    //_sTbUserCntrtPre        =  tConf.get("/main/db/<tbcntrtpre>","user_contacts_");
    _sTbUserFollowerPre    = tConf.get("/main/db/<tbuserfollowerpre>","user_reversal_relation_");
    _uDbNum             =  TC_Common::strto<int>(tConf.get("/main/db/<dbnum>","1"));
    _uTbNum             =  TC_Common::strto<int>(tConf.get("/main/db/<tbnum>","1"));

    
    _uDbNum = _uDbNum<1?1:_uDbNum;   //至少配置一个db、tb
    _uTbNum = _uTbNum<1?1:_uTbNum;

    LOG->debug()<<__FUNCTION__<<  "dbnum:" << _uDbNum <<" tbnum: "<<_uTbNum <<"charset: sCharset"<< endl;
    
    for (unsigned short i =0;i< _uDbNum; i++)
    {
        _mMysql[i] = new TC_Mysql();
        _mMysql[i]->init(sHost,sUser,sPassword,_sDbPre+TC_Common::tostr(i),sCharset,sPort,0);
    }

    LOG->debug()<<"User:"<<_sTbUserInfoPre<<endl;
    LOG->debug()<<"UserRelation:"<<_sTbUserRelatPre<<endl;
    LOG->debug()<<"UserFollower:"<<_sTbUserFollowerPre<<endl;
    
    
    return 0;
    
    __CATCH__       

    exit(0);
    return -1;
}

int DbHandle::getFriendsList(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds, bool bUseCache)
{
    int iRet  = -1;

    iRet = getFriendsListFromDb(lId, iRelat, vUserIds);

    return iRet;
}

int DbHandle::getFriendsListFromDb(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds)
{
    string sSql;

    __TRY__

    if (iRelat == ONE)
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " and degree=1";
    }
    else if (iRelat == TWO)
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " and degree=2";
    }
    else
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " order by degree";
    }

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(lId)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  success, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }
    vUserIds.reserve(tRes.size());
    for (size_t i = 0; i < tRes.size(); i++)
    {
        FriendsList tTmp;
        tTmp.lUId    = TC_Common::strto<Int64>(tRes[i]["friend_uid"]);
        tTmp.iRelat  = TC_Common::strto<Int64>(tRes[i]["degree"]);
        tTmp.iTimespan=TC_Common::strto<Int64>(tRes[i]["post_time"]);

        if(tTmp.lUId!=lId)
        {
            vUserIds.push_back(tTmp);
        }
        else
        {
            FDLOG("WARNING")<<"UID:"<<lId<<" has self relation:"<<tTmp.iRelat<<endl;
        }
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

int DbHandle::getFriendsListFromDb(taf::Int64 lId, int iRelat, vector<FriendsListDb>& vUserIds)
{
    string sSql;

    __TRY__

    if (iRelat == ONE)
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time,UNIX_TIMESTAMP(favor_time) as favor_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " and degree=1";
    }
    else if (iRelat == TWO)
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time,UNIX_TIMESTAMP(favor_time) as favor_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " and degree=2";
    }
    else
    {
        sSql = "select friend_uid,degree,UNIX_TIMESTAMP(post_time) as post_time,UNIX_TIMESTAMP(favor_time) as favor_time from "+ getRelationTbName(lId) + " where main_uid= " + TC_Common::tostr(lId) + " order by degree";
    }


    taf::TC_Mysql::MysqlData tRes = getMysql(lId)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  success, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }
    vUserIds.reserve(tRes.size());
    for (size_t i = 0; i < tRes.size(); i++)
    {
        FriendsListDb tTmp;
        tTmp.lUId    = TC_Common::strto<Int64>(tRes[i]["friend_uid"]);
        tTmp.iRelat  = TC_Common::strto<Int64>(tRes[i]["degree"]);
        tTmp.iPostTime=TC_Common::strto<Int32>(tRes[i]["post_time"]);
        tTmp.iFavorTime=TC_Common::strto<Int32>(tRes[i]["favor_time"]);

        if(tTmp.lUId!=lId)
        {
            vUserIds.push_back(tTmp);
        }
        else
        {
            FDLOG("WARNING")<<"UID:"<<lId<<" has self relation:"<<tTmp.iRelat<<endl;
        }
    }

    return SUCCESS;
    __CATCH__

    LOG->error()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    return FAILURE;
}

int DbHandle::getFollowersListFromDb(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds)
{
    string sSql;

    __TRY__

    if (iRelat == ONE)
    {
        sSql = "select main_uid,degree,UNIX_TIMESTAMP(visit_time) as visit_time  from "+ getFollowerTbName(lId) + " where friend_uid= " + TC_Common::tostr(lId) + " and degree=1";
    }
    else if (iRelat == TWO)
    {
        sSql = "select main_uid,degree,UNIX_TIMESTAMP(visit_time) as visit_time  from "+ getFollowerTbName(lId) + " here friend_uid= " + TC_Common::tostr(lId) + " and degree=2";
    }
    else
    {
        sSql = "select main_uid,degree,UNIX_TIMESTAMP(visit_time) as visit_time  from "+ getFollowerTbName(lId) + " where friend_uid= " + TC_Common::tostr(lId) + " order by degree";
    }

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(lId)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  success, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }

    vUserIds.reserve(tRes.size());
    for (size_t i = 0; i < tRes.size(); i++)
    {
        FriendsList tTmp;
        tTmp.lUId    = TC_Common::strto<Int64>(tRes[i]["main_uid"]);
        tTmp.iRelat  = TC_Common::strto<Int64>(tRes[i]["degree"]);
        tTmp.iTimespan=TC_Common::strto<Int64>(tRes[i]["visit_time"]);
        if(tTmp.lUId!=lId)
        {
            vUserIds.push_back(tTmp);
        }
        else
        {
            FDLOG("WARNING")<<"UID:"<<lId<<" has self relation:"<<tTmp.iRelat<<endl;
        }
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

int DbHandle::SaveFriendListCache(taf::Int64 lId,FriendListCacheValue &value)
{
    int iRet=-1;
    try
    {
        for(vector<FriendsList>::iterator it=value.vPostFriends.begin();it!=value.vPostFriends.end();++it)
        {
            if(it->iTimespan<=0) continue;
            string sCondition=" where main_uid="+TC_Common::tostr(lId)+" and friend_uid="+TC_Common::tostr(it->lUId) + " and (post_time is NULL or post_time<from_unixtime("+TC_Common::tostr(it->iTimespan)+"))";
            TC_Mysql* mysql=getMysql(lId);

            TC_Mysql::RECORD_DATA rd;
            rd["post_time"]=make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(it->iTimespan)+")");

            int iRet = mysql->updateRecord(getRelationTbName(lId), rd,sCondition);

            if(iRet==0)
            {
                FDLOG("ASYNC_DB")<<__FUNCTION__<<"|POSTTIME|"<<lId<<"|"<<it->lUId<<"|"<<"ERROR:"<<it->iTimespan<<endl;
            }
        }
        for(vector<FriendsList>::iterator it=value.vFavorFriends.begin();it!=value.vFavorFriends.end();++it)
        {
            if(it->iTimespan<=0) continue;
            string sCondition=" where main_uid="+TC_Common::tostr(lId)+" and friend_uid="+TC_Common::tostr(it->lUId) + " and (favor_time is NULL or favor_time<from_unixtime("+TC_Common::tostr(it->iTimespan)+"))";
            TC_Mysql* mysql=getMysql(lId);

            TC_Mysql::RECORD_DATA rd;
            rd["favor_time"]=make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(it->iTimespan)+")");

            int iRet = mysql->updateRecord(getRelationTbName(lId), rd,sCondition);

            if(iRet==0)
            {
                FDLOG("ASYNC_DB")<<__FUNCTION__<<"|FAVORTIME|"<<lId<<"|"<<it->lUId<<"|"<<"ERROR:"<<it->iTimespan<<endl;
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<ex.what()<<"|"<<lId<<endl;
        iRet=-1;
    }
    return iRet;
}

int DbHandle::SaveFollowerListCache(taf::Int64 lId,FollowerListCacheValue &value)
{
    int iRet=-1;
    try
    {
        for(vector<FriendsList>::iterator it=value.vFollowers.begin();it!=value.vFollowers.end();++it)
        {
            if(it->iTimespan<=0) continue;
            string sCondition=" where main_uid="+TC_Common::tostr(lId)+" and friend_uid="+TC_Common::tostr(it->lUId)+ " and (visit_time is NULL or visit_time<from_unixtime("+TC_Common::tostr(it->iTimespan)+"))";
            TC_Mysql* mysql=getMysql(lId);

            TC_Mysql::RECORD_DATA rd;
            rd["post_time"]=make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(it->iTimespan)+")");

            int iRet = mysql->updateRecord(getFollowerTbName(lId), rd,sCondition);

            if(iRet==0)
            {
                FDLOG("ASYNC_DB")<<__FUNCTION__<<"|VISITTIME|"<<lId<<"|"<<it->lUId<<"|"<<"ERROR:"<<it->iTimespan<<endl;
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<ex.what()<<"|"<<lId<<endl;
    }
    return iRet;
}

int DbHandle::loadUserInfo(taf::Int64 lId,UserInfoCacheValue &value)
{
    string sSql;
    int iRet=-1;

    sSql = "select UNIX_TIMESTAMP(regist_time) as regist_time"
        ",UNIX_TIMESTAMP(post_time) as post_time"
        ",UNIX_TIMESTAMP(favor_time) as favor_time"
        //",UNIX_TIMESTAMP(visit_time) as visit_time"
        " from "+ getUserTbName(lId) + " where id= " + TC_Common::tostr(lId);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    try
    {
        taf::TC_Mysql::MysqlData tRes = getMysql(lId)->queryRecord(sSql);
        if(tRes.size()==1)
        {
            int iRegistTime=TC_Common::strto<int>(tRes[0]["regist_time"]);
            if(iRegistTime>value.iRegistTime) 
            {
                value.iRegistTime=iRegistTime;
            }
            int iPostTime=TC_Common::strto<int>(tRes[0]["post_time"]);
            if(iPostTime>value.iPostTime) 
            {
                value.iPostTime=iPostTime;
            }
            int iFavorTime=TC_Common::strto<int>(tRes[0]["favor_time"]);
            if(iFavorTime>value.iFavorTime) 
            {
                value.iFavorTime=iFavorTime;
            }
            //int iVisitTime=TC_Common::strto<int>(tRes[0]["visit_time"]);
            //if(iVisitTime>value.iVisitTime) {value.iVisitTime=iVisitTime;}
            iRet=0;

            LOG->debug()<<__FUNCTION__<<"|"<<"RegistTime:"<<value.iRegistTime<<endl;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|"<<"Get User RET SIZE:"<<tRes.size()<<endl;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<ex.what()<<"|"<<lId<<endl;
    }

    return iRet;
}

int DbHandle::updatePostTime(taf::Int64 lId,int iTimespan)
{
    if (iTimespan<=0) return 0;
    try
    {
        string sCondition=" where id="+TC_Common::tostr(lId) + " and (post_time is NULL or post_time<from_unixtime("+TC_Common::tostr(iTimespan)+"))";
        TC_Mysql* mysql=getMysql(lId);

        TC_Mysql::RECORD_DATA rd;
        rd["post_time"]=make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(iTimespan)+")");

        int iRet = mysql->updateRecord(getUserTbName(lId), rd,sCondition);

        if(iRet==0)
        {
            return -1;
        }
        return 0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<ex.what()<<"|"<<lId<<endl;
        return -1;
    }
}

int DbHandle::updateFavorTime(taf::Int64 lId,int iTimespan)
{
    if (iTimespan<=0) return 0;
    try
    {
        string sCondition=" where id="+TC_Common::tostr(lId) + " and (favor_time is NULL or favor_time<from_unixtime("+TC_Common::tostr(iTimespan)+"))";
        TC_Mysql* mysql=getMysql(lId);

        TC_Mysql::RECORD_DATA rd;
        rd["favor_time"]=make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(iTimespan)+")");

        int iRet = mysql->updateRecord(getUserTbName(lId), rd,sCondition);

        if(iRet==0)
        {
            return -1;
        }
        return 0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<ex.what()<<"|"<<lId<<endl;
        return -1;
    }
}

TC_Mysql* DbHandle::getMysql(Int64 iId)
{
    unsigned short uIndex = ((unsigned long long)iId) %(_uDbNum * _uTbNum) /_uTbNum;
    return _mMysql[uIndex];
}

string DbHandle::getRelationTbName(Int64 lId)
{
    unsigned short uIndex = ((unsigned long long)lId) %(_uDbNum *_uTbNum) %_uTbNum;
    return _sTbUserRelatPre+TC_Common::tostr(uIndex);
}

string DbHandle::getFollowerTbName(Int64 lId)
{
    unsigned short uIndex = ((unsigned long long)lId) %(_uDbNum *_uTbNum) %_uTbNum;
    return _sTbUserFollowerPre+TC_Common::tostr(uIndex);
}

string DbHandle::getUserTbName(Int64 lId)
{
    unsigned short uIndex = ((unsigned long long)lId) %(_uDbNum *_uTbNum) % _uTbNum;
    return _sTbUserInfoPre+TC_Common::tostr(uIndex);
}
