#ifndef __TAF_MESSAGE_H_
#define __TAF_MESSAGE_H_

#include "servant/Global.h"
#include "servant/EndpointInfo.h"
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"

namespace taf
{

/**
 * 超时一定比率后进行切换
 * 设置超时检查参数
 * 计算到某台服务器的超时率, 如果连续超时次数或者超时比例超过阀值
 * 默认60s内, 超时调用次数>=2, 超时比率0.5或者连续超时次数>5,
 * 则失效
 * 服务失效后, 请求将尽可能的切换到其他可能的服务器, 并每隔tryTimeInterval尝试一次, 如果成功则认为恢复
 * 如果其他服务器都失效, 则随机选择一台尝试
 * 如果是灰度状态的服务, 服务失效后如果请求切换, 也只会转发到相同灰度状态的服务
 * @uint16_t minTimeoutInvoke, 计算的最小的超时次数, 默认2次(在checkTimeoutInterval时间内超过了minTimeoutInvoke, 才计算超时)
 * @uint32_t frequenceFailInvoke, 连续失败次数
 * @uint32_t checkTimeoutInterval, 统计时间间隔, (默认60s, 不能小于30s)
 * @float radio, 超时比例 > 该值则认为超时了 ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, 重试时间间隔
 */
struct CheckTimeoutInfo
{
    uint16_t minTimeoutInvoke;

    uint32_t checkTimeoutInterval;

    uint32_t frequenceFailInvoke;

    float radio;

    uint32_t tryTimeInterval;

    CheckTimeoutInfo() : minTimeoutInvoke(2), checkTimeoutInterval(60), frequenceFailInvoke(5), radio(0.5), tryTimeInterval(30) {}
};

/**
 * 用于同步调用时的条件变量
 */
struct ReqMonitor : public TC_ThreadLock {};//, public TC_HandleBase {};

//typedef TC_AutoPtr<ReqMonitor> ReqMonitorPtr;

#define IS_MSG_TYPE(m, t) ((m & t) != 0)
#define SET_MSG_TYPE(m, t) do { (m |= t); } while (0);
#define CLR_MSG_TYPE(m, t) do { (m &=~t); } while (0);

/**
 * Proxy端的请求消息结构
 */
struct ReqMessage : public TC_HandleBase//, public TC_ThreadPool::ThreadData
{
    /**
     * 超时的时候调用(异步调用超时)
     * 
     * @param ptr 
     */
    static void timeout(ReqMessagePtr& ptr);

    //调用类型
    enum CallType
    {
        SYNC_CALL = 1, //同步
        ASYNC_CALL,    //异步
        ONE_WAY,       //单向
    };

    /**
     * 构造函数
     */
    ReqMessage(Communicator *comm);

    /**
     * 析构函数
     */
    ~ReqMessage();

public:
    /**
     * 判断消息类型
     * 
     * @return bool
     */
    bool is_message_type(uint32_t msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void set_message_type(uint32_t& msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void clr_message_type(uint32_t& msg, uint32_t type);

protected:
    //不能赋值
    ReqMessage& operator=(const ReqMessage &);

protected:
    Communicator*               comm;       //通信器

public:
    ReqMessage::CallType        type;       //调用类型
    ServantPrx::element_type*   proxy;      //调用端的proxy对象
    ReqMonitor*                 monitor;    //用于同步的monitor
    AdapterProxy*               adapter;    //哪台服务器发送的
    RequestPacket               request;    //请求消息体
    ResponsePacket              response;   //响应消息体
    ServantProxyCallbackPtr     callback;   //异步调用时的回调对象
    timeval                     begtime;    //请求时间
    timeval                     endtime;    //完成时间
    int64_t                     hashCode;   //hash值，用户保证一个用户的请求发送到同一个server(不严格保证)
    bool                        fromRpc;    //是否是第三方协议的rcp_call，缺省为false

};
////////////////////////////////////////////////////////////////////////////////////////////////////
}
#include "servant/ServantProxy.h"

#endif
