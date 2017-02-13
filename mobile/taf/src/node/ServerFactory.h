#ifndef __SERVER_FACTORY_H_
#define __SERVER_FACTORY_H_
#include "Node.h"
#include <unistd.h>
#include "ServerObject.h"
#include "util/tc_common.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_singleton.h"

using namespace taf;
using namespace std;

typedef JceHashMap<ServerInfo,ServerDescriptor,ThreadLockPolicy,FileStorePolicy> HashMap;

extern HashMap g_serverInfoHashmap;

typedef map<string, ServerObjectPtr> ServerGroup;

class ServerFactory : public TC_Singleton<ServerFactory>, public TC_ThreadLock
{
public:
    /**
     *
     */
    ServerFactory();
    ~ServerFactory()
    {
    };

    /**
    * 创建指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  ServerObjectPtr
    */
    ServerObjectPtr createServer( const ServerDescriptor& tDesc)
    {
        string result;
        return createServer(tDesc,result);
    }
    ServerObjectPtr createServer( const ServerDescriptor& tDesc,string& result);

    /**
    * 删除指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  int
    */
    int eraseServer( const string& application, const string& serverName );

    /**
    * 获取指定服务
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  ServerObjectPtr  服务不存在返回NULL
    */
    ServerObjectPtr getServer( const string& application, const string& serverName );

    /**
    * load服务 若application
    * serverName为空load所有服务，只返回最后一个load对象。
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  ServerObjectPtr  服务不存在返回NULL
    */
    ServerObjectPtr loadServer( const string& application="", const string& serverName="",bool enableCache = true)
    {
        string result;
        return loadServer(application,serverName,enableCache,result);
    }
    ServerObjectPtr loadServer( const string& application, const string& serverName,bool enableCache, string& result);
    /**
    * get服务 若application
    * serverName为空获取所有服务，只返回最后一个load对象。
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  vector<ServerDescriptor>  服务不存在返回NULL
    */
    vector<ServerDescriptor> getServerFromRegistry( const string& application, const string& serverName, string& result);
    /**
    * get服务 若application
    * serverName为空获取所有服务，只返回最后一个load对象。
    * @param application    服务所属应用名
    * @param serverName  服务名
    * @return  vector<ServerDescriptor>  服务不存在返回NULL
    */
    vector<ServerDescriptor> getServerFromCache( const string& application, const string& serverName, string& result);

    /**
    * 获取node上服务所有服务
    * @return    map<string, ServerGroup>
    */
    map<string, ServerGroup> getAllServers();
	/**
	 *@brief 加载服务core属性的配置文件
	 *@return bool
	 */
	bool loadConfig();
	/**
	 *@brief 设置node上所有服务的core属性，如果node更新过core配置，则不需要设置
	 *
	 */
	void setAllServerResourceLimit();
private:
	bool IsCoreLimitEnable() const { return _bCoreLimitEnable;}
private:
    //<app,<server,serverObjectPtr>>
    map<string, ServerGroup> _mmServerList;
    PlatformInfo _tPlatformInfo;
private:

	//<app.server,ServerLimitStateInfo>
	map<string,ServerObject::ServerLimitInfo> _mCoreLimitConfig;

	//配置是否有更新
	bool _bReousceLimitConfChanged;
	//最大服务异常停止个数，超过限制则自动屏蔽coredump
    int     _iMaxExcStopCount;
	//最大服务异常停止个数，超过限制则自动屏蔽coredump
    int32_t _iCoreLimitTimeInterval;
	//服务的coredump属性被屏蔽之后的有效期限，单位是分钟
    int32_t  _iCoreLimitExpiredTime;
	//资源属性开关
	bool     _bCoreLimitEnable;
};


#endif


