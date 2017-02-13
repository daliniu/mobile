#include "CircleCache.h"
#include "LogHelper.h"


int CircleCache::CreateCircle(taf::Int64 lUId,QUAN::CircleInfo &tCircleInfo,bool bIsAdmin)
{
    return tDbHandle.CreateCircle(lUId,tCircleInfo,bIsAdmin);
}


int CircleCache::GetCircle(taf::Int64 lCircleId,QUAN::CircleInfo &tCircleInfo)
{
    if(!CircleSharedCache::getInstance()->GetCircle(lCircleId,tCircleInfo))
    {
        if(tDbHandle.GetCircle(lCircleId,tCircleInfo)==0)
        {
            CircleSharedCache::getInstance()->UpdateCircle(lCircleId,tCircleInfo);
            return tCircleInfo.bAuditState?0:-1;
        }
        return -1;
    }
    else
    {
        return tCircleInfo.bAuditState?0:-1;
    }
}

int CircleCache::GetCircleLocation(taf::Int64 lCircleId,vector<QUAN::CircleLocation> &vCircleLocation)
{
    if(!CircleSharedCache::getInstance()->GetCircleLocation(lCircleId,vCircleLocation))
    {
        if(tDbHandle.GetCircleLocation(lCircleId,vCircleLocation)==0)
        {
            CircleSharedCache::getInstance()->UpdateCircleLocation(lCircleId,vCircleLocation);
            return 0;
        }
        return -1;
    }
    else
    {
        return 0;
    }
}

int CircleCache::AddCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation &vCircleLocation)
{
    if(tDbHandle.AddCircleLocation(lCircleId,vCircleLocation)==0)
    {
        CircleSharedCache::getInstance()->ClearCircleLocation(lCircleId); 
        int iLat,iLon;
        tCProject.Gauss_BL2xy(vCircleLocation.dLat,vCircleLocation.dLon,iLat,iLon);
        CircleSharedCache::getInstance()->ClearCircleLbs(iLat,iLon);
        return 0;
    }
    return -1;
}

int CircleCache::modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation &vCircleLocation)
{
    if(tDbHandle.modifyCircleLocation(lCircleLocationId,vCircleLocation)==0)
    {
        int iLat,iLon;
        tCProject.Gauss_BL2xy(vCircleLocation.dLat,vCircleLocation.dLon,iLat,iLon);
        CircleSharedCache::getInstance()->ClearCircleLbs(iLat,iLon);
        return 0;
    }
    return -1;
}

int CircleCache::SearchCircle(const std::string& sCond,const std::string &sCoord,vector<QUAN::CircleGroup>& vCircleGroup)
{
    vector<CircleInfo> vCircleInfo;
    if(tDbHandle.SearchCircle(sCond,vCircleInfo)==0)
    {
        vector<double> vCoord=TC_Common::sepstr<double>(sCoord,",&");
        if(vCoord.size()==2)
        {
            for(auto &tCircleInfo:vCircleInfo)
            {
                LoadCircleNearestLocation(vCoord[0],vCoord[1],tCircleInfo);
            }
            sort(vCircleInfo.begin(),vCircleInfo.end(),[](const QUAN::CircleInfo &lhs,const QUAN::CircleInfo &rhs)
                    {
                    return (unsigned)lhs.iDistance<(unsigned)rhs.iDistance;   //负数排最后
                    });
        }
        else
        {
            for(auto &tCircleInfo:vCircleInfo)
            {
                LoadCircleNearestLocation(tCircleInfo);
            }
        }
        GroupingCircle(vCircleInfo,vCircleGroup,true);
        return 0;
    }
    return -1;
}


int CircleCache::GetUserCircle(taf::Int64 lUId,vector<taf::Int64>& vConcern,vector<taf::Int64>& vAccessable)
{
    if(!CircleSharedCache::getInstance()->GetCircleUser(lUId,vConcern,vAccessable))
    {
        if(tDbHandle.GetUserCircle(lUId,vConcern,vAccessable)==0)
        {
            CircleSharedCache::getInstance()->UpdateCircleUser(lUId,vConcern,vAccessable);
            return 0;
        }
        return -1;
    }
    else
    {
        return 0;
    }
}
int CircleCache::GetNearbyCircle(double dLat,double dLon,int iRadius,int iMaxCount,vector<QUAN::CircleInfo>& vCircleInfo)
{
    int iLat,iLon;
    tCProject.Gauss_BL2xy(dLat,dLon,iLat,iLon);

    map<taf::Int64,vector<QUAN::CircleLocation>> mpLocation;
    vector<QUAN::CircleLbsInfo> vCircleLbsInfo;
    
    for(int x=iLat-iRadius;x<iLat+iRadius;x+=splitsize)
    {
        for(int y=iLon-iRadius;y<iLon+iRadius;y+=splitsize)
        {
            vector<QUAN::CircleLbsInfo> vtCircleLbsInfo;
            GetLocationCircle(x,y,vtCircleLbsInfo);
            vCircleLbsInfo.insert(vCircleLbsInfo.end(),vtCircleLbsInfo.begin(),vtCircleLbsInfo.end());
        }
    }
    for(auto &tCircleLbsInfo:vCircleLbsInfo)
    {
        auto &vLbsList=mpLocation[tCircleLbsInfo.lCircleId];
        vLbsList.push_back(tCircleLbsInfo.tLocation);
    }


    for(auto &itLocation:mpLocation)
    {
        CircleInfo tCircleInfo;
        if(GetCircle(itLocation.first,tCircleInfo)==0)
        {
            for(auto &tLocation:itLocation.second)
            {
                taf::Int32 iDistance=tCProject.Distance(tLocation.dLat,tLocation.dLon,dLat,dLon);           
                if(iDistance<tLocation.iRadius &&  (tCircleInfo.iDistance<0 || iDistance<tCircleInfo.iDistance))
                {
                    tCircleInfo.iDistance=iDistance;
                    tCircleInfo.sCity=tLocation.sCity;
                    tCircleInfo.sLocation=tLocation.sLocation;
                    tCircleInfo.sCoord=TC_Common::tostr(tLocation.dLat)+","+TC_Common::tostr(tLocation.dLon);
                }
            }
            if(tCircleInfo.iDistance>=0)
            {
                vCircleInfo.push_back(tCircleInfo);
            }
        }
    }

    sort(vCircleInfo.begin(),vCircleInfo.end(),[](const CircleInfo& lhs,const CircleInfo& rhs){return lhs.iDistance<rhs.iDistance;});
    if(iMaxCount>0 && vCircleInfo.size()>(unsigned)iMaxCount) 
    {
        vCircleInfo.resize(iMaxCount);
    }

    return 0;
}
int CircleCache::GetNearbyCircleAll( double dLat, double dLon, int iRadius, int iMaxCount, vector<QUAN::CircleInfo> &vCircleInfo )
{
    int iLat,iLon;
    tCProject.Gauss_BL2xy(dLat,dLon,iLat,iLon);

    map<taf::Int64,vector<QUAN::CircleLocation>> mpLocation;
    vector<QUAN::CircleLbsInfo> vCircleLbsInfo;
    
    for(int x=iLat-iRadius;x<iLat+iRadius;x+=splitsize)
    {
        for(int y=iLon-iRadius;y<iLon+iRadius;y+=splitsize)
        {
            vector<QUAN::CircleLbsInfo> vtCircleLbsInfo;
            GetLocationCircle(x,y,vtCircleLbsInfo);
            vCircleLbsInfo.insert(vCircleLbsInfo.end(),vtCircleLbsInfo.begin(),vtCircleLbsInfo.end());
        }
    }
    for(auto &tCircleLbsInfo:vCircleLbsInfo)
    {
        auto &vLbsList=mpLocation[tCircleLbsInfo.lCircleId];
        vLbsList.push_back(tCircleLbsInfo.tLocation);
    }


    for(auto &itLocation:mpLocation)
    {
        CircleInfo tCircleInfo;
        if(GetCircle(itLocation.first,tCircleInfo)==0)
        {
            for(auto &tLocation:itLocation.second)
            {
                taf::Int32 iDistance=tCProject.Distance(tLocation.dLat,tLocation.dLon,dLat,dLon);
                if ( tCircleInfo.iDistance < 0 || iDistance < tCircleInfo.iDistance )
                {
                    tCircleInfo.iDistance=iDistance;
                    tCircleInfo.sCity=tLocation.sCity;
                    tCircleInfo.sLocation=tLocation.sLocation;
                    tCircleInfo.sCoord=TC_Common::tostr(tLocation.dLat)+","+TC_Common::tostr(tLocation.dLon);
                }
            }
            if(tCircleInfo.iDistance>=0)
            {
                vCircleInfo.push_back(tCircleInfo);
            }
        }
    }

    sort(vCircleInfo.begin(),vCircleInfo.end(),[](const CircleInfo& lhs,const CircleInfo& rhs){return lhs.iDistance<rhs.iDistance;});
    if(iMaxCount>0 && vCircleInfo.size()>(unsigned)iMaxCount) 
    {
        vCircleInfo.resize(iMaxCount);
    }
    return 0;
}


int CircleCache::GetLocationCircle(taf::Int32 iLat,taf::Int32 iLon,vector<QUAN::CircleLbsInfo>& vCircleId)
{
    int x=iLat/splitsize;
    int y=iLon/splitsize;
    iLat=x*splitsize;
    iLon=y*splitsize;
    if(!CircleSharedCache::getInstance()->GetCircleLbs(x,y,vCircleId))
    {
        if(tDbHandle.GetLocationCircle(iLat,iLon,splitradius,vCircleId)==0)
        {
            LOG->info()<<__FUNCTION__<<"|"<<"Pos:"<<iLat<<","<<iLon<<"|"<<JCETOSTR(vCircleId)<<endl;
            CircleSharedCache::getInstance()->UpdateCircleLbs(x,y,vCircleId);
        }
        else
        {
            return -1;
        }
    }
    return 0;
}
int CircleCache::ClearLocationCircleCache(double dLat,double dLon)
{
    int iLat,iLon;
    tCProject.Gauss_BL2xy(dLat,dLon,iLat,iLon);
    int x=iLat/splitsize;
    int y=iLon/splitsize;
    CircleSharedCache::getInstance()->ClearCircleLbs(x,y);
    return 0;
}


int CircleCache::GetRecommCircle(QUAN::CircleGroup& tCircleGroup)
{
    return tDbHandle.GetRecommCircle(tCircleGroup);
}


int CircleCache::SetCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern)
{
    if(tDbHandle.SetCircleConcern(lUId,lCircleId,bIsConcern)==0)
    {
        CircleSharedCache::getInstance()->ClearCircleUser(lUId);
        return 0;
    }
    return -1;
}


int CircleCache::PostInCircle(taf::Int64 lUId,taf::Int64 lCircleId,bool bPostNearBy)
{
    if(tDbHandle.PostInCircle(lUId,lCircleId,bPostNearBy)==0)
    {
        CircleSharedCache::getInstance()->ClearCircleUser(lUId);
        CircleSharedCache::getInstance()->ClearCircleUnlock(lUId,lCircleId);
        CircleSharedCache::getInstance()->ClearCircle(lCircleId);
        return 0;
    }
    return -1;
}


int CircleCache::AuditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string &sReason)
{
    if(tDbHandle.AuditCircle(lCircleId,iAuditState,sReason)==0)
    {
        CircleSharedCache::getInstance()->ClearCircle(lCircleId);
        return 0;
    }
    return -1;
}


int CircleCache::RecommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue)
{
    return tDbHandle.RecommCircle(lCircleId,iRecommValue);
}

int CircleCache::GetCicleUnlockNum(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 &iLockNum)
{
    if(!CircleSharedCache::getInstance()->GetCircleUnlock(lUId,lCircleId,iLockNum))
    {
        if(tDbHandle.GetCicleUnlockNum(lUId,lCircleId,iLockNum)==0)
        {
            CircleSharedCache::getInstance()->UpdateCircleUnlock(lUId,lCircleId,iLockNum);
        }
        else
        {
            return -1;
        }
    }
    return 0;

}

void CircleCache::GroupingCircle(vector<CircleInfo>& vCircleInfo,vector<CircleGroup>& vCircleGroup,bool bIsShowCity)
{
    for(auto& tCircleInfo:vCircleInfo)    
    {
        bool isFound=false;
        string sTitle;
        if(tCircleInfo.sCity.empty() || tCircleInfo.sLocation.empty())
        {
            sTitle="暂无定位信息";
        }
        else
        {
            if(bIsShowCity)
            {
                sTitle=tCircleInfo.sCity+" - "+tCircleInfo.sLocation;
            }
            else
            {
                sTitle=tCircleInfo.sLocation;
            }
        }
        for(auto& tCircleGroup:vCircleGroup)
        {
            if(tCircleGroup.sTitle==sTitle)
            {
                isFound=true;
                tCircleGroup.vCircleInfo.push_back(tCircleInfo);
            }
        }
        if(!isFound)
        {
            CircleGroup tCircleGroup;
            tCircleGroup.sTitle=sTitle;

            tCircleGroup.vCircleInfo.push_back(tCircleInfo);
            vCircleGroup.push_back(tCircleGroup);
        }
    }
    for(auto &tCircleGroup:vCircleGroup)
    {
        taf::Int32 iDistance=-1;
        for(auto &tCircleInfo:tCircleGroup.vCircleInfo)
        {
            if(iDistance<0|| tCircleInfo.iDistance<tCircleInfo.iDistance)
            {
                iDistance=tCircleInfo.iDistance;
            }
        }
        if(iDistance>=100)
        {
            tCircleGroup.sDistance=TC_Common::tostr(iDistance/100/10.0)+"km";
        }
        else if(iDistance>=0)
        {
            tCircleGroup.sDistance=TC_Common::tostr(iDistance)+"m";
        }
        sort(tCircleGroup.vCircleInfo.begin(),tCircleGroup.vCircleInfo.end(),[](const QUAN::CircleInfo &lhs,const QUAN::CircleInfo &rhs)
                {
                return lhs.lSecretCount>rhs.lSecretCount;
                });
    }

}


int CircleCache::LoadCircleNearestLocation(double dLat,double dLon,QUAN::CircleInfo& tCircleInfo)
{
    vector<QUAN::CircleLocation> vCircleLocation;
    if(GetCircleLocation(tCircleInfo.lCircleId,vCircleLocation)==0)
    {
        for(auto &tLocation:vCircleLocation)
        {
            taf::Int32 iDistance=tCProject.Distance(tLocation.dLat,tLocation.dLon,dLat,dLon);           
            if(tCircleInfo.iDistance<0 || iDistance<tCircleInfo.iDistance)
            {
                tCircleInfo.iDistance=iDistance;
                tCircleInfo.sCity=tLocation.sCity;
                tCircleInfo.sLocation=tLocation.sLocation;
                tCircleInfo.sCoord=TC_Common::tostr(tLocation.dLat)+","+TC_Common::tostr(tLocation.dLon);
            }
            if(tCircleInfo.iDistance>=0 && iDistance<tLocation.iRadius)
            {
                tCircleInfo.lCircleType|=QUAN::ENearBy;
            }
        }
        LOG_ASSERT(tCircleInfo.sCity.empty()==false);
        return 0;
    }
    return -1;
}
int CircleCache::LoadCircleNearestLocation(std::string sCoord,QUAN::CircleInfo& tCircleInfo)
{
    vector<double> vCoord=TC_Common::sepstr<double>(sCoord,",&");
    if(vCoord.size()==2)
    {
        return LoadCircleNearestLocation(vCoord[0],vCoord[1],tCircleInfo);
    }
    else
    {
        return LoadCircleNearestLocation(tCircleInfo);
    }
}

int CircleCache::LoadCircleNearestLocation(QUAN::CircleInfo& tCircleInfo)
{
    vector<QUAN::CircleLocation> vCircleLocation;
    if(GetCircleLocation(tCircleInfo.lCircleId,vCircleLocation)==0)
    {
        if(vCircleLocation.size()>0)
        {
            auto &tLocation=vCircleLocation.front();
            taf::Int32 iDistance=-1;
            if(tCircleInfo.iDistance<0 || iDistance<tCircleInfo.iDistance)
            {
                tCircleInfo.iDistance=iDistance;
                tCircleInfo.sCity=tLocation.sCity;
                tCircleInfo.sLocation=tLocation.sLocation;
                tCircleInfo.sCoord=TC_Common::tostr(tLocation.dLat)+","+TC_Common::tostr(tLocation.dLon);
            }
            if(tCircleInfo.iDistance>=0 && iDistance<tLocation.iRadius)
            {
                tCircleInfo.lCircleType|=QUAN::ENearBy;
            }
        }
        LOG_ASSERT(tCircleInfo.sCity.empty()==false);
        return 0;
    }
    return -1;
}
