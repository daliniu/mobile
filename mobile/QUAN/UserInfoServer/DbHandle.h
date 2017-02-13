#ifndef __SECRET_DBHANDLE_H_
#define __SECRET_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "UserInfo.h"
#include "Base.h"

using namespace  QUAN;

/* callback of async proxy for client */


typedef map<unsigned short, TC_Mysql*>  DwDbMap;


struct FriendsListDb
{
    taf::Int64 lUId;
    taf::Int32 iRelat;

    taf::Int32 iPostTime;
    taf::Int32 iFavorTime;
};

class DbHandle
{

public:
    DbHandle()          
    {
        init();
    }

    int init();
    int getFriendsList(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds, bool bUseCache = true);
    int getFriendsListFromDb(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds);
    int getFriendsListFromDb(taf::Int64 lId, int iRelat, vector<FriendsListDb>& vUserIds);
    int getFollowersListFromDb(taf::Int64 lId, int iRelat, vector<FriendsList>& vUserIds);

    int updatePostTime(taf::Int64 lId,int iTimespan);
    int updateFavorTime(taf::Int64 lId,int iTimespan);
    int updateVisitTime(taf::Int64 lId,int iTimespan);

    int loadUserInfo(taf::Int64 lId,UserInfoCacheValue &value);

    int SaveFriendListCache(taf::Int64 lId,FriendListCacheValue &value);
    int SaveFollowerListCache(taf::Int64 lId,FollowerListCacheValue &value);

private:

    TC_Mysql* getMysql(Int64 lId);
    string getRelationTbName(Int64 lId);
    string getUserTbName(Int64 lId);
    string getFollowerTbName(Int64 lId);

private:

    DwDbMap         _mMysql;
    unsigned short  _uDbNum;
    unsigned short  _uTbNum;
    string          _sDbPre;
    string          _sTbUserInfoPre;
    string          _sTbUserRelatPre;
    //string          _sTbUserCntrtPre;
    string          _sTbUserFollowerPre;   
};

class LockDbHandle:public DbHandle,public TC_ThreadLock,public TC_Singleton<LockDbHandle>
{
};

#endif                                             
