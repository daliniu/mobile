

#ifndef __UserInfoCallback_H_
#define __UserInfoCallback_H_
#include "UserInfo.h"
#include "servant/Application.h"
#include "util/tc_timeprovider.h"
#include "Base.h"
#include "GetSecret.h"


using namespace  QUAN;

class PushSecretInfo:public TC_HandleBase
{
public:
    UserSecretCacheReq tUserSecretCacheReq;
};

typedef TC_AutoPtr<PushSecretInfo> PushSecretInfoPtr;


/* callback of async proxy for client */
class UserInfoCallback: public UserInfoPrxCallback
{

public:
    UserInfoCallback(taf::JceCurrentPtr current,
                     const UserId &tUserId,
                     UserInfoPrx pUserProxy,
                     GetSecretPrx  pSecretProxy)
    {
        _tCurrent           = current;
        _tUserId            = tUserId;
        _pUserProxy         = pUserProxy;
        _pSecretProxy       = pSecretProxy;

    }

    void callback_getActiveFollowers(taf::Int32 ret,  const QUAN::GetActiveFollowersRsp& tRsp);
    void callback_getActiveFollowers_exception(taf::Int32 ret);

public:

    void setCallerLog(const string&sLog)
    {   
        _sLog       = sLog;
    }

    void setPushSecretInfo(const PushSecretInfoPtr &pPushSecretInfoPtr);

    void updatePostTime(const vector<FriendsList> &vFollowers);

    void pushPostSecret(const vector<FriendsList> &vFollowers);

private:
    UserId              _tUserId;
    UserInfoPrx         _pUserProxy;
    GetSecretPrx        _pSecretProxy;
    string              _sLog;
    taf::JceCurrentPtr  _tCurrent;

private:
    PushSecretInfoPtr   _pPushSecretInfoPtr;
};
typedef taf::TC_AutoPtr<UserInfoCallback> UserInfoCallbackPtr;

#endif
