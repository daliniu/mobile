#include "DbHandle.h"

#define DBINT(value) make_pair(TC_Mysql::DB_INT,TC_Common::tostr(value))
#define DBSTR(value) make_pair(TC_Mysql::DB_STR,(value))
#define DBDAT(value) make_pair(TC_Mysql::DB_INT,"from_unixtime("+TC_Common::tostr(value)+")")
#define DBFUN(value) make_pair(TC_Mysql::DB_INT,(value))

int DbHandle::init()
{

    try
    {
        TC_Config tConf;
        tConf.parseFile(ServerConfig::BasePath + "PostServer.conf");

        string sHost        =  tConf.get("/main/db/<dbhost>");
        string sUser        =  tConf.get("/main/db/<dbuser>");
        string sPassword    =  tConf.get("/main/db/<dbpass>");
        string sCharset     =  tConf.get("/main/db/<charset>");
        int sPort           =  TC_Common::strto<int>(tConf.get("/main/db/<dbport>"));

        _sDbPre             =  tConf.get("/main/db/<dbpre>","secret");
        _sTbUserInfoPre     =  tConf.get("/main/db/<tbuserpre>","user_");
        _sPostTbNamePre     =  tConf.get("/main/db/<tbpost>","posts");
        _sPostOperTbNamePre =  tConf.get("/main/db/<tbpostoper>","posts_operation");
        _sPostCommentTbNamePre= tConf.get("/main/db/<tbpostcomment>","posts_comment");
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

int DbHandle::shareSecret(taf::Int64 lSId,taf::Int64 lShareId)
{
    return updatePosts(lSId,"shareid",TC_Common::tostr(lShareId));
}

int DbHandle::updatePosts(taf::Int64 lSId,const std::string &updateField,const std::string &updateFieldTo,const std::string &condition)
{
    string sCondition=" where id=" +TC_Common::tostr(lSId);
    if(!condition.empty())
    {
        sCondition+=" and "+condition;
    }


    TC_Mysql::RECORD_DATA rd;
    rd[updateField]=DBFUN(updateFieldTo);

    return getMysql(lSId)->updateRecord(getPostTbName(lSId),rd,sCondition);
}

int DbHandle::updatePostsComment(taf::Int64 lCId,const std::string &updateField,const std::string &updateFieldTo,const std::string &condition)
{
    string sCondition=" where id=" +TC_Common::tostr(lCId);
    if(!condition.empty())
    {
        sCondition+=" and "+condition;
    }

    TC_Mysql::RECORD_DATA rd;
    rd[updateField]=DBFUN(updateFieldTo);

    return getMysql(lCId)->updateRecord(getPostCommentTbName(lCId),rd,sCondition);
}

int DbHandle::updatePostsOp(taf::Int64 lUId,taf::Int32 iType,taf::Int64 lObjectId,taf::Int64 lRelateId,taf::Int32 iOp,taf::Int32 iAvailable)
{
    TC_Mysql::RECORD_DATA rd;

    rd["uid"]=DBINT(lUId);
    rd["type"]=DBINT(0);
    rd["objectid"]=DBINT(lObjectId);
    rd["relatid"]=DBINT(lRelateId);
    rd["operation"]=DBINT(iOp);
    rd["isavailable"]=DBINT(iAvailable);
    rd["update_time"]=DBDAT(TC_TimeProvider::getInstance()->getNow());
    string sSQL=getMysql(lUId)->buildInsertSQL(getPostOperTbName(lUId),rd);
    sSQL.append(" on duplicate key update isavailable="+TC_Common::tostr(iAvailable));

    TC_Mysql* mysql=getMysql(lUId);
    mysql->execute(sSQL);
    
    return mysql->getAffectedRows();
}

int DbHandle::postSecret(QUAN::SecretInfo &tSecret,SecretExtra &tExtra)
{
    int iRet=-1;

    try
    {
        TC_Mysql *mysql=getMysql(tSecret.lSId);

        int now=TC_TimeProvider::getInstance()->getNow();

        TC_Mysql::RECORD_DATA rd;
        rd["id"]=DBINT(tSecret.lSId);
        rd["uid"]=DBINT(tSecret.lUId);
        rd["content"]=DBSTR(tSecret.sContent);
        rd["colorid"]=DBINT(tSecret.iColorId);
        rd["biturl"]=DBSTR(tSecret.sBitUrl);
        rd["favor_num"]=DBINT(0);
        rd["cmt_num"]=DBINT(0);
        rd["report_num"]=DBINT(0);
        rd["location"]=DBSTR(tSecret.sLocal);
        rd["coord"]=DBSTR(tExtra.sCoord);
        rd["source"]=DBINT(0);
        rd["islock"]=DBINT(0);
        rd["isremove"]=DBINT(0);
        rd["update_time"]=DBDAT(now);
        rd["circleid"]=DBINT(tSecret.lCircleId);

        if(mysql->insertRecord(getPostTbName(tSecret.lSId),rd))
        {
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<JCETOSTR(tSecret)<<endl;
    }
    return iRet;
}

int DbHandle::postContent(ContentInfo &tContent)
{
    int iRet=-1;

    try
    {
        TC_Mysql *mysql=getMysql(tContent.lSId);

        TC_Mysql::RECORD_DATA rd;
        rd["id"]=DBINT(tContent.lCId);
        rd["pid"]=DBINT(tContent.lSId);
        rd["uid"]=DBINT(tContent.lUId);
        rd["avatarid"]=DBINT(tContent.iPortrait);
        rd["content"]=DBSTR(tContent.sContent);
        rd["favor_num"]=DBINT(0);
        rd["report_num"]=DBINT(0);
        rd["isremove"]=DBINT(0);
        rd["update_time"]=DBSTR(tContent.Time);

        //2.1
        if (tContent.sContent == "$[position]")
        {
            rd["isremove"]=DBINT(2);
        }

        mysql->insertRecord(getPostCommentTbName(tContent.lSId),rd);

        //2.1
        if (tContent.sContent != "$[position]")
        {
            if(updatePosts(tContent.lSId,"cmt_num","cmt_num+1"))
            {
                iRet=0;
            }
        }
        else
        {
            iRet=0;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<"|"<<JCETOSTR(tContent)<<endl;
    }
    return iRet;
}
int DbHandle::postSecretOp(taf::Int32 iType,taf::Int64 lUId,taf::Int64 lTargetUId,taf::Int64 lSId,taf::Int32 iOp)
{
    int iRet=-1;
    int affectedRows=0;

    try
    {
        switch(iType)
        {
            case SECRET:
                switch(iOp)
                {
                    case FAVORY:
                        if(updatePostsOp(lUId,SECRET,lSId,lTargetUId,0,1))
                        {
                            affectedRows=updatePosts(lSId,"favor_num","favor_num+1");
                        }
                        break;
                    case FAVORN:
                        if(updatePostsOp(lUId,SECRET,lSId,lTargetUId,0,0))
                        {
                            affectedRows=updatePosts(lSId,"favor_num","favor_num-1","favor_num>0");
                        }
                        break;
                    case REPORTY:
                        if(updatePostsOp(lUId,SECRET,lSId,lTargetUId,1,1))
                        {
                            affectedRows=updatePosts(lSId,"report_num","report_num+1");
                        }
                        break;
                    case REMOVEY:
                        affectedRows=updatePostsOp(lUId,SECRET,lSId,lTargetUId,2,1);
                        break;
                    case CLEANY:
                        updatePosts(lSId,"isremove","1");
                        break;
                    default:
                        throw std::runtime_error("Unknown Op Code:"+TC_Common::tostr(iOp));
                }
                break;
            case CONTENT:
                switch(iOp)
                {
                    case FAVORY:
                        if(updatePostsOp(lUId,CONTENT,lSId,lTargetUId,0,1))
                        {
                            affectedRows=updatePostsComment(lSId,"favor_num","favor_num+1");
                        }
                        break;
                    case FAVORN:
                        if(updatePostsOp(lUId,CONTENT,lSId,lTargetUId,0,0))
                        {
                            affectedRows=updatePostsComment(lSId,"favor_num","favor_num-1","favor_num>0");
                        }
                        break;
                    case REMOVEY:
                        affectedRows=updatePostsOp(lUId,CONTENT,lSId,lTargetUId,2,1);
                        break;
                    case CLEANY:
                        updatePostsComment(lSId,"isremove","1");
                        break;
                    default:
                        throw std::runtime_error("Unknown Op Code:"+TC_Common::tostr(iOp));
                }
                break;
            default:
                throw std::runtime_error("Unknown Type Code:"+TC_Common::tostr(iType));
        }
        iRet=affectedRows>0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::cleanTrail(taf::Int64 lUId)
{
    int iRet=-1;
    try
    {
        string sCondition=" where id=" +TC_Common::tostr(lUId);

        TC_Mysql::RECORD_DATA rd;
        rd["cleantrail_time"]=DBDAT(TC_TimeProvider::getInstance()->getNow());

        getUserMysql(lUId)->updateRecord(getUserTbName(lUId),rd,sCondition);
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::getComments(taf::Int64 lSId,vector<ContentInfo> &vContents)
{
    int iRet=-1;
    try
    {   
        vContents.clear();
        string sSql = " select id,uid,update_time,content,favor_num,avatarid,isremove "
            " from " +getPostCommentTbName(lSId)+ " where pid=" + TC_Common::tostr(lSId) + 
            " order by id asc";
        taf::TC_Mysql::MysqlData tRes=getMysql(lSId)->queryRecord(sSql);
        
        for(size_t i=0;i<tRes.size();++i)
        {
            ContentInfo tContent;
            tContent.lCId=TC_Common::strto<Int64>(tRes[i]["id"]);
            tContent.lSId=lSId;
            tContent.isRemove=TC_Common::strto<Int32>(tRes[i]["isremove"]);
            tContent.iPortrait=TC_Common::strto<Int32>(tRes[i]["avatarid"]);
            tContent.lUId=TC_Common::strto<Int64>(tRes[i]["uid"]);
            tContent.Time=tRes[i]["update_time"];
            if(tContent.isRemove==0)
            {
                tContent.sContent=tRes[i]["content"];
                tContent.iFavorNum=TC_Common::strto<Int32>(tRes[i]["favor_num"]);
            }
            vContents.push_back(tContent);
        }

        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::getCleanTrailTime(taf::Int64 lUId,taf::Int32 &iCleanTrailTime)
{
    int iRet=-1;
    try
    {
        string sSql = "select UNIX_TIMESTAMP(cleantrail_time) as cleantrail_time from "+getUserTbName(lUId)
            +" where id="+TC_Common::tostr(lUId);
        taf::TC_Mysql::MysqlData tRes=getUserMysql(lUId)->queryRecord(sSql);

        if(tRes.size()==1)
        {
            iCleanTrailTime=TC_Common::strto<Int32>(tRes[0]["cleantrail_time"]);
            iRet=0;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|"<<lUId<<"| RET SIZE:"<<tRes.size()<<endl;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::getSecret(taf::Int64 lSId,SecretInfo &tSecret)
{
    int iRet=-1;

    try
    {
        string sSql = "select uid,colorid,biturl,content,source,location,favor_num,cmt_num,report_num,isremove,islock,audit_status,shareid,circleid "
            " from " + getPostTbName(lSId) + " where id="  + TC_Common::tostr(lSId);

        taf::TC_Mysql::MysqlData tRes=getMysql(lSId)->queryRecord(sSql);

        if(tRes.size()==1)
        {
            tSecret.lSId=lSId;
            tSecret.isRemove=TC_Common::strto<Int32>(tRes[0]["isremove"]);
            tSecret.isRemove|=TC_Common::strto<Int32>(tRes[0]["islock"]);
            tSecret.isRead=false;
            if(tSecret.isRemove==0) //贴子未被移除,才加载完整信息cache
            {
                tSecret.lUId=TC_Common::strto<Int64>(tRes[0]["uid"]);
                tSecret.iColorId=TC_Common::strto<Int64>(tRes[0]["colorid"]);
                tSecret.sBitUrl=tRes[0]["biturl"];
                tSecret.sContent=tRes[0]["content"];
                tSecret.sLocal=tRes[0]["location"];
                tSecret.iFavorNum=TC_Common::strto<Int64>(tRes[0]["favor_num"]);
                tSecret.iCmtNum=TC_Common::strto<Int64>(tRes[0]["cmt_num"]);
                tSecret.lShareId=TC_Common::strto<Int64>(tRes[0]["shareid"]);
                tSecret.lCircleId=TC_Common::strto<Int64>(tRes[0]["circleid"]);
                int audit_status=TC_Common::strto<Int32>(tRes[0]["audit_status"]);
                if(audit_status==1)
                {
                    tSecret.iReportNum=0;
                }
                else if(audit_status==2)
                {
                    tSecret.isRemove=true;
                }
                else if(audit_status==3)
                {
                    tSecret.iReportNum=3;
                }
                else
                {
                    tSecret.iReportNum=TC_Common::strto<Int64>(tRes[0]["report_num"]);
                }
            }
            iRet=0;
        }
        else
        {
            LOG->error()<<__FUNCTION__<<"|"<<lSId<<"| RET SIZE:"<<tRes.size()<<endl;
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
}

int DbHandle::auditSecret(taf::Int64 lUId,taf::Int32 iLevel)
{
    int iRet=-1;
    try
    {
        updatePosts(lUId,"audit_status",TC_Common::tostr(iLevel));
        iRet=0;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<"|"<<ex.what()<<endl;
    }
    return iRet;
    
}

int DbHandle::getUserFavor(taf::Int64 lUId,std::map<taf::Int64,taf::Int32> &mpFavorList)
{
    int iRet=-1;
    try
    {
        mpFavorList.clear();
        string sSql = "select objectid,operation from "+ getPostOperTbName(lUId)+" where uid=" + TC_Common::tostr(lUId)
            + " and (operation=0 or operation=2) and isavailable=1 order by update_time desc limit 500";

        taf::TC_Mysql::MysqlData tRes = getMysql(lUId)->queryRecord(sSql);

        for(size_t i=0;i<tRes.size();++i)
        {
            taf::Int64 lSId=TC_Common::strto<Int64>(tRes[i]["objectid"]);
            taf::Int32 iOp=TC_Common::strto<Int32>(tRes[i]["operation"]);

            if(iOp==2)
            {
                mpFavorList[lSId]=iOp;     //移除贴子
            }
            else
            {
                mpFavorList.insert(make_pair(lSId,iOp));  //点赞
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

TC_Mysql* DbHandle::getMysql(Int64 lSId)
{
    return _mMysql[0];
}

TC_Mysql* DbHandle::getUserMysql(Int64 lUId)
{
    unsigned short uIndex = ((unsigned long long)lUId) %(_uDbNum * _uTbNum) /_uTbNum;
    return _mMysql[uIndex];
}

string DbHandle::getPostTbName(Int64 lSId)
{
    return _sPostTbNamePre;
}
string DbHandle::getPostOperTbName(Int64 lSId)
{
    return _sPostOperTbNamePre;
}
string DbHandle::getPostCommentTbName(Int64 lSId)
{
    return _sPostCommentTbNamePre;
}
string DbHandle::getUserTbName(Int64 lUId)
{
    unsigned short uIndex = ((unsigned long long)lUId) %(_uDbNum *_uTbNum) % _uTbNum;
    return _sTbUserInfoPre+TC_Common::tostr(uIndex);
}


