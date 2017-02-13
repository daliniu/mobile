#ifndef _CircleImp_H_
#define _CircleImp_H_

#include "servant/Application.h"
#include "Circle.h"
#include "CircleCache.h"

/**
 *
 *
 */
class CircleImp : public QUAN::Circle
{
    public:
        /**
         *
         */
        virtual ~CircleImp() {}

        /**
         *
         */
        virtual void initialize();

        /**
         *
         */
        virtual void destroy();

        virtual taf::Int32 createCircle(const QUAN::CreateCircleReq & tReq,QUAN::CreateCircleRsp & tRsp,taf::JceCurrentPtr current);
        virtual taf::Int32 createCircleByAdmin(const QUAN::CreateCircleReq & tReq,const vector<QUAN::CircleLocation> & vCircleLocation,QUAN::CreateCircleRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 searchCircle(const QUAN::SearchCircleReq & tReq,QUAN::SearchCircleRsp & tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 getCircle(const QUAN::GetCircleReq & tReq,QUAN::GetCircleRsp & tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 getCircleInfo(const QUAN::UserId & tId,taf::Int64 lCircleId,const std::string & sCoord,QUAN::CircleInfo &tCircleInfo,taf::JceCurrentPtr current);

        virtual taf::Int32 setCircleConcern(const QUAN::SetCircleReq & tReq,taf::JceCurrentPtr current);

        virtual taf::Int32 postInCircle(const QUAN::UserId & tId,taf::Int64 lCircleId,const std::string & sCoord,taf::Bool bIsComment,taf::JceCurrentPtr current);

        virtual taf::Int32 auditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string & sReason,taf::JceCurrentPtr current);

        virtual taf::Int32 recommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue,taf::JceCurrentPtr current);

        virtual taf::Int32 clearCircleCache(taf::Int64 lCircleId,taf::JceCurrentPtr current);
        virtual taf::Int32 clearLbsCache(const std::string & sCoord,taf::JceCurrentPtr current);
        
        virtual taf::Int32 addCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation & tCircleLocation,taf::JceCurrentPtr current);
        virtual taf::Int32 modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation & tCircleLocation,taf::JceCurrentPtr current);
    protected:
        bool checkCreateCircleReq(const QUAN::CreateCircleReq &tReq);
    private:
        CircleCache tCircleCache;
        CProject tCProject;
};
/////////////////////////////////////////////////////
#endif
