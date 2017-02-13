#ifndef NOTIFY_I_H
#define NOTIFY_I_H

#include "Notify.h"
#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "log/taf_logger.h"

using namespace taf;

class NotifyImp : public Notify
{
public:
    /**
     * ³õÊ¼»¯
     *
     * @return int
     */
    virtual void initialize();

    /**
     * ÍË³ö
     */
    virtual void destroy() {};

    /**
     * report
     * @param sServerName
     * @param sThreadId
     * @param sResult
     * @param current
     */
	virtual void reportServer(const string& sServerName,
									 const string& sThreadId,
									 const string& sResult,
									 taf::JceCurrentPtr current);

    /**
     * notify
     * @param sServerName
     * @param sThreadId
     * @param sCommand
     * @param sResult
     * @param current
     */
	virtual void notifyServer(const string& sServerName,
									 NOTIFYLEVEL level,
									 const string& sMessage,
									 taf::JceCurrentPtr current);

    /**
     * get notify info
     */
    virtual taf::Int32 getNotifyInfo(const taf::NotifyKey & stKey,taf::NotifyInfo &stInfo,taf::JceCurrentPtr current);

protected:
	void loadconf();

protected:
	TC_Mysql _mysqlConfig;
    size_t uMaxPageSize;
    size_t uMaxPageNum;

};

#endif
