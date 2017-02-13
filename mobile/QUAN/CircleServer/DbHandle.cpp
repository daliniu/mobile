#include "Circle.h"
#include "DbHandle.h"
#include "StringUtil.h"
#include "LogHelper.h"
#include "Var.h"

#define DBINT(value) make_pair(TC_Mysql::DB_INT,TC_Common::tostr(value))
#define DBSTR(value) make_pair(TC_Mysql::DB_STR,(value))
#define DBDAT(value) make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(value)+")")
#define DBFUN(value) make_pair(TC_Mysql::DB_INT,(value))

int DbHandle::init()
{

    try
    {
        TC_Config tConf;
        tConf.parseFile(ServerConfig::BasePath + "CircleServer.conf");

        string sHost        =  tConf.get("/main/db/<dbhost>");
        string sUser        =  tConf.get("/main/db/<dbuser>");
        string sPassword    =  tConf.get("/main/db/<dbpass>");
        string sCharset     =  tConf.get("/main/db/<charset>");
        int sPort           =  TC_Common::strto<int>(tConf.get("/main/db/<dbport>"));

        _sDbPre             =  tConf.get("/main/db/<dbpre>","secret");
        _sCircleTbName     =  tConf.get("/main/db/<tbcircle>","circle");
        _sCircleUserTbName     =  tConf.get("/main/db/<tbpost>","circle_user");
        _sCirclePostTbName =  tConf.get("/main/db/<tbpostoper>","circle_post");
        _sCircleLocationTbName =  tConf.get("/main/db/<tbpostoper>","circle_location");
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

        return 0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    exit(0);
    return -1;
}

int DbHandle::CreateCircle(taf::Int64 lUId,QUAN::CircleInfo &tCircleInfo,bool bIsAdmin)
{
    int iRet=-1;
    try
    {
        {
            taf::TC_Mysql::RECORD_DATA rd;
            rd["id"]=DBINT(tCircleInfo.lCircleId);
            rd["name"]=DBSTR(tCircleInfo.sName);
            rd["creatorid"]=DBINT(lUId);
            rd["post_time"]=DBDAT(tCircleInfo.iLastPostTime);
            rd["type"]=DBINT(tCircleInfo.iLocationType);


            rd["secretcount"]=DBINT(0);
            rd["concerncount"]=DBINT(0);
            rd["auditstate"]=DBINT(1);
            rd["recomm"]=DBINT(0);
            rd["create_time"]=DBDAT(TC_TimeProvider::getInstance()->getNow());
            rd["createType"]=DBINT(bIsAdmin?2:1);

            getMysql(tCircleInfo.lCircleId)->insertRecord(getCircleTbName(tCircleInfo.lCircleId),rd);
        }
        {
            vector<double> vCoord=TC_Common::sepstr<double>(tCircleInfo.sCoord,",&");
            if(vCoord.size()==2) 
            {
                CircleLocation tCircleLocation;

                tCircleLocation.sCity=tCircleInfo.sCity;
                tCircleLocation.sLocation=tCircleInfo.sLocation;
                tCircleLocation.dLat=vCoord[0];
                tCircleLocation.dLon=vCoord[1];
                switch(tCircleInfo.iLocationType)
                {
#if 1
                    case QUAN::EOffice:
                        tCircleLocation.iRadius=500;
                        break;
                    case QUAN::EShcool:
                        tCircleLocation.iRadius=1000;
                        break;
                    case QUAN::ECommunity:
                        tCircleLocation.iRadius=1000;
                        break;
#else
                    case QUAN::EOffice:
                        tCircleLocation.iRadius=20;
                        break;
                    case QUAN::EShcool:
                        tCircleLocation.iRadius=1000;
                        break;
                    case QUAN::ECommunity:
                        tCircleLocation.iRadius=50;
                        break;
#endif
                    default:
                        LOG->error()<<__FUNCTION__<<"|Invaild LocationType:"<<tCircleInfo.iLocationType<<endl;
                }
                AddCircleLocation(tCircleInfo.lCircleId,tCircleLocation);
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::GetCircle(taf::Int64 lCircleId,QUAN::CircleInfo &tCircleInfo)
{
    int iRet=-1;
    try
    {
        string sSql="select id,name,type,UNIX_TIMESTAMP(post_time) as post_time,secretcount,concerncount,auditstate from "+getCircleTbName(lCircleId)+
            " where id="+TC_Common::tostr(lCircleId);

        auto rd=getMysql(0)->queryRecord(sSql);

        if(rd.size()==1)
        {
            for(auto &row:rd.data())
            {
                int iAuditState=Var(row["auditstate"]);
                tCircleInfo.lCircleId=Var(row["id"]);
                tCircleInfo.sName=row["name"];
                tCircleInfo.iLocationType=Var(row["type"]);
                tCircleInfo.iLastPostTime=Var(row["post_time"]);
                tCircleInfo.lSecretCount=Var(row["secretcount"]);
                tCircleInfo.lConcernCount=Var(row["concerncount"]);
                tCircleInfo.bAuditState=iAuditState==3;
            }
            iRet=0;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|DB FAILED:"<<lCircleId<<"=>"<<rd.size()<<endl;
            iRet=-1;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::GetCircleLocation(taf::Int64 lCircleId,vector<QUAN::CircleLocation> &vCircleLocation)
{
    int iRet=-1;
    try
    {
        string sSql="select city,location,dLat,dLon,radius from "+getCircleLocationTbName()+
            " where circleid="+TC_Common::tostr(lCircleId);

        auto rd=getMysql(0)->queryRecord(sSql);

        for(auto &row:rd.data())
        {
            QUAN::CircleLocation tCircleLocation;
            tCircleLocation.sCity=row["city"];
            tCircleLocation.sLocation=row["location"];
            tCircleLocation.dLat=Var(row["dLat"]);
            tCircleLocation.dLon=Var(row["dLon"]);
            tCircleLocation.iRadius=Var(row["radius"]);
            vCircleLocation.push_back(tCircleLocation);
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::AddCircleLocation(taf::Int64 lCircleId,const QUAN::CircleLocation &vCircleLocation)
{
    int iRet=-1;
    try
    {
        taf::TC_Mysql::RECORD_DATA rd;
        rd["circleid"]=DBINT(lCircleId);
        rd["city"]=DBSTR(vCircleLocation.sCity);
        rd["location"]=DBSTR(vCircleLocation.sLocation);
        rd["dlat"]=DBINT(vCircleLocation.dLat);
        rd["dlon"]=DBINT(vCircleLocation.dLon);

        int iLat,iLon;
        tCProject.Gauss_BL2xy(vCircleLocation.dLat,vCircleLocation.dLon,iLat,iLon);

        rd["ilat"]=DBINT(iLat);
        rd["ilon"]=DBINT(iLon);
        rd["radius"]=DBINT(vCircleLocation.iRadius);
        rd["createtime"]=DBDAT(TC_TimeProvider::getInstance()->getNow());

        getMysql(0)->insertRecord(getCircleLocationTbName(),rd);
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::modifyCircleLocation(taf::Int64 lCircleLocationId,const QUAN::CircleLocation &vCircleLocation)
{
    int iRet=-1;
    try
    {
        taf::TC_Mysql::RECORD_DATA rd;
        rd["city"]=DBSTR(vCircleLocation.sCity);
        rd["location"]=DBSTR(vCircleLocation.sLocation);
        rd["dlat"]=DBINT(vCircleLocation.dLat);
        rd["dlon"]=DBINT(vCircleLocation.dLon);

        int iLat,iLon;
        tCProject.Gauss_BL2xy(vCircleLocation.dLat,vCircleLocation.dLon,iLat,iLon);

        rd["ilat"]=DBINT(iLat);
        rd["ilon"]=DBINT(iLon);
        rd["radius"]=DBINT(vCircleLocation.iRadius);
        rd["createtime"]=DBDAT(TC_TimeProvider::getInstance()->getNow());

        getMysql(0)->updateRecord(getCircleLocationTbName(),rd," where id="+TC_Common::tostr(lCircleLocationId));
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::SearchCircle(const std::string& sCond,vector<CircleInfo>& vCircleInfo)
{
    int iRet=-1;
    try
    {
        string tCond=getMysql(0)->escapeString(sCond);
        string sSql="select id,name,type,UNIX_TIMESTAMP(post_time) as post_time,secretcount,concerncount,auditstate from "+getCircleTbName(0) +
            " where auditstate=3 and name like '%"+tCond+"%' order by secretcount desc limit 100";

        auto rd=getMysql(0)->queryRecord(sSql);

        for(auto &row:rd.data())
        {
            CircleInfo tCircleInfo;

            int iAuditState=Var(row["auditstate"]);
            tCircleInfo.lCircleId=Var(row["id"]);
            tCircleInfo.sName=row["name"];
            tCircleInfo.iLocationType=Var(row["type"]);
            tCircleInfo.iLastPostTime=Var(row["post_time"]);
            tCircleInfo.lSecretCount=Var(row["secretcount"]);
            tCircleInfo.lConcernCount=Var(row["concerncount"]);
            tCircleInfo.bAuditState=iAuditState==3;

            vCircleInfo.push_back(tCircleInfo);
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::GetUserCircle(taf::Int64 lUId,vector<taf::Int64>& vConcern,vector<taf::Int64>& vAccessable)
{
    int iRet=-1;
    try
    {
        string sSql="select concern_circle,accessable_circle from "+getCircleUserTbName(lUId)+ " where id="+TC_Common::tostr(lUId);
        auto rd=getMysql(lUId)->queryRecord(sSql);
        for(auto &row:rd.data())
        {
            {
                QUAN::CircleIdList idList;            
                taf::JceInputStream<taf::BufferReader> tReader;
                string &concern=row["concern_circle"];
                tReader.setBuffer(concern.c_str(),concern.length());
                idList.readFrom(tReader);
                vConcern=idList.vCircleId;
            }

            {
                QUAN::CircleIdList idList;            
                taf::JceInputStream<taf::BufferReader> tReader;
                string &accessable=row["accessable_circle"];
                tReader.setBuffer(accessable.c_str(),accessable.length());
                idList.readFrom(tReader);
                vAccessable=idList.vCircleId;
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::GetLocationCircle(taf::Int32 iLat,taf::Int32 iLon,taf::Int32 iRadius,vector<QUAN::CircleLbsInfo>& vCircleLbsInfo)
{
    int iRet=-1;
    try
    {
        double maxLat,minLat;
        double maxLon,minLon;

        maxLat=iLat+iRadius;
        minLat=iLat-iRadius;
        maxLon=iLon+iRadius;
        minLon=iLon-iRadius;

        string sSql="select circleid,city,location,dlat,dlon,radius from " +getCircleLocationTbName()+
            " where ilat>"+TC_Common::tostr(minLat)+" and ilat<="+TC_Common::tostr(maxLat)+
            " and ilon>"+TC_Common::tostr(minLon)+" and ilon<="+TC_Common::tostr(maxLon);


        auto rd=getMysql(0)->queryRecord(sSql);

        for(auto &row:rd.data())
        {
            QUAN::CircleLbsInfo tCircleLbsInfo;
            tCircleLbsInfo.lCircleId=Var(row["circleid"]);
            tCircleLbsInfo.tLocation.sCity=row["city"];
            tCircleLbsInfo.tLocation.sLocation=row["location"];
            tCircleLbsInfo.tLocation.dLat=Var(row["dlat"]);
            tCircleLbsInfo.tLocation.dLon=Var(row["dlon"]);
            tCircleLbsInfo.tLocation.iRadius=Var(row["radius"]);
            vCircleLbsInfo.push_back(tCircleLbsInfo);
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::GetRecommCircle(QUAN::CircleGroup& tCircleGroup)
{
    int iRet=-1;
    try
    {
        string sSql="select id,name,type,UNIX_TIMESTAMP(post_time) as post_time,secretcount,concerncount,auditstate,rand() as rn from " +getCircleTbName(0)+
            " where (auditstate=3) and recomm>0"
            " order by rn,recomm,secretcount,concerncount limit 10";

        auto rd=getMysql(0)->queryRecord(sSql);
        tCircleGroup.sTitle="推荐";

        for(auto &row:rd.data())
        {
            CircleInfo tCircleInfo;
            tCircleInfo.lCircleId=Var(row["id"]);
            tCircleInfo.sName=row["name"];
            tCircleInfo.iLocationType=Var(row["type"]);
            tCircleInfo.iLastPostTime=Var(row["post_time"]);
            tCircleInfo.lSecretCount=Var(row["secretcount"]);
            tCircleInfo.lConcernCount=Var(row["concerncount"]);
            tCircleInfo.bAuditState=Var(row["auditstate"]);

            tCircleGroup.vCircleInfo.push_back(tCircleInfo);
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::SetCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern)
{
    int iRet=-1;
    try
    {
        if(setInnerCircleConcern(lUId,lCircleId,bIsConcern)==0)
        {
            if(bIsConcern)
            {
                updateCircle(lCircleId,"concerncount","concerncount+1");
            }
            else
            {
                updateCircle(lCircleId,"concerncount","concerncount-1","concerncount>0");
            }
        }
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::setInnerCircleConcern(taf::Int64 lUId,taf::Int64 lCircleId,bool bIsConcern)
{
    int iRet=-1;
    QUAN::CircleIdList idList;            
    auto &vCircleId=idList.vCircleId;
    string sSql="select concern_circle from "+getCircleUserTbName(lUId)+ " where id="+TC_Common::tostr(lUId)+ " limit 1";
    auto ret=getMysql(lUId)->queryRecord(sSql);
    if(ret.size()>0)
    {
        for(auto &row:ret.data())
        {
            taf::JceInputStream<taf::BufferReader> tReader;
            string &concern=row["concern_circle"];
            tReader.setBuffer(concern.c_str(),concern.length());
            idList.readFrom(tReader);
        }

        //if(bIsConcern>0 && idList.vCircleId.size()>=20)
        //{
        //    LOG->error()<<__FUNCTION__<<"|"<<"Concernt Count Limit:"<<lUId<<endl;
        //    iRet=-1;
        //}
        //else
        {
            if(bIsConcern)
            {
                vCircleId.push_back(lCircleId);
                sort(vCircleId.begin(),vCircleId.end());
                vCircleId.erase(unique(vCircleId.begin(),vCircleId.end()),vCircleId.end());
            }
            else
            {
                auto iters=equal_range(vCircleId.begin(),vCircleId.end(),lCircleId);
                if(iters.first!=iters.second)
                {
                    vCircleId.erase(iters.first);
                }
            }
            taf::TC_Mysql::RECORD_DATA rd;
            taf::JceOutputStream<taf::BufferWriter> tWriter;
            idList.writeTo(tWriter);
            rd["concern_circle"]=DBSTR(string(tWriter.getBuffer(),tWriter.getLength()));
            iRet=getMysql(lUId)->updateRecord(getCircleUserTbName(lUId),rd,"where id="+TC_Common::tostr(lUId));
        }
    }
    else if(bIsConcern)
    {
        vCircleId.push_back(lCircleId);
        taf::TC_Mysql::RECORD_DATA rd;
        taf::JceOutputStream<taf::BufferWriter> tWriter;
        idList.writeTo(tWriter);
        rd["id"]=DBINT(lUId);
        rd["concern_circle"]=DBSTR(string(tWriter.getBuffer(),tWriter.getLength()));
        iRet=getMysql(lUId)->insertRecord(getCircleUserTbName(lUId),rd);
    }
    return iRet>0?0:-1;
}

int DbHandle::setInnerCircleAccessable(taf::Int64 lUId,taf::Int64 lCircleId)
{
    int iRet=-1;

    QUAN::CircleIdList idList;            
    auto &vCircleId=idList.vCircleId;
    string sSql="select accessable_circle from "+getCircleUserTbName(lUId)+ " where id="+TC_Common::tostr(lUId)+ " limit 1";
    auto ret=getMysql(lUId)->queryRecord(sSql);
    if(ret.size()>0)
    {
        for(auto &row:ret.data())
        {
            taf::JceInputStream<taf::BufferReader> tReader;
            string &accessable=row["accessable_circle"];
            tReader.setBuffer(accessable.c_str(),accessable.length());
            idList.readFrom(tReader);
        }
        vCircleId.push_back(lCircleId);
        sort(vCircleId.begin(),vCircleId.end());
        vCircleId.erase(unique(vCircleId.begin(),vCircleId.end()),vCircleId.end());

        taf::TC_Mysql::RECORD_DATA rd;
        taf::JceOutputStream<taf::BufferWriter> tWriter;
        idList.writeTo(tWriter);
        rd["accessable_circle"]=DBSTR(string(tWriter.getBuffer(),tWriter.getLength()));
        iRet=getMysql(lUId)->updateRecord(getCircleUserTbName(lUId),rd," where id="+TC_Common::tostr(lUId));
    }
    else
    {
        vCircleId.push_back(lCircleId);
        taf::TC_Mysql::RECORD_DATA rd;
        taf::JceOutputStream<taf::BufferWriter> tWriter;
        idList.writeTo(tWriter);
        rd["id"]=DBINT(lUId);
        rd["accessable_circle"]=DBSTR(string(tWriter.getBuffer(),tWriter.getLength()));
        iRet=getMysql(lUId)->insertRecord(getCircleUserTbName(lUId),rd);
    }
    return iRet>0?0:-1;
}

int DbHandle::PostInCircle(taf::Int64 lUId,taf::Int64 lCircleId,bool bPostNearBy)
{
    int iRet=-1;
    try
    {
        taf::TC_Mysql::RECORD_DATA rd;
        auto now=TC_TimeProvider::getInstance()->getNow();
        rd["post_time"]=DBDAT(now);
        rd["secretcount"]=DBFUN("secretcount+1");
        getMysql(lCircleId)->updateRecord(getCircleTbName(lCircleId),rd," where id="+TC_Common::tostr(lCircleId));


        if(bPostNearBy)
        {
            rd.clear();
            rd["uid"]=DBINT(lUId);
            rd["circleid"]=DBINT(lCircleId);
            rd["post_time"]=DBDAT(now);

            int ret=getMysql(lUId)->insertRecord(getCirclePostTbName(lUId),rd);
            if(ret>0)
            {
                int iLockNum;
                if(GetCicleUnlockNum(lUId,lCircleId,iLockNum)==0 && iLockNum>=2)
                {
                    setInnerCircleAccessable(lUId,lCircleId);
                }
                LOG->debug()<<__FUNCTION__<<"|"<<"Post In Circle Nearby :"<<iLockNum<<endl;
            }
        }

        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::AuditCircle(taf::Int64 lCircleId,taf::Int32 iAuditState,const std::string &sReason)
{
    int iRet=-1;
    try
    {
        taf::TC_Mysql::RECORD_DATA rd;
        rd["auditstate"]=DBINT(TC_Common::tostr(iAuditState));
        rd["audit_time"]=DBDAT(TC_TimeProvider::getInstance()->getNow());
        rd["reason"]=DBSTR(sReason);
        getMysql(lCircleId)->updateRecord(getCircleTbName(lCircleId),rd," where id="+TC_Common::tostr(lCircleId));
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}


int DbHandle::RecommCircle(taf::Int64 lCircleId,taf::Int32 iRecommValue)
{
    int iRet=-1;
    try
    {
        updateCircle(lCircleId,"recomm",TC_Common::tostr(iRecommValue));
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::GetCicleUnlockNum(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 &iLockNum)
{
    int iRet=-1;
    try
    {
        string sSql="select count(*) as locknum from "+getCirclePostTbName(lUId)+" where  "
            " uid="+TC_Common::tostr(lUId) +
            " and circleid="+TC_Common::tostr(lCircleId) +
            " and post_time>adddate(now(),-60)";
        auto rd=getMysql(lUId)->queryRecord(sSql);
        LOG_ASSERT_DO(rd.size()>0)
        {
            iLockNum=Var(rd[0]["locknum"]);
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

void DbHandle::updateCircle(taf::Int64 lCircleId,const string &field,const string &value,const string &where)
{
    taf::TC_Mysql::RECORD_DATA rd;
    rd[field]=DBFUN(value);

    if(where.empty())
    {
        getMysql(lCircleId)->updateRecord(getCircleTbName(lCircleId),rd," where id="+TC_Common::tostr(lCircleId));
    }
    else
    {
        getMysql(lCircleId)->updateRecord(getCircleTbName(lCircleId),rd," where id="+TC_Common::tostr(lCircleId)+" and "+where);
    }
}

TC_Mysql* DbHandle::getMysql(Int64 lSId)
{
    return _mMysql[0];
}

string DbHandle::getCircleTbName(Int64 lSId)
{
    return _sCircleTbName;
}
string DbHandle::getCircleUserTbName(Int64 lSId)
{
    return _sCircleUserTbName;
}
string DbHandle::getCirclePostTbName(Int64 lSId)
{
    return _sCirclePostTbName;
}

string DbHandle::getCircleLocationTbName()
{
    return _sCircleLocationTbName;
}
