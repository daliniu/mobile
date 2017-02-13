#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"


using namespace taf;

/**
 * 用于执行定时操作的线程类
 */
class DBThread : public TC_Thread,public TC_ThreadLock
{
public:
     /**
     * 定义常量
     */
    enum
    {
        REAP_INTERVAL = 30000
    };
    /**
     * 构造
     */
    DBThread();

    /**
     * 析够
     */
    ~DBThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();

	void tryConnect();

	int connect(MYSQL **_pstMql,const TC_DBConf& tcConf);

private:
    bool _bTerminate;

};

#endif
