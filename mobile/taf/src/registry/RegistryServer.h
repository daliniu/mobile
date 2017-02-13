#include "servant/Application.h"
#include "RegistryImp.h"
#include "QueryImp.h"
#include "AdminRegImp.h"
#include "ReapThread.h"
#include "CheckSettingState.h"
#include "RegistryProcThread.h"

using namespace taf;


/**
 *  Registry Server
 */
class RegistryServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析构, 每个进程都会调用一次
     */
    virtual void destroyApp();

public:
    /**
     * 获取registry对象的端口信息
     */
    map<string, string> getServantEndpoint() { return _mapServantEndpoint; }

    /**
     * 加载registry对象的端口信息
     */
    int loadServantEndpoint();

    RegistryProcThread * getRegProcThread(){ return _registryProcThread;}

protected:
    //用于执行定时操作的线程对象
    ReapThread _reapThread;
	CheckSettingState _checksetingThread;

    RegistryProcThread *_registryProcThread;

    //对象-适配器 列表
    map<string, string> _mapServantEndpoint;

};


