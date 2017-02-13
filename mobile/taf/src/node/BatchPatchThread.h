#ifndef __BATCH_PATCH_THREAD_H_
#define __BATCH_PATCH_THREAD_H_
#include "servant/Application.h"
#include "Node.h"
#include "ServerObject.h"

class BatchPatchThread;

class BatchPatch : public taf::TC_ThreadLock
{
public:
    /**
    * 构造函数
    */
    BatchPatch();

    /**
    * 析构函数
    */
    ~BatchPatch();

    /**
     * 结束线程
     */
    void terminate();

    /**
     * 线程运行
     */
    void start(int iThreadNum);

public:
    /**
     * 插入发布请求
     */
    void push_back(const taf::PatchRequest & request, ServerObjectPtr servePtr);

    /**
     * 从发布队列中获取发布请求
     */
    bool pop_front(taf::PatchRequest & request, ServerObjectPtr &servePtr);

    /**
     * 获取发布队列大小
     */
    size_t getPatchQueueSize();

    /**
     * 设置下载暂存目录
     */
    void setPath(const std::string & sDownloadPath)
    {
        _sDownLoadPath = sDownloadPath;
    }

private:
    taf::TC_ThreadLock                  _QueueMutex;

    std::deque<taf::PatchRequest>       _PatchQueue;
    std::deque<ServerObjectPtr>    _PatchServerQueue;


    std::set<std::string>               _PatchIng;

    std::vector<BatchPatchThread *>     _vecRunners;

    std::string                         _sDownLoadPath;       
};


class BatchPatchThread : public TC_Thread
{
public:
    BatchPatchThread(BatchPatch * patch);

    ~BatchPatchThread();

    virtual void run();

    void terminate();

public:
    /**
     * 设置下载暂存目录
     */
    void setPath(const std::string & sDownloadPath)
    {
        _sDownloadPath = sDownloadPath;
    }

    /**
     * 执行发布单个请求
     */
    void doPatchRequest(const taf::PatchRequest & request, ServerObjectPtr server);

protected:
    BatchPatch *    _BatchPatch;

    std::string     _sDownloadPath; //文件下载目录

protected:
    bool            _bShutDown; 
};

#endif
