#include "servant/Transceiver.h"
#include "log/taf_logger.h"

namespace taf
{
///////////////////////////////////////////////////////////////////////
Transceiver::Transceiver(ObjectProxy *op, const EndpointInfo &ep)
: _fd(-1)
, _ep(ep)
, _objectProxy(op)
, _connStatus(eUnconnected)
, _connectFailed(false)
{
    //预分配好一定的内存
    _sendBuffer.reserve(1024);
}

void Transceiver::reInitialize(int fd, ConnectStatus connStatus)
{
    assert(!isValid());

    _fd = fd;

    _connStatus = connStatus;
}

Transceiver::~Transceiver()
{
    close();
}

void Transceiver::close()
{
    if(!isValid()) return;

    NetworkUtil::closeSocketNoThrow(_fd);

    _connStatus = eUnconnected;

    _fd         = -1;

    _connectFailed = false;

    _sendBuffer.clear();

    _recvBuffer.clear();

    LOGINFO("[TAF][trans close:" << _objectProxy->name() << "," << _ep.desc() << "]" << endl);
}

int Transceiver::fd() const
{
    return _fd;
}

bool Transceiver::isValid() const
{
    return (_fd != -1);
}

int Transceiver::doRequest()
{
    if(!isValid()) return -1;

    int ret = 0;

    do
    {
        ret = 0;

        if (!_sendBuffer.empty())
        {
            size_t length = _sendBuffer.length();

            ret = this->send(_sendBuffer.c_str(), _sendBuffer.length(), 0);
            if(ret > 0)
            {
                if((size_t)ret == length)
                {
                    _sendBuffer.clear();
                }
                else
                {
                    _sendBuffer.erase(_sendBuffer.begin(), _sendBuffer.begin() + min((size_t)ret, length));
                }
            }
        }
    }
    while (ret > 0);

    return ret;
}

void Transceiver::writeToSendBuffer(const string& msg)
{
    _sendBuffer.append(msg);
}

//////////////////////////////////////////////////////////
TcpTransceiver::TcpTransceiver(ObjectProxy *op, const EndpointInfo &ep)
: Transceiver(op, ep)
{
    //预分配好一定的内存
    _recvBuffer.reserve(1024);
}

int TcpTransceiver::doResponse(list<ResponsePacket>& done)
{
    if(!isValid()) return -1;

    int recv = 0;

    done.clear();

    char buff[8192] = {0};

    do
    {
        if ((recv = this->recv(buff, sizeof(buff), 0)) > 0)
        {
            _recvBuffer.append(buff, recv);
        }
    }
    while (recv > 0);

    LOGINFO("[TAF][tcp doResponse, " << _objectProxy->name() << ",fd:" << _fd << ",recvbuf:" << _recvBuffer.length() << "]" << endl);

    if(!_recvBuffer.empty())
    {
        try
        {
            size_t pos = _objectProxy->getProxyProtocol().responseFunc(_recvBuffer.c_str(), _recvBuffer.length(), done);

            if(pos > 0)
            {
                //用erase, 不用substr, 从而可以保留预分配的空间
                _recvBuffer.erase(_recvBuffer.begin(), _recvBuffer.begin() + min(pos, _recvBuffer.length()));

                if(_recvBuffer.capacity() - _recvBuffer.length() > 102400)
                {
                   _recvBuffer.reserve(max(_recvBuffer.length(),(size_t)1024));
                }
            }
        }
        catch (exception &ex)
        {
            LOG->error() << "[TAF][tcp doResponse," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",tcp recv decode error:" << ex.what() << endl;

            close();
        }
        catch (...)
        {
            LOG->error() << "[TAF][tcp doResponse," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",tcp recv decode error." << endl;

            close();
        }
    }
    return done.empty()?0:1;
}

int TcpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid()) return -1;

    int ret = ::send(_fd, buf, len, flag);

    if (ret < 0 && errno != EAGAIN)
    {
        LOGINFO("[TAF][tcp send," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }
    LOGINFO("[TAF][tcp send," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",len:" << len << "]" << endl);

    return ret;
}

int TcpTransceiver::recv(void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid()) return -1;

    int ret = ::recv(_fd, buf, len, flag);

    if (ret == 0 || (ret < 0 && errno != EAGAIN))
    {
        LOGINFO("[TAF][tcp recv, " << _objectProxy->name() << ",fd:" << _fd << ", " << _ep.desc() << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }
    LOGINFO("[TAF][tcp recv," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",ret:" << ret << "]" << endl);

    return ret;
}

/////////////////////////////////////////////////////////////////
UdpTransceiver::UdpTransceiver(ObjectProxy *op, const EndpointInfo &ep)
: Transceiver(op, ep),_pRecvBuffer(NULL)
{
	if(!_pRecvBuffer)
	{

		_pRecvBuffer = new char[DEFAULT_RECV_BUFFERSIZE];

		if(!_pRecvBuffer)
		{
			throw TC_Exception("objproxy '" + _objectProxy->name()	+ "' malloc udp receive buffer fail");
		}
	}

}

UdpTransceiver::~UdpTransceiver()
{
	if(!_pRecvBuffer)
	{
		delete _pRecvBuffer;
	}
}

int UdpTransceiver::doResponse(list<ResponsePacket>& done)
{
    if(!isValid()) return -1;

    int recv = 0;

    done.clear();

    //char buff[8192] = {0};

    do
    {
        if ((recv = this->recv(_pRecvBuffer, DEFAULT_RECV_BUFFERSIZE, 0)) > 0)
        {
            LOGINFO("[TAF][udp doResponse, " << _objectProxy->name() << ",fd:" << _fd << ",recvbuf:" << recv << "]" << endl);

            try
            {
                _objectProxy->getProxyProtocol().responseFunc(_pRecvBuffer, recv, done);
            }
            catch (exception &ex)
            {
                LOG->error() << "[TAF][udp doResponse, " << _objectProxy->name() << ",fd:" << _fd<< "," << _ep.desc() << ", udp recv decode error:" << ex.what() << endl;
            }
            catch (...)
            {
                LOG->error() << "[TAF][udp doResponse, " << _objectProxy->name() << ",fd:" << _fd<< "," << _ep.desc() << ", udp recv decode error." << endl;
            }
        }
    }
    while (recv > 0);

    return done.empty()?0:1;
}

int UdpTransceiver::send(const void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid()) return -1;

    int ret = ::sendto(_fd, buf, len, flag, (struct sockaddr*) &(_ep.addr()), sizeof(sockaddr));

    if (ret < 0 && errno != EAGAIN)
    {
        LOGINFO("[TAF][udp send " << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }
    return ret;
}

int UdpTransceiver::recv(void* buf, uint32_t len, uint32_t flag)
{
    if(!isValid()) return -1;

    int ret = ::recvfrom(_fd, buf, len, flag, NULL, NULL); //need check from_ip & port

    if (ret < 0 && errno != EAGAIN)
    {
        LOGINFO("[TAF][udp recv " << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ", fail! errno:" << errno << "," << strerror(errno) << ",close]" << endl);

        close();

        return 0;
    }
    return ret;
}
/////////////////////////////////////////////////////////////////
/// 共享内存通讯组件的实现
	BusTransceiver::BusTransceiver(ObjectProxy *op, EndpointInfo ep)
: Transceiver(op, ep)
, _c2sMmapSize(1*1024*1024)
, _s2cMmapSize(1*1024*1024)
{

}

BusTransceiver::~BusTransceiver()
{
    _c2sFifo.close();
    _c2sMmap.munmap();

    _s2cFifo.close();
    _s2cMmap.munmap();
}

void BusTransceiver::init()
{
    if (_c2sFifo.open(_c2sFifoName, TC_Fifo::EM_WRITE) != 0 && errno != ENXIO)
    {
        throw TC_Exception("init error", errno);
    }
    _c2sMmap.mmap(_c2sMmapName.c_str(), _c2sMmapSize);
    if (_c2sMmap.iscreate())
    {
        _c2sMemQueue.create((char*) _c2sMmap.getPointer(), _c2sMmapSize);
    }
    else
    {
        _c2sMemQueue.attach((char*) _c2sMmap.getPointer(), _c2sMmapSize);
    }

    _s2cFifo.open(_s2cFifoName, TC_Fifo::EM_READ);
    _s2cMmap.mmap(_s2cMmapName.c_str(), _s2cMmapSize);
    if (_s2cMmap.iscreate())
    {
        _s2cMemQueue.create((char*) _s2cMmap.getPointer(), _s2cMmapSize);
    }
    else
    {
        _s2cMemQueue.attach((char*) _s2cMmap.getPointer(), _s2cMmapSize);
    }

    _fd = _s2cFifo.fd();
}

int BusTransceiver::doResponse(list<ResponsePacket> & done)
{
	if(!isValid()) return -1;

    done.clear();

	char szBuff[2048];

	for ( ; true; )
	{
		memset(szBuff, 0, sizeof(szBuff));

		int iLen = _s2cFifo.read(szBuff, sizeof(szBuff));

		if (iLen < 0)
		{
			/*
			bClose = errno == EAGAIN?false:true;

			if(bClose)
			{
				ostringstream os;
				os << __FILE__ << "|" << __LINE__ << "|recv (" << _pcommu->c2sFifoName << ")|EN:" << errno << "|ES:" << strerror(errno) << endl;
				_pBindAdapter->getEpollServer()->debug(os.str());
			}
			*/
			break;
		}
		else if (iLen == 0)
		{
			break;
		}
	}


	//bool bClose = (iLen == 0) || (iLen < 0 && errno != EAGAIN);

	for ( ; ; )
	{
		std::string buff;
		if (!_s2cMemQueue.pop(buff))
		{
			break;
		}
		_recvBuffer.append(buff);
	}

	LOGINFO("[TAF][bus doResponse, " << _objectProxy->name() << ",fd:" << _fd << ",recvbuf:" << _recvBuffer.length() << "]" << endl);

    if(!_recvBuffer.empty())
    {
        try
        {
            size_t pos = _objectProxy->getProxyProtocol().responseFunc(_recvBuffer.c_str(), _recvBuffer.length(), done);

            if(pos > 0)
            {
                //用erase, 不用substr, 从而可以保留预分配的空间
                _recvBuffer.erase(_recvBuffer.begin(), _recvBuffer.begin() + min(pos, _recvBuffer.length()));

                if(_recvBuffer.capacity() - _recvBuffer.length() > 102400)
                {
                   _recvBuffer.reserve(max(_recvBuffer.length(),(size_t)1024));
                }
            }
        }
        catch (exception &ex)
        {
            LOG->error() << "[TAF][bus doResponse," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",tcp recv decode error:" << ex.what() << endl;

            close();
        }
        catch (...)
        {
            LOG->error() << "[TAF][bus doResponse," << _objectProxy->name() << ",fd:" << _fd << "," << _ep.desc() << ",tcp recv decode error." << endl;

            close();
        }
    }

    return done.empty()?0:1;
}

int BusTransceiver::send(const void * buf, uint32_t len, uint32_t flag)
{
    try
    {
        if(!_c2sMemQueue.push((const char *)buf, len))
		{
			LOG->error() << "[TAF][BusTransceiver::send]error: push queue error, maybe queue full" << endl;
			return 0;
		}
        _c2sFifo.write(" ", 1);
    }
    catch (TC_Exception & ex)
    {
        LOG->error() << "[TAF][BusTransceiver::send]Exception:" << ex.what() << endl;
        return -1;
    }

    return len;
}

int BusTransceiver::recv(void * buf, uint32_t len, uint32_t flag)
{
    assert(true);
    return 0;
}

void BusTransceiver::close()
{
	_c2sFifo.close();
    _c2sMmap.munmap();

    _s2cFifo.close();
    _s2cMmap.munmap();
}

/////////////////////////////////////////////////////////////////
}
