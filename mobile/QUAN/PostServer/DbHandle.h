#ifndef __SECRET_DBHANDLE_H_
#define __SECRET_DBHANDLE_H_
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "servant/Application.h"
#include "SecretBase.h"
#include "Base.h"

using namespace  QUAN;

/* callback of async proxy for client */


typedef map<unsigned short, TC_Mysql*>  DwDbMap;

struct SecretExtra
{
    std::string sCoord;
};

class DbHandle
{

public:
    DbHandle()          
    {
        init();
    }

    int init();

    int shareSecret(taf::Int64 lUId,taf::Int64 lShareId);

    int postSecret(SecretInfo &tSecret,SecretExtra &tExtra);
    int postContent(ContentInfo &tContent);
    int postSecretOp(taf::Int32 iType,taf::Int64 lUId,taf::Int64 lTargetUId,taf::Int64 lSId,taf::Int32 iOp);
    int cleanTrail(taf::Int64 lUId);
    int auditSecret(taf::Int64 lUId,taf::Int32 iLevel);

    int getSecret(taf::Int64 lSId,SecretInfo &tSecret);
    int getComments(taf::Int64 lSId,vector<ContentInfo> &vContents);
    int getUserFavor(taf::Int64 lUId,std::map<taf::Int64,taf::Int32> &mpFavorList);
    int getCleanTrailTime(taf::Int64 lUId,taf::Int32 &iCleanTrailTime);

    
    int updatePosts(taf::Int64 lSId,const std::string &updateField,const std::string &updateFieldTo,const std::string &condition="");
    int updatePostsComment(taf::Int64 lCId,const std::string &updateField,const std::string &updateFieldTo,const std::string &condition="");
    int updatePostsOp(taf::Int64 lUId,taf::Int32 iType,taf::Int64 lObjectId,taf::Int64 lRelateId,taf::Int32 iOp,taf::Int32 iAvailable);
private:

    TC_Mysql* getMysql(Int64 lSId);
    TC_Mysql* getUserMysql(Int64 lUId);
    string getPostTbName(Int64 lSId);
    string getPostOperTbName(Int64 lSId);
    string getPostCommentTbName(Int64 lSId);
    string getUserTbName(Int64 lUId);

private:

    DwDbMap         _mMysql;
    unsigned short  _uDbNum;
    unsigned short  _uTbNum;
    string          _sDbPre;
    string          _sTbUserInfoPre;    
    string          _sPostTbNamePre;
    string          _sPostOperTbNamePre;
    string          _sPostCommentTbNamePre;
};

class LockDbHandle:public DbHandle,public TC_ThreadLock,public TC_Singleton<LockDbHandle>
{
};

#endif                                             
