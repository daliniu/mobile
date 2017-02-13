#ifndef __SERVERLIMITRESOURCE_H_
#define __SERVERLIMITRESOURCE_H_

#include <unistd.h>
#include <iostream>
#include "util/tc_monitor.h"
#include "util/tc_autoptr.h"

using namespace taf;
using namespace std;

class ServerLimitResource : public TC_ThreadLock, public TC_HandleBase
{
public:

	ServerLimitResource(int iMaxCount=3,int iTimeInterval=5/*分钟*/,int iExpiredTime=30/*分钟*/,const string& app="",const string& servername="");

	~ServerLimitResource(){}

public:
	void setLimitCheckInfo(int iMaxCount,int iTimeInterval,int iExpiredTime);
	/**
	 * @brief 服务异常重启记录
	 *
	 *1 如果在_iCoreLimitTimeInterval时间段内，则增加异常个数
	 *2 判断异常个数，如果在时间段内达到core限制_iMaxExcStopCount，则屏蔽core属性_bCloseCore
	 *  并记录当前时间戳_tEnableCoreLimitTime
	 */
	void addExcStopRecord();
	/**
	 *@brief 检查是否需要关闭或者打开core
	 *
	 *1 如果服务core被关闭，则先检查被关闭的时间戳，如果过期，且本服务没有配置manual关闭，则打开core属性
	 *2 检查core开关，如果true则关闭core，则异步通知服务关闭，否则打开开关
	 *
	 */
	int IsCoreLimitNeedClose(bool& bClose);

private:
	bool    _bCloseCore;  //屏蔽core开关
	int 	_iCurExcStopCount;  //当前服务异常停止个数
	int 	_iMaxExcStopCount;  //最大服务异常停止个数
	time_t  _iExcStopRecordTime;//第一次异常停止时间
	int32_t _iCoreLimitTimeInterval; //时间间隔内core的限制，单位是分钟
	time_t  _tEnableCoreLimitTime; //上一次关闭core的时间
	int32_t  _iCoreLimitExpiredTime;//core关闭的持续时间,单位为分钟
	string _sAppname;
	string _sServername;
};

typedef TC_AutoPtr<ServerLimitResource> ServerLimitResourcePtr;

#endif


