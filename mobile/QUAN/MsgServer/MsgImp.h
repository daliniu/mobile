#ifndef _MsgImp_H_
#define _MsgImp_H_

#include "servant/Application.h"
#include "Base.h"
#include "UserInfo.h"
#include "Secret.h"
#include "Post.h"
#include "Msg.h"
#include "Lbs.h"
#include "Circle.h"
#include "PushProxy.h"
#include "DbHandle.h"
#include "redisclient.h"

using namespace QUAN;
using namespace MDW;

/**
 *
 *
 */
class MsgImp : public QUAN::Msg
{
public:
	/**
	 *
	 */
	virtual ~MsgImp() {}

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

        virtual taf::Int32 getPushMsg(const QUAN::PushMsgReq& tReq, QUAN::PushMsgRsp& tRsp, taf::JceCurrentPtr current);
    
        virtual taf::Int32 addMsg(const QUAN::AddMsgReq& tReq, taf::JceCurrentPtr current);
    
        virtual taf::Int32 changeMsgStatus(const QUAN::ChangeMsgReq & tReq,taf::JceCurrentPtr current);

        virtual taf::Int32 getMsgNum(const QUAN::GetMsgNumReq & tReq,QUAN::GetMsgNumRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 atMsg(const QUAN::AtMsgReq & tReq,taf::JceCurrentPtr current);

        virtual taf::Int32 bindMsg(const QUAN::BindMsgReq & tReq,taf::JceCurrentPtr current);

        virtual taf::Int32 cleanCache(const vector<Int64> & vUserId,taf::JceCurrentPtr current);

        virtual taf::Int32 getSysMsg(const QUAN::GetSysMsgReq & tReq,QUAN::GetSysMsgRsp &tRsp,taf::JceCurrentPtr current);

        virtual taf::Int32 pushSysMsg(const QUAN::PushSysMsgReq & tReq,taf::JceCurrentPtr current);
		virtual	taf::Int32 cleanSysCache(taf::Int64 tUserId, taf::JceCurrentPtr current);
                virtual taf::Int32 getNewSysMsg(const QUAN::GetNewSysMsgReq & tReq,QUAN::GetNewSysMsgRsp &tRsp,taf::JceCurrentPtr current);
        virtual taf::Int32 pushImMsg(const PushImMsgReq& tReq, taf::JceCurrentPtr current);
        virtual taf::Int32 pushBindMsg(const QUAN::UserId & tUserId, Int32 iType, string const& sId, taf::JceCurrentPtr current);
private:
        taf::Int32 getMsgContent(const QUAN::PushMsgReq& tReq,QUAN::PushMsgRsp &tRsp, map<Int64, MsgRmd> const& mSIds);
        taf::Int32 addFrdsRelatForMsg(const QUAN::PushMsgReq& tReq,QUAN::PushMsgRsp &tRsp, map<Int64, MsgRmd> const& mSIds, map<Int64, FriendsList> const& tFrdsMap);
		taf::Int32 pushMsg(UserId const& tUserId, Int64 lSId, Int64 lFirstId, Int32 iOp, string const& sIsSys, taf::JceCurrentPtr current, Int64 senderId = 0, Int64 postId = 0, Int64 commentId = 0, char const* sContent = "", char const* sSource = "", char const* sBadge = "", char const* sSmallpicurl="");
		taf::Int32 getNearSecrets(PushMsgReq const& tReq, map<Int64, Int64>& vMaps);
		taf::Int32 getCirclesList(PushMsgReq const& tReq, map<Int64, CircleInfo>& mCircles);
		int getImNum(UserId const& tUserId, Int32& iNum);
private:
	int connectReddis();
	string tokenServerHost;
	int tokenServerPort;
	redis::client *rclient;
private:

        UserInfoPrx         _pUserProxy;
        PostPrx             _pPostProxy;
		PushProxyPrx        _pPushProxy;
		LbsPrx              _pLbsProxy;
		CirclePrx           _pCircleProxy;
        DbHandle            _tDbHandle;
};
/////////////////////////////////////////////////////
#endif
