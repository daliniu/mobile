#ifndef __STAT_IMP_H_
#define __STAT_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include "StatQueryImp.h"
#include "StatHashMap.h"


using namespace taf;

class StatImp : public StatF,public TC_ThreadLock
{
public:
    /**
     *
     */
    StatImp()
    {
    };
    ~StatImp()
    {
    };

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy()
    {
    };


    /**
    * udp上报模块间调用信息
    * @param statmsg, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int reportMicMsg( const map<taf::StatMicMsgHead, taf::StatMicMsgBody>& statmsg,
                              bool bFromClient,
                              taf::JceCurrentPtr current );
    /**
    * udp上报模块间调用采样信息
    * @param sample, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int reportSampleMsg(const vector<StatSampleMsg> &msg,taf::JceCurrentPtr current );

protected:

    int addHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body);

    int addQueryHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body);

private:
    void dump2file();
    string getSlaveName(const string& sSlaveName);
	/**
	 * 针对taf服务调用第三方taserver服务时的被调名进行处理
	 */
	string getNameFromTaServerName(const string& sSlaveName);
};

#endif


