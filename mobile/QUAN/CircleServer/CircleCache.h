#ifndef __CIRCLECACHE_H__
#define __CIRCLECACHE_H__

#include "DbHandle.h"
#include "Circle.h"
#include "LockedSingleton.h"
#include "Project.h"
#include "CircleSharedCache.h"

const int splitradius=1000;
const int splitsize=splitradius*2;


class CircleCache:public LockedSingleton<CircleCache>
{
    public:
        CircleCache()
        {
        }
    public:

        int CreateCircle(taf::Int64 lUId,QUAN::CircleInfo &tCircleInfo,bool bIsAdmin=false);

        int GetCircle(taf::Int64 lCircleId,QUAN::CircleInfo &tCircleInfo);

        int GetCircleLocation(taf::Int64 lCircleId,vector<QUAN::CircleLocation> &vCircleLocation);

        int AddCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation &vCircleLocation);
        int modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation &vCircleLocation);

        int SearchCircle(const std::string& sCond,const std::string &sCoord,vector<QUAN::CircleGroup>& vCircleGroup);

        int GetUserCircle(taf::Int64 lUId,vector<taf::Int64>& vConcern,vector<taf::Int64>& vAccessable);
        
        int GetNearbyCircle(double dLat,double dLon,int iRadius,int iMaxCount,vector<QUAN::CircleInfo>& vCircleInfo);
        int GetNearbyCircleAll(double dLat,double dLon,int iRadius,int iMaxCount,vector<QUAN::CircleInfo>& vCircleInfo);
        int ClearLocationCircleCache(double dLat,double dLon);

        int GetRecommCircle(QUAN::CircleGroup& tCircleGroup);

        int SetCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern);

        int PostInCircle(taf::Int64 lUId,taf::Int64 lCircleId,bool bPostNearBy);

        int AuditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string &sReason);

        int RecommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue);

        int GetCicleUnlockNum(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 &iLockNum);

        void GroupingCircle(vector<CircleInfo>& vCircleInfo,vector<CircleGroup>& vCircleGroup,bool bIsShowCity=false);

        int LoadCircleNearestLocation(double dLat,double dLon,QUAN::CircleInfo& tCircleInfo);
        int LoadCircleNearestLocation(std::string sCoord,QUAN::CircleInfo& tCircleInfo);
        int LoadCircleNearestLocation(QUAN::CircleInfo& tCircleInfo);
    protected:
        int GetLocationCircle(taf::Int32 iLat,taf::Int32 iLon,vector<QUAN::CircleLbsInfo>& vCircleId);



    protected:
        DbHandle tDbHandle;
        CProject tCProject;
        
};

#endif
