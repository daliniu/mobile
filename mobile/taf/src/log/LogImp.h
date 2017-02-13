#ifndef _LOG_IMP_H
#define _LOG_IMP_H

#include "Log.h"
#include "util/tc_common.h"
#include "util/tc_file.h"
#include "util/tc_logger.h"
#include "util/tc_monitor.h"
#include "LogServer.h"

using namespace taf;

/**
 * 全局信息
 */
struct GlobeInfo : public TC_ThreadLock
{
public:
    /**
     * 获取按天日志
     * @param app
     * @param server
     * @param file
     * @param format
     * @param ip 服务所在节点ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger& getLogger(const string &app, const string &server, const string &file, const string &format,const string& ip);
    /**
     * 获取按天日志
     * @param info 详情参看LogInfo
     * @param ip 服务所在节点ip
     *
     * @return TC_DayLogger&
     */
	TC_DayLogger& getLogger(const LogInfo & info,const string& ip);
protected:
    /**
     * 生成按天日志
     * @param info 详情参看LogInfo
     * @param logname
     * @param format
     * @param ip 服务所在节点ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger &makeDayLogger(const LogInfo & info, const string &logname, const string &format,const string& ip);

	/**
     * 生成按天日志
     * @param app
     * @param server
     * @param logname
     * @param format
     * @param ip 服务所在节点ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger &makeDayLogger(const string &app, const string &server, const string &logname, const string &format,const string& ip);

	/**
     * 判断相应的logname下是否有指定的format
     * @param logname 由file.empty() ? (app + "." + server) : (app + "." + server + "_" + file)方式组成
     * @param format
     * @param[out] ip 如果有指定的format存在，则返回该format对应的节点ip
     *
     * @return bool
     */
    bool HasSameFormat(const string& logname,const string& format,string& ip);
	/**
     * 判断在logname下，除了节点sExcludeIp外，是否还有其它节点在使用pLoger指定的logger实例
     * @param logname
     * @param sExcludeIp
     * @param pLogger
     *
     * @return bool
     */
	bool IsLoggerAttached(const string& logname,const string& sExcludeIp,const TC_DayLogger* pLoger);
private:
	/**
	 * 获取set完整格式
	 * @param sSetDivision set分组信息,例如:mtt.s.1 mtt.s.*
	 *
	 * @return 规整后的格式，例如:mtt.s.* 去掉"*"符号，返回mtts；mtts1则原样返回
	 */
	string getSetGoodFormat(const string& sSetDivision);
	/**
	 * 从LogInfo中获取相关信息得到标识一个logger对象的字符串
	 * @param info
	 */
	string getLogName(const LogInfo & info);

	/**
	 * 从LogInfo中获取日志文件名称，包括路径名在内
	 * @param info
	 */
	string getRealLogName(const LogInfo & info);

public:
    /**
     * 写日志线程
     */
    TC_LoggerThreadGroup        _group;

    /**
     * 写日志路径
     */
    string                      _log_path;

    /**
     * logger对象:logname,ip,logger
     */
    map<string, map<string,TC_DayLogger*> >  _loggers;

    /**
     * logger格式:logname,ip,format
     */
    map<string, map<string,string> >        _formats;

	vector<string> _vHourlist;
	
	/**
     * app idc关系:MDWWX,MDW
     */	 
    map<string,string> _mAppIdcRelation;

};

extern GlobeInfo   g_globe;

/**
 * log实现
 */
class LogImp : public Log
{
public:
    /**
     *
     */
    LogImp(){};

    /**
     *
     */
    ~LogImp(){};

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() {};

    /**
     * 输出日志信息到指定文件
     * @param app    业务名称
     * @param server 服务名称
     * @param file   日志文件名称
     * @param format 日志输出格式
     * @param buffer 日志内容
     *
     *
     */
    void logger(const string &app, const string &server, const string &file, const string &format, const vector<string> &buffer, taf::JceCurrentPtr current);
    /**
     * 获取数据
     * @param info
     * @param buffer
     *
     */
	void loggerbyInfo(const LogInfo & info,const vector<std::string> & buffer,taf::JceCurrentPtr current);
protected:

    /**
     * 加载配置文件中日志format format与客户端不一致时
     * 以logserver配置文件中格式为准
     */

    //bool loadLogFormat(const string& command, const string& params, string& result);
    /**
     * 检查日志路径
     * @param app
     * @param server
     */
    //void checkLogPath(const string &app, const string &server);

private:

    //vector<string> _vHourlist;
};

#endif

