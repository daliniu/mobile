#include "servant/QueueTimeout.h"
#include "servant/Communicator.h"
#include "log/taf_logger.h"

using namespace std;

namespace taf
{

QueueTimeout::QueueTimeout(Communicator *comm)
: _terminate(false)
,_comm(comm)
{
}

QueueTimeout::~QueueTimeout()
{
}

void QueueTimeout::terminate()
{
    TC_ThreadLock::Lock lock(*this);

    _terminate = true;

    notify();
}

void QueueTimeout::run()
{
    while (!_terminate)
    {
        try
        {
            TC_ThreadLock::Lock lock(*this);

            timedWait(100);

			if(_terminate) break;

            handleQueueTimeout();

        }
        catch (exception& e)
        {
            LOG->error() << "[TAF][QueueTimeout:run exception:" << e.what() << "]" << endl;
        }
        catch (...)
        {
            LOG->error() << "[TAF][QueueTimeout:run exception.]" << endl;
        }
    }
}

void QueueTimeout::handleQueueTimeout()
{
    map<string, ObjectPrx> objectProxys = _comm->objectProxyFactory()->getObjectProxys();

    map<string, ObjectPrx>::iterator it = objectProxys.begin();

    while(it != objectProxys.end())
    {
        it->second->handleQueueTimeout();

        ++it;
    }
}
//////////////////////////////////////////////////////////////////////////////////
}

