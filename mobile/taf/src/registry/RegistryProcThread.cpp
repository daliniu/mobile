#include "RegistryProcThread.h"

using namespace taf;

RegistryProcThread::RegistryProcThread()
: _terminate(false)
{
	LOG->debug()<<"RegistryProcThread init ok"<<endl;
}

RegistryProcThread::~RegistryProcThread()
{
   terminate();
}

void RegistryProcThread::terminate()
{
	_terminate = true;

	for (uint32_t i = 0; i < _runners.size(); ++i)
	{
		if (_runners[i]->isAlive())
		{
			_runners[i]->terminate();

			_queue.notifyT();
		}
	}

	for (uint32_t i = 0; i < _runners.size(); ++i)
	{
		if(_runners[i]->isAlive())
		{
			_runners[i]->getThreadControl().join();
		}
	}

	_queue.clear();

}

void RegistryProcThread::start(int num)
{
	for (int i = 0; i < num; ++i)
	{
		RegistryProcThreadRunnerPtr r = new RegistryProcThreadRunner(this);

		r->start();

		_runners.push_back(r);
	}
}

void RegistryProcThread::put(const RegistryProcInfo& info)
{
    if(!_terminate)
    {
        _queue.push_back(info);
    }
}

bool RegistryProcThread::pop(RegistryProcInfo& info)
{
    return _queue.pop_front(info,1000);
}


RegistryProcThreadRunner::RegistryProcThreadRunner(RegistryProcThread* proc)
: _terminate(false)
, _proc(proc)
{
	//初始化数据库实例
    extern TC_Config * g_pconf;
    _db.init(g_pconf);
}

void RegistryProcThreadRunner::terminate()
{
    _terminate = true;
}
void RegistryProcThreadRunner::run()
{
    while (!_terminate)
    {
        try
        {
            RegistryProcInfo info;
            if(_proc->pop(info) && !_terminate)
            {
				if(EM_NODE_KEEPALIVE == info.cmd)
				{
                	_db.keepAlive(info.nodeName,info.loadinfo);
				}
				else if(EM_UPDATEPATCHRESULT == info.cmd)
				{
					_db.setPatchInfo(info.appName,info.serverName,info.nodeName,info.patchVersion,info.patchUserName);
				}
				else if(EM_REPORTVERSION == info.cmd)
				{
					_db.setServerTafVersion(info.appName, info.serverName, info.nodeName, info.tafVersion);
				}
				LOG->debug()<< "RegistryProcThreadRunner:run|cmd:"<<info.cmd<<endl;
            }

        }
        catch (exception& e)
        {
            LOG->error()<<"RegistryProcThreadRunner::run catch exception|"<<e.what()<<endl;
        }
        catch (...)
        {
            LOG->error()<<"RegistryProcThreadRunner::run catch unkown exception|"<<endl;
        }
    }
}

