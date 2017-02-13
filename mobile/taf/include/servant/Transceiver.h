#ifndef __TAF_TRANSCEIVER_H_
#define __TAF_TRANSCEIVER_H_

#include "servant/EndpointInfo.h"
#include "servant/Message.h"
#include "servant/Global.h"
#include "servant/NetworkUtil.h"
#include <list>

using namespace std;

namespace taf
{
/**
 * 网络传输基类，主要提供send/recv接口
 */
class Transceiver
{
public:
    /**
     * 连接状态
     */
    enum ConnectStatus
    {
        eUnconnected,
        eConnecting,
        eConnected,
	eBusConnecting,
    };

    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    Transceiver(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * 初始化
     * 
     * @param fd 
     * @param connected 
     */
    void reInitialize(int fd, ConnectStatus connStatus);

    /**
     * 初始化FIFO，MMAP
     */
    virtual void init(){};

    /**
     *
     *析构函数
     */
    virtual ~Transceiver();

     /**
     * 获取文件描述符
     * @return int
     */
    virtual int fd() const;

	/**
	 * 获取读文件描述符
	 */
	virtual int getrfd() const { return _fd; }

	/**
	 * 获取写文件描述符
	 */
	virtual int getwfd() const { return _fd; }

    /**
     * 关闭连接
     */
   //virtual void doClose();
	
    /**
     * 是否有效
     * @return bool
     */
	bool isValid() const;

    /**
     * 端口信息
     * @return const EndpointInfo& 
     */
	EndpointInfo& getEndpointInfo() { return _ep;}

public:
    /**
     * 处理请求，判断Send BufferCache是否有完整的包
     * @return int
     */
	virtual int doRequest();

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done) = 0;

    /**
     * 发送到server的消息缓冲区
     *
     */
    void writeToSendBuffer(const string& msg);

    /**
     * 关闭连接
     */
    void close();

public:
	/**
	 * 判断是否已经连接到服务端
	 */
	bool hasConnected() { return isValid() && (_connStatus == eConnected); }
	
	/**
	 * 设置当前连接态
	 */
	void setConnected() { _connectFailed = false; _connStatus = eConnected; }

    
    /**
     * 正在Bus连接
     * 
     * @return bool 
     */
    bool isConnecting() { return isValid() && (_connStatus == eConnecting); }

    /**
     * 设置当前连接态
     */
    void setBusConnecting() { _connectFailed = false; _connStatus = eBusConnecting; }

    /**
     * 正在连接
     * 
     * @return bool 
     */
    bool isBusConnecting() { return isValid() && (_connStatus == eBusConnecting); }

    /**
     * 是否连接异常
     * 
     * @return bool 
     */
    bool isConnectFailed() { return _connectFailed; }

    /**
     * 设置连接失败
     * 
     */
    void setConnectFailed() { _connectFailed = true; _connStatus = eUnconnected; }



public:

    /**
     * 网络发送接口
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    /**
     * 网络接收接口
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag) = 0;

protected:
    int _fd;

	EndpointInfo _ep;

    ObjectProxy *_objectProxy;

    string _sendBuffer;

    string _recvBuffer;

	ConnectStatus _connStatus;	    //表示是否已经连接到服务端

    bool          _connectFailed;   //是否建立连接失败
};

//////////////////////////////////////////////////////////
/**
 * TCP 传输实现
 */
class TcpTransceiver : public Transceiver
{
public:
    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    TcpTransceiver(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * TCP 发送实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * TCP 接收实现
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int, =1,表示有数据就包
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
/**
 * UDP 传输实现
 */
class UdpTransceiver : public Transceiver
{
public:
	enum
	{
        DEFAULT_RECV_BUFFERSIZE = 64*1024	   /*缺省数据接收buffer的大小*/
	};

public:
    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    UdpTransceiver(ObjectProxy *op, const EndpointInfo &ep);

	~UdpTransceiver();
    /**
     * UDP 发送实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP 接收实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);

private:
	char				*_pRecvBuffer;

};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//	BusTransceiver
class BusTransceiver : public Transceiver
{
public:
    /**
     * 构造函数
     * @param ep
     * @param fd
     */
    BusTransceiver(ObjectProxy *op, EndpointInfo ep);

	/**
	 * 析构函数
	 */
	virtual ~BusTransceiver();

	/**
	 * 判断当前连接是否有效
	 * 
	 * @author kevintian (11-01-01)
	 * 
	 * @return bool 
	 */
	virtual bool isValid() const { return _s2cFifo.fd() >= 0 && _c2sFifo.fd() >= 0; }

	/**
	 * 返回共享内存通讯组件的写FD
	 * 
	 * @author kevintian (11-01-04)
	 * 
	 * @return int 
	 */
	virtual int fd() const { return _c2sFifo.fd(); }

	/**
     * UDP 发送实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP 接收实现
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * 关闭连接
     */
    void close();

    /**
     * 处理返回，判断Recv BufferCache是否有完整的包
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);

	/**
	 * 获取读文件描述符
	 */
	virtual int getrfd() const { return _s2cFifo.fd(); }

	/**
	 * 获取写文件描述符
	 */
	virtual int getwfd() const { return _c2sFifo.fd(); }

	/**
	 * 设置客户端到服务端的信息
	 */
	void setc2s(const std::string & sFifoName, const std::string & sMmapName, const uint32_t iMmapSize)
	{
		_c2sFifoName = sFifoName;
		_c2sMmapName = sMmapName;
		_c2sMmapSize = iMmapSize;
	}

	/**
	 * 设置服务端到客户端的信息
	 */
	void sets2c(const std::string & sFifoName, const std::string & sMmapName, const uint32_t iMmapSize)
	{
		_s2cFifoName = sFifoName;
		_s2cMmapName = sMmapName;
		_s2cMmapSize = iMmapSize;
	}

	/**
	 * 初始化FIFO，MMAP
	 */
	void init();

private:
	std::string 	_c2sFifoName;
	std::string 	_c2sMmapName;
	uint32_t		_c2sMmapSize;
	TC_Fifo 		_c2sFifo;
	TC_Mmap 		_c2sMmap;
	TC_SQueue		_c2sMemQueue;

	std::string		_s2cFifoName;
	std::string 	_s2cMmapName;
	uint32_t		_s2cMmapSize;
	TC_Fifo 		_s2cFifo;
	TC_Mmap 		_s2cMmap;
	TC_SQueue 	_s2cMemQueue;
};
}
#endif
