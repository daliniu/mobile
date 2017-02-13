#ifndef _UserInfoImp_H_
#define _UserInfoImp_H_

#include "servant/Application.h"
#include "UserInfo.h"
#include "DbHandle.h"
#include "redisclient.h"

using namespace QUAN;

/**
 *
 *
 */
class UserInfoImp : public QUAN::UserInfo
{
public:
	/**
	 *
	 */
	virtual ~UserInfoImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	/**
	 *
	 */
	virtual int test(taf::JceCurrentPtr current) { return 0;};

    virtual taf::Int32 postUserEnroll(const QUAN::UserEnrollReq& tReq, QUAN::UserEnrollRsp& tRsp, taf::JceCurrentPtr current);
    virtual taf::Int32 postUserLogin(const QUAN::UserLoginReq& tReq, QUAN::UserLoginRsp& tRsp, taf::JceCurrentPtr current);
    virtual taf::Int32 postAddressList(const QUAN::PostAddressListReq& tReq, QUAN::PostAddressListRsp& tRsp, taf::JceCurrentPtr current);
    virtual taf::Int32 getFriendsList(const QUAN::FriendsListReq& tReq, QUAN::FriendsListRsp& tRsp, taf::JceCurrentPtr current);
    virtual taf::Int32 verifyToken(const QUAN::UserId & tId,taf::Int64 &lUId,taf::JceCurrentPtr current);
    virtual taf::Int32 updatePostTime(const QUAN::UpdatePostTimeReq & tReq,taf::JceCurrentPtr current);
    virtual taf::Int32 updateFavorTime(const QUAN::UpdateFavorTimeReq & tReq,taf::JceCurrentPtr current);
    virtual taf::Int32 getActiveFriends(const QUAN::GetActiveFriendsReq & tReq,QUAN::GetActiveFriendsRsp &tRsp,taf::JceCurrentPtr current);
    virtual taf::Int32 getActiveFollowers(const QUAN::GetActiveFollowersReq & tReq,QUAN::GetActiveFollowersRsp &tRsp,taf::JceCurrentPtr current);
    virtual taf::Int32 setActive(const QUAN::SetActiveReq & tReq,taf::JceCurrentPtr current);
    virtual taf::Int32 clearUserCache(taf::Int64 lUId,taf::JceCurrentPtr current);

private:
    int connectReddis();
    string tokenServerHost;
    int tokenServerPort;
    redis::client *rclient;
    bool isVerifyToken;
    

    DbHandle    _tDbHandle;
    int         MaxFollowerCount;
    int         MaxFriendCount;
};
/////////////////////////////////////////////////////
#endif
