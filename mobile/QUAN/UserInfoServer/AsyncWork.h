#ifndef __ASYNCWORK_H__
#define __ASYNCWORK_H__

#include <set>
#include <queue>

#include "util/tc_thread.h"
#include "util/tc_autoptr.h"
#include "util/tc_logger.h"

#include "DbHandle.h"



class AsyncMaster
{
    class AsyncWorker;
    public:
    class AsyncWork:public taf::TC_HandleBase
    {
        public:
            AsyncWork(){}
            virtual ~AsyncWork(){}

        public:
            virtual void run(DbHandle& tDbHandle)=0;
    };
    typedef taf::TC_AutoPtr<AsyncWork> AsyncWorkPtr;
    private:
    class AsyncWorker:public taf::TC_Thread
    {
        public:
            AsyncWorker(AsyncMaster& master):master(master){}
            void run()
            {
                while(!_terminate)
                {
                    AsyncWorkPtr workPtr;
                    if(master.getTask(workPtr))
                    {
                        try
                        {
                            workPtr->run(mDbHandle);
                        }
                        catch(exception &ex)
                        {
                            LOG->error() <<__FUNCTION__<<"|"<<ex.what()<<endl;
                        }
                    }
                    else
                    {
                        taf::TC_ThreadLock::Lock lock(master.mPoolWakeLock);
                        master.mPoolWakeLock.timedWait(1000);
                    }
                }
            }

            void terminate()
            {
                _terminate=true;
            }
        private:
            DbHandle mDbHandle;
            AsyncMaster& master;
            bool _terminate;
    };
    public:
    AsyncMaster(int threadCount=0)
    {
        _terminate=false;
        init(threadCount);
    }
    ~AsyncMaster()
    {
        taf::TC_ThreadLock::Lock lock(mPoolLock);
        _terminate=true;
        for(set<AsyncWorker*>::iterator it=mPool.begin();it!=mPool.end();++it)
        {
            (*it)->terminate();
        }
        {
            taf::TC_ThreadLock::Lock lock(mPoolWakeLock);
            mPoolWakeLock.notifyAll();
        }
        mPoolLock.wait();
    }
    void init(int threadCount)
    {
        for(int i=0;i<threadCount;++i)
        {
            AsyncWorker* worker=new AsyncWorker(*this);
            worker->start();
            mPool.insert(worker);
        }
    }
    int doAsyncWork(AsyncWorkPtr work)
    {
        int ret;
        {
            taf::TC_ThreadLock::Lock lock(mWorkQueueLock);
            mWorkQueue.push(work);
            ret=mWorkQueue.size();
        }

        if(!_terminate)
        {
            taf::TC_ThreadLock::Lock lock(mPoolWakeLock);
            mPoolWakeLock.notify();
        }
        return ret;
    }
    private:
    bool getTask(AsyncWorkPtr& work)
    {
        taf::TC_ThreadLock::Lock lock(mWorkQueueLock);
        if(!mWorkQueue.empty())
        {
            work=mWorkQueue.front();
            mWorkQueue.pop();
            return true;
        }
        else
        {
            return false;
        }
    }
    void exitThread(AsyncWorker* worker)
    {
        taf::TC_ThreadLock::Lock lock(mPoolLock);
        mPool.erase(worker);
        if(mPool.empty()) mPoolLock.notify();
    }
    friend class AsyncWorker;

    std::queue<AsyncWorkPtr> mWorkQueue;
    taf::TC_ThreadLock mWorkQueueLock;

    taf::TC_ThreadLock mPoolWakeLock;

    std::set<AsyncWorker*> mPool;
    taf::TC_ThreadLock mPoolLock;

    bool _terminate;

};

typedef AsyncMaster::AsyncWork AsyncWork;

#endif
