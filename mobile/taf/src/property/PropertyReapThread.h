#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "servant/PropertyF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include "PropertyImp.h"


using namespace taf;

extern TC_Config* g_pconf;



/**
 * 用于执行定时操作的线程类
 */
class PropertyReapThread : public TC_Thread,public TC_ThreadLock
{
public:
     /**
     * 定义常量
     */
    enum
    {
        REAP_INTERVAL = 10000, /**轮训入库间隔时间**/
    };

    /**
     * 构造
     */
    PropertyReapThread();

    /**
     * 析够
     */
    ~PropertyReapThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();

private:


    void getData(const string &sCloneFile,map<PropHead,PropBody> &mStatMsg, vector<map<PropHead,PropBody> > &mAllStatMsg);

private:
	bool _bTerminate;
    //mysql连接对象
    TC_Mysql _mysql;
};

#endif
