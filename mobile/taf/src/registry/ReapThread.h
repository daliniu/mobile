#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__

#include <iostream>

#include "util/tc_thread.h"

#include "DbHandle.h"

using namespace taf;

/**
 * 用于执行定时操作的线程类
 */
class ReapThread : public TC_Thread, public TC_ThreadLock
{
public:

    ReapThread();

    ~ReapThread();

    /*
     * 结束线程
     */
    void terminate();

    /**
     * 初始化
     */
    int init();

    /**
     * 轮询函数
     */
    virtual void run();

protected:
    //线程结束标志
    bool _bTerminate;

    //数据库操作
    CDbHandle _db;

    //加载对象列表的时间间隔,单位是秒
    //第一阶段加载时间
    int _iLoadObjectsInterval1;
	int _iLeastChangedTime1;

	//全量加载时间,单位是秒
	int _iLoadObjectsInterval2;
	int _iLeastChangedTime2;

    //node心跳超时时间
    int _iNodeTimeout;

    //registry心跳超时时间
    int _iRegistryTimeout;

	//是否启用DB恢复保护功能，默认为打开
	bool _bRecoverProtect;

	//启用DB恢复保护功能状态下极限值
	int _iRecoverProtectRate;

	//主控心跳时间更新开关
	bool _bHeartBeatOff;
};

#endif
