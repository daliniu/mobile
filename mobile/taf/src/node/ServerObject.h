#ifndef __SERVER_OBJECT_H_
#define __SERVER_OBJECT_H_
#include "Node.h"
#include "Registry.h"
#include <unistd.h>
#include "Activator.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "servant/AdminF.h"
#include "nodeF/NodeF.h"
#include "log/taf_logger.h"
#include "PlatformInfo.h"
#include "PropertyReporter.h"
#include "ServerLimitResource.h"
using namespace taf;
using namespace std;

class ServerObject : public TC_ThreadRecLock, public TC_HandleBase
{

public:
    enum InternalServerState
    {
        Inactive,
        Activating,
        Active,
        Deactivating,
        Destroying,
        Destroyed,
        Loading,
        Patching,
        BatchPatching,
        AddFilesing
    };
	enum EM_CoreType
	{
		EM_AUTO_LIMIT,
		EM_MANUAL_LIMIT
	};
	/**
	 *@brief 服务进程的limit资源状态，比如core属性资源
	 *
	 *服务的corelimit初始状态默认为EM_AUTO_LIMIT+false
	 */
    struct ServerLimitInfo
    {
		bool    bEnableCoreLimit;  		   //资源属性开关
		bool    bCloseCore;                //core属性的状态，屏蔽:true,打开:false
		EM_CoreType eCoreType;             //服务core的当前屏蔽方式
		int 	iMaxExcStopCount;         //最大服务异常停止个数
		int32_t iCoreLimitTimeInterval;  //时间间隔内core的限制，单位是分钟
		int32_t iCoreLimitExpiredTime;   //core关闭的持续时间,单位为分钟
		ServerLimitInfo():bEnableCoreLimit(false),bCloseCore(false),eCoreType(EM_AUTO_LIMIT),
								iMaxExcStopCount(3),iCoreLimitTimeInterval(5),iCoreLimitExpiredTime(30)
		{
		}
    };

public:

    /**
     * 构造函数
     * @param tDesc  server描述结构
     */
    ServerObject( const ServerDescriptor& tDesc);

    /**
     * 析够
     */
    ~ServerObject() { };

    /**
     * 是否是taf服务
     *
     * @return bool
     */
    bool isTafServer() {return _bTafServer;}

    /**
     * 是否是enable
     *
     * @return bool
     */
    bool isEnabled() {return _bEnabled;}

    /**
    * 服务是否自动启动
    *,用来防止问题服务不断重启影响其它服务
    * @return  bool
    */
    bool isAutoStart() ;

    /**
     * 设置服务enable
     * @param enable
     */
    void setEnabled(bool enable){_bEnabled = enable; }

     /**
     * 设置服务已patch
     * @param enable
     */
    void setPatched(bool bPatched)
    {
        _bPatched               = bPatched;
        _tPatchInfo.iPercent    = 100;
    }

      /**
      *设置服务已load
     * @param enable
     */
    void setLoaded(bool bLoaded){_bLoaded = bLoaded;}

	/**
	 *是否允许同步服务的状态到主控中心，在某些情况下，服务发布了，但是还不想对外提供服务
	 *则可以设置这个值
	 *@param  bEn
	 */
	void setEnSynState(bool bEn) { _bEnSynState = bEn;}

	bool IsEnSynState(){ return _bEnSynState;}

public:

    /**
    * 验证server对应pid是否存在
    * @return  int
    */
    int checkPid();

    /**
    * 设置server对应pid
    */
    void setPid( pid_t pid );

    /**
    * 获取server对应pid
    */

    int getPid() { return _pid; }


    /**
    * 获取server对应本地socket
    */

    TC_Endpoint getLocalEndpoint() { return _tLocalEndpoint; }


    /**
    * 设置server对应本地socket
    */

    void setLocalEndpoint(const TC_Endpoint &tLocalEndpoint) {  _tLocalEndpoint = tLocalEndpoint; }


    /**
    * 获取server上报信息
    * @para string obj
    * @para pid_t pid上报pid
    * @return void
    */
    void keepAlive( pid_t pid, const string &adapter="");

    /**
    * 取的server最近keepAlive时间
    * @return int
    */
    int getLastKeepAliveTime(const string &adapter="");

    /**
    * 设置server最近keepAlive时间
    * @return void
    */
    void setLastKeepAliveTime(int t,const string &adapter="");

    /**
     * 服务是否已timeout
     * @param iTimeout
     * @return bool
     */
    bool isTimeOut(int iTimeout);

public:

    /**
     * 跟registry同步server当前状态
     * @return void
     */
    void synState();

    /**
     * 异步跟registry同步server当前状态
     * @return void
     */
    void asyncSynState();

    /**
     * 设置server当前状态
     * @param eState
     * @param bSynState  是否同步server状态到registry
     * @return  void
     */
    void setState( InternalServerState eState, bool bSynState=true);

    /**
    *设置server上一次状态，当server状态发生变化时调用
    */
    void setLastState(InternalServerState eState);

    /**
     * 获取服务状态
     *
     * @return taf::ServerState
     */
    taf::ServerState getState();

    /**
     * 获取server内部状态
     * @return void
     */
    InternalServerState getInternalState() { return  _eState; }

    /**
    *获取server上一次内部状态
    *@return InternalServerState
    */
    InternalServerState getLastState() {return _eLastState;}

    /**
    *转换指定server内部状态为字符串形式
    * @para  eState
    * @return string
    */
    string toStringState( InternalServerState eState ) const;

    /**
    *转换指定server内部状态为one::ServerState形式
    * @para  eState
    * @return ServerState
    */
    ServerState toServerState( InternalServerState eState ) const;

    /**
    *监控server状态
    * @para  iTimeout 心跳超时时间
    * @return void
    */
    void checkServer(int iTimeout);

    /**
    *设置脚本文件
    * @para tConf 脚本文件
    * @return void
    */
    string decodeMacro(const string& value) const ;

public:

    /**
    *是否为脚本文件
    * @para sFileName 文件名
    * @return bool
    */
    bool isScriptFile(const string &sFileName);

    /**
    * 拉取脚本文件
    * @para sFileName 文件名
    * @return bool
    */
    bool getRemoteScript(const string &sFileName);

    /**
    *执行监控脚本文件
    * @para sFileName 文件名
    * @return void
    */
    void doMonScript();

public:
    /**
    *s设置下载百分比
    * @para iPercent 文件已下载百分比
    * @return bool
    */
    void setPatchPercent(const int iPercent);

    /**
    *s设置下载结果
    * @para sPatchResult
    * @return bool
    */
    void setPatchResult(const string &sPatchResult,const bool bSucc = false);

    /**
    *s设置下载版本
    * @para sPatchResult
    * @return bool
    */
    void setPatchVersion(const string &sVersion);

    /**
     * 获取正在下载的版本号
     *
     * @author kevintian (2010-9-21)
     *
     * @return string
     */
    string getPatchVersion();

    /**
    * 获取下载信息
    * @para tPatchInfo
    * @return int
    */
    int getPatchPercent(PatchInfo &tPatchInfo);

public:
    ServerDescriptor getServerDescriptor() { return  _tDesc; }
    ActivatorPtr getActivator() { return  _pActivatorPtr; }
    string getExePath(){return _sExePath;}
    string getExeFile(){return _sExeFile;}
    string getConfigFile(){return _sConfFile;}
    string getLogPath(){return _sLogPath;}
    string getLibPath(){return _sLibPath;}
    string getServerDir(){return _sServerDir;}
    string getServerId(){return _sServerId;}
    string getServerType(){return _sServerType;}
    string getStartScript() {return _sStartScript;}
    string getStopScript() {return _sStopScript;}
    string getMonitorScript() {return _sMonitorScript;}
    string getEnv() { return _sEnv; }
	string getRedirectPath() {return _sRedirectPath;}

    //java服务
    string getJvmParams() {return _sJvmParams;}
    string getMainClass() {return _sMainClass;}
    string getClassPath() {return _sClassPath;}
	string getBackupFileNames(){return _sBackupFiles;}

    void setServerDescriptor( const ServerDescriptor& tDesc );
    void setVersion( const string &version );
    void setExeFile(const string &sExeFile){_sExeFile = sExeFile;}
    void setExePath(const string &sExePath){_sExePath = sExePath;}
    void setConfigFile(const string &sConfFile){_sConfFile = sConfFile;}
    void setLogPath(const string &sLogPath){_sLogPath = sLogPath;}
    void setLibPath(const string &sLibPath){_sLibPath = sLibPath;}
    void setServerDir(const  string &sServerDir){_sServerDir = sServerDir;}
    void setNodeInfo(const NodeInfo &tNodeInfo){_tNodeInfo = tNodeInfo;}
    void setServerType( const string &sType ){ _sServerType = TC_Common::lower(TC_Common::trim(sType));_sServerType == "not_taf"?_bTafServer = false:_bTafServer=true;}
    void setMacro(const map<string,string>& mMacro);
    void setScript(const string &sStartScript,const string &sStopScript,const string &sMonitorScript);

    void setEnv(const string & sEnv) { _sEnv = sEnv; }
    void setHeartTimeout(int iTimeout) { _iTimeout = iTimeout; }

    //java服务
    void setJvmParams(const string &sJvmParams){_sJvmParams = sJvmParams;}
    void setMainClass(const string &sMainClass){_sMainClass = TC_Common::trim(sMainClass);}
    void setClassPath(const  string &sClassPath){_sClassPath = sClassPath;}
	void setRedirectPath(const string& sRedirectpath) {_sRedirectPath = sRedirectpath;}
	void setBackupFileNames(const string& sFileNames){_sBackupFiles = sFileNames;}

public:
	/**
	 * auto check routine
	 */
	void checkCoredumpLimit();

	/**
	 * server restart and set limitupdate state
	 */
	void setLimitInfoUpdate(bool bUpdate);

	void setServerLimitInfo(const ServerLimitInfo& tInfo);

	bool setServerCoreLimit(bool bCloseCore);

private:
    bool    _bTafServer;                //是否taf服务
    string  _sServerType;               //服务类型  taf_cpp taf_java not_taf
private:
    bool    _bEnabled;                  //服务是否有效
    bool    _bLoaded;                   //服务配置是否已成功加载
    bool    _bPatched;                  //服务可执行程序是否已成功下载
    bool    _bNoticed;                  //服务当前状态是否已通知registry。
    unsigned _uNoticeFailTimes;         //同步服务状态到registry连续失败次数
    bool    _bEnSynState;				  //是否允许同步服务的状态到主控

private:
    string  _sApplication;              //服务所属app
    string  _sServerName;               //服务名称
    string  _sServerId;                 //服务id为App.ServerName.Ip形式

private:
    string _sJvmParams;             //java服务jvm
    string _sMainClass;             //java服务main class
    string _sClassPath;             //java服务 class path
	string _sRedirectPath;		     //标准输出和错误输出重定向目录
private:
    string _sStartScript;           //启动脚本
    string _sStopScript;            //停止脚本
    string _sMonitorScript;         //监控脚本

private:
    string _sServerDir;               //服务数据目录
    string _sConfFile;                //服务配置文件目录 _sConfPath/conf
    string _sExePath;                 //一般为_sServerDir+"/bin" 可个性指定
    string _sExeFile;                 //一般为_sExePath+_sServerName 可个性指定
    string _sLogPath;                 //服务日志目录
    string _sLibPath;                 //动态库目录 一般为_tDesc.basePath/lib
    map<string,string> _mMacro;       //服务宏

private:
    PatchInfo           _tPatchInfo;        //下载信息

private:
    pid_t               _pid;                   //服务进程号
    string              _sVersion;              //TAF版本
    NodeInfo            _tNodeInfo;             //服务所在node 信息
    TC_Endpoint         _tLocalEndpoint;        //本地socket
    ServerDescriptor    _tDesc;                 //服务描述，主要用于生成配置文件等
    ActivatorPtr        _pActivatorPtr;         //用于启动、停止服务
    time_t              _tKeepAliveTime;        //服务最近上报时间
    map<string,time_t>  _mAdapterKeepAliveTime; //各adapter最近上报时间，用来判断adapter是否上报超时

    InternalServerState _eState;                //当前服务状态
    InternalServerState _eLastState;        //上一次服务状态

    int                 _iTimeout;              //心跳超时时间
    string              _sEnv;                  //环境变量字符串
    string 				_sBackupFiles;          //针对java服务发布时bin目录下需要保留的文件；可以用;|来分隔
private:
	bool 				_bLimitStateUpdated;         //服务的limit配置是否有更新，重启也算更新
	ServerLimitInfo  	_tLimitStateInfo;             //通过node配置设置的资源信息
	ServerLimitResourcePtr _pServiceLimitResource;
};

typedef TC_AutoPtr<ServerObject> ServerObjectPtr;

#endif

