#ifndef __MSG_DBHANDLE_H_
#define __MSG_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "Msg.h"
#include "../UserInfoServer/UserInfo.h"
#include "Base.h"
#include "jmem/jmem_hashmap.h"
#include "../SecretServer/CommHashMap.h"

using namespace  QUAN;

#define DBINT(value) make_pair(TC_Mysql::DB_INT,TC_Common::tostr(value))
#define DBSTR(value) make_pair(TC_Mysql::DB_STR,(value))
#define DBDAT(value) make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(value)+")")

/* callback of async proxy for client */

typedef map<unsigned short, TC_Mysql*>  DwDbMap;
typedef JceHashMap<MsgRemindKey, MsgRemindValue, ThreadLockPolicy, FileStorePolicy> RemindHashMap;
typedef JceHashMap<SysMsgKey, SysMsgValue, ThreadLockPolicy, FileStorePolicy> SysMsgHashMap;
typedef JceHashMap<PushMsgKey, PushMsgValue, ThreadLockPolicy, FileStorePolicy> PushMsgHashMap;

struct EqualIsRemove   
{
    bool operator()(const SecretInfo &e1)
    {
        return e1.isRemove == 1;
    }
};



class DbHandle
{

public:
    DbHandle()          
    {
        init();
    }

    int init();

	int setMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp, bool bUseCache = true);
	int setMsgToUserIdToDb(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp);

	int getMsgsRemind(PushMsgReq const& tReq, map<Int64, MsgRmd>& mSIds, bool bUseCache = true);
	int getMsgsRemindFromDb(PushMsgReq const& tReq, map<Int64, MsgRmd>& mSIds);
	int updMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp);
	int updCleanMsgToUserId(Int64 lUId, Int64 lId, Int64 lFirstId, Int32 iType, Int32 iOp);
	int updMsgRead(Int64 lUId, Int64 lId);
	int cleanCache(const vector<Int64> & vUserId);
	int cleanSysCache(Int64 lUserId);

	int getSysMsgList(GetSysMsgReq const& tReq, vector<SysMsgInfo>& vSysMsgs, bool bUseCache = true);
	int getSysMsgListFromDb(GetSysMsgReq const& tReq, vector<SysMsgInfo>& vSysMsgs);

	int setSysMsgToDb(Int64 lUId, SysMsgInfo& tSysMsg);
	int setSysMsg(Int64 lUId, SysMsgInfo& tSysMsg, bool bUseCache = true);

	int setSysMsgReadedToDb(Int64 lUId);
	int setSysMsgReaded(Int64 lUId, bool bUseCache = true);

	int checkPushMsg(UserId const& tUserId, PushMsgValue& tValue);
	int setPushMsg(UserId const& tUserId, Int32 iOnline, string const& sToken);

private:

    TC_Mysql* getMysql(Int64 yyuid);
    string getTbName(Int64 yyuid);

private:

    DwDbMap         _mMysql;
    unsigned short  _uDbNum;
    unsigned short  _uTbNum;
    unsigned short _uMaxMsgNum;
    unsigned short _uCacheTimeOut;
    string          _sDbPre;
    string          _sTbSecretPre;
    string          _sTbContentPre;
    string          _sTbRelatPre;
    TC_ThreadMutex  _tRemindLock;
};

#endif                                             
