#ifndef __SECRET_DBHANDLE_H_
#define __SECRET_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "SecretBase.h"
#include "Base.h"
#include "Project.h"
#include "Circle.h"

using namespace  QUAN;

/* callback of async proxy for client */


typedef map<unsigned short, TC_Mysql*>  DwDbMap;

class DbHandle
{

public:
    DbHandle()          
    {
        init();
    }
    int init();

    int CreateCircle(taf::Int64 lUId,QUAN::CircleInfo &tCircleInfo,bool bIsAdmin);

    int GetCircle(taf::Int64 lCircleId,QUAN::CircleInfo &tCircleInfo);

    int GetCircleLocation(taf::Int64 lCircleId,vector<QUAN::CircleLocation> &vCircleLocation);

    int AddCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation &vCircleLocation);
    int modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation &vCircleLocation);

    int SearchCircle(const std::string& sCond,vector<CircleInfo>& vCircleInfo);

    int GetUserCircle(taf::Int64 lUId,vector<taf::Int64>& vConcern,vector<taf::Int64>& vAccessable);

    int GetLocationCircle(taf::Int32 iLat,taf::Int32 iLon,taf::Int32 iRadius,vector<QUAN::CircleLbsInfo>& vCircleLbsInfo);

    int GetRecommCircle(QUAN::CircleGroup& tCircleGroup);

    int SetCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern);
    
    int PostInCircle(taf::Int64 lUId,taf::Int64 lCircleId,bool bPostNearBy);

    int AuditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string &sReason);

    int RecommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue);

    int GetCicleUnlockNum(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 &iLockNum);
protected:
    int setInnerCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern);

    int setInnerCircleAccessable(taf::Int64 lUId,taf::Int64 lCircleId);

    void updateCircle(taf::Int64 lCircleId,const string &field,const string &value,const string &where="");

private:
    TC_Mysql* getMysql(Int64 lCircleId);
    string getCircleTbName(Int64 lCircleId);
    string getCircleUserTbName(Int64 lUId);
    string getCirclePostTbName(Int64 lUId);
    string getCircleLocationTbName();
    CProject tCProject;

private:
    DwDbMap         _mMysql;
    unsigned short  _uDbNum;
    unsigned short  _uTbNum;
    string          _sDbPre;
    string          _sCircleTbName;
    string          _sCircleUserTbName;
    string          _sCirclePostTbName;
    string          _sCircleLocationTbName;
};

class LockDbHandle:public DbHandle,public TC_ThreadLock,public TC_Singleton<LockDbHandle>
{
};

#endif                                             
