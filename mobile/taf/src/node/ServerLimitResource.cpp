#include "ServerLimitResource.h"
#include "util/tc_timeprovider.h"
#include "log/taf_logger.h"

ServerLimitResource::ServerLimitResource( int iMaxCount,int iTimeInterval/*分钟*/ ,int iExpiredTime/*分钟*/,const string& app,const string& servername)
:_bCloseCore(false)
,_iCurExcStopCount(0)
,_iMaxExcStopCount(iMaxCount)
,_iExcStopRecordTime(0)
,_iCoreLimitTimeInterval(iTimeInterval*60)
,_tEnableCoreLimitTime(0)
,_iCoreLimitExpiredTime(iExpiredTime*60)
,_sAppname(app)
,_sServername(servername)
{
}
void ServerLimitResource::setLimitCheckInfo(int iMaxCount,int iTimeInterval,int iExpiredTime)
{
	TC_ThreadLock::Lock lock(*this);
	_iMaxExcStopCount = (iMaxCount>0)?iMaxCount:1;
	_iCoreLimitTimeInterval = (iTimeInterval>0?iTimeInterval:1)*60;
	_iCoreLimitExpiredTime = (iExpiredTime>0?iExpiredTime:1)*60;
}

void ServerLimitResource::addExcStopRecord()
{
	time_t tNow 	= TC_TimeProvider::getInstance()->getNow();

	TC_ThreadLock::Lock lock(*this);
	if(_iCurExcStopCount > 0)
	{
		//在允许的时间段内才检查是否增加计数和关闭core属性
		if((tNow - _iExcStopRecordTime) < _iCoreLimitTimeInterval)
		{
			_iCurExcStopCount++;
			if(!_bCloseCore && (_iCurExcStopCount >= _iMaxExcStopCount))
			{
				_bCloseCore = true;
				_tEnableCoreLimitTime = tNow;
			}
		}
		else
		{
			//重新计数
			_iCurExcStopCount = 1;
			_iExcStopRecordTime = tNow;
		}
	}
	//第一次异常重启，记录
	else
	{
		_iCurExcStopCount++;
		_iExcStopRecordTime = tNow;
	}
}

int ServerLimitResource::IsCoreLimitNeedClose(bool& bClose)
{
	time_t tNow 	= TC_TimeProvider::getInstance()->getNow();

	LOG->debug() <<__FUNCTION__<<"|"<<(_sAppname+"."+_sServername)<<"|"<<_bCloseCore<<"|"<< _iCurExcStopCount<<"|"<<tNow<<"|"<<_tEnableCoreLimitTime<< endl;
	if(_bCloseCore)
	{
		if((tNow -_tEnableCoreLimitTime) < _iCoreLimitExpiredTime)
		{
			bClose = true;
			return 1;
		}
		//如果core屏蔽过期，则重新打开
		else
		{
			TC_ThreadLock::Lock lock(*this);
			LOG->debug()<<"IsCoreLimitNeedClose expired|"<<(_sAppname+"."+_sServername)<<"|"<<tNow<<"|"<<_tEnableCoreLimitTime<<endl;
			_iCurExcStopCount = 0;
			_bCloseCore = false;
			_iExcStopRecordTime = 0;
			_tEnableCoreLimitTime = 0;
			bClose = false;
			return 2;
		}
	}
	else
	{
		bClose = false;
		return 1;
	}
}

