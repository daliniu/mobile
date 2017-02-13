#ifndef __KEEP_ALIVE_THREAD_H_
#define __KEEP_ALIVE_THREAD_H_
#include "Node.h"
#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_monitor.h"
#include "util/tc_thread.h"
#include "PlatformInfo.h"

using namespace taf;
using namespace std;

class KeepAliveThread : public TC_Thread,public TC_ThreadLock
{
public:
    /**
    * 构造函数
    */

    KeepAliveThread();
    ~KeepAliveThread();
    /**
     * 结束线程
     */
    void terminate();

protected:

    virtual void run();
    
    /**
    * 往registry注册node信息
    * @return bool false失败
    */
    
    bool registerNode();
    
    /**
    * 获取本node在registry配置服务信息
    * @return bool  false失败
    */
    bool loadAllServers();

    /**
    * 上报registry node当前状态
    * @return int  非0失败
    */
    int reportAlive();

    /**
    * 检查node上server状态
    * @return void 
    */
    void checkAlive();

    /**
    * 与registry 同步node上所属服务状态
    * @return int  非0失败
    */
    int synStat();

protected:

    NodeInfo            _tNodeInfo;
    PlatformInfo        _tPlatformInfo;
    RegistryPrx         _pRegistryPrx;
    bool                _bShutDown; 
    time_t              _tRunTime; //node运行时间
    int                 _iHeartTimeout ;//业务心跳超时时间(s)
    int                 _iMonitorInterval ;//监控server状态的间隔时间(s)
    int                 _iSynInterval;//同步与regisrty server状态的间隔时间(s)
    string              _sSynStatBatch;//批量同步
private:

    vector<ServerStateInfo>     _vStat;//服务状态列表
};


#endif

