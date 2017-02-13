#ifndef __StatQuery_IMP_H_
#define __StatQuery_IMP_H_

#include "StatQuery.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "StatQueryHashMap.h"
#include "StatDbManager.h"


using namespace taf;


class StatQueryImp : public StatQuery
{
public:
    /**
     *
     */
    StatQueryImp()
    {
    };
    ~StatQueryImp()
    {
    };

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize()
    {
        cout << "ok" << endl;
    };

    /**
     * 退出
     */
    virtual void destroy()
    {
    };

    /**
    * udp上报模块间调用信息
    * @param StatQuery, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int query(const StatQueryKey& tKey,bool bMaster,StatQueryValue &tValue, taf::JceCurrentPtr current );

    /**
    * 获取模块数据存储db信息
    * @param string, 查询模块名
    * @param DbInf, db信息
    * @return int, 返回0表示成功
    */
    virtual int queryDbInf(const string &sModule, Db &tDb, taf::JceCurrentPtr current );
protected:
};

#endif


