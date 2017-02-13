#include "servant/FDReactor.h"
#include "servant/Communicator.h"
#include "log/taf_logger.h"

using namespace std;

namespace taf
{

FDReactor::FDReactor()
:_terminate(false)
{
    _ep.create(1024);

    _shutdown.createSocket();

    _ep.add(_shutdown.getfd(), 0, EPOLLIN);
}

FDReactor::~FDReactor()
{
}

void FDReactor::terminate()
{
    _terminate = true;
    //通知epoll响应
    _ep.mod(_shutdown.getfd(), 0, EPOLLOUT);
}

void FDReactor::registerAdapter(AdapterProxy* adapter, uint32_t events)
{
    if (adapter->hasBusCommu() == false)
    {
        _ep.del(adapter->trans()->fd(), (uint64_t)adapter, 0);

        _ep.add(adapter->trans()->fd(), (uint64_t)adapter, events);
    }
    else
    {
        _ep.del(((BusTransceiver *)adapter->trans())->getrfd(), (uint64_t)adapter, 0);
        _ep.del(((BusTransceiver *)adapter->trans())->getwfd(), (uint64_t)adapter, 0);

        _ep.add(((BusTransceiver *)adapter->trans())->getrfd(), (uint64_t)adapter, EPOLLIN);
        _ep.add(((BusTransceiver *)adapter->trans())->getwfd(), (uint64_t)adapter, EPOLLOUT);
    }
}

void FDReactor::unregisterAdapter(AdapterProxy* adapter, uint32_t event)
{
    _ep.del(adapter->trans()->fd(), (uint64_t)adapter, event);
}

void FDReactor::notify(AdapterProxy* adapter)
{
    if(adapter->trans()->fd() != -1)
    {
        _ep.mod(adapter->trans()->fd(), (uint64_t)adapter, EPOLLOUT|EPOLLIN);
    }
}

void FDReactor::handleInputImp(AdapterProxy* adapter)
{
    if (adapter->trans()->isValid())
    {
        list<ResponsePacket> done;

        if (adapter->trans()->doResponse(done) > 0)
        {
            for (list<ResponsePacket>::iterator lit = done.begin(); lit != done.end(); ++lit)
            {
                adapter->finished(*lit);
            }
        }
    }
}

void FDReactor::handleOutputImp(AdapterProxy* adapter)
{
    if (adapter->trans()->isValid())
    {
        while (adapter->trans()->doRequest() >= 0 && adapter->sendRequest());
    }
}

void FDReactor::handle(AdapterProxy* adapter, int events)
{
    assert(adapter != NULL);

    try
    {
        if (events & (EPOLLERR | EPOLLHUP))
        {
            adapter->trans()->close();
        }
        else if (events != 0)
        {
            //需要关闭连接
            if(adapter->shouldCloseTrans())
            {
                adapter->setCloseTrans(false);

                adapter->trans()->close();

                return;
            }

            //正在连接, 完成连接
            if(adapter->trans()->isConnecting())
            {
                if(!adapter->finishConnect())
                    return;
            }
			else if(adapter->trans()->isBusConnecting())
			{
				if(!adapter->finishBusConnect())
                    return;
			}

            if (events & FDReactor::R)
            {
                handleInputImp(adapter);
            }

            if (events & FDReactor::W)
            {
                handleOutputImp(adapter);
            }
        }
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][FDReactor:handle exception:" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][FDReactor:handle exception.]" << endl;
    }
}

void FDReactor::run()
{
    while (!_terminate)
    {
        try
        {
            int num = _ep.wait(1000);

            if(_terminate) break;

            //先处理epoll的网络事件
            for (int i = 0; i < num; ++i)
            {
                const epoll_event& ev = _ep.get(i);

                uint64_t data = ev.data.u64;

                if(data == 0) continue; //data非指针, 退出循环

                int events = (ev.events & EPOLLIN) ? FDReactor::R : 0;

                events = events | ((ev.events & EPOLLOUT) ? FDReactor::W : 0);

                handle((AdapterProxy*)data, events);
            }
        }
        catch (exception& e)
        {
            LOG->error() << "[TAF][FDReactor:run exception:" << e.what() << "]" << endl;
        }
        catch (...)
        {
            LOG->error() << "[TAF][FDReactor:run exception.]" << endl;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////
}
