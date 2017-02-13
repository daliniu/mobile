#include "util/tc_epoll_server.h"
#include "util/tc_clientsocket.h"
#include "util/tc_common.h"
#include "servant/AppProtocol.h"
#include <iostream>
#include <cassert>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace taf
{

#define H64(x) (((uint64_t)x) << 32)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// handle的实现
TC_EpollServer::Handle::Handle()
: _pEpollServer(NULL)
, _handleGroup(NULL)
, _iWaitTime(100)
{
}

TC_EpollServer::Handle::~Handle()
{
}

void TC_EpollServer::Handle::handleClose(const tagRecvData &stRecvData)
{
}

void TC_EpollServer::Handle::sendResponse(uint32_t uid, const string &sSendBuffer, const string &ip, int port)
{
    _pEpollServer->send(uid, sSendBuffer, ip, port);
}

void TC_EpollServer::Handle::close(uint32_t uid)
{
    _pEpollServer->close(uid);
}

void TC_EpollServer::Handle::handleTimeout(const tagRecvData &stRecvData)
{
    _pEpollServer->error("[Handle::handleTimeout] queue timeout, close [" + stRecvData.ip + ":" + TC_Common::tostr(stRecvData.port)+ "].");

    close(stRecvData.uid);
}

void TC_EpollServer::Handle::handleOverload(const tagRecvData &stRecvData)
{
    _pEpollServer->error("[Handle::handleOverload] adapter '"
                         + stRecvData.adapter->getName() + "',over load:"
                         + TC_Common::tostr(stRecvData.adapter->getRecvBufferSize()) +  ">"
                         + TC_Common::tostr(stRecvData.adapter->getQueueCapacity()) + ".");

    close(stRecvData.uid);
}

void TC_EpollServer::Handle::run()
{
    initialize();

    handleImp();
}

bool TC_EpollServer::Handle::allAdapterIsEmpty()
{
    map<string, BindAdapterPtr>& adapters = _handleGroup->adapters;

    for (map<string, BindAdapterPtr>::iterator it = adapters.begin(); it != adapters.end(); ++it)
    {
        BindAdapterPtr& adapter = it->second;

        if (adapter->getRecvBufferSize() > 0)
        {
            return false;
        }
    }
    return true;
}

bool TC_EpollServer::Handle::allFilterIsEmpty()
{
    return true;
}

void TC_EpollServer::Handle::setEpollServer(TC_EpollServer *pEpollServer)
{
    TC_ThreadLock::Lock lock(*this);

    _pEpollServer = pEpollServer;
}

TC_EpollServer* TC_EpollServer::Handle::getEpollServer()
{
    return _pEpollServer;
}

void TC_EpollServer::Handle::setHandleGroup(TC_EpollServer::HandleGroupPtr& pHandleGroup)
{
    TC_ThreadLock::Lock lock(*this);

    _handleGroup = pHandleGroup;
}

TC_EpollServer::HandleGroupPtr& TC_EpollServer::Handle::getHandleGroup()
{
    return _handleGroup;
}

void TC_EpollServer::Handle::notifyFilter()
{
    TC_ThreadLock::Lock lock(_handleGroup->monitor);

    //如何做到不唤醒所有handle呢？
    _handleGroup->monitor.notifyAll();
}

void TC_EpollServer::Handle::setWaitTime(uint32_t iWaitTime)
{
    TC_ThreadLock::Lock lock(*this);

    _iWaitTime = iWaitTime;
}

void TC_EpollServer::Handle::handleImp()
{
    startHandle();

    while (!getEpollServer()->isTerminate())
    {
        {
            TC_ThreadLock::Lock lock(_handleGroup->monitor);

            if (allAdapterIsEmpty() && allFilterIsEmpty())
            {
                _handleGroup->monitor.timedWait(_iWaitTime);
            }
        }

        //上报心跳
        heartbeat();

        //为了实现所有主逻辑的单线程化,在每次循环中给业务处理自有消息的机会
        handleAsyncResponse();
        handleCustomMessage(true);


        tagRecvData* recv = NULL;

        map<string, BindAdapterPtr>& adapters = _handleGroup->adapters;

        for (map<string, BindAdapterPtr>::iterator it = adapters.begin(); it != adapters.end(); ++it)
        {
            BindAdapterPtr& adapter = it->second;

            try
            {

                while (adapter->waitForRecvQueue(recv, 0))
                {
                    //上报心跳
                    heartbeat();

                    //为了实现所有主逻辑的单线程化,在每次循环中给业务处理自有消息的机会
                   	handleAsyncResponse();

                    tagRecvData& stRecvData = *recv;

                    time_t now = TC_TimeProvider::getInstance()->getNow();


                    stRecvData.adapter = adapter;

                    //数据已超载 overload
                    if (stRecvData.isOverload)
                    {
                        handleOverload(stRecvData);
                    }
                    //关闭连接的通知消息
                    else if (stRecvData.isClosed)
                    {
                        handleClose(stRecvData);
                    }
                    //数据在队列中已经超时了
                    else if ( (now - stRecvData.recvTimeStamp)*1000 > adapter->getQueueTimeout())
                    {
                        handleTimeout(stRecvData);
                    }
                    else
                    {
                        handle(stRecvData);
                    }
					 handleCustomMessage(false);

					delete recv;
					recv = NULL;
                }
            }
            catch (exception &ex)
            {
                if(recv)
				{
					close(recv->uid);
					delete recv;
					recv = NULL;
				}

                getEpollServer()->error("[Handle::handleImp] error:" + string(ex.what()));
            }
            catch (...)
            {
                if(recv)
				{
					close(recv->uid);
					delete recv;
					recv = NULL;
				}

                getEpollServer()->error("[Handle::handleImp] unknown error");
            }
        }
    }

    stopHandle();
}
///////////////////////BindAdapter///////////////////////

TC_EpollServer::Adapter_ShmQueue::Adapter_ShmQueue():_shmKey(0), _shmSize(0)
{

}

bool TC_EpollServer::Adapter_ShmQueue::initialize()
{
    _shm.init(_shmSize, _shmKey, false);

    if (_shm.iscreate())
    {
        _shmQueue.create(_shm.getPointer(), _shmSize);
    }
    else
    {
        _shmQueue.connect(_shm.getPointer(), _shmSize);
    }

    return true;
}

void TC_EpollServer::Adapter_ShmQueue::push_back(const recv_queue_type & qdata)
{
    Lock lock(*this);

    for (recv_queue_type::const_iterator it = qdata.begin(); it != qdata.end(); it++)
    {
        if ((*it)->isClosed == true)
        {
            _memQueue.push_back(*it);
        }
        else if (_push_back(*it) == false)
        {
            (*it)->isOverload = true;

            _memQueue.push_back(*it);
        }

        notify();
    }
}

bool TC_EpollServer::Adapter_ShmQueue::pop_front(tagRecvData* & data, size_t millsecond)
{
    Lock lock(*this);

    if (_shmQueue.isEmpty() && _memQueue.empty())
    {
        if (millsecond == 0)
        {
            return false;
        }
        if (millsecond == (size_t) -1)
        {
            wait();
        }
        else
        {
            if (!timedWait(millsecond))
            {
                return false;
            }
        }
    }

    if (!_memQueue.empty())
    {
        data = _memQueue.front();
        _memQueue.pop_front();
        return true;
    }

    if (!_shmQueue.isEmpty())
    {
        return _pop_front(data);
    }

    return false;
}

bool TC_EpollServer::Adapter_ShmQueue::_push_back(const tagRecvData* const & data)
{
    if (_shmQueue.isFull(sizeof(struct shmHeader) + data->buffer.size()))
    {
        return false;
    }

    struct shmHeader head;
    head.uid            = data->uid;
    head.port           = data->port;
    head.recvTimeStamp  = data->recvTimeStamp;
    head.isOverload     = data->isOverload;
    head.isClosed       = data->isClosed;
    strncpy(head.ip, data->ip.c_str(), sizeof(head.ip));

    string sBuffer;
    sBuffer.reserve(sizeof(struct shmHeader) + data->buffer.size());
    sBuffer.append((char *)&head, sizeof(struct shmHeader));
    sBuffer.append(data->buffer);

    return _shmQueue.push_back((const char *)sBuffer.data(), sBuffer.size());
}

bool TC_EpollServer::Adapter_ShmQueue::_pop_front(tagRecvData* & data)
{
    string sBuffer;
    if (_shmQueue.pop_front(sBuffer) == false)
    {
        return false;
    }

    if (sBuffer.size() < sizeof(struct shmHeader))
    {
        return false;
    }

    struct shmHeader * head = (struct shmHeader *)sBuffer.data();

    data                = new tagRecvData();
    data->uid           = head->uid;
    data->ip            = head->ip;
    data->port          = head->port;
    data->recvTimeStamp = head->recvTimeStamp;
    data->isOverload    = head->isOverload;
    data->isClosed      = head->isClosed;
    data->buffer.assign((char *)sBuffer.data() + sizeof(struct shmHeader), sBuffer.size() - sizeof(struct shmHeader));

    return true;
}

size_t TC_EpollServer::Adapter_ShmQueue::getQueueSize()
{
    Lock lock(*this);

    return _shmQueue.elementCount() + _memQueue.size();
}

TC_EpollServer::BindAdapter::BindAdapter(TC_EpollServer *pEpollServer)
: _pEpollServer(pEpollServer)
, _handleGroup(NULL)
, _pf(echo_protocol)
, _hf(echo_header_filter)
, _name("")
, _handleGroupName("")
, _iMaxConns(DEFAULT_MAX_CONN)
, _iCurConns(0)
, _iHandleNum(0)
, _eOrder(ALLOW_DENY)
, _iQueueCapacity(DEFAULT_QUEUE_CAP)
, _iQueueTimeout(DEFAULT_QUEUE_TIMEOUT/1000)
, _iHeaderLen(0)
, _iHeartBeatTime(0)
, _protocolName("taf")
, _bUseShm(false)
, _bUseBusCommu(false)
, _sizeBusCommu(1*1024*1024)
{
}

TC_EpollServer::BindAdapter::~BindAdapter()
{
    //adapter析够的时候, 服务要退出
    _pEpollServer->terminate();
    for (std::map<string, BusCommu *>::iterator it = _mapBusCommu.begin(); it != _mapBusCommu.end(); it++)
    {
        delete it->second;
    }
}

void TC_EpollServer::BindAdapter::initialize()
{
    if (_bUseShm)
    {
        _shmQueue.initialize();
    }
}

void TC_EpollServer::BindAdapter::setShm(int iKey, int iSize)
{
    if (iKey > 0 && iSize > 0)
    {
        _shmQueue.setShmKey(static_cast<key_t>(iKey));
        _shmQueue.setShmSize(static_cast<size_t>(iSize));

        _bUseShm = true;
    }
}

void TC_EpollServer::BindAdapter::setProtocolName(const string& name)
{
    TC_ThreadLock::Lock lock(*this);

    _protocolName = name;
}

const string& TC_EpollServer::BindAdapter::getProtocolName()
{
    return _protocolName;
}

bool TC_EpollServer::BindAdapter::isTafProtocol()
{
    return (_protocolName == "taf");
}

bool TC_EpollServer::BindAdapter::isIpAllow(const string& ip) const
{
    TC_ThreadLock::Lock lock(*this);

    if(_eOrder == ALLOW_DENY)
    {
        if(TC_Common::matchPeriod(ip,_vtAllow))
        {
            return true;
        }
        if(TC_Common::matchPeriod(ip,_vtDeny))
        {
            return false;
        }
    }
    else
    {
        if(TC_Common::matchPeriod(ip,_vtDeny))
        {
            return false;
        }
        if(TC_Common::matchPeriod(ip,_vtAllow))
        {
            return true;
        }
    }
    return _vtAllow.size() == 0;
}

void TC_EpollServer::BindAdapter::insertRecvQueue(const recv_queue::queue_type &vtRecvData, bool bPushBack)
{
    if (_bUseShm)
    {
        _shmQueue.push_back(vtRecvData);
    }
    else
    {
        if (bPushBack)
        {
            _rbuffer.push_back(vtRecvData);
        }
        else
        {
            _rbuffer.push_front(vtRecvData);
        }
    }

    TC_ThreadLock::Lock lock(_handleGroup->monitor);

    _handleGroup->monitor.notify();
}

bool TC_EpollServer::BindAdapter::waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime)
{
    if (_bUseShm)
    {
        return _shmQueue.pop_front(recv, iWaitTime);
    }
    else
    {
        return _rbuffer.pop_front(recv, iWaitTime);
    }
}

size_t TC_EpollServer::BindAdapter::getRecvBufferSize()
{
    if (_bUseShm)
    {
        return _shmQueue.getQueueSize();
    }
    else
    {
        return _rbuffer.size();
    }
}

TC_EpollServer* TC_EpollServer::BindAdapter::getEpollServer()
{
    return _pEpollServer;
}

int TC_EpollServer::BindAdapter::echo_protocol(string &r, string &o)
{
    o = r;

    r = "";

    return 1;
}

int TC_EpollServer::BindAdapter::echo_header_filter(int i, string &o)
{
    return 1;
}

void TC_EpollServer::BindAdapter::setHandleGroupName(const string& handleGroupName)
{
    _handleGroupName = handleGroupName;
}

string TC_EpollServer::BindAdapter::getHandleGroupName() const
{
    return _handleGroupName;
}

void TC_EpollServer::BindAdapter::setName(const string &name)
{
    TC_ThreadLock::Lock lock(*this);

    _name = name;
}

string TC_EpollServer::BindAdapter::getName() const
{
    return _name;
}

int TC_EpollServer::BindAdapter::getHandleNum()
{
    return _iHandleNum;
}

void TC_EpollServer::BindAdapter::setHandleNum(int n)
{
    TC_ThreadLock::Lock lock(*this);

    _iHandleNum = n;
}

int TC_EpollServer::BindAdapter::getQueueCapacity() const
{
    return _iQueueCapacity;
}

void TC_EpollServer::BindAdapter::setQueueCapacity(int n)
{
    _iQueueCapacity = n;
}

bool TC_EpollServer::BindAdapter::isOverload()
{
    return _iQueueCapacity > 0 && (int)_rbuffer.size() > _iQueueCapacity;
}

void TC_EpollServer::BindAdapter::setQueueTimeout(int t)
{
    if (t >= MIN_QUEUE_TIMEOUT)
    {
        _iQueueTimeout = t/1000;
    }
    else
    {
        _iQueueTimeout = MIN_QUEUE_TIMEOUT/1000;
    }
}

int TC_EpollServer::BindAdapter::getQueueTimeout() const
{
    return _iQueueTimeout*1000;
}

void TC_EpollServer::BindAdapter::setEndpoint(const string &str)
{
    TC_ThreadLock::Lock lock(*this);

    _ep.parse(str);
}

TC_Endpoint TC_EpollServer::BindAdapter::getEndpoint() const
{
    return _ep;
}

TC_Socket& TC_EpollServer::BindAdapter::getSocket()
{
    return _s;
}

void TC_EpollServer::BindAdapter::setMaxConns(int iMaxConns)
{
    _iMaxConns = iMaxConns;
}

size_t TC_EpollServer::BindAdapter::getMaxConns() const
{
    return _iMaxConns;
}

void TC_EpollServer::BindAdapter::setHeartBeatTime(time_t t)
{
    TC_ThreadLock::Lock lock(*this);

    _iHeartBeatTime = t;
}

time_t TC_EpollServer::BindAdapter::getHeartBeatTime() const
{
    return _iHeartBeatTime;
}

void TC_EpollServer::BindAdapter::setOrder(EOrder eOrder)
{
    _eOrder = eOrder;
}

void TC_EpollServer::BindAdapter::setAllow(const vector<string> &vtAllow)
{
    TC_ThreadLock::Lock lock(*this);

    _vtAllow = vtAllow;
}

void TC_EpollServer::BindAdapter::setDeny(const vector<string> &vtDeny)
{
    TC_ThreadLock::Lock lock(*this);

    _vtDeny = vtDeny;
}

TC_EpollServer::BindAdapter::EOrder TC_EpollServer::BindAdapter::getOrder() const
{
    return _eOrder;
}

vector<string> TC_EpollServer::BindAdapter::getAllow() const
{
    TC_ThreadLock::Lock lock(*this);

    return _vtAllow;
}

vector<string> TC_EpollServer::BindAdapter::getDeny() const
{
    TC_ThreadLock::Lock lock(*this);

    return _vtDeny;
}

bool TC_EpollServer::BindAdapter::isLimitMaxConnection() const
{
    return _iCurConns + 1 > _iMaxConns;
}

void TC_EpollServer::BindAdapter::decreaseNowConnection()
{
    _iCurConns.dec();
}

void TC_EpollServer::BindAdapter::increaseNowConnection()
{
    _iCurConns.inc();
}

size_t TC_EpollServer::BindAdapter::getNowConnection() const
{
    return _iCurConns.get();
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::BindAdapter::getConnStatus()
{
    return _pEpollServer->getConnStatus(_s.getfd());
}

void TC_EpollServer::BindAdapter::setProtocol(const TC_EpollServer::protocol_functor &pf, int iHeaderLen, const header_filter_functor &hf)
{
    _pf = pf;

    _hf = hf;

    _iHeaderLen = iHeaderLen;
}

TC_EpollServer::protocol_functor& TC_EpollServer::BindAdapter::getProtocol()
{
    return _pf;
}

TC_EpollServer::header_filter_functor& TC_EpollServer::BindAdapter::getHeaderFilterFunctor()
{
    return _hf;
}

int TC_EpollServer::BindAdapter::getHeaderFilterLen()
{
    return _iHeaderLen;
}

void TC_EpollServer::BindAdapter::initBusCommu(BusCommu * pcommu)
{
    //打开从客户端到服务端的FIFO以及Mmap
    if (pcommu->c2sFifo.open(pcommu->c2sFifoName, TC_Fifo::EM_READ) != 0)
    {
        throw TC_Exception("open read fifo (" + pcommu->c2sFifoName + ") fail", errno);
    }

    pcommu->c2sMmap.mmap(pcommu->c2sMmapName.c_str(), pcommu->c2sMmapSize);

    if (pcommu->c2sMmap.iscreate())
    {
        pcommu->c2sMemQueue.create ((char*) pcommu->c2sMmap.getPointer(), pcommu->c2sMmapSize);
    }
    else
    {
cout << "begin attach c2s" <<endl;
        pcommu->c2sMemQueue.attach((char*)pcommu->c2sMmap.getPointer(), pcommu->c2sMmapSize);
cout << "end attach c2s" <<endl;
    }


    //打开从服务端到的客户端FIFO以及Mmap
    if (pcommu->s2cFifo.open(pcommu->s2cFifoName, TC_Fifo::EM_WRITE) != 0 && errno != ENXIO)
    {
        throw TC_Exception("open write fifo (" + pcommu->s2cFifoName + ") fail", errno);
    }

    pcommu->s2cMmap.mmap(pcommu->s2cMmapName.c_str(), pcommu->s2cMmapSize);

    if (pcommu->s2cMmap.iscreate())
    {
        pcommu->s2cMemQueue.create ((char*)pcommu->s2cMmap.getPointer(), pcommu->s2cMmapSize);
    }
    else
    {
cout << "begin attach s2c" <<endl;
        pcommu->s2cMemQueue.attach((char*)pcommu->s2cMmap.getPointer(), pcommu->s2cMmapSize);
cout << "end attach s2c" <<endl;
    }
}

void TC_EpollServer::BindAdapter::rstBusConnection(BusCommu * pcommu)
{
	BusCommu * pNewCommu;
    try
    {
        pNewCommu   = new BusCommu();
        pNewCommu->CommuKey    = pcommu->CommuKey;
        pNewCommu->c2sFifoName = pcommu->c2sFifoName;
        pNewCommu->c2sMmapName = pcommu->c2sMmapName;
        pNewCommu->c2sMmapSize = pcommu->c2sMmapSize;
        pNewCommu->s2cFifoName = pcommu->s2cFifoName;
        pNewCommu->s2cMmapName = pcommu->s2cMmapName;
        pNewCommu->s2cMmapSize = pcommu->s2cMmapSize;

        delete pcommu;

        initBusCommu(pNewCommu);

        _mapBusCommu[pNewCommu->CommuKey] = pNewCommu;

        //将该通讯组件放入到Epoll事件中
        _pEpollServer->addBusConnection(new BusConnection(this, pNewCommu));
    }
    catch (TC_Exception & ex)
    {
        _pEpollServer->debug(string("[TAF][rstBusConnection]Exception:") + ex.what());
		if(pNewCommu)
			delete pNewCommu;
        throw ex;
    }
}

TC_EpollServer::BindAdapter::BusCommu * TC_EpollServer::BindAdapter::connectBusCommu(const string & sIdName, const string & sCommuKey)
{
    //检查当前绑定端口是否允许使用共享内存通许
    if (_bUseBusCommu == false)
    {
        throw TC_Exception("server config: don't use bus commu");
    }

    //查询当前已经建立的通讯组件
    std::map<std::string, BusCommu *>::iterator it = _mapBusCommu.find(sCommuKey);
    if (it != _mapBusCommu.end())
    {
        return it->second;
    }

    //该关键字上的共享内存通信还没打开，重新打开或者新建一个
	BusCommu * pcommu;
    try
    {
        pcommu   = new BusCommu();
        static int iNo = 1;

        //查询是否是以前建立过的通讯组件
        if (TC_File::isFileExist(_sBusCommuPath + "/" + sCommuKey + "_c2s.mmap"))
        {
            pcommu->CommuKey= sCommuKey;
        }
        else
        {
            pcommu->CommuKey= sIdName + "_" + TC_Common::tm2str(time(NULL)) + "_" + TC_Common::tostr(iNo++);
        }

        //打开FIFO以及Mmap
        pcommu->c2sFifoName = _sBusCommuPath + "/" + pcommu->CommuKey + "_c2s.fifo";
        pcommu->c2sMmapName = _sBusCommuPath + "/" + pcommu->CommuKey + "_c2s.mmap";
        pcommu->c2sMmapSize = _sizeBusCommu;
        pcommu->s2cFifoName = _sBusCommuPath + "/" + pcommu->CommuKey + "_s2c.fifo";
        pcommu->s2cMmapName = _sBusCommuPath + "/" + pcommu->CommuKey + "_s2c.mmap";
        pcommu->s2cMmapSize = _sizeBusCommu;

        //初始化共享内存通讯组件
        initBusCommu(pcommu);

        _mapBusCommu[pcommu->CommuKey] = pcommu;

        //将该通讯组件放入到Epoll事件中
        _pEpollServer->addBusConnection(new BusConnection(this, pcommu));

        return pcommu;
    }
    catch (TC_Exception & ex)
    {
        _pEpollServer->error("[TAF]connectBusCommu" + sIdName + "|" + sCommuKey + "|Exception: " + ex.what());
		if(pcommu)
			delete pcommu;
        throw ex;
    }

    return NULL;
}

TC_EpollServer::BindAdapter::BusCommu * TC_EpollServer::BindAdapter::connectBusCommu(const string & sCommuKey)
{
    //检查当前绑定端口是否允许使用共享内存通许
    if (_bUseBusCommu == false)
    {
        _pEpollServer->debug("server config: don't use bus commu, " + sCommuKey);
		return NULL;
    }

    //查询当前已经建立的通讯组件
    std::map<std::string, BusCommu *>::iterator it = _mapBusCommu.find(sCommuKey);
    if (it != _mapBusCommu.end())
    {
        return it->second;
    }

	BusCommu * pcommu;
    //该关键字上的共享内存通信还没打开，重新打开或者新建一个
    try
    {
        pcommu   = new BusCommu();

        //查询是否是以前建立过的通讯组件
        if (TC_File::isFileExist(_sBusCommuPath + "/" + sCommuKey + "_c2s.mmap"))
        {
            pcommu->CommuKey= sCommuKey;
        }
        else
        {
			_pEpollServer->error(sCommuKey + "_c2s.mmap not exist");
			delete pcommu;
            return NULL;
        }

        //打开FIFO以及Mmap
        pcommu->c2sFifoName = _sBusCommuPath + "/" + pcommu->CommuKey + "_c2s.fifo";
        pcommu->c2sMmapName = _sBusCommuPath + "/" + pcommu->CommuKey + "_c2s.mmap";
        pcommu->c2sMmapSize = _sizeBusCommu;
        pcommu->s2cFifoName = _sBusCommuPath + "/" + pcommu->CommuKey + "_s2c.fifo";
        pcommu->s2cMmapName = _sBusCommuPath + "/" + pcommu->CommuKey + "_s2c.mmap";
        pcommu->s2cMmapSize = _sizeBusCommu;

        //初始化共享内存通讯组件
cout << "begin initBusCommu" << endl;
        initBusCommu(pcommu);
cout << "end initBusCommu" << endl;
        _mapBusCommu[pcommu->CommuKey] = pcommu;

        //将该通讯组件放入到Epoll事件中
cout << "begin addBusConnection" << endl;
        _pEpollServer->addBusConnection(new BusConnection(this, pcommu));
cout << "end addBusConnection" << endl;

        return pcommu;
    }
    catch (TC_Exception & ex)
    {
        _pEpollServer->error("[TAF]connectBusCommu "  + sCommuKey + "|Exception: " + ex.what());
		if(pcommu)
			delete pcommu;
    }

    return NULL;
}

////////////////////////////////////////////////////////////////
//
TC_EpollServer::ConnectionList::ConnectionList(TC_EpollServer *pEpollServer)
:_pEpollServer(pEpollServer)
,_total(0)
,_free_size(0)
,_vConn(NULL)
,_lastTimeoutTime(0)
,_iConnectionMagic(0)
{
}

void TC_EpollServer::ConnectionList::init(uint32_t size)
{
    _lastTimeoutTime = TC_TimeProvider::getInstance()->getNow();

    _total = size;

    _free_size  = 0;

    //初始化链接链表
    if(_vConn) delete[] _vConn;

    //分配total+1个空间(多分配一个空间, 第一个空间其实无效)
    _vConn = new list_data[_total+1];

    _iConnectionMagic   = ((uint32_t)_lastTimeoutTime) << 20;

    //free从1开始分配, 这个值为uid, 0保留为管道用, epollwait根据0判断是否是管道消息
    for(uint32_t i = 1; i <= _total; i++)
    {
        _vConn[i].first = NULL;

        _free.push_back(i);

        ++_free_size;
    }
}

uint32_t TC_EpollServer::ConnectionList::getUniqId()
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t uid = _free.front();

    assert(uid > 0 && uid <= _total);

    _free.pop_front();

    --_free_size;

    return _iConnectionMagic | uid;
}

TC_EpollServer::Connection* TC_EpollServer::ConnectionList::get(uint32_t uid)
{
    uint32_t magi = uid & (0xFFFFFFFF << 20);
    uid           = uid & (0x7FFFFFFF >> 11);

    if (magi != _iConnectionMagic) return NULL;

    return _vConn[uid].first;
}

void TC_EpollServer::ConnectionList::add(Connection *cPtr, time_t iTimeOutStamp)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t muid = cPtr->getId();
    uint32_t magi = muid & (0xFFFFFFFF << 20);
    uint32_t uid  = muid & (0x7FFFFFFF >> 11);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && !_vConn[uid].first);

    _vConn[uid] = make_pair(cPtr, _tl.insert(make_pair(iTimeOutStamp, uid)));
}

void TC_EpollServer::ConnectionList::refresh(uint32_t uid, time_t iTimeOutStamp)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t magi = uid & (0xFFFFFFFF << 20);
    uid           = uid & (0x7FFFFFFF >> 11);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && _vConn[uid].first);

    if(iTimeOutStamp - _vConn[uid].first->_iLastRefreshTime < 1 || _vConn[uid].first->getType() == Connection::EM_BUS)
    {
        return;
    }
    _vConn[uid].first->_iLastRefreshTime = iTimeOutStamp;

    //删除超时链表
    _tl.erase(_vConn[uid].second);

    _vConn[uid].second = _tl.insert(make_pair(iTimeOutStamp, uid));
}

void TC_EpollServer::ConnectionList::checkTimeout(time_t iCurTime)
{
    //至少1s才能检查一次
    if(iCurTime - _lastTimeoutTime < 1)
    {
        return;
    }

    _lastTimeoutTime = iCurTime;

    TC_ThreadLock::Lock lock(*this);

    multimap<time_t, uint32_t>::iterator it = _tl.begin();

    while(it != _tl.end())
    {
        //已经检查到当前时间点了, 后续不用在检查了
        if(it->first > iCurTime)
        {
            break;
        }

        uint32_t uid = it->second;

        ++it;

        //udp的监听端口, 不做处理
        if(_vConn[uid].first->getListenfd() == -1 || _vConn[uid].first->getType() == Connection::EM_BUS)
        {
            continue;
        }

        //超时关闭
        _pEpollServer->delConnection(_vConn[uid].first, false);

        //从链表中删除
        _del(uid);
    }


	if(_pEpollServer->IsEmptyConnCheck())
	{
		it = _tl.begin();
		while(it != _tl.end())
		{
			uint32_t uid = it->second;

			//遍历所有的空连接
			if(_vConn[uid].first->IsEmptyConn())
			{
				//获取空连接的超时时间点
				time_t iEmptyTimeout = (it->first - _vConn[uid].first->getTimeout()) + (_pEpollServer->getEmptyConnTimeout()/1000);

				 //已经检查到当前时间点了, 后续不用在检查了
				if(iEmptyTimeout > iCurTime)
				{
					break;
				}

				//udp的监听端口, 不做处理
				if(_vConn[uid].first->getListenfd() == -1 || _vConn[uid].first->getType() == Connection::EM_BUS)
				{
					++it;
					continue;
				}

				//超时关闭
				_pEpollServer->delConnection(_vConn[uid].first, false);

				//从链表中删除
				_del(uid);
			}

			++it;
		}
	}
}

vector<TC_EpollServer::ConnStatus> TC_EpollServer::ConnectionList::getConnStatus(int lfd)
{
    vector<TC_EpollServer::ConnStatus> v;

    TC_ThreadLock::Lock lock(*this);

    for(size_t i = 1; i <= _total; i++)
    {
        //是当前监听端口的连接
        if(_vConn[i].first != NULL && _vConn[i].first->getListenfd() == lfd)
        {
            TC_EpollServer::ConnStatus cs;

            cs.iLastRefreshTime    = _vConn[i].first->_iLastRefreshTime;
            cs.ip                  = _vConn[i].first->getIp();
            cs.port                = _vConn[i].first->getPort();
            cs.timeout             = _vConn[i].first->getTimeout();
            cs.uid                 = _vConn[i].first->getId();

            v.push_back(cs);
        }
    }

    return v;
}

void TC_EpollServer::ConnectionList::del(uint32_t uid)
{
    TC_ThreadLock::Lock lock(*this);

    uint32_t magi = uid & (0xFFFFFFFF << 20);
    uid           = uid & (0x7FFFFFFF >> 11);

    assert(magi == _iConnectionMagic && uid > 0 && uid <= _total && _vConn[uid].first);

    _del(uid);
}

void TC_EpollServer::ConnectionList::_del(uint32_t uid)
{
    assert(uid > 0 && uid <= _total && _vConn[uid].first);

    _tl.erase(_vConn[uid].second);

    delete _vConn[uid].first;

    _vConn[uid].first = NULL;

    _free.push_back(uid);

    ++_free_size;
}

size_t TC_EpollServer::ConnectionList::size()
{
    TC_ThreadLock::Lock lock(*this);

    return _total - _free_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 服务连接

TC_EpollServer::Connection::Connection(TC_EpollServer::BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(lfd)
, _timeout(timeout)
, _ip(ip)
, _port(port)
, _sendPos(0)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
,_bEmptyConn(true)
,_pRecvBuffer(NULL)
,_nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)

{
    assert(fd != -1);

    _iLastRefreshTime = TC_TimeProvider::getInstance()->getNow();

    _sock.init(fd, true, AF_INET);
}

TC_EpollServer::Connection::Connection(BindAdapter *pBindAdapter, int fd)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(-1)
, _timeout(2)
, _port(0)
, _sendPos(0)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
,_bEmptyConn(false) /*udp is always false*/
,_pRecvBuffer(NULL)
,_nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)


{
    _iLastRefreshTime = TC_TimeProvider::getInstance()->getNow();

    _sock.init(fd, false, AF_INET);
}

TC_EpollServer::Connection::Connection(BindAdapter *pBindAdapter)
: _pBindAdapter(pBindAdapter)
, _uid(0)
, _lfd(-1)
, _timeout(0)
, _port(0)
, _sendPos(0)
, _iHeaderLen(0)
, _bClose(false)
, _iMaxTemQueueSize(100)
, _enType(EM_TCP)
,_bEmptyConn(false) /*udp is always false*/
,_pRecvBuffer(NULL)
,_nRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)

{
    _iLastRefreshTime = TC_TimeProvider::getInstance()->getNow();
}
TC_EpollServer::Connection::~Connection()
{
	if(_pRecvBuffer)
	{
		delete _pRecvBuffer;
		_pRecvBuffer = NULL;
	}

    if(_lfd != -1)
    {
        assert(!_sock.isValid());
    }
}

void TC_EpollServer::Connection::close()
{
    if(_lfd != -1)
    {
        if(_sock.isValid())
        {
            _sock.close();
        }
    }
}

void TC_EpollServer::Connection::insertRecvQueue(recv_queue::queue_type &vRecvData)
{
    if(!vRecvData.empty())
    {
        //服务队列已超载  数据放在队列头部
        if(_pBindAdapter->isOverload())
        {
            recv_queue::queue_type::iterator it = vRecvData.begin();

            recv_queue::queue_type::iterator itEnd = vRecvData.end();

            while(it != itEnd)
            {
               (*it)->isOverload = true;

               ++it;
            }
            _pBindAdapter->insertRecvQueue(vRecvData,false);
        }
        else
        {
            _pBindAdapter->insertRecvQueue(vRecvData);
        }
    }
}

int TC_EpollServer::Connection::parseProtocol(recv_queue::queue_type &o)
{
    try
    {
        while (true)
        {
            //需要过滤首包包头
            if(_iHeaderLen > 0)
            {
                if(_recvbuffer.length() >= (unsigned) _iHeaderLen)
                {
                    string header = _recvbuffer.substr(0, _iHeaderLen);
                    _pBindAdapter->getHeaderFilterFunctor()((int)(TC_EpollServer::PACKET_FULL), header);
                    _recvbuffer = _recvbuffer.substr(_iHeaderLen);
                    _iHeaderLen = 0;
                }
                else
                {
                    _pBindAdapter->getHeaderFilterFunctor()((int)(TC_EpollServer::PACKET_LESS), _recvbuffer);
                    _iHeaderLen -= _recvbuffer.length();
                    _recvbuffer = "";
                    break;
                }
            }

            string ro;

            int b = _pBindAdapter->getProtocol()(_recvbuffer, ro);

            if(b == TC_EpollServer::PACKET_LESS)
            {
                //包不完全
                break;
            }
            else if(b == TC_EpollServer::PACKET_FULL)
            {
                tagRecvData* recv = new tagRecvData();
                recv->buffer           = ro;
                recv->ip               = _ip;
                recv->port             = _port;
                recv->recvTimeStamp    = TC_TimeProvider::getInstance()->getNow();
                recv->uid              = getId();
                recv->isOverload       = false;
                recv->isClosed         = false;

				//收到完整的包才算
				this->_bEmptyConn = false;

                //收到完整包
                o.push_back(recv);

                if((int) o.size() > _iMaxTemQueueSize)
                {
                    insertRecvQueue(o);
                    o.clear();
                }

                if(_recvbuffer.empty())
                {
                    break;
                }
            }
            else
            {
                _pBindAdapter->getEpollServer()->error("recv [" + _ip + ":" + TC_Common::tostr(_port) + "],packet error.");
                return -1;                      //协议解析错误
            }
        }
    }
    catch(exception &ex)
    {
        _pBindAdapter->getEpollServer()->error("recv protocol error:" + string(ex.what()));
        return -1;
    }
    catch(...)
    {
        _pBindAdapter->getEpollServer()->error("recv protocol error");
        return -1;
    }

    return o.size();
}

int TC_EpollServer::Connection::recv(recv_queue::queue_type &o)
{
    o.clear();

    char buffer[8192] = "\0";

    while(true)
    {
        int iBytesReceived = 0;

        if(_lfd == -1)
        {
			if(_pRecvBuffer)
			{
				iBytesReceived = _sock.recvfrom((void*)_pRecvBuffer,_nRecvBufferSize, _ip, _port, 0);
			}
			else
			{
				iBytesReceived = _sock.recvfrom((void*)buffer,sizeof(buffer), _ip, _port, 0);
			}
		}
        else
        {
            iBytesReceived = ::read(_sock.getfd(), (void*)buffer, sizeof(buffer));
        }

        if (iBytesReceived < 0)
        {
            if(errno == EAGAIN)
            {
                //没有数据了
                break;
            }
            else
            {
                //客户端主动关闭
                _pBindAdapter->getEpollServer()->debug("recv [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection");
                return -1;
            }
        }
        else if( iBytesReceived == 0)
        {
            //客户端主动关闭
            _pBindAdapter->getEpollServer()->debug("recv [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection");
            return -1;
        }

        //保存接收到数据
        if(_lfd == -1)
        {
			if(_pRecvBuffer)
			{
				_recvbuffer.append(_pRecvBuffer, iBytesReceived);
			}
			else
			{
				_recvbuffer.append(buffer, iBytesReceived);
			}
        }
		else
		{
			_recvbuffer.append(buffer, iBytesReceived);
		}

        //UDP协议
        if(_lfd == -1)
        {
            if(_pBindAdapter->isIpAllow(_ip) == true)
            {
                 parseProtocol(o);
            }
            else
            {
                 //udp ip无权限
                _pBindAdapter->getEpollServer()->debug("accept [" + _ip + ":" + TC_Common::tostr(_port) + "] [" + TC_Common::tostr(_lfd) + "] not allowed");
            }
            _recvbuffer = "";
        }
        else
        {
            //接收到数据不超过buffer,没有数据了(如果有数据,内核会再通知你)
            if((size_t)iBytesReceived < sizeof(buffer))
            {
                break;
            }
            //字符串太长时substr性能会急剧下降
            if(_recvbuffer.length() > 8192)
            {
                parseProtocol(o);
            }
        }
    }

    if(_lfd != -1)
    {
        return parseProtocol(o);
    }

    return o.size();
}

int TC_EpollServer::Connection::send(const string& buffer, const string &ip, uint16_t port)
{
	if(_lfd == -1)
	{
		int iRet = _sock.sendto((const void*) buffer.c_str(), buffer.length(), ip, port, 0);
		if(iRet < 0)
		{
			_pBindAdapter->getEpollServer()->error("[TC_EpollServer::Connection] send [" + _ip + ":" + TC_Common::tostr(_port) + "] error");
			return -1;
		}
		return 0;
	}
    _sendbuffer += buffer;

    size_t pos = 0;
	size_t sendLen = _sendbuffer.length() - _sendPos;

	const char *sendBegin = _sendbuffer.c_str() + _sendPos;

    while (pos < sendLen )
    {
        int iBytesSent = 0;

        iBytesSent = write(_sock.getfd(), (const void*)(sendBegin + pos), sendLen - pos);

        if (iBytesSent < 0)
        {
            if(errno == EAGAIN)
            {
                break;
            }
            else
            {
                _pBindAdapter->getEpollServer()->debug("send [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection by peer.");
                return -1;
            }
        }

        pos += iBytesSent;

        //发送的数据小于需要发送的,break, 内核会再通知你的
        if(pos < sendLen)
        {
            break;
        }
    }

    if(pos > 0)
    {
		if(_sendbuffer.length() > 8192)
		{
			_sendbuffer = _sendbuffer.substr(_sendPos + pos);
			_sendPos = 0;
		}
        else
		{
			_sendPos += pos;
		}
    }

    //需要关闭链接
    if(_bClose && ( (_sendbuffer.length() - _sendPos) == 0 ) )
    {
        _pBindAdapter->getEpollServer()->debug("send [" + _ip + ":" + TC_Common::tostr(_port) + "] close connection by user.");
		_sendbuffer.clear();
		_sendPos = 0;
        return -1;
    }

    return 0;
}

int TC_EpollServer::Connection::send()
{
    if(_sendbuffer.length() == 0) return 0;

//    assert(_lfd != -1);

    return send("", _ip, _port);
}
bool TC_EpollServer::Connection::setRecvBuffer(size_t nSize)
{
	//only udp type needs to malloc
	if(_lfd == -1 && !_pRecvBuffer)
	{
		_nRecvBufferSize = nSize;

		_pRecvBuffer = new char[_nRecvBufferSize];
		if(!_pRecvBuffer)
		{
			throw TC_Exception("adapter '" + _pBindAdapter->getName() + "' malloc udp receive buffer fail");
		}
	}
	return true;
}

/**************************************************************************************************
 * 共享内存通讯组件的连接
 *
 */
TC_EpollServer::BusConnection::BusConnection(TC_EpollServer::BindAdapter * pBindAdapter, TC_EpollServer::BindAdapter::BusCommu * pcommu) : Connection(pBindAdapter), _pcommu(pcommu)
{
    _enType = EM_BUS;
}

TC_EpollServer::BusConnection::~BusConnection()
{

}

int TC_EpollServer::BusConnection::send(const string & buffer, const string & ip, uint16_t port)
{
    try
    {
        //如果写管道还没有打开，需要重新打开
        if (_pcommu->s2cFifo.fd() < 0)
        {
            _pcommu->s2cFifo.open(_pcommu->s2cFifoName, TC_Fifo::EM_WRITE);
        }

		list<ResponsePacket> done;
		ProxyProtocol::tafResponse(buffer.c_str(), buffer.length(), done);
		string sid;
		for(list<ResponsePacket>::iterator it = done.begin(); it != done.end(); ++it)
		{
			sid += TC_Common::tostr(it->iRequestId) + " ";
		}

        if(!_pcommu->s2cMemQueue.push(buffer))
		{
			_pBindAdapter->getEpollServer()->error(string("[TAF][BusConnection send] push queue error, maybe queue full"));
		}
        _pcommu->s2cFifo.write(" ", 1);
    }
    catch (TC_Exception & ex)
    {
        _pBindAdapter->getEpollServer()->error(string("[TAF][BusConnection send]|") + ex.what());
    }

    return 0;
}

int TC_EpollServer::BusConnection::recv(recv_queue::queue_type & o)
{
	o.clear();

	char szBuff[2048];
	bool bClose = false;
	for ( ; true; )
	{
		memset(szBuff, 0, sizeof(szBuff));

		int iLen = _pcommu->c2sFifo.read(szBuff, sizeof(szBuff));

		if (iLen < 0)
		{
			bClose = errno == EAGAIN?false:true;

			if(bClose)
			{
				ostringstream os;
				os << __FILE__ << "|" << __LINE__ << "|recv (" << _pcommu->c2sFifoName << ")|EN:" << errno << "|ES:" << strerror(errno) << endl;
				_pBindAdapter->getEpollServer()->debug(os.str());
			}
			break;
		}
		else if (iLen == 0)
		{
			bClose = false;
			break;
		}
	}

    while(true)
    {
		std::string sBuff;
		if (!_pcommu->c2sMemQueue.pop(sBuff))
		{
			break;
		}
		//保存接收到数据
        _recvbuffer.append(sBuff);


		if(_recvbuffer.length() > 8192)
		{
			parseProtocol(o);
		}

	}
    parseProtocol(o);

    return o.size();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
TC_EpollServer::TC_EpollServer()
: _bTerminate(false)
, _handleStarted(false)
, _createEpoll(false)
, _list(this)
, _pLocalLogger(NULL)
,_bEmptyConnAttackCheck(false)
,_iEmptyCheckTimeout(MIN_EMPTY_CONN_TIMEOUT)
,_nUdpRecvBufferSize(DEFAULT_RECV_BUFFERSIZE)
{
    _shutdown.createSocket();

    _notify.createSocket();
}

TC_EpollServer::~TC_EpollServer()
{
    map<int, BindAdapterPtr>::iterator it = _listeners.begin();

    while(it != _listeners.end())
    {
        ::close(it->first);

        ++it;
    }
    _listeners.clear();
}

map<int, TC_EpollServer::BindAdapterPtr> TC_EpollServer::getListenSocketInfo()
{
    return _listeners;
}

void TC_EpollServer::debug(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->debug() << "[TAF]" << s << endl;
    }
}

void TC_EpollServer::info(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->info() << "[TAF]" << s << endl;
    }
}

void TC_EpollServer::error(const string &s)
{
    if(_pLocalLogger)
    {
        _pLocalLogger->error() << "[TAF]" << s << endl;
    }
}

void TC_EpollServer::EnAntiEmptyConnAttack(bool bEnable)
{
	_bEmptyConnAttackCheck  = bEnable;
}

void TC_EpollServer::setEmptyConnTimeout(int timeout)
{
	_iEmptyCheckTimeout = (timeout>=MIN_EMPTY_CONN_TIMEOUT)?timeout:MIN_EMPTY_CONN_TIMEOUT;
}

void TC_EpollServer::setUdpRecvBufferSize(size_t nSize)
{
	_nUdpRecvBufferSize = (nSize >= 8192 && nSize <=DEFAULT_RECV_BUFFERSIZE)?nSize:DEFAULT_RECV_BUFFERSIZE;
}

bool TC_EpollServer::IsEmptyConnCheck() const
{
	return 	_bEmptyConnAttackCheck;
}

int TC_EpollServer::getEmptyConnTimeout() const
{
	return _iEmptyCheckTimeout;
}

int TC_EpollServer::bind(BindAdapterPtr &lsPtr)
{
    map<int, BindAdapterPtr>::iterator it = _listeners.begin();

    while (it != _listeners.end())
    {
        if(it->second->getName() == lsPtr->getName())
        {
            throw TC_Exception("bind name '" + lsPtr->getName() + "' conflicts.");
        }
        ++it;
    }

    const TC_Endpoint &ep = lsPtr->getEndpoint();

    TC_Socket& s = lsPtr->getSocket();

    bind(ep, s);

    _listeners[s.getfd()] = lsPtr;

    return s.getfd();
}

TC_EpollServer::BindAdapterPtr TC_EpollServer::getBindAdapter(const string &sName)
{
    map<int, BindAdapterPtr>::iterator it = _listeners.begin();

    while(it != _listeners.end())
    {
        if(it->second->getName() == sName)
        {
            return it->second;
        }

        ++it;
    }
    return NULL;
}

void TC_EpollServer::bind(const TC_Endpoint &ep, TC_Socket &s)
{
    int type = ep.isUnixLocal()?AF_LOCAL:AF_INET;

    if(ep.isTcp())
    {
        s.createSocket(SOCK_STREAM, type);
    }
    else
    {
        s.createSocket(SOCK_DGRAM, type);
    }

    //设置网络qos的dscp标志
    if(0 != ep.getQos())
    {
        int iQos=ep.getQos();
        s.setSockOpt(IP_TOS,&iQos,sizeof(iQos),SOL_IP);
    }

    if(ep.isUnixLocal())
    {
        s.bind(ep.getHost().c_str());
    }
    else
    {
        s.bind(ep.getHost(), ep.getPort());
    }

    if(ep.isTcp() && !ep.isUnixLocal())
    {
        s.listen(1024);
        s.setKeepAlive();
        s.setTcpNoDelay();
        //不要设置close wait否则http服务回包主动关闭连接会有问题
        s.setNoCloseWait();
    }

    s.setblock(false);
}

void TC_EpollServer::startHandle()
{
    if (!_handleStarted)
    {
        _handleStarted = true;

        map<string, HandleGroupPtr>::iterator it;

        for (it = _handleGroups.begin(); it != _handleGroups.end(); ++it)
        {
            vector<HandlePtr>& hds = it->second->handles;

            for (uint32_t i = 0; i < hds.size(); ++i)
            {
                if (!hds[i]->isAlive())
                {
                    hds[i]->start();
                }
            }
        }
    }
}

void TC_EpollServer::stopThread()
{
    map<string, HandleGroupPtr>::iterator it;

    for (it = _handleGroups.begin(); it != _handleGroups.end(); ++it)
    {
        {
            TC_ThreadLock::Lock lock(it->second->monitor);

            it->second->monitor.notifyAll();
        }
        vector<HandlePtr>& hds = it->second->handles;

        for (uint32_t i = 0; i < hds.size(); ++i)
        {
            if (hds[i]->isAlive())
            {
                hds[i]->getThreadControl().join();
            }
        }
    }
}

void TC_EpollServer::createEpoll()
{
	if(!_createEpoll)
	{
		_createEpoll = true;
		//创建epoll
		_epoller.create(10240);

		_epoller.add(_shutdown.getfd(), H64(ET_CLOSE), EPOLLIN);
		_epoller.add(_notify.getfd(), H64(ET_NOTIFY), EPOLLIN);

		size_t maxAllConn   = 0;
		bool   hasUdp       = false;

		//监听socket
		map<int, BindAdapterPtr>::iterator it = _listeners.begin();

		while(it != _listeners.end())
		{
			if(it->second->getEndpoint().isTcp())
			{
				//获取最大连接数
				maxAllConn += it->second->getMaxConns();

				_epoller.add(it->first, H64(ET_LISTEN) | it->first, EPOLLIN);
			}
			else
			{
				maxAllConn++;
				hasUdp = true;
			}

			++it;
		}

		//初始化连接管理链表
		_list.init(maxAllConn);

		if(hasUdp)
		{
			//监听socket
			map<int, BindAdapterPtr>::iterator it = _listeners.begin();

			while(it != _listeners.end())
			{
				if(!it->second->getEndpoint().isTcp())
				{
					Connection *cPtr = new Connection(it->second.get(), it->first);
					//udp分配接收buffer
					cPtr->setRecvBuffer(_nUdpRecvBufferSize);

					addUdpConnection(cPtr);
				}

				++it;
			}
		}
	}
}

void TC_EpollServer::terminate()
{
    _bTerminate = true;

    //通知队列醒过来
    _sbuffer.notifyT();

    //通知epoll响应, 关闭连接
    _epoller.mod(_shutdown.getfd(), H64(ET_CLOSE), EPOLLOUT);
}

bool TC_EpollServer::accept(int fd)
{
    struct sockaddr_in stSockAddr;

    socklen_t iSockAddrSize = sizeof(sockaddr_in);

    TC_Socket cs;

    cs.setOwner(false);

    //接收连接
    TC_Socket s;

    s.init(fd, false, AF_INET);

    int iRetCode = s.accept(cs, (struct sockaddr *) &stSockAddr, iSockAddrSize);

    if (iRetCode > 0)
    {
        string  ip;

        uint16_t port;

        char sAddr[INET_ADDRSTRLEN] = "\0";

        struct sockaddr_in *p = (struct sockaddr_in *)&stSockAddr;

        inet_ntop(AF_INET, &p->sin_addr, sAddr, sizeof(sAddr));

        ip      = sAddr;
        port    = ntohs(p->sin_port);

        debug("accept [" + ip + ":" + TC_Common::tostr(port) + "] [" + TC_Common::tostr(cs.getfd()) + "] incomming");

        if(!_listeners[fd]->isIpAllow(ip))
        {
            debug("accept [" + ip + ":" + TC_Common::tostr(port) + "] [" + TC_Common::tostr(cs.getfd()) + "] not allowed");

            cs.close();

            return true;
        }

        if(_listeners[fd]->isLimitMaxConnection())
        {
            cs.close();

            debug("accept [" + ip + ":" + TC_Common::tostr(port) + "][" + TC_Common::tostr(cs.getfd()) + "] beyond max connection:" + TC_Common::tostr(_listeners[fd]->getMaxConns()));

            return true;
        }

        cs.setblock(false);
        cs.setKeepAlive();
        cs.setTcpNoDelay();
        cs.setCloseWaitDefault();

        //设置网络qos的dscp标志
        if(0 != _listeners[fd]->getEndpoint().getQos())
        {
            int iQos=_listeners[fd]->getEndpoint().getQos();
            cs.setSockOpt(IP_TOS,&iQos,sizeof(iQos),SOL_IP);
            info("setSockOpt [" + ip + ":" + TC_Common::tostr(port) + "][" + TC_Common::tostr(cs.getfd()) + "] listen server qos is:"+TC_Common::tostr(iQos));
        }

        int timeout = _listeners[fd]->getEndpoint().getTimeout()/1000;

        Connection *cPtr = new Connection(_listeners[fd].get(), fd, (timeout < 2 ? 2 : timeout), cs.getfd(), ip, port);

        addTcpConnection(cPtr);

        //过滤连接首个数据包包头
        cPtr->setHeaderFilterLen(_listeners[fd]->getHeaderFilterLen());

        return true;
    }
    else
    {
        //直到发生EAGAIN才不继续accept
        if(errno == EAGAIN)
        {
            return false;
        }

        return true;
    }
    return true;
}

void TC_EpollServer::addTcpConnection(TC_EpollServer::Connection *cPtr)
{
    uint32_t uid = _list.getUniqId();

    cPtr->init(uid);

    _list.add(cPtr, cPtr->getTimeout() + TC_TimeProvider::getInstance()->getNow());

    cPtr->getBindAdapter()->increaseNowConnection();

    //注意epoll add必须放在最后, 否则可能导致执行完, 才调用上面语句
    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN | EPOLLOUT);
}

void TC_EpollServer::addUdpConnection(TC_EpollServer::Connection *cPtr)
{
    uint32_t uid = _list.getUniqId();

    cPtr->init(uid);

    _list.add(cPtr, cPtr->getTimeout() + TC_TimeProvider::getInstance()->getNow());

    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN | EPOLLOUT);
}

void TC_EpollServer::addBusConnection(TC_EpollServer::Connection *cPtr)
{
    assert(cPtr->getType() == Connection::EM_BUS);

    uint32_t uid = _list.getUniqId();

    cPtr->init(uid);

    _list.add(cPtr, cPtr->getTimeout() + TC_TimeProvider::getInstance()->getNow());

    _epoller.add(cPtr->getfd(), cPtr->getId(), EPOLLIN);
}
vector<TC_EpollServer::ConnStatus> TC_EpollServer::getConnStatus(int lfd)
{
    return _list.getConnStatus(lfd);
}

void TC_EpollServer::delConnection(TC_EpollServer::Connection *cPtr, bool bEraseList)
{
    //如果是共享内存通讯组件关闭了,再次打开继续监听
    if (cPtr->getType() == Connection::EM_BUS)
    {
        BindAdapter * padapter = cPtr->getBindAdapter();
        BindAdapter::BusCommu * pcommu = ((BusConnection *)cPtr)->getBusCommu();

        _epoller.del(cPtr->getfd(), cPtr->getId(), 0);

        _list.del(cPtr->getId());

        padapter->rstBusConnection(pcommu);

        return ;
    }
    //如果是TCP的连接才真正的关闭连接
    if (cPtr->getListenfd() != -1)
    {
        //false的情况,是超时被主动删除
        if(!bEraseList)
        {
            info("timeout [" + cPtr->getIp() + ":" + TC_Common::tostr(cPtr->getPort()) + "] del from list");
        }

        uint32_t uid = cPtr->getId();

        //构造一个tagRecvData，通知业务该连接的关闭事件

        tagRecvData* recv = new tagRecvData();
        recv->adapter    = cPtr->getBindAdapter();
        recv->uid        =  uid;
        recv->ip         = cPtr->getIp();
        recv->port       = cPtr->getPort();
        recv->isClosed   = true;
        recv->isOverload = false;
        recv->recvTimeStamp = TC_TimeProvider::getInstance()->getNow();

        recv_queue::queue_type vRecvData;

        vRecvData.push_back(recv);

        cPtr->getBindAdapter()->insertRecvQueue(vRecvData);

        cPtr->getBindAdapter()->decreaseNowConnection();

        //从epoller删除句柄放在close之前, 否则重用socket时会有问题
        _epoller.del(cPtr->getfd(), uid, 0);

        cPtr->close();

        //对于超时检查, 由于锁的原因, 在这里不从链表中删除
        if(bEraseList)
        {
            _list.del(uid);
        }
    }
}

int TC_EpollServer::sendBuffer(TC_EpollServer::Connection *cPtr, const string &buffer, const string &ip, uint16_t port)
{
    return cPtr->send(buffer, ip, port);
}

int TC_EpollServer::sendBuffer(TC_EpollServer::Connection *cPtr)
{
    return cPtr->send();
}

int TC_EpollServer::recvBuffer(TC_EpollServer::Connection *cPtr, recv_queue::queue_type &v)
{
    return cPtr->recv(v);
}

void TC_EpollServer::close(uint32_t uid)
{
    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 'c';

    _sbuffer.push_back(send);

    //通知epoll响应, 关闭连接
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

void TC_EpollServer::send(uint32_t uid, const string &s, const string &ip, uint16_t port)
{
    if(_bTerminate)
    {
        return;
    }

    tagSendData* send = new tagSendData();

    send->uid = uid;

    send->cmd = 's';

    send->buffer = s;

    send->ip = ip;

    send->port = port;

    _sbuffer.push_back(send);

    //通知epoll响应, 有数据要发送
    _epoller.mod(_notify.getfd(), H64(ET_NOTIFY), EPOLLOUT);
}

void TC_EpollServer::processPipe()
{
    send_queue::queue_type deSendData;

    _sbuffer.swap(deSendData);

    send_queue::queue_type::iterator it = deSendData.begin();

    send_queue::queue_type::iterator itEnd = deSendData.end();

    while(it != itEnd)
    {
        switch((*it)->cmd)
        {
        case 'c':
            {
                Connection *cPtr = getConnectionPtr((*it)->uid);

                if(cPtr)
                {
                    if(cPtr->setClose())
                    {
                        delConnection(cPtr);
                    }
                }
                break;
            }
        case 's':
            {
                Connection *cPtr = getConnectionPtr((*it)->uid);

                if(cPtr)
                {
                    int ret = sendBuffer(cPtr, (*it)->buffer, (*it)->ip, (*it)->port);

                    if(ret < 0)
                    {
                        delConnection(cPtr);
                    }
                }
                break;
            }
        default:
            assert(false);
        }
		delete (*it);
        ++it;
    }
}

void TC_EpollServer::processNet(const epoll_event &ev)
{
    uint32_t uid = ev.data.u32;

    Connection *cPtr = getConnectionPtr(uid);

    if(!cPtr)
    {
        debug("NetThread::processNet connection[" + TC_Common::tostr(uid) + "] not exists.");
        return;
    }

    if ((ev.events & EPOLLERR || ev.events & EPOLLHUP) && cPtr->getType() != Connection::EM_BUS)
    {
        delConnection(cPtr);

        return;
    }

    if(ev.events & EPOLLIN)               //有数据需要读取
    {
        recv_queue::queue_type vRecvData;

        int ret = recvBuffer(cPtr, vRecvData);

        if(ret < 0)
        {
            delConnection(cPtr);

            return;
        }

        if(!vRecvData.empty())
        {
            cPtr->insertRecvQueue(vRecvData);
        }
    }

    if (ev.events & EPOLLOUT)              //有数据需要发送
    {
        int ret = sendBuffer(cPtr);

        if (ret < 0)
        {
            delConnection(cPtr);

            return;
        }
    }

    _list.refresh(uid, cPtr->getTimeout() + TC_TimeProvider::getInstance()->getNow());
}

void TC_EpollServer::waitForShutdown()
{
    startHandle();

    createEpoll();

    //循环监听网路连接请求
    while(!_bTerminate)
    {
        _list.checkTimeout(TC_TimeProvider::getInstance()->getNow());

        int iEvNum = _epoller.wait(2000);

        for(int i = 0; i < iEvNum; ++i)
        {
            try
            {
                const epoll_event &ev = _epoller.get(i);

                uint32_t h = ev.data.u64 >> 32;

                switch(h)
                {
                case ET_LISTEN:
                    {
                        //监听端口有请求
                        map<int, BindAdapterPtr>::const_iterator it = _listeners.find(ev.data.u32);
                        if( it != _listeners.end())
                        {
                            if(ev.events & EPOLLIN)
                            {
                                bool ret;
                                do
                                {
                                    ret = accept(ev.data.u32);
                                }while(ret);
                            }
                        }
                    }
                    break;
                case ET_CLOSE:
                    //关闭请求
                    break;
                case ET_NOTIFY:
                    //发送通知
                    processPipe();
                    break;
                case ET_NET:
                    //网络请求
                    processNet(ev);
                    break;
                default:
                    assert(true);
                }
            }
            catch(exception &ex)
            {
                error("waitForShutdown exception:" + string(ex.what()));
            }
        }
    }
    stopThread();
}
////////////////////////////////////////////////////////////////////////////////////
}
