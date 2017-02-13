#ifndef __NODE_SERVER_H_
#define __NODE_SERVER_H_

#include "servant/Application.h"
#include "KeepAliveThread.h"
#include "QueryF.h"
#include "BatchPatchThread.h"

using namespace taf;

class NodeServer;
extern NodeServer g_app;
class NodeServer : public Application
{
public:
     /**
     * 获取Adapter Endpoint
     */
    TC_Endpoint getAdapterEndpoint(const string& name ) const;

      /**
     * 添加Config
     * @param filename
     */
    bool addConfig(const string &filename){return Application::addConfig(filename);}

    /*
    *   权限控制
    */
    bool isValid(const string& ip);

     /*
    *   上报string至notify
    */
    void reportServer(const string &sServerId,const string &sResult);

public:
    /*
    *   管理指令
    */
    bool cmdViewServerDesc(const string& command, const string& params, string& result);
	/**
	 *@brief  加载服务配置文件,并使之生效
	 *
	 */
	bool cmdReLoadConfig(const string& command, const string& params, string& result);

protected:

    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 只会进程调用一次
     */
    virtual void destroyApp();

    /**
     * 初始化hashmap
     */
    void initHashMap();
private:
	/**
	 *初始化主控obj名字
	 */
	void initRegistryObj();
private:
    KeepAliveThread *   _pKeepAliveThread;

    BatchPatch *        _pBatchPatchThread;

    static string g_sNodeIp;
};
#endif


