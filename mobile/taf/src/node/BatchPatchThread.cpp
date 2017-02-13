#include "util/tc_md5.h"
#include "BatchPatchThread.h"
#include "ServerFactory.h"
#include "CommandLoadPatch.h"

using namespace taf;

BatchPatch::BatchPatch()
{
    
}

BatchPatch::~BatchPatch()
{
    terminate();
}

void BatchPatch::start(int iNum)
{
    for (int i = 0; i < iNum; i++)
    {
        BatchPatchThread * t = new BatchPatchThread(this);
        t->setPath(_sDownLoadPath);
        t->start();

        _vecRunners.push_back(t);
    }
}

void BatchPatch::terminate()
{
    for (size_t i = 0; i < _vecRunners.size(); ++i)
    {
        if(_vecRunners[i]->isAlive())
        {
            _vecRunners[i]->terminate();
        }
    }

    for (size_t i = 0; i < _vecRunners.size(); ++i)
    {
        if(_vecRunners[i]->isAlive())
        {
            _vecRunners[i]->getThreadControl().join();
        }
    }
}

void BatchPatch::push_back(const taf::PatchRequest & request, ServerObjectPtr serverPtr)
{
    TC_ThreadLock::Lock LockQueue(_QueueMutex);

    if (_PatchIng.count(request.appname + request.servername) == 1)
    {
        std::string sException = "reduplicate patch request:" + request.version + "," + request.user;

        LOG->debug() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << sException << endl;
        throw TC_Exception(sException);
    }

    if (!serverPtr)
    {
    	LOG->error() << __FUNCTION__  << ": server null " <<endl;
	throw TC_Exception("sever null");
    }

    _PatchQueue.push_back(request);
    _PatchServerQueue.push_back(serverPtr);

    _PatchIng.insert(request.appname + request.servername);

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

bool BatchPatch::pop_front(taf::PatchRequest & request, ServerObjectPtr &serverPtr)
{
    TC_ThreadLock::Lock LockQueue(_QueueMutex);

    if (_PatchQueue.empty())
    {
        return false;
    }	

    request = _PatchQueue.front();
    _PatchQueue.pop_front();
   serverPtr = _PatchServerQueue.front();
   _PatchServerQueue.pop_front();


    _PatchIng.erase(request.appname + request.servername);

    return true;
}

size_t BatchPatch::getPatchQueueSize()
{
    TC_ThreadLock::Lock lock(_QueueMutex);

    return _PatchQueue.size();
}

BatchPatchThread::BatchPatchThread(BatchPatch * patch) : _BatchPatch(patch)
{
    _bShutDown  = false;
}

BatchPatchThread::~BatchPatchThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void BatchPatchThread::terminate()
{
    _bShutDown = true;
}

void BatchPatchThread::doPatchRequest(const taf::PatchRequest & request, ServerObjectPtr server)
{
    LOG->debug() << "BatchPatchThread::doPatchRequest" << "|" 
    << request.appname + "." + request.servername + "_" + request.nodename << "|"
    << request.groupname   << "|"
    << request.version     << "|"
    << request.user        << "|"
    << request.servertype  << "|"
    << request.patchobj    << "|"
    << request.md5         << endl;


    //设置发布状态
    try
    {
        std::string sError;

        //获取预发布服务
        //ServerObjectPtr server = ServerFactory::getInstance()->loadServer(request.appname, request.servername, true, sError);

	if (!server)
        {
            LOG->error() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|get server object fault:" << sError << endl;
            return ;        
        }

        //查看本地硬盘是否已经存在该文件
        LOG->debug() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|patch begin" << endl;
        CommandLoadPatch command(server, _sDownloadPath, request);
        if (command.canExecute(sError) != ServerCommand::EXECUTABLE)
        {
            LOG->error() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|canExecute error:" << sError << endl;
            return ;
        }

        if (command.execute(sError) == 0)
        {
            LOG->debug() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|patch succ" << endl;
        }
        else
        {
            LOG->error() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|patch fault:" << sError << endl;
        }
    }
    catch (exception & e)
    {
        LOG->error() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|Exception:" << e.what() << endl;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << request.appname + "." + request.servername << "|" << request.md5 << "|Unknown Exception" << endl;
    }
}

void BatchPatchThread::run()
{
    while (!_bShutDown)
    {
        try
        {
            {
                TC_ThreadLock::Lock lock(*_BatchPatch);

                if (_BatchPatch->getPatchQueueSize() == 0)
                {
                    _BatchPatch->timedWait(2000);
                }
            }

            PatchRequest request;
	    ServerObjectPtr server;
            if (_BatchPatch->pop_front(request, server))
            {
            	if (!server)
            	{
            		LOG->error() << __FUNCTION__ << ":" << __LINE__ << " server error" << endl;
			throw TC_Exception("server error");
            	}
                doPatchRequest(request, server);
            }
        }
        catch (exception& e)
        {
            LOG->error()<<"BatchPatchThread::run catch exception|"<<e.what()<<endl;
        }
        catch (...)
        {
            LOG->error()<<"BatchPatchThread::run catch unkown exception|"<<endl;
        }
    }
}

