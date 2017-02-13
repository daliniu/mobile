#ifndef __TAF_ADAPTER_PROXY_H_
#define __TAF_ADAPTER_PROXY_H_

#include "util/tc_timeout_queue.h"
#include "servant/EndpointInfo.h"
#include "servant/Connector.h"
#include "servant/ObjectProxy.h"
#include "servant/FDReactor.h"
#include "servant/Transceiver.h"
#include "servant/Global.h"

namespace taf
{
/**
 * 每个Adapter对应一个Endpoint，也就是一个服务端口
 */
class AdapterProxy : public TC_ThreadMutex //public BaseProxy
{
public:
    /**
     * 构造函数
     * @param ep
     * @param op
     */
    AdapterProxy(Communicator* comm, const EndpointInfo &ep, ObjectProxy* op);

    /**
     * 析构函数
     */
    ~AdapterProxy();

public:
    /**
     * 调用server端对象方法
     * @param req
     * @return int
     */
	int invoke(ReqMessagePtr& req);

    /**
     * server端方法返回
     * @param req
     * @return int
     */
    int finished(ResponsePacket &rsp);

    /**
     * 端口是否有效,当连接全部失效时返回false
     * @bForceConnect : 是否强制发起连接,为true时不对状态进行判断就发起连接
     * @return bool
     */
    bool checkActive(bool bForceConnect = false);

	/**
	 * 服务在住空的状态是否为active,不管是出问题导致，还是人为停止服务还是服务下线，都认为是inactive的
	 * @return bool
	 */
	inline bool isActiveInReg() {return _activeStateInReg;}

	inline void setActiveInReg(bool bActive) {_activeStateInReg = bActive;}

	inline bool isConnTimeout(){ return _bConnTimeout; }

    /**
     * 获取超时队列
     *
     * @return TC_TimeoutQueue<ReqMessagePtr>*
     */
    inline TC_TimeoutQueue<ReqMessagePtr>* getTimeoutQueue() { return _timeoutQueue; }

    /**
     * 需要关闭连接
     *
     * @param closeTrans
     */
    inline void setCloseTrans(bool closeTrans) { _closeTrans = closeTrans; }

    /**
     * 是否需要关闭连接
     *
     * @return bool
     */
    inline bool shouldCloseTrans() { return _closeTrans; }

    /**
     * 增加调用数
     * @param bTimeout, 是否是超时的
     */
    void finishInvoke(bool bTimeout);

    /**
     * 获取端口信息
     * @return const EndpointInfo&
     */
    const EndpointInfo& endpoint();

    /**
     * 从队列里取一个请求
     * @param trans
     * @return bool
     */
	bool sendRequest();

    /**
     * 获取连接
     *
     * @return Transceiver*
     */
    inline Transceiver* trans() { return _trans; }

	/**
	 * 如果需要异步连接服务，那么再次完成连接
	 */
	bool finishConnect();

   /**
	 * 连接共享内存通讯组件
	 */
   bool doBusReconnect();

   bool finishBusConnect();

   /**
    * 判断是否使用共享内存通信
    */
   bool hasBusCommu() { return _bUseBus; }

protected:

    /**
     * 处理没有找到request的response(可能是push消息)
     * @param req
     * @return bool
     */
    bool dispatchPushCallback(ResponsePacket &rsp);

    /**
     * 设置服务的有效状态
     * @param value
     */
    void setActive(bool value);

    /**
     * 重新设置信息
     */
    void resetInvoke();

    /**
     * 从队列中获取一个请求
     * @param req
     * @return bool
     */
	bool popRequest(ReqMessagePtr& req);

    /**
     * 重连
     * @return TransceiverPtr
     */
    Transceiver* doReconnect();

private:
    bool            _bUseBus;

    Communicator* _comm;

    EndpointInfo _endpoint;

    ObjectProxy* _objectProxy;

    FDReactor* _reactor;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

    Transceiver*             _trans;

    bool                    _closeTrans;

    int32_t _checkTransInterval;

    time_t _lastCheckTransTime;

    int32_t _currentGridGroup;

    uint32_t _timeoutInvoke;

	uint32_t _bConnTimeout;

    uint32_t _totalInvoke;

    uint32_t _frequenceFailInvoke;

    time_t _lastFinishInvokeTime;

    time_t _lastRetryTime;

    bool _activeStatus;

	//该节点在主控的状态是否是active的,出故障和认为停止服务都认为是inactive的
	bool _activeStateInReg;
};
////////////////////////////////////////////////////////////////////
}
#endif
