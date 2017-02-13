#include "CircleImp.h"
#include "servant/Application.h"
#include "servant/TafReturn.h"
#include "util/tc_common.h"
#include "CircleServer.h"
#include "SecretBase.h"
#include "Base.h"
#include "ErrorCode.h"

#include "LogHelper.h"

using namespace std;

inline bool hasFlag(taf::Int64 value,taf::Int64 flag)
{
    return (value&flag)==flag;
}

//////////////////////////////////////////////////////
void CircleImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void CircleImp::destroy()
{
    //destroy servant here:
    //...
}

taf::Int32 CircleImp::createCircle(const QUAN::CreateCircleReq & tReq,QUAN::CreateCircleRsp & tRsp,taf::JceCurrentPtr current)
{
    if(checkCreateCircleReq(tReq))
    {
        struct storage
        {
            QUAN::CreateCircleReq tReq;
            QUAN::CreateCircleRsp tRsp;
        };
        auto tafRet=tafAsync::TafLogReturn<storage>::create(current);
        tafRet->getStorage().tReq=tReq;
        tafRet<<__FUNCTION__<<"|"<<tReq.tUserId.lUId<<"|"<<tReq.sName<<"|"<<tReq.sCoord<<"|"<<tReq.sCity+"-"+tReq.sLocation<<"|"<<tReq.iLocationType;

        MDW::IDReq tIDReq;
        tIDReq.sAppId="quan_circleid";
        auto ret=g_app.getUniqIDProxy()->async_getAutoIncrID(tIDReq);
        when(ret>=0)+=[tafRet](taf::Int32 ret,const MDW::IDRsp &tRsp)
        {
            QUAN::CircleInfo &tCircleInfo=tafRet->getStorage().tRsp.tCircleInfo;
            tCircleInfo.lCircleId=tRsp.lId;
            tCircleInfo.sName=tafRet->getStorage().tReq.sName;
            tCircleInfo.sCity=tafRet->getStorage().tReq.sCity;
            tCircleInfo.iLocationType=tafRet->getStorage().tReq.iLocationType;
            tCircleInfo.sLocation=tafRet->getStorage().tReq.sLocation;
            tCircleInfo.sCoord=tafRet->getStorage().tReq.sCoord;
            tCircleInfo.bAuditState=false;

            tafRet<<"$$"<<JCETOSTR(tCircleInfo);

            auto tCircleCache=CircleCache::getLockedInstance();
            bool bIsAdmin=tafRet->getStorage().tReq.tUserId.lUId<10000;
            if(tCircleCache->CreateCircle(tafRet->getStorage().tReq.tUserId.lUId,tCircleInfo,bIsAdmin)==0)
            {
                if(bIsAdmin==false)
                {
                    tCircleCache->SetCircleConcern(tafRet->getStorage().tReq.tUserId.lUId,tRsp.lId,true);
                }
                tafRet->instantReturn(ret);
            }
        };

        tafRet+=[this](JceCurrentPtr current,taf::Int32 ret,storage& stRet)
        {
            CircleImp::async_response_createCircle(current,ret,stRet.tRsp);   
        };
    }
    else
    {
        LOG->error()<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<endl;
        FDLOG()<<-1<<__FUNCTION__<<"|"<<tReq.tUserId.lUId<<"|"<<tReq.sName<<"|"<<tReq.sCoord<<"|"<<tReq.sCity+"-"+tReq.sLocation<<"|"<<tReq.iLocationType;
    }

    return -1;
}

taf::Int32 CircleImp::createCircleByAdmin(const QUAN::CreateCircleReq & tReq,const vector<QUAN::CircleLocation> & vCircleLocation,QUAN::CreateCircleRsp &tRsp,taf::JceCurrentPtr current)
{
    struct storage
    {
        QUAN::CreateCircleReq tReq;
        QUAN::CreateCircleRsp tRsp;
        vector<QUAN::CircleLocation> vCircleLocation;
    };
    auto tafRet = tafAsync::TafLogReturn<storage>::create( current );
    tafRet->getStorage().tReq = tReq;
    tafRet->getStorage().vCircleLocation=vCircleLocation;
    tafRet << __FUNCTION__ << "|" << tReq.tUserId.lUId << "|" << tReq.sName << "|" << tReq.sCoord << "|" << tReq.sCity + "-" + tReq.sLocation << "|" << tReq.iLocationType;

    MDW::IDReq tIDReq;
    tIDReq.sAppId = "quan_circleid";
    auto ret = g_app.getUniqIDProxy()->async_getAutoIncrID( tIDReq );
    when( ret >= 0 ) += [tafRet]( taf::Int32 ret, const MDW::IDRsp &tRsp )
    {
        QUAN::CircleInfo &tCircleInfo=tafRet->getStorage().tRsp.tCircleInfo;
        tCircleInfo.lCircleId=tRsp.lId;
        tCircleInfo.sName=tafRet->getStorage().tReq.sName;
        tCircleInfo.sCity=tafRet->getStorage().tReq.sCity;
        tCircleInfo.iLocationType=tafRet->getStorage().tReq.iLocationType;
        tCircleInfo.sLocation=tafRet->getStorage().tReq.sLocation;
        tCircleInfo.sCoord=tafRet->getStorage().tReq.sCoord;
        tCircleInfo.bAuditState=false;

        tafRet<<"$$"<<JCETOSTR(tCircleInfo);

        auto tCircleCache=CircleCache::getLockedInstance();
        if(tCircleCache->CreateCircle(tafRet->getStorage().tReq.tUserId.lUId,tCircleInfo,true)==0)
        {
            for(auto& tCircleLocation:tafRet->getStorage().vCircleLocation)
            {
                tCircleCache->AddCircleLocation(tRsp.lId,tCircleLocation);
            }
            tafRet->instantReturn(ret);
        }
    };

    tafRet += [this]( JceCurrentPtr current, taf::Int32 ret, storage &stRet )
    {
        CircleImp::async_response_createCircleByAdmin(current,ret,stRet.tRsp);
    };


    return -1;
}


taf::Int32 CircleImp::searchCircle(const QUAN::SearchCircleReq & tReq,QUAN::SearchCircleRsp & tRsp,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        vector<taf::Int64> vConcern,vAccessable;
        tCircleCache.GetUserCircle(tReq.tUserId.lUId,vConcern,vAccessable);

        map<taf::Int64,taf::Int32> mpCircleId;
        for(auto lCircleId:vAccessable)
        {
            mpCircleId[lCircleId]|=QUAN::EAccessable;
        }
        for(auto lCircleId:vConcern)
        {
            mpCircleId[lCircleId]|=QUAN::EConcern;
        }

        iRet=tCircleCache.SearchCircle(tReq.sCond,tReq.sCoord,tRsp.vCircleGroup);
        vector<double> vCoord=TC_Common::sepstr<double>(tReq.sCoord,",&");
        for(auto &tCircleGroup:tRsp.vCircleGroup)
        {
            for(auto &tCircleInfo:tCircleGroup.vCircleInfo)
            {
                tCircleInfo.lCircleType|=mpCircleId[tCircleInfo.lCircleId];
            }
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    LOG->debug()<<__FUNCTION__<<"|"<<JCETOSTR(tReq)<<"$$"<<JCETOSTR(tRsp)<<endl;
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<JCETOSTR(tReq)<<"$$"<<JCETOSTR(tRsp)<<endl;
    return iRet;
}


taf::Int32 CircleImp::auditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string & sReason,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        tCircleCache.AuditCircle(lCircleId,iAuditState,sReason);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lCircleId<<"|"<<iAuditState<<endl;
    return iRet;
}


taf::Int32 CircleImp::getCircle(const QUAN::GetCircleReq & tReq,QUAN::GetCircleRsp & tRsp,taf::JceCurrentPtr current)
{
    taf::Int32 iRet=-1;
    bool additionRecomm=false;
    map<taf::Int64,taf::Int32> mpCircleId;
    try
    {
        vector<taf::Int64> vConcern,vAccessable;
        tCircleCache.GetUserCircle(tReq.tUserId.lUId,vConcern,vAccessable);

        if(hasFlag(tReq.lCircleType,QUAN::EAccessable))
        {
            CircleGroup tCircleGroup;
            for(auto lCircleId:vAccessable)
            {
                CircleInfo tCircleInfo;
                if ( tCircleCache.GetCircle( lCircleId, tCircleInfo ) == 0 )
                {
                    if ( tCircleInfo.bAuditState )
                    {
                        if ( mpCircleId.find( lCircleId ) == mpCircleId.end() )
                        {
                            tCircleGroup.vCircleInfo.push_back( tCircleInfo );
                        }
                        mpCircleId[lCircleId] |= QUAN::EAccessable;
                    }
                }
            }
            if(!tCircleGroup.vCircleInfo.empty())
            {
                tCircleGroup.sTitle="我的树洞";
                tRsp.vCircleGroup.push_back(tCircleGroup);
            }
        }
        if(hasFlag(tReq.lCircleType,QUAN::EConcern))
        {
            CircleGroup tCircleGroup;
            for(auto lCircleId:vConcern)
            {
                CircleInfo tCircleInfo;
                if ( tCircleCache.GetCircle( lCircleId, tCircleInfo ) == 0 )
                {
                    if ( mpCircleId.find( lCircleId ) == mpCircleId.end() )
                    {
                        tCircleGroup.vCircleInfo.push_back(tCircleInfo);
                    }
                    mpCircleId[lCircleId] |= QUAN::EConcern;
                }
            }
            if(!tCircleGroup.vCircleInfo.empty())
            {
                tCircleGroup.sTitle="我关注的树洞";
                tRsp.vCircleGroup.push_back(tCircleGroup);
            }
        }

        if(hasFlag(tReq.lCircleType,QUAN::ENearBy) || hasFlag(tReq.lCircleType,QUAN::ENearByAll)) 
        {
            vector<double> vCoord=taf::TC_Common::sepstr<double>(tReq.sCoord,",&",true);
            LOG_ASSERT_DO(vCoord.size()==2)
            {
                vector<CircleInfo> vCircleInfoNew;

                {
                    vector<CircleInfo> vCircleInfo;
                    tCircleCache.GetNearbyCircle( vCoord[0], vCoord[1], 5 * 1000, 100, vCircleInfo );
                    for ( auto &tCircleInfo : vCircleInfo )
                    {
                        if ( mpCircleId.find( tCircleInfo.lCircleId ) == mpCircleId.end() )
                        {
                            vCircleInfoNew.push_back( tCircleInfo );
                        }
                        mpCircleId[tCircleInfo.lCircleId] |= QUAN::ENearBy;
                    }
                }

                //##如果附近没有圈子,则扩大范围搜,但是不要给ENearBy标记
                if(hasFlag(tReq.lCircleType,QUAN::ENearByAll) && vCircleInfoNew.size()<50)
                {
                    vector<CircleInfo> vCircleInfo;
                    tCircleCache.GetNearbyCircleAll( vCoord[0], vCoord[1], 100 * 1000, 50, vCircleInfo );
                    for ( auto &tCircleInfo : vCircleInfo )
                    {
                        if ( mpCircleId.find( tCircleInfo.lCircleId ) == mpCircleId.end() )
                        {
                            vCircleInfoNew.push_back( tCircleInfo );
                        }
                        mpCircleId[tCircleInfo.lCircleId];
                        if ( vCircleInfoNew.size() >= 50 )
                        {
                            break;
                        }
                    }
                    if(vCircleInfoNew.size()<50)
                    {
                        additionRecomm=true;
                    }
                }

                vector<CircleGroup> vCircleGroup;
                tCircleCache.GroupingCircle(vCircleInfoNew,vCircleGroup);

                for(auto &tCircleGroup:vCircleGroup)
                {
                    tRsp.vCircleGroup.push_back(tCircleGroup);
                }
            }
        }
        if(!hasFlag(tReq.lCircleType,QUAN::EConcern))
        {
            for(auto lCircleId:vConcern)
            {
                mpCircleId[lCircleId]|=QUAN::EConcern;
            }
        }
        if(!hasFlag(tReq.lCircleType,QUAN::EAccessable))
        {
            for(auto lCircleId:vAccessable)
            {
                mpCircleId[lCircleId]|=QUAN::EAccessable;
            }
        }
        if(hasFlag(tReq.lCircleType,QUAN::ERecomm) || additionRecomm)
        {
            CircleGroup tCircleGroup;
            tCircleCache.GetRecommCircle(tCircleGroup);
            tRsp.vCircleGroup.push_back(tCircleGroup);
        }
        for(auto &tCircleGroup:tRsp.vCircleGroup)
        {
            for(auto &tCircleInfo:tCircleGroup.vCircleInfo)
            {
                tCircleCache.LoadCircleNearestLocation(tReq.sCoord,tCircleInfo);
                tCircleInfo.lCircleType|=mpCircleId[tCircleInfo.lCircleId];
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    LOG->debug()<<__FUNCTION__<<"|"<<tReq.tUserId.lUId<<"|"<<tReq.lCircleType<<"|"<<tReq.sCoord<<"$$"<<JCETOSTR(tRsp)<<endl;
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.tUserId.lUId<<"|"<<tReq.lCircleType<<endl;
    return iRet;
}

taf::Int32 CircleImp::getCircleInfo(const QUAN::UserId & tId,taf::Int64 lCircleId,const std::string & sCoord,QUAN::CircleInfo &tCircleInfo,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        vector<taf::Int64> vConcern,vAccessable;
        tCircleCache.GetUserCircle(tId.lUId,vConcern,vAccessable);
        if(tCircleCache.GetCircle(lCircleId,tCircleInfo)==0)
        {
            if(binary_search(vConcern.begin(),vConcern.end(),lCircleId))
            {
                tCircleInfo.lCircleType|=QUAN::EConcern;
            }
            if(binary_search(vAccessable.begin(),vAccessable.end(),lCircleId))
            {
                tCircleInfo.lCircleType|=QUAN::EAccessable;
            }
            tCircleCache.LoadCircleNearestLocation(sCoord,tCircleInfo);
            tCircleCache.GetCicleUnlockNum(tId.lUId,tCircleInfo.lCircleId,tCircleInfo.iLockNum);
            iRet=0;
        }
        else
        {
            iRet=ERR_NOEXISTS;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tId.lUId<<"|"<<lCircleId<<"|"<<sCoord<<"$$"<<JCETOSTR(tCircleInfo)<<endl;
    return iRet;
}

taf::Int32 CircleImp::setCircleConcern(const QUAN::SetCircleReq & tReq,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        iRet=tCircleCache.SetCircleConcern(tReq.tUserId.lUId,tReq.lCircleId,tReq.bIsConcern);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tReq.tUserId.lUId<<"|"<<tReq.lCircleId<<"|"<<tReq.bIsConcern<<endl;
    return iRet;
}


taf::Int32 CircleImp::postInCircle(const QUAN::UserId & tId,taf::Int64 lCircleId,const std::string & sCoord,taf::Bool bIsComment,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CircleInfo tCircleInfo;
        if(tCircleCache.GetCircle(lCircleId,tCircleInfo)==0)
        {
            tCircleCache.LoadCircleNearestLocation(sCoord,tCircleInfo);
            iRet=tCircleCache.PostInCircle(tId.lUId,lCircleId,hasFlag(tCircleInfo.lCircleType,QUAN::ENearBy));   //todo postnearby flag
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<tId.lUId<<"|"<<lCircleId<<"|"<<sCoord<<endl;
    return iRet;
}

taf::Int32 CircleImp::recommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        iRet=tCircleCache.RecommCircle(lCircleId,iRecommValue);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lCircleId<<"|"<<iRecommValue<<endl;
    return iRet;
}

bool CircleImp::checkCreateCircleReq(const QUAN::CreateCircleReq &tReq)
{
    if(tReq.sName.empty() || tReq.sCity.empty() || tReq.sLocation.empty())
    {
        return false;
    }
    vector<double> vCoord=TC_Common::sepstr<double>(tReq.sCoord,",&");
    if(vCoord.size()!=2 || vCoord[0]==0 || vCoord[1]==0)
    {
        return false;
    }

    return true;
}

taf::Int32 CircleImp::clearCircleCache(taf::Int64 lCircleId,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        CircleSharedCache::getInstance()->ClearCircle(lCircleId);
        CircleSharedCache::getInstance()->ClearCircleLocation(lCircleId);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lCircleId<<endl;
    return iRet;
}

taf::Int32 CircleImp::clearLbsCache(const std::string & sCoord,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        vector<double> vCoord=TC_Common::sepstr<double>(sCoord,",&");
        if(vCoord.size()==2)
        {
            tCircleCache.ClearLocationCircleCache(vCoord[0],vCoord[1]);
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<sCoord<<endl;
    return iRet;
}

taf::Int32 CircleImp::addCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation & tCircleLocation,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        iRet=tCircleCache.AddCircleLocation(lCircleId,tCircleLocation);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lCircleId<<endl;
    return iRet;
}

taf::Int32 CircleImp::modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation & tCircleLocation,taf::JceCurrentPtr current)
{
    int iRet=-1;
    try
    {
        iRet=tCircleCache.modifyCircleLocation(lCircleLocationId,tCircleLocation);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    FDLOG()<<iRet<<"|"<<__FUNCTION__<<"|"<<lCircleLocationId<<"|"<<tCircleLocation<<endl;
    return iRet;
}
