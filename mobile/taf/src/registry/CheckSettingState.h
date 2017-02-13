#ifndef __CHECKSETINGSTATE_H__
#define __CHECKSETINGSTATE_H__

#include <iostream>

#include "util/tc_thread.h"
#include "Node.h"
#include "DbHandle.h"

using namespace taf;

/**
 * 用于执行定时操作的线程类
 */
class CheckSettingState : public TC_Thread, public TC_ThreadLock
{
public:

    CheckSettingState();

    ~CheckSettingState();

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
private:
    //线程结束标志
    bool _bTerminate;

    //数据库操作
    CDbHandle _db;

    //轮询server状态的间隔时间
    int _iCheckingInterval;

    //增量查询服务状态的时间,单位是秒
    int _iLeastChangedTime;
};

#endif
