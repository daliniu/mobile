#ifndef __REAP_SSD_THREAD_H__
#define __REAP_SSD_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include "StatImp.h"
#include "StatDbManager.h"

using namespace taf;

/**
 * 用于执行定时操作的线程类
 */
class ReapSSDThread : public TC_Thread,public TC_ThreadLock
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
    ReapSSDThread();

    /**
     * 析够
     */
    ~ReapSSDThread();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 轮询函数
     */
    virtual void run();

private:
    void getData(const string &sCloneFile,vector<StatMsg> &vAllStatMsg);

	/**
	 * 通过权重轮询调度算法获取要插入数据的db index
	 * @param iMaxDb db个数
	 * @param iGcd  所有权重的最大公约数
	 * @param iMaxW 最大权重值
	 * @param vDbWeight 所有db的权重值
	 *
	 * @return int
	 */
	int getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight);

private:
	bool _bTerminate;
	int _iCurWeight;
	int _iLastSq;
};

#endif
