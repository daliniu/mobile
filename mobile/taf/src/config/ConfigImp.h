#ifndef _CONFIG_IMP_H_
#define _CONFIG_IMP_H_

#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "log/taf_logger.h"

#include "Config.h"


using namespace taf;
enum Level
{
    eLevelApp = 1,
    eLevelAllServer,
    eLevelIpServer
};

class ConfigImp : public Config
{
public:
    /**
     *
     */
    ConfigImp(){};

    /**
     *
     */
    ~ConfigImp(){};

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
     * 获取配置文件列表 
     * param app :应用 
     * param server:  server名 
     * param vf: 配置文件名
     *
     * return  : 配置文件内容
     */
    virtual int ListConfig(const string &app,const string &server,  vector<string> &vf,taf::JceCurrentPtr current);

    /**
     * 加载配置文件
     * param app :应用
     * param server:  server名
     * param filename:  配置文件名
     *
     * return  : 配置文件内容
     */
    virtual int loadConfig(const std::string & app, const std::string & server, const std::string & filename, string &config, taf::JceCurrentPtr current);

    /**
     * 根据ip获取配置
     * @param appServerName
     * @param filename
     * @param host
     * @param config
     *
     * @return int
     */
    virtual int loadConfigByHost(const string &appServerName, const string &filename, const string &host, string &config, taf::JceCurrentPtr current);

    /**
     *
     * @param appServerName
     * @param filename
     * @param host
     * @param current
     *
     * @return int
     */
    virtual int checkConfig(const string &appServerName, const string &filename, const string &host, string &result, taf::JceCurrentPtr current);

	 /**
     * 获取配置文件列表
     * param configInfo ConfigInfo
     * param vf: 配置文件名
     *
     * return  : 配置文件内容
     */
	virtual int ListConfigByInfo(const ConfigInfo& configInfo,  vector<string> &vf,taf::JceCurrentPtr current);
    /**
     * 加载配置文件
     * param configInfo ConfigInfo
     * param config:  配置文件内容
     *
     * return  :
     */
	virtual int loadConfigByInfo(const ConfigInfo & configInfo, string &config,taf::JceCurrentPtr current);
    /**
     *
     * @param configInfo ConfigInfo
     *
     * @return int
     */
    virtual int checkConfigByInfo(const ConfigInfo & configInfo, string &result,taf::JceCurrentPtr current);

protected:
    /**
     * 加载本server的配置文件
     */
    void loadconf();

private:

    /**
     *
     * 查询configId对应的配置
     *
     * @param iConfigId
     * @param sConfig
     *
     * @return int 0，成功或者无此config,表示应用级配置
     *             2,表示set应用配置
     */
    int loadConfigByPK(int iConfigId, string &sConfig);
    /**
     *
     * 查询configId对应的配置信息，同时会查看该
     *
     * @param iConfigId
     * @param setdivision set分组信息
     * @param sConfig
     *
     * @return int 0，成功
     *
     */
    int loadRefConfigByPK(int iConfigId, const string& setdivision,string &sConfig);
    /**
     * 
     * 获取iConfigId对应的引用id列表
     * 
     * @param iConfigId 
     * @param referenceIds 
     * 
     * @return int 
     */
    int getReferenceIds(int iConfigId,vector<int> &referenceIds);


    /**
     * 获取应用级公共配置
     * @param appName  应用名称
     * @param fileName 文件名称
     * @out param      获取到的配置文件内容
     *
     * @return int 
     */
    int loadAppConfig(const std::string& appName, const std::string& fileName, string &config, taf::JceCurrentPtr current);

    /**
     * 获取应用级公共配置
     * @param configInfo  ConfigInfo
     * @out param      获取到的配置文件内容
     *
     * @return int
     */
    int loadAppConfigByInfo(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current);
    /**
     * 根据ip获取配置
     * @param configInfo  ConfigInfo；configInfo中指定ip，如果没有则从current获取当前请求的ip
     * @param config
     *
     * @return int
     */
	int loadConfigByHost(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current);

    /**
     * 获取set分组信息
     * @out param sSetName  set名称
     * @out param sSetArea  set地区名称
     * @out param sSetGroup set分组名称
     * @param  sSetDivision   set信息:sSetName.sSetArea.sSetGroup格式
     *
     * @return bool set信息正确则返回true
     */
	bool getSetInfo(string& sSetName,string& sSetArea,string& sSetGroup,const string& sSetDivision);

	/**
	 * 合并两个配置文件,如果有冲突项,则sHighConf优先级比sLowConf高
	 *
	 * @param sLowConf  应用名称
	 * @param sHighConf  应用名称
	 *
	 * @ return string 合并的结果
	 */
	string mergeConfig(const string& sLowConf,const string& sHighConf);

protected:
    TC_Mysql _mysqlConfig;
};

#endif

