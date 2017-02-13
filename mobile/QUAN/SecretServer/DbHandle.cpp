#include "DbHandle.h"

SecretHashMap   g_secret_hashmap;
ContentHashMap  g_content_hashmap;

Int32 DbHandle::init()
{
    __TRY__

    TC_Config tConf;
    tConf.parseFile(ServerConfig::BasePath + "SecretServer.conf");

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
    _uCacheTimeOut      = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","10"));
    _uRecmdTimeOut      = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","10"));
    _uSysTimeOut        = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<timeout>","10"));

    _uUserScrtCacheTimeOut = TC_Common::strto<unsigned short>(tConf.get("/main/hashmap/<userscrttimeout>","60"));
    
    _uDbNum = _uDbNum<1?1:_uDbNum;   //至少配置一个db、tb
    _uTbNum = _uTbNum<1?1:_uTbNum;

    LOG->debug()<<__FUNCTION__<<  "dbnum:" << _uDbNum <<" tbnum: "<<_uTbNum <<"charset: sCharset"<< endl;
    
    for (unsigned short i =0;i< _uDbNum; i++)
    {
        _mMysql[i] = new TC_Mysql();
        _mMysql[i]->init(sHost,sUser,sPassword,_sDbPre+TC_Common::tostr(i),sCharset,sPort,0);
    }
    
    
    return 0;
    
    __CATCH__       

    exit(0);
    return -1;
}

Int32 DbHandle::getSystemSecret(SecretInfo& tSecret, bool bUseCache)
{
    Int32 iRet = FAILURE;
    static SecretInfo  tSysScrt;
    static time_t      iSysTime = 0;

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();
    Int32 iIntev = tTimeNow - iSysTime;
    
    LOG->info()<<"cache value |time Intev :|"<<iIntev<<"|UseCache|"<<bUseCache<<endl;

    if (tSysScrt.lSId == 0 || iIntev > _uSysTimeOut)
    {
        iRet = getSystemSecretFromDb(tSecret);
        if (iRet != FAILURE)
        {
            tSysScrt = tSecret;
            iSysTime = tTimeNow;
        }

        LOG->debug()<<" get System Secrets From Db " <<endl;
    }
    else
    {
        tSecret = tSysScrt;
        LOG->debug()<<" get System Secrets From cache " <<endl;
        return SUCCESS;
    }

    return iRet;
}

Int32 DbHandle::getSystemSecretFromDb(SecretInfo& tSecret)
{
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

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 系统推荐帖子
Int32 DbHandle::getRecommendSecrets(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets, bool bUseCache)
{
    Int32 iRet  = FAILURE;
    static ScrtVector  vRcmdCaches;
    static time_t      iRcmdTime = 0;

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();
    Int32 iIntev = tTimeNow - iRcmdTime;
    
    LOG->info()<<"cache value |time Intev :|"<<iIntev<<"| UId: "<<tUserId.lUId<<"|UseCache|"<<bUseCache<<endl;

    if (vRcmdCaches.size() == 0 || iIntev > _uRecmdTimeOut)
    {
        iRet = getRecommendSecretsFromDb(tUserId, _u24SecretsNum, _u72SecretsNum, vSecrets);
        if (iRet != FAILURE)
        {
            vRcmdCaches = vSecrets;
            iRcmdTime = tTimeNow;
            iRet = SUCCESS;
        }

        LOG->debug()<<" get Recommend Secrets From Db " <<endl;
    }
    else
    {
        vSecrets = vRcmdCaches;
        LOG->debug()<<" get Recommend Secrets From cache " <<endl;
        iRet = SUCCESS;
    }

    return iRet;
}

// 用户移除的关系
Int32 DbHandle::getUserRemoveSecrets(Int64 lUId, SecretRelatMap& tRemoveScrts, bool bUseCache)
{
    Int32 iRet    = -1;
    Int32 iDbRet  = -1;
    Int32 iMemRet = -1;

    SecretListKey   tSecretKey;
    SecretListValue tSecretValue;
    tSecretKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tSecretKey,tSecretValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tSecretValue.tRemoveScrts.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        iDbRet = getUserRemoveSecretsFromDb(lUId, tRemoveScrts);

        LOG->info()<<"get User Remove Secrets RELATION from db ret:"<<iRet<<"| UId: "<<lUId<<endl;

        if (iDbRet == 0 && lUId != 0)
        {
            tSecretValue.tRemoveScrts = tRemoveScrts;
            tSecretValue.tRemoveScrts.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tSecretKey, tSecretValue);
        }
    }
    else
    {
        LOG->info()<<"get User Remove Secrets RELATION from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    tRemoveScrts = tSecretValue.tRemoveScrts;

    if (iDbRet ==0 )
    {
        iRet = 1; //Cahce命中超时

    }
    else if (iMemRet==0 && iIntev<=60*10)
    {
        iRet = 0; //CacheOK
    }

     display(" Remove Relation : ", tSecretValue.tRemoveScrts.mSId);

    return iRet;
}

// 用户移除的关系
Int32 DbHandle::getUserRemoveSecretsFromDb(Int64 lUId, SecretRelatMap& tRemoveScrts)
{
    __TRY__

    string sSql = "select objectid,relatid from posts_operation where uid=  " + TC_Common::tostr(lUId) 
                  + "  and type=0 and operation=2 and isavailable=1 order by update_time desc limit 0, " + TC_Common::tostr(_uUserRemoveNum);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }
    else 
    {
        // 清空缓存
        if (tRemoveScrts.mSId.size() != 0)
        {
            tRemoveScrts.mSId.clear();
        }
        // 重新载入数据 
        for (size_t i = 0; i < tRes.size(); i++)
        {
            IdNum tTmp;
            Int64 lSId = TC_Common::strto<Int64>(tRes[i]["objectid"]); 
            tTmp.lId  = TC_Common::strto<Int64>(tRes[i]["relatid"]);
            tTmp.iNum = 0;
            tRemoveScrts.mSId[lSId] = tTmp;            
        }
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 用户点赞评论的关系
Int32 DbHandle::getUserContentSecrets(Int64 lUId, Int64 lSId, ContentRelatMap& tFavorCnts, bool bUseCache)
{
    Int32 iRet    = -1;
    Int32 iDbRet  = -1;
    Int32 iMemRet = -1;

    SecretListKey   tSecretKey;
    SecretListValue tSecretValue;
    tSecretKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tSecretKey,tSecretValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tSecretValue.tFavorCnts.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        iDbRet =  getUserFavorContentsFromDb(lUId, tFavorCnts);

        LOG->info()<<"get User Favor Contents from db ret:"<<iRet<<"| UId: "<<lUId<<endl;

        if (iDbRet == 0 && lUId != 0)
        {
            tSecretValue.tFavorCnts = tFavorCnts;
            tSecretValue.tFavorCnts.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tSecretKey, tSecretValue);
        }
    }
    else
    {
        LOG->info()<<"get User Favor Contents from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    tFavorCnts = tSecretValue.tFavorCnts;

    if (iDbRet ==0)
    {
        iRet = 1; //Cahce命中超时

    }
    else if (iMemRet==0 && iIntev<=60*10)
    {
        iRet = 0; //CacheOK
    }

     display(" Favor CONENT Relation : ", tSecretValue.tFavorCnts.mCId);

    return iRet;
}
// 用户点赞评论的关系
Int32 DbHandle::getUserFavorContentsFromDb(Int64 lUId, ContentRelatMap& tFavorCnts)
{
    __TRY__

    string sSql = "select objectid,relatid from posts_operation where uid=  " + TC_Common::tostr(lUId) 
                  + "  and type=1 and operation=0 and isavailable=1 order by update_time desc limit 0, " + TC_Common::tostr(_uUserFavorNum);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }
    else 
    {
        // 清空缓存
        if (tFavorCnts.mCId.size() != 0)
        {
            tFavorCnts.mCId.clear();
        }
        // 重新载入数据 
        for (size_t i = 0; i < tRes.size(); i++)
        {
            IdNum tTmp;
            Int64 lCId = TC_Common::strto<Int64>(tRes[i]["objectid"]); 
            tTmp.lId = TC_Common::strto<Int64>(tRes[i]["relatid"]);
            tTmp.iNum = 0;
            tFavorCnts.mCId[lCId]= tTmp;            
        }
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 用户点赞帖子的关系
Int32 DbHandle::getUserFavorSecrets(Int64 lUId, SecretRelatMap& tFavorScrts, bool bUseCache)
{
    Int32 iRet    = -1;
    Int32 iDbRet  = -1;
    Int32 iMemRet = -1;

    SecretListKey   tSecretKey;
    SecretListValue tSecretValue;
    tSecretKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tSecretKey,tSecretValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tSecretValue.tFavorScrts.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"
    <<iIntev<<"NowTime: |"<<tTimeNow<<"|ValueTime :|"<<tSecretValue.tFavorScrts.iTime<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        iDbRet = getUserFavorSecretsFromDb(lUId, tFavorScrts);

        LOG->info()<<" get User Favor Secrets RELATION from db ret:"<<iDbRet<<"| UId: "<<lUId<<endl;

        if (iDbRet == 0 && lUId != 0)
        {
            tSecretValue.tFavorScrts = tFavorScrts;
            tSecretValue.tFavorScrts.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tSecretKey, tSecretValue);
        }
    }
    else
    {
        LOG->info()<<" get User Favor Secrets RELATION from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    tFavorScrts = tSecretValue.tFavorScrts;

    if (iDbRet ==0)
    {
        iRet = 1; //Cahce命中超时

    }
    else if (iMemRet==0 && iIntev<=60*10)
    {
        iRet = 0; //CacheOK
    }

    display(" Favor Relation : ", tSecretValue.tFavorScrts.mSId);

    return iRet;
}

// 用户点赞帖子的关系
Int32 DbHandle::getUserFavorSecretsFromDb(Int64 lUId, SecretRelatMap& tFavorScrts)
{
    __TRY__

    string sSql = "select objectid,relatid from posts_operation where uid=  " + TC_Common::tostr(lUId) 
                  + "  and type=0 and operation=0 and isavailable=1 order by update_time desc limit 0, " + TC_Common::tostr(_uUserFavorNum);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }
    else 
    {
        // 清空缓存
        if (tFavorScrts.mSId.size() != 0)
        {
            tFavorScrts.mSId.clear();
        }       
        // 重新载入数据 
        for (size_t i = 0; i < tRes.size(); i++)
        {
            IdNum tTmp;
            Int64 lSId = TC_Common::strto<Int64>(tRes[i]["objectid"]); 
            tTmp.lId = TC_Common::strto<Int64>(tRes[i]["relatid"]);
            tTmp.iNum = 1;
            tFavorScrts.mSId[lSId] = tTmp;            
        }
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 系统推荐帖子
Int32 DbHandle::getRecommendSecretsFromDb(UserId const& tUserId, Int32 _u24SecretsNum, Int32 _u72SecretsNum, ScrtVector& vSecrets)
{
    __TRY__

    string sSql = " select id, uid, colorid, biturl, content, source, location, favor_num, cmt_num "  
                  " from posts p where source=0 and isremove=0 and location is not null and location !='' and datediff(update_time, now())<=1 "
                  " order by favor_num desc limit 0, " + TC_Common::tostr(_u24SecretsNum);

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
           SecretInfo    tTmp;
           tTmp.lSId      = TC_Common::strto<Int64>(tRes[i]["id"]); 
           tTmp.lUId      = TC_Common::strto<Int64>(tRes[i]["uid"]); 
           tTmp.iColorId  = TC_Common::strto<Int32>(tRes[i]["colorid"]); 
           tTmp.sBitUrl   = tRes[i]["biturl"]; 
           tTmp.sContent  = tRes[i]["content"]; 
           tTmp.iType     = RECMD;
           tTmp.sLocal    = tRes[i]["location"]; 
           tTmp.iFavorNum = TC_Common::strto<Int32>(tRes[i]["favor_num"]); 
           if (tTmp.iFavorNum < 0)
               tTmp.iFavorNum = 0;
           tTmp.iCmtNum   = TC_Common::strto<Int32>(tRes[i]["cmt_num"]); 
           if (tTmp.iCmtNum < 0)
               tTmp.iCmtNum   = 0;
           tTmp.isRead    = false;
           tTmp.isFavor   = false;
           vSecrets.push_back(tTmp);            
       }
    }

    // 测试的时候应该做屏蔽
    sSql = " select id, uid, colorid, biturl, content, source, location, favor_num, cmt_num "  
                  " from posts p where source=0 and isremove=0 and location is not null and location !='' and datediff(update_time, now())<=3 "
                  " order by favor_num desc limit 0, " + TC_Common::tostr(_u72SecretsNum);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql);

    if (tRes1.size() == 0)
    {
        LOG->debug()<<__FUNCTION__<< "  SUCCESS, size(" << tRes1.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }

    for (size_t i = 0; i < tRes1.size(); i++)
    {
        SecretInfo    tTmp;
        tTmp.lSId      = TC_Common::strto<Int64>(tRes1[i]["id"]); 
        tTmp.lUId      = TC_Common::strto<Int64>(tRes1[i]["uid"]); 
        tTmp.iColorId  = TC_Common::strto<Int32>(tRes1[i]["colorid"]); 
        tTmp.sBitUrl   = tRes1[i]["biturl"]; 
        tTmp.sContent  = tRes1[i]["content"]; 
        tTmp.iType     = RECMD;
        tTmp.sLocal    = tRes1[i]["location"]; 
        tTmp.iFavorNum = TC_Common::strto<Int32>(tRes1[i]["favor_num"]); 
        if (tTmp.iFavorNum < 0)
           tTmp.iFavorNum = 0;
        tTmp.iCmtNum   = TC_Common::strto<Int32>(tRes1[i]["cmt_num"]); 
        if (tTmp.iCmtNum < 0)
           tTmp.iCmtNum   = 0;
        tTmp.isRead    = false;
        tTmp.isFavor   = false;
        vSecrets.push_back(tTmp);            
    }

    // 擦除用户自己的帖子
    vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), EqualUId(tUserId.lUId)), vSecrets.end()); 

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 获取用户赞帖列表
Int32 DbHandle::getBatchUserFavorSecrets(Int64 lUId, Int32 iSum, Int32 iFrdsFvrNum, vector<FriendsList> const& vFrds, ScrtVector& vSecrets)
{
    Int32 iRet = FAILURE;

    vector<FriendsList>::const_iterator iBegin = vFrds.begin();
    vector<FriendsList>::const_iterator iEnd   = vFrds.end();

    RelatList     mPIdFavorNum;    // 点赞计数
    RelatIterator pos;

    // 遍历好友列表 
    for (vector<FriendsList>::const_iterator i = iBegin; i != iEnd; ++i)
    {
        SecretRelatMap     tFavorScrts;
        // 获取用户点赞帖子关系
        iRet = this->getUserFavorSecrets(i->lUId, tFavorScrts);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<<" FriendsId: "<<i->lUId<<" getUserFavorSecrets(i->lUId, tFavorScrts) failure!"<<endl;
        }

        LOG->info() << __FUNCTION__<<" FriendsId: "<<i->lUId<<" getUserFavorSecrets(i->lUId, tFavorScrts) success!"<<endl;

        // 生成点赞关系
        for (pos = tFavorScrts.mSId.begin(); pos != tFavorScrts.mSId.end(); ++pos) 
        {
            RelatIterator i = mPIdFavorNum.find(pos->first); 
            if (i != mPIdFavorNum.end())
            {
                mPIdFavorNum.insert(make_pair(pos->first, pos->second));
            }
            else
            {
                mPIdFavorNum[pos->first].iNum += 1;
            }                        
        }
    }

    Int32 i = 0;
    // 取帖子内容
    for (pos = mPIdFavorNum.begin(); pos != mPIdFavorNum.end(); ++pos) 
    {
        if ((pos->second).iNum > iFrdsFvrNum)
        {
            SecretInfo  tTmp;
            iRet = getUserSecrets((pos->second).lId, pos->first, RECMD, tTmp);
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<<" FriendsId: "<<(pos->second).lId<<" SId: "<<pos->first<<" ONE getUserSecrets failure! "<<endl;
            } 
            else 
            {
                LOG->info() << __FUNCTION__<<" FriendsId: "<<(pos->second).lId<<" SId: "<<pos->first<<" ONE getUserSecrets success! "<<endl;
                vSecrets.push_back(tTmp);
                if (i++ > iSum)
                    break;
            }
        }
    }

    return SUCCESS;
}

// 批量获取用户，用户好友，好友好友发帖子列表
Int32 DbHandle::getAllUserSecrets(vector<FriendsList> const& vFrdsAll, size_t iScrtsNum, Int64 lBeginId, Int64& lNextBeginId, vector<SecretInfo>& vSecrets)
{
    Int32 iRet = FAILURE;
    Int64 lMinNextId = LONG_MAX;

    LOG->info() << __FUNCTION__<< " begin "<<endl;

    if (vFrdsAll.size() == 0)
    {
        LOG->info() << __FUNCTION__<<" Don't have Friends getBatchUserSecrets SUCCESS!"<<endl;
        return SUCCESS;
    }

    vector<FriendsList>::const_iterator iBegin = vFrdsAll.begin();
    vector<FriendsList>::const_iterator iEnd   = vFrdsAll.end();

    for (vector<FriendsList>::const_iterator i = iBegin; i != iEnd; ++i)
    {
        vector<SecretInfo>    vTmp;
        Int64                 lTmp;
        iRet = this->getBatchUserSecrets(i->lUId, i->iRelat, lBeginId, lTmp, vTmp);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<<" FriendsId: "<<i->lUId<<" | "<<" getBatchUserSecrets failure!"<<endl;
        }

        LOG->info() << __FUNCTION__<<" UserId: "<<i->lUId<<" | "<<" getBatchUserSecrets success!"<<endl;

        if (lTmp < lMinNextId)
        {
            lMinNextId = lTmp;
        }

        if (vTmp.size() >= iScrtsNum && lBeginId != -1)
        { 
            ScrtIterator pos = find_if(vTmp.begin(), vTmp.end(), LessSId(lBeginId));
            if (pos == vTmp.end())
                continue;
            if (ScrtVector::size_type(vTmp.end() - pos) >= iScrtsNum)
                copy(pos, pos + iScrtsNum, back_inserter(vSecrets));
            else
                copy(vTmp.begin(), vTmp.end(), back_inserter(vSecrets));
        }
        else if (vTmp.size() >= iScrtsNum && lBeginId == -1)
        { 
            copy(vTmp.begin(), vTmp.begin()+iScrtsNum, back_inserter(vSecrets));
        }
        else
        {
            copy(vTmp.begin(), vTmp.end(), back_inserter(vSecrets));
        }
    }

    lNextBeginId = lMinNextId;

    LOG->info() << __FUNCTION__<< " end "<<endl;

    return SUCCESS;
}

// 批量获取用户帖子列表
Int32 DbHandle::getBatchUserSecrets(Int64 lUId, Int32 iType, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets, bool bUseCache)
{

    Int32 iRet    = FAILURE;
    Int32 iDbRet  = FAILURE;
    Int32 iMemRet = FAILURE;

    SecretListKey   tSecretKey;
    SecretListValue tSecretValue;
    tSecretKey.lUId = lUId;

    __TRY__

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tSecretKey,tSecretValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tSecretValue.iTime;

    LOG->info()<<" cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;
  
    if (lBeginId == -1 || lBeginId < tSecretValue.lNextBeginId || iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        iDbRet =  getBatchUserSecretsFromDb(lUId, iType, lBeginId, lNextBeginId, vSecrets);

        LOG->info()<<" get User Batch Secrets from db ret:"<<iDbRet<<"| UId: "<<lUId<<endl;

        if (iDbRet == SUCCESS && lUId != 0)
        {
            tSecretValue.vSecrets = vSecrets;
            tSecretValue.lNextBeginId = lNextBeginId;
            tSecretValue.iTime        = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tSecretKey, tSecretValue);
        }
    }
    else
    {
        LOG->info()<<" get Batch User Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    vSecrets = tSecretValue.vSecrets;
    lNextBeginId = tSecretValue.lNextBeginId;

    if (iDbRet == SUCCESS)
    {
        iRet = 1; //Cahce命中超时

    }
    else if (iMemRet== SUCCESS && iIntev<=60*10)
    {
        iRet = 0; //CacheOK
    }

    return iRet;
}

// 批量获取用户帖子列表
Int32 DbHandle::getBatchUserSecretsFromDb(Int64 lUId, Int32 iType, Int64 lBeginId, Int64& lNextBeginId, ScrtVector& vSecrets)
{
    __TRY__

    string sPage;
    string sl;

    if (lBeginId == -1)
        sPage = "";
    else 
        sPage = " and id < " + TC_Common::tostr(lBeginId);
/*  // 消息提醒调用有点问题
    if (iType == RECMD)
        sl = " and p.location is not null and p.location !='' " ;
    else
        sl = "";
*/
    sl = "";

    string sSql = " select id, uid, colorid, biturl, content, source, location, favor_num, cmt_num "  
                  " from posts p "
                  " where uid="  + TC_Common::tostr(lUId) +  
                  " and p.isremove=0 " + sl +
                  " and p.islock=0 "+ sPage + " order by p.update_time desc limit 0,500; ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        lNextBeginId = -1;
        return SUCCESS;
    }
    
    for (size_t i = 0; i < tRes.size(); i++)
    {
        SecretInfo    tTmp;
        tTmp.lSId      = TC_Common::strto<Int64>(tRes[i]["id"]); 
        tTmp.lUId      = TC_Common::strto<Int64>(tRes[i]["uid"]); 
        tTmp.iColorId  = TC_Common::strto<Int32>(tRes[i]["colorid"]); 
        tTmp.sBitUrl   = tRes[i]["biturl"]; 
        tTmp.sContent  = tRes[i]["content"]; 
        tTmp.iType     = iType;
        tTmp.sLocal    = tRes[i]["location"]; 
        tTmp.iFavorNum = TC_Common::strto<Int32>(tRes[i]["favor_num"]); 
        if (tTmp.iFavorNum < 0)
            tTmp.iFavorNum = 0;
        tTmp.iCmtNum   = TC_Common::strto<Int32>(tRes[i]["cmt_num"]); 
        if (tTmp.iCmtNum < 0)
            tTmp.iCmtNum  = 0;
        tTmp.isFavor   = false;
        tTmp.isRead    = false;
        tTmp.isRemove  = 0;
        vSecrets.push_back(tTmp);            
    }

    // 下一个起始号
    lNextBeginId = vSecrets.back().lSId;

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 获取用户帖帖子
Int32 DbHandle::getUserSecrets(Int64 lUId, Int64 lSId, Int32 iType, SecretInfo& tSecret)
{
    Int32  iRet  = FAILURE;
    Int64 lBeginId = -1;
    Int64 lNextBeginId = 0;
    ScrtVector   vSecrets;
   
    iRet = this->getBatchUserSecrets(lUId, iType, lBeginId, lNextBeginId, vSecrets);
    if (iRet != FAILURE)
    {
        LOG->info()<<__FUNCTION__<< " get Batch User Secrets find the lSId success"<<endl;
        iRet = -1;
        for (ScrtIterator i = vSecrets.begin(); i != vSecrets.end(); ++i)
        {
            if (i->lSId == lSId)
            {
                tSecret = *i;
                iRet = 0;
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

Int32 DbHandle::getUserBatchSecrets(Int64 lUId, map<Int64, MsgRmd> const& mSIds, ScrtVector& vSecrets)
{
    Int32  iRet  = FAILURE;
    Int64 lBeginId = -1;
    Int64 lNextBeginId = 0;
    ScrtVector   vTmp;
    map<Int64, Int64>  mTmp;
    
    for (map<Int64, MsgRmd>::const_iterator pos = mSIds.begin(); pos != mSIds.end(); ++pos)
    {
        mTmp.insert(make_pair(pos->second.lFirstId, pos->first));
    }

    for (map<Int64, Int64>::iterator pos = mTmp.begin(); pos != mTmp.end(); ++pos)
    {
        ScrtVector   vSecretsSum;
        map<Int64, MsgRmd>::const_iterator it = mSIds.find(pos->second);
        iRet = this->getBatchUserSecrets(pos->first, it->second.iRelat, lBeginId, lNextBeginId, vSecretsSum);
        if (iRet != FAILURE && vSecretsSum.size() != 0)
        {
            LOG->info()<<__FUNCTION__<< " lUId: "<<lUId<<" get Batch User Secrets find the lUId success"<<endl;
            for (ScrtIterator i = vSecretsSum.begin(); i != vSecretsSum.end(); ++i)
            {
                map<Int64, MsgRmd>::const_iterator p = mSIds.find(i->lSId);
                if (p != mSIds.end())
                {
                    i->isRead = p->second.isRead;
                    if (i->isRead == true)
                    {
                        i->iNotRead = 0;
                        vTmp.push_back(*i);
                    }
                    else
                    {
                        i->iNotRead = 0;
                        vSecrets.push_back(*i);
                    }
                }
            }
        } 
        else
        {
            LOG->debug()<<__FUNCTION__<< " get Batch User Secrets find the lSId failure"<<endl; 
        }
    }

    if (vTmp.size() != 0)
        copy(vTmp.begin(), vTmp.end(), back_inserter(vSecrets));

    sort(vSecrets.rbegin(), vSecrets.rend());

    return SUCCESS;
}

// 发表帖子
Int32 DbHandle::setSecret(PostSecretReq const& tReq, Int64 lSId, bool bUseCache)
{
    Int32 iRet  = FAILURE;

    iRet = setSecretToDb(tReq, lSId);

    if(iRet)
    {
        return FAILURE;
    }
 
    __TRY__ 

    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = tReq.tUserId.lUId;

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }

    SecretInfo tSecret;
    tSecret.lSId = lSId;
    tSecret.lUId = tReq.tUserId.lUId;
    tSecret.iColorId = tReq.iColorId;
    tSecret.sBitUrl = tReq.sBitUrl;         // 图URL
    tSecret.sContent = tReq.sContent;        // 内容
    tSecret.sLocal = tReq.sLocal;          // 地理位置(来源type为推荐时需要来源城市名)
    tSecret.isFavor = false;
    tSecret.isRead  = false;

    tValue.vSecrets.insert(tValue.vSecrets.begin(), tSecret);
    tValue.lNextBeginId = lSId;
    tValue.iTime = TC_TimeProvider::getInstance()->getNow();

    ostringstream os1; tSecret.displaySimple(os1);
    LOG->info()<<__FUNCTION__<<" Add To Hash Map Secret List "<<os1.str()<<endl;

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    __CATCH__

    return SUCCESS;
}

Int32 DbHandle::setSecretToDb(PostSecretReq const& tReq, Int64 lSId)
{
    Int32 iRet = FAILURE;
    __TRY__

    TC_Mysql  *tMysql       = getMysql(0);

    string sSql = "INSERT INTO  posts (id, uid, content, colorid, biturl, favor_num, cmt_num, report_num, location, coord, source, islock, isremove, update_time) values("
                  "'" + TC_Common::tostr(lSId) + "', "
                  "'" + TC_Common::tostr(tReq.tUserId.lUId) + "', "
                  "'" + tMysql->escapeString(tReq.sContent) + "', "
                  "'" + TC_Common::tostr(tReq.iColorId) + "', "
                  "'" + tMysql->escapeString(tReq.sBitUrl) + "', "
                  + "0, 0, 0, "
                  "'" + tMysql->escapeString(tReq.sLocal) + "', "
                  "'" + tMysql->escapeString(tReq.sCoord) + "', "
                  "0, 0, 0, now());";

    LOG->info() << __FUNCTION__ << " |the SQL is:  " << sSql << endl;


    

    getMysql(0)->execute(sSql);

    iRet = SUCCESS;
    __CATCH__

    return iRet;
}

// 发表评论
Int32 DbHandle::setContent(PostContentReq const& tReq, string const& sCId, Int32 iAvatarId, ContentInfo& tReturnCnt, bool bUseCache)
{
    Int32 iRet  = FAILURE;
    Int64 lSId = 0;

    iRet =  setContentToDb(tReq, sCId, iAvatarId);

    if(iRet)
    {
        return FAILURE;
    }

    ContentListKey tKey;
    ContentListValue tValue;
    tKey.lSId = tReq.lSId;

    __TRY__

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_content_hashmap,tKey,tValue);
    }

    __CATCH__

    ContentInfo tContent;
    tContent.lCId = TC_Common::strto<Int64>(sCId);
    tContent.lSId = lSId;
    tContent.lUId = tReq.tUserId.lUId;
    tContent.sContent = tReq.sContent;       
    tContent.iPortrait = iAvatarId;
    tContent.Time = TC_Common::now2str("%Y-%m-%d %H:%M:%S");
    tContent.isFavor = false;
    tContent.bIsMe = true;
    if (tReq.lFirstId == tReq.tUserId.lUId)
        tContent.isFirst = true;
    else
        tContent.isFirst = false;

    tReturnCnt = tContent;

    tValue.vContents.push_back(tContent);     // 长度呢？
    tValue.iTime = TC_TimeProvider::getInstance()->getNow();

    CommHashMap::getInstance()->addHashMap(g_content_hashmap, tKey, tValue);
    
    return SUCCESS;
}

Int32 DbHandle::setContentToDb(PostContentReq const& tReq, string const& sCId, Int32 iAvatarId)
{
    Int32 iRet = FAILURE;
    __TRY__

    TC_Mysql  *tMysql       = getMysql(0);

    string sSql = " INSERT INTO `posts_comment` (`id`, `pid`, `uid`, `avatarid`,`content`, `favor_num`, `report_num`, `isremove`, `update_time`) VALUES ("
                  "'" + sCId + "', "
                  "'" + TC_Common::tostr(tReq.lSId) + "', "
                  "'" + TC_Common::tostr(tReq.tUserId.lUId) + "', "
                  "'" + TC_Common::tostr(iAvatarId) + "', "
                  "'" + tMysql->escapeString(tReq.sContent) + "', "
                  "0, 0, 0, now());";

    LOG->info() << __FUNCTION__ << " |the SQL is:  " << sSql << endl;

    getMysql(0)->execute(sSql);

    iRet = SUCCESS;
    __CATCH__

    return iRet;
}

Int32 DbHandle::getPortraitId(PostContentReq const& tReq, Int32& iPortraitId, bool bUseCache)
{
    Int32 iRet = FAILURE;
    ContentListRsp tRsp;

    iRet = getContents(tReq.tUserId, tReq.lFirstId, tReq.lSId, tRsp);

    if (iRet == FAILURE)
    {
        return FAILURE;
    }

    vector<Int32> vPortrait;

    if (tRsp.vContents.size() != 0)
    {
        // 查找头像ID 看看是否可以找到
        CntIterator pos = find_if(tRsp.vContents.begin(), tRsp.vContents.end(), EqualCntUId(tReq.tUserId.lUId));

        if (pos != tRsp.vContents.end())
        {
            if (pos->isFirst == true)
            {
                iPortraitId = 1;
            }
            else
            {
                iPortraitId = pos->iPortrait;
            }
            return SUCCESS;
        }
        else
        {
            for (CntIterator i = tRsp.vContents.begin(); i != tRsp.vContents.end(); ++i)
            {
                vPortrait.push_back(i->iPortrait);
            }

            sort(vPortrait.begin(), vPortrait.end());
            vPortrait.erase(unique(vPortrait.begin(), vPortrait.end()), vPortrait.end());
            iPortraitId = vPortrait.back() + 1;
            if (iPortraitId % 30 == 0)
            {
                iPortraitId++;
            }
        }
    }
    else
    {
        if (tReq.tUserId.lUId == tReq.lFirstId)
            iPortraitId = 1;
        else
            iPortraitId = 2;
    }

    return SUCCESS;
}

// 获取头像ID 未实现
Int32 DbHandle::getPortraitIdFromDb(PostContentReq const& tReq, Int32& iPortraitId)
{
    return SUCCESS;
}
  
Int32 DbHandle::getContents(UserId const& tUserId, Int64 lFirstId, Int64 lSId, ContentListRsp& tRsp, bool bUseCache)
{
    Int32 iRet     = FAILURE;
    Int32 iDbRet   = FAILURE;
    Int32 iMemRet  = FAILURE;
    Int32 iMemRet1  = FAILURE;

    SecretListKey   tKey;
    SecretListValue tValue;
    tKey.lUId = lFirstId;

    ContentListKey   tContentKey;
    ContentListValue tContentValue;
    tContentKey.lSId = lSId;

    __TRY__

    if(bUseCache)
    {
        iMemRet1 = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
        iMemRet = CommHashMap::getInstance()->getHashMap(g_content_hashmap,tContentKey,tContentValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tContentValue.iTime;

    LOG->info()<<" cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| SId: "<<lSId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet1 == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end() && iMemRet == 0 && tContentValue.vContents.size() == (size_t)pos->iCmtNum)
        {
            pos->iCmtNum = tContentValue.vContents.size(); 
            tRsp.iFavorNum = pos->iFavorNum;
            tRsp.iCmtNum   = pos->iCmtNum;
        }
        else
        {
            iMemRet = 1;
        }
    }
    else 
    {
        iMemRet = 1;
    }

    if (iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        LOG->info()<<" 4:"<<endl;
        iDbRet = getContentsFromDb(tUserId, lSId, tRsp);

        LOG->info()<<" get Contents from DB Ret: "<<iRet<<" | SId: "<<lSId<<endl;

        if (iDbRet == 0 && lSId != 0)
        {
            tContentValue.vContents = tRsp.vContents;
            tContentValue.iFavorNum = tRsp.iFavorNum;
            tContentValue.iCmtNum   = tRsp.iCmtNum;
            tContentValue.iTime     = tTimeNow;
            if (iMemRet1 == 0)
            {
                ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
                if (pos != tValue.vAccepts.end() && tContentValue.vContents.size() != (size_t)pos->iCmtNum)
                {
                    pos->iCmtNum = tContentValue.vContents.size(); 
                }
            }
        }
    }
    else
    {
        LOG->info()<<" get Contents from cache Ret:"<<iMemRet<<" | SId: "<<lSId<<endl;
    }

    if (tContentValue.vContents.size() != 0)
    {
        tRsp.vContents = tContentValue.vContents;
    }
  
    CommHashMap::getInstance()->addHashMap(g_content_hashmap, tContentKey, tContentValue);
    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    LOG->info()<<" favor num:"<<tRsp.iFavorNum<<endl;
    LOG->info()<<" cmt num:"<<tRsp.iCmtNum<<endl;

    if (iDbRet ==0 )
    {
        iRet = 1; //Cahce命中超时

    }
    else if (iMemRet==0 && iIntev<=60*10)
    {
        iRet = 0; //CacheOK
    }

#if 0
    iRet = getContentsFromDb(tUserId, lSId, tRsp);
#endif

    LOG->info()<<" get Contents from DB Ret: "<<iRet<<" | SId: "<<lSId<<endl;

    display(" Vector Contents " + TC_Common::tostr(tUserId.lUId), tRsp.vContents);

    return iRet;
}

Int32 DbHandle::getContentsFromDb(UserId const& tUserId, Int64 lSId, ContentListRsp& tRsp) 
{
    __TRY__

    string sSql = " select p.favor_num, p.cmt_num, p.uid "  
                  " from posts p where  p.id=" + TC_Common::tostr(lSId);

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;

    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);

    if (tRes.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        tRsp.iFavorNum = 0;
        tRsp.iCmtNum = 0;
        return SUCCESS;
    }

    tRsp.iFavorNum = TC_Common::strto<Int32>(tRes[0]["favor_num"]);
    tRsp.iCmtNum = TC_Common::strto<Int32>(tRes[0]["cmt_num"]);
    Int64 lPUid = TC_Common::strto<Int64>(tRes[0]["uid"]); 

    string sSql1 = " select id, pid, uid, update_time, content, favor_num, avatarid "  
                  " from posts_comment pc where pid=" + TC_Common::tostr(lSId) + " and isremove=0 order by pc.update_time asc ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql1 << endl;

    taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql1);

    if (tRes1.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes1.size() << ") ne 1  sql:" <<sSql1<< endl;
    }
    else
    {
        for (size_t i = 0; i < tRes1.size(); i++)
        {
            ContentInfo    tTmp;
    
            tTmp.lCId      = TC_Common::strto<Int64>(tRes1[i]["id"]);   // 评论ID
            tTmp.lSId      = TC_Common::strto<Int64>(tRes1[i]["pid"]);   // 父ID
            tTmp.lUId      = TC_Common::strto<Int64>(tRes1[i]["uid"]);   // 评论者ID
            
            if (tTmp.lUId == lPUid) 
            {
                tTmp.isFirst   = true;
                tTmp.iPortrait = 0;
            }
            else
            {
                tTmp.isFirst   = false;
            }
    
            tTmp.Time      = tRes1[i]["update_time"];                            // 时间 (YYYY-MM-DD HH:MM)
            tTmp.sContent  = tRes1[i]["content"];                            // 内容
            tTmp.iFavorNum = TC_Common::strto<Int32>(tRes1[i]["favor_num"]);   // 赞数
            if (tTmp.iFavorNum < 0)
                tTmp.iFavorNum = 0;
            tTmp.isFavor   = false;
            tTmp.iPortrait = TC_Common::strto<Int32>(tRes1[i]["avatarid"]);   // 头像ID
    
            if (tUserId.lUId == tTmp.lUId)
                tTmp.bIsMe   = true;
            else
                tTmp.bIsMe   = false;

            tRsp.vContents.push_back(tTmp);            
        }
    }

    // 修正评论数
    if (tRsp.vContents.size() != (size_t)tRsp.iCmtNum)
    {
        string sSql3 = " update posts set cmt_num = " + TC_Common::tostr(tRsp.vContents.size()) + " where id = " + TC_Common::tostr(lSId);

        LOG->info()<<__FUNCTION__<<  "SQL:" << sSql3 << endl;

        taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql3);
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 点赞  举报 清除 移除 修改数据库操作
Int32 DbHandle::setSecretOp(UserId const& tUserId, Int64 lUId, Int64 lId, Int32 iOp, bool bUseCache)
{
    Int32 iRet     = FAILURE;

    iRet = setSecretOpToDb(tUserId, lUId, lId, iOp);

    if(iRet)
    {
        return FAILURE;
    }


    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = tUserId.lUId;     // 没错

    CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);

    if (iOp == FAVORY)
    {
        IdNum tTmp;
        tTmp.lId = lUId;
        tTmp.iNum = 1;
        tValue.tFavorScrts.mSId[lId] = tTmp;
        tValue.tFavorScrts.iTime = TC_TimeProvider::getInstance()->getNow();
    }
    else if (iOp == FAVORN)
    {
        RelatIterator ps = tValue.tFavorScrts.mSId.find(lId);
        if (ps != tValue.tFavorScrts.mSId.end())
        {
            tValue.tFavorScrts.mSId.erase(ps);
        }
    }
    else if (iOp == REMOVEY)
    {
        IdNum tTmp;
        tTmp.lId = lUId;
        tTmp.iNum = 1;
        tValue.tRemoveScrts.mSId[lId] = tTmp;
        tValue.tRemoveScrts.iTime = TC_TimeProvider::getInstance()->getNow();
        LOG->debug()<<__FUNCTION__<<" the dest UID is : "<<lUId<<endl;

        LOG->debug()<<__FUNCTION__<<" Cache In Secret Insert Remove Secret lSId: "<<lId<<endl;
    }
    else if (iOp == CLEANY)
    {
        // 帖子列表清除痕迹
        tValue.vSecrets.clear();
        tValue.vAccepts.clear();
        LOG->info()<<__FUNCTION__<<" Cache In Secret Insert clean Secret lUId: "<<tUserId.lUId<<endl;
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<" This OP Don't Need Update Cache Op: "<<iOp<<" lId: "<<lId<<endl;
    }

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    _tSecretLock.lock();
    SecretListKey tKey1;
    SecretListValue tValue1;
    tKey1.lUId = lUId;     // 没错

    CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey1,tValue1);

    if (iOp == FAVORY)
    {
        // 帖子列表的赞数
        ScrtIterator pos = find_if(tValue1.vSecrets.begin(), tValue1.vSecrets.end(), EqualSId(lId));
        if (pos != tValue1.vSecrets.end())
        {
            pos->iFavorNum += 1;
            LOG->info()<<__FUNCTION__<<" 1 Cache In Secret Update Favor Num lSId: "<<lId<<endl;
        }
        else
        {
            LOG->debug()<<__FUNCTION__<<" 1 Cache In Secret Update Favor Num But Don't Find lSId: "<<lId<<endl;
        }

    }
    else if (iOp == FAVORN)
    {
        // 帖子列表的赞数
        ScrtIterator pos = find_if(tValue1.vSecrets.begin(), tValue1.vSecrets.end(), EqualSId(lId));
        if (pos != tValue1.vSecrets.end() && pos->iFavorNum > 0)
        {
            pos->iFavorNum -= 1;
        }
        else
        {
            LOG->debug()<<__FUNCTION__<<" 1 Cache In Secret Update Favor Num But Don't Find lSId: "<<lId<<endl;
        }
    }

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey1, tValue1);
    _tSecretLock.unlock();

    LOG->info()<<__FUNCTION__<<" Cache In Secret Op UId: "<<tUserId.lUId<<endl;

    return SUCCESS;
}

Int32 DbHandle::setSecretOpToDb(UserId const& tUserId, Int64 lUId, Int64 lId, Int32 iOp)
{
    __TRY__

    string sSql;

    if (iOp == FAVORY)
    {
        sSql = "update posts set favor_num=favor_num+1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 0, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lUId) + "', '0', '1', now()) on duplicate key update isavailable = 1";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == FAVORN)
    {
        sSql = "update posts set favor_num=favor_num-1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 0, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lUId) + "', '0', '1', now()) on duplicate key update isavailable = 0";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == REPORTY)
    {
        sSql = "update posts set report_num=report_num+1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 0, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lUId) + "', '1', '1', now()) on duplicate key update isavailable = 1";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == REMOVEY)
    {
        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 0, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lUId) + "', '2', '1', now()) on duplicate key update isavailable = 1";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == CLEANY)
    {
        if (lId == 0)
        {
            sSql = "update posts set isremove=1 where uid="+ TC_Common::tostr(tUserId.lUId);
        }
        else 
        {
            sSql = "update posts set isremove=1 where uid="+ TC_Common::tostr(tUserId.lUId) + " and id="+ TC_Common::tostr(lId);
        }
        LOG->info()<<__FUNCTION__<< " SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

Int32 DbHandle::setContentOp(UserId const& tUserId, Int64 lUId, Int64 lId, Int64 lSId, Int32 iOp, bool bUseCache)
{
    Int32 iRet  = FAILURE;

    LOG->debug()<<__FUNCTION__<<" begin lUId:"<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;

    iRet = setContentOpToDb(tUserId, lUId, lId, lSId, iOp);

    if(iRet)
    {
        return FAILURE;
    }

    _tContentLock.lock();

    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = tUserId.lUId;      // ?

    CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
   
    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    if (iOp == FAVORY)
    {
        IdNum tTmp;
        tTmp.lId = lSId;
        tTmp.iNum = 1;
        tValue.tFavorCnts.mCId.insert(make_pair(lId, tTmp));
        tValue.tFavorCnts.iTime = tTimeNow;
    }

    // 更新cache评论赞数
    ContentListKey tContentKey;
    ContentListValue tContentValue;
    tContentKey.lSId = lSId;

    if (lSId != 0)
    {
        CommHashMap::getInstance()->getHashMap(g_content_hashmap, tContentKey, tContentValue); 

        if (iOp == FAVORY)
        {
            // 更新cache 点赞数
            CntIterator pos = find_if(tContentValue.vContents.begin(), tContentValue.vContents.end(), EqualCId(lId));
            if (pos != tContentValue.vContents.end())
            {
                pos->iFavorNum += 1;
                LOG->info()<<__FUNCTION__<<" Num Cache in content lUId: "<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;
            }
            else
            {
                LOG->debug()<<__FUNCTION__<<" Num Cache in content but dont't find lUId: "<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;
            }
            tContentValue.iTime = tTimeNow;
            CommHashMap::getInstance()->addHashMap(g_content_hashmap, tContentKey, tContentValue);
        }
        else if (lId != 0 && iOp == CLEANY)
        {
            // 清除痕迹  清空
            CntIterator pos = find_if(tContentValue.vContents.begin(), tContentValue.vContents.end(), EqualCId(lId));
            if (pos != tContentValue.vContents.end())
            {
                tContentValue.vContents.erase(pos);
                tContentValue.iTime = TC_TimeProvider::getInstance()->getNow();
                LOG->info()<<__FUNCTION__<<" Cache in Clean Content lUId: "<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;
                CommHashMap::getInstance()->addHashMap(g_content_hashmap, tContentKey, tContentValue);
            }
        }
        else
        {
            LOG->debug()<<__FUNCTION__<<" Cache in Content Op but don't need update lUId: "<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;
        }
        display(" After OP cache Content", tContentValue.vContents);
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<" Num Cache lost in content lUId: "<<lUId<<" lCId: "<<lId<<"lSId: "<<lSId<<"iOp: "<<iOp<<endl;
    }

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    // 更新帖子评论数
    if (iOp == CLEANY)
    {
        iRet = this->updScrtContentNum(tUserId, lUId, lSId, CLEANY);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<<" updScrtContentNum failure! "<<endl;
            return FAILURE;
        }
    }

    _tContentLock.unlock();


    return SUCCESS;
}

Int32 DbHandle::setContentOpToDb(UserId const& tUserId, Int64 lUId, Int64 lId, Int64 lSId, Int32 iOp)
{
    __TRY__

    string sSql;

    if (iOp == FAVORY)
    {
        sSql = "update posts_comment set favor_num=favor_num+1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 1, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lSId) + "', '0', '1', now()) on duplicate key update isavailable = 1";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == FAVORN)
    {
        sSql = "update posts_comment set favor_num=favor_num-1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 1, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lSId) + "', '0', '1', now()) on duplicate key update isavailable = 0";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == REPORTY)
    {
        sSql = "update posts_comment set report_num=report_num+1 where id="+ TC_Common::tostr(lId);
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);

        sSql = "INSERT INTO `posts_operation` (`uid`, `type`, `objectid`, `relatid`, `operation`, `isavailable`, `update_time`) VALUES ("
               "'" + TC_Common::tostr(tUserId.lUId) +"', "
               " 1, " 
               "'" + TC_Common::tostr(lId) + "', "
               "'" + TC_Common::tostr(lSId) + "', '1', '1', now()) on duplicate key update isavailable = 1";

        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }
    else if (iOp == CLEANY)
    {
        if (lId == 0)
        {
            sSql = "update posts_comment set isremove=1 where uid="+ TC_Common::tostr(tUserId.lUId);
        }
        else
        {
            sSql = "update posts_comment set isremove=1 where uid="+ TC_Common::tostr(tUserId.lUId) + " and id=" + TC_Common::tostr(lId);
        }
        LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
        getMysql(0)->execute(sSql);
    }

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

Int32 DbHandle::updScrtContentNum(UserId const& tUserId, Int64 lFirstId, Int64 lSId, Int32 iOp, bool bUseCache)
{
    Int32 iRet = FAILURE;
    Int32 iMemRet  = FAILURE;

    LOG->info() << __FUNCTION__<<" updScrtContentNum ....................."<<iOp<<endl;

    iRet = updScrtContentNumToDb(tUserId, lSId, iOp);

    if (iRet == FAILURE)
    {
        return iRet;
    }

    _tCmtNumLock.lock();

    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = lFirstId;

    if(bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }
   
    ostringstream os1; 
    tValue.displaySimple(os1);

    if (iMemRet == 0)
    {
        // 更新cache 评论数
        ScrtIterator pos = find_if(tValue.vSecrets.begin(), tValue.vSecrets.end(), EqualSId(lSId));
        if (pos != tValue.vSecrets.end())
        {
            if (iOp == CLEANN)
                pos->iCmtNum += 1;
            else if (pos->iCmtNum > 0)
                pos->iCmtNum -= 1;
            LOG->info()<<__FUNCTION__<<" Cache Success lSId: "<<lSId<<" tvalue: "<<os1.str()<<endl;
        }
        else 
        {
            LOG->info()<<__FUNCTION__<<" Cache Success But don't Find the secret lSId: "<<lSId<<" tvalue: "<<os1.str()<<endl;
        }
        CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<" Cache Lost in secret lUId: "<<tUserId.lUId<<" tvalue: "<<os1.str()<<endl;
    }

    _tCmtNumLock.unlock();

    return iRet;
}

Int32 DbHandle::updScrtContentNumToDb(UserId const& tUserId, Int64 lSId, Int32 iOp)
{
    __TRY__

    string sSql;
    if (iOp == CLEANN)
        sSql = "update posts set cmt_num=cmt_num+1 where id="+ TC_Common::tostr(lSId);
    else
        sSql = "update posts set cmt_num=cmt_num-1 where id="+ TC_Common::tostr(lSId) + " and cmt_num > 0";
    LOG->info()<<__FUNCTION__<<" SQL:" << sSql << endl;
    getMysql(0)->execute(sSql);

    return SUCCESS;
    __CATCH__

    return FAILURE;
}

// 取评论用户ID集合
Int32 DbHandle::getContentUserIds(Int64 lUId, Int64 lSId, vector<Int64>& vUserIds, bool bUseCache)
{   
    Int32 iRet    = FAILURE;
    Int32 iDbRet  = FAILURE;
    Int32 iMemRet = FAILURE;

    ContentListKey   tKey;
    ContentListValue tValue;
    tKey.lSId = lSId;

    __TRY__

    if (bUseCache)
    {
        iMemRet = CommHashMap::getInstance()->getHashMap(g_content_hashmap,tKey,tValue);
    }

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tValue.iTime;

    LOG->info()<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<"|UseCache|"<<bUseCache<<endl;

    if (iMemRet != 0  || iIntev > _uCacheTimeOut)
    {
        iDbRet = getContentUserIdsFromDb(lUId, lSId, vUserIds);
        LOG->info()<<"get Content UserIds from db ret:"<<iRet<<"| UId: "<<lUId<<endl;
        return iDbRet;
    }
    else
    {
        LOG->info()<<"get Content UserIds from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
        for (CntIterator i = tValue.vContents.begin(); i != tValue.vContents.end(); ++i)
        {
            if (lUId != i->lUId)
                vUserIds.push_back(i->lUId);
        }
        return SUCCESS;
    }

    return iRet;
}
  
Int32 DbHandle::getContentUserIdsFromDb(Int64 lUId, Int64 lSId, vector<Int64>& vUserIds)
{   
    __TRY__
        
    string sSql = "select distinct(uid) from posts_comment where pid=" + TC_Common::tostr(lSId) + " and  uid!= " + TC_Common::tostr(lUId);
        
    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql << endl;
            
    taf::TC_Mysql::MysqlData tRes = getMysql(0)->queryRecord(sSql);
        
    if (tRes.size() == 0)
    {   
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
        return SUCCESS;
    }   
        
    for (size_t i = 0; i < tRes.size(); i++)
    {       
        vUserIds.push_back(TC_Common::strto<Int64>(tRes[i]["uid"]));
    }           
            
    return SUCCESS;
    __CATCH__
        
    return FAILURE;
}  

Int32 DbHandle::getUserSecretListFromCache(Int64 lUId, Int64 lBeginId, Int32 iScrtsNum, ScrtVector& vSecrets)
{
    Int32 iRet    = FAILURE;
    Int32 iMemRet = FAILURE;

    SecretListKey   tSecretKey;
    SecretListValue tSecretValue;
    tSecretKey.lUId = lUId;

    __TRY__

    iMemRet = CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tSecretKey,tSecretValue);

    __CATCH__

    time_t tTimeNow = TC_TimeProvider::getInstance()->getNow();

    Int32 iIntev = tTimeNow - tSecretValue.iAcceptTime;

    LOG->debug()<<__FUNCTION__<<"cache value ret:|"<<iMemRet<<"|time Intev :|"<<iIntev<<"| UId: "<<lUId<<" Accepts size :"<<tSecretValue.vAccepts.size()<<endl;

    if (iMemRet !=0 || tSecretValue.vAccepts.size() == 0 || iIntev > _uUserScrtCacheTimeOut)
    {
        LOG->debug()<<__FUNCTION__<<" no data or timeout "<<lUId<<endl;
        return 1;
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<"get User Accept Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
    }

    if (lBeginId != -1)
    {
        ScrtIterator pos = find_if(tSecretValue.vAccepts.begin(), tSecretValue.vAccepts.end(), LessSId(lBeginId));

        if (pos == tSecretValue.vAccepts.end())
        {
            // 收帖箱找不到需要的帖子，找DB
            LOG->debug()<<__FUNCTION__<<" User Accept can't find Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
            return 1;
        }
        if (ScrtVector::size_type(tSecretValue.vAccepts.end() - pos) >= iScrtsNum)
        {
            // 找到,返回
            // copy(pos, pos + iScrtsNum, back_inserter(vSecrets));
            vSecrets = tSecretValue.vAccepts;
            LOG->debug()<<__FUNCTION__<<" User Accept has finded Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
        }
        else
        {
            // 找到，但不够需求
            vSecrets = tSecretValue.vAccepts;
            LOG->debug()<<__FUNCTION__<<" User Accept has finded Secrets from cache but .......ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
            return 1;
        }
    }
    else
    {
        LOG->debug()<<__FUNCTION__<<" User Accept find All Secrets from cache ret:"<<iMemRet<<"| UId: "<<lUId<<endl;
        // 返回整个收件箱
        if (tSecretValue.vAccepts.size() >= (size_t)iScrtsNum)
        {
            vSecrets = tSecretValue.vAccepts;
        }
        else
        {
            vSecrets = tSecretValue.vAccepts;
            return 1;
        }
    }

    if (iMemRet==0 && iIntev <= 60*10)
    {
        iRet = SUCCESS; //CacheOK
    }

    return iRet;
}

// 发表帖子 缓存粉丝收帖箱
Int32 DbHandle::setBatchSecretsToCache(vector<SecretInfo>& vSecrets, long lUId, bool bUseCache)
{
    __TRY__ 

    LOG->debug()<<__FUNCTION__<<" the UId is : "<<lUId<<endl;

    SecretListKey tKey;
    SecretListValue tValue;
    tKey.lUId = lUId;

    if(bUseCache)
    {
        CommHashMap::getInstance()->getHashMap(g_secret_hashmap,tKey,tValue);
    }

    tValue.vAccepts = vSecrets;
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
        SecretInfo tSecret;
        tSecret.lSId = lSId;
        tSecret.lUId = tReq.tUserId.lUId;
        tSecret.iColorId = tReq.iColorId;
        tSecret.sBitUrl = tReq.sBitUrl;         // 图URL
        tSecret.sContent = tReq.sContent;        // 内容
        tSecret.sLocal = tReq.sLocal;          // 地理位置(来源type为推荐时需要来源城市名)
        tSecret.iType = tFriendsList.iRelat;      // 来源 
        tSecret.isFavor = false;
        tSecret.isRead  = false;
        if (tValue.vAccepts.size() != 0)
            tValue.vAccepts.insert(tValue.vAccepts.begin(), tSecret);
        else
            tValue.vAccepts.push_back(tSecret);
        tValue.iTime = TC_TimeProvider::getInstance()->getNow();
        LOG->debug()<<__FUNCTION__<<" post success "<<endl;
    }
    if (iType == USER_FAVORY && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end() && pos->iFavorNum >= 0)
        {
            pos->iFavorNum += 1;
            LOG->info()<<__FUNCTION__<<" favor y success "<<endl;
        }        
        else
            LOG->debug()<<__FUNCTION__<<" favor y failure "<<endl;
    }
    else if (iType == USER_FAVORN && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end() && pos->iFavorNum > 0)
        {
            pos->iFavorNum -= 1;
            LOG->info()<<__FUNCTION__<<" favor n success "<<endl;
        }       
        else 
            LOG->debug()<<__FUNCTION__<<" favor n failure "<<endl;
    }
    else if (iType == USER_REMOVEY && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end())
        {
            tValue.vAccepts.erase(pos);
            LOG->info()<<__FUNCTION__<<" remove success "<<endl;
        } 
        else        
            LOG->debug()<<__FUNCTION__<<" remove failure "<<endl;
    }
    else if (iType == USER_REMOVEY_CONT && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end())
        {
            pos->iCmtNum -= 1;
            LOG->debug()<<__FUNCTION__<<" remove content success "<<endl;
        } 
        else        
            LOG->debug()<<__FUNCTION__<<" remove content failure "<<endl;
    }
    else if (iType == USER_CONTENT && iMemRet == 0)
    {
        ScrtIterator pos = find_if(tValue.vAccepts.begin(), tValue.vAccepts.end(), EqualSId(lSId));
        if (pos != tValue.vAccepts.end())
        {
            pos->iCmtNum += 1;
            LOG->info()<<__FUNCTION__<<" content success "<<endl;
        } 
        else        
            LOG->debug()<<__FUNCTION__<<" content failure "<<endl;
    }
    // 移除评论也要
    else 
    {
    
    }

    CommHashMap::getInstance()->addHashMap(g_secret_hashmap, tKey, tValue);

    __CATCH__

    return SUCCESS;
}

Int32 DbHandle::getUserSingleSecretFromDb(const QUAN::SglSecretReq & tReq, QUAN::SglSecretRsp &tRsp)
{
    __TRY__

    string sSql = " select id, uid, colorid, biturl, content, location, favor_num, cmt_num "  
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

    string sSql1 = " select id, pid, uid, update_time, content, favor_num , avatarid "  
                  " from posts_comment pc where pc.pid=" + TC_Common::tostr(tReq.lSId) + " and pc.isremove=0 order by pc.update_time asc ";

    LOG->info()<<__FUNCTION__<<  "SQL:" << sSql1 << endl;

    taf::TC_Mysql::MysqlData tRes1 = getMysql(0)->queryRecord(sSql1);

    if (tRes1.size() == 0)
    {
        LOG->info()<<__FUNCTION__<< "  SUCCESS, size(" << tRes.size() << ") ne 1  sql:" <<sSql<< endl;
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

TC_Mysql* DbHandle::getMysql(Int64 iId)
{
    unsigned short uIndex = ((unsigned Int64)iId%10) %_uDbNum;
    return _mMysql[uIndex];
}
