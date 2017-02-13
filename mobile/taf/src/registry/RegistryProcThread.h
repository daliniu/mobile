#ifndef __NODE_KEEPALIVE_THREAD_H_
#define __NODE_KEEPALIVE_THREAD_H_

#include "DbHandle.h"
#include "util/tc_thread_queue.h"
#include "util/tc_autoptr.h"
#include "Node.h"
#include "Registry.h"

using namespace taf;
enum ProcCmd
{
	EM_REPORTVERSION,
	EM_NODE_KEEPALIVE,
	EM_UPDATEPATCHRESULT
};

struct RegistryProcInfo
{
	ProcCmd  cmd;
	string   appName;
	string   serverName;
	string   nodeName;
	/*-------EM_REPORTVERSION-------------*/
	string   tafVersion;
	/*-------EM_REPORTVERSION-------------*/
	string   patchVersion;
	string   patchUserName;
    /*-------EM_NODE_KEEPALIVE------------*/
	LoadInfo  loadinfo;
};

class RegistryProcThreadRunner;
typedef TC_AutoPtr<RegistryProcThreadRunner> RegistryProcThreadRunnerPtr;

class RegistryProcThread
{
public:
    RegistryProcThread();
   virtual ~RegistryProcThread();

public:
    void terminate();
    void put(const RegistryProcInfo& info);
    bool pop(RegistryProcInfo& info);
	/**
	 * 启动处理请求线程
	 * @param num 线程数
	 */
    void start(int num=1);

protected:
	bool _terminate;

	TC_ThreadQueue<RegistryProcInfo> _queue;

    /**
     * runner对象(每个对象维护一个线程)
     */
    vector<RegistryProcThreadRunnerPtr> _runners;

	friend class RegistryProcThreadRunner;

};


class RegistryProcThreadRunner : public TC_Thread, public TC_HandleBase
{
public:
    RegistryProcThreadRunner(RegistryProcThread* proc);

    virtual void run();

    void terminate();
private:
    bool _terminate;
    RegistryProcThread *_proc;
    CDbHandle _db;
};

#endif
