#include "servant/Message.h"
#include "servant/StatReport.h"
#include "servant/Communicator.h"
#include "log/taf_logger.h"

namespace taf
{

ReqMessage::ReqMessage(Communicator *c)
: comm(c)
, type(SYNC_CALL)
, proxy(NULL)
, monitor(NULL)
, adapter(NULL)
, callback(NULL)
, hashCode(ThreadPrivateData::INVALID_HASH_CODE)
, fromRpc(false)
{
    begtime.tv_sec = 0;
    begtime.tv_usec = 0;
    endtime.tv_sec = 0;
    endtime.tv_usec = 0;
}

ReqMessage::~ReqMessage() 
{
    //删除锁
    if(monitor)
    {
        delete monitor;

        monitor = NULL;
    }
}

void ReqMessage::timeout(ReqMessagePtr& ptr) 
{ 
    //异步请求, 超时了(异步调用才会出现)
    if (ptr->proxy && ptr->callback && ptr->response.iVersion == -1)
    {
        if (!ptr->comm->isTerminating())
        {
            ptr->comm->asyncProcThread()->put(ptr);
        }
    }
}

//////////////////////////////////////////////////////////////
}
