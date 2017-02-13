#include "TransImp.h"
#include "TransServer.h"
#include "CCProtocol.h"

using namespace taf;

TransImp::TransImp()
: _ccserverPrx(NULL)
{ 
}

void TransImp::initialize()
{
    cout << "doInitialize......" << endl;

	_ccserverObjName = "CC.CCServer.Object";

	_ccserverObjHost = "tcp -h 172.25.38.142 -p 40195 -t 10000";

	//设置访问CCServer的协议解析器
    ProxyProtocol prot;

    prot.requestFunc = ProxyProtocol::streamRequest;

    prot.responseFunc = ProxyProtocol::streamResponse<0, uint32_t, true, 9, uint32_t, true>;

	_ccserverPrx = Application::getCommunicator()->stringToProxy<ServantPrx>(_ccserverObjName + "@" + _ccserverObjHost);

    _ccserverPrx->taf_set_protocol(prot);
}

void TransImp::destroy()
{
}

/**
 * 做为server端，处理客户端的请求
 */
int TransImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
	//接收CCClient的cap协议请求
    const vector<char>& request = current->getRequestBuffer();

    string buf((const char*)(&request[0]), request.size());

    ResponsePacket rsp;

	//协议解析
    CCProtocol::packc pc;

    uint32_t len = buf.length();

    pc.Decode(buf.c_str(), len);

    LOG->debug() << "[Adapter:" << current->getBindAdapter()->getName() << "]->C2S Cmd:" << pc.Cmd << ",Seq:" <<  pc.SeqNo << ",Len:" << pc.Len << endl;


	/*(1)做为客户端，同步调用ccserver*/
    /*
    try
    {
        _ccserverPrx->rpc_call(pc.SeqNo, "Transfer", buf.c_str(), buf.length(), rsp);

        LOG->debug() << "[rcp_call,ret len:" << rsp.sBuffer.size() << "]" << endl;

        response = rsp.sBuffer;
    }
    catch (TafSyncCallTimeoutException& e)
    {
        //同步调用超时
        //...
    }
    catch (TafException& e)
    {
        //其他异常错误
        //...
    }
    */
	/**/

	/*(2)做为客户端，异步调用ccserver*/
	ServantCallbackPtr cb = new ServantCallback("ServantCallback", this, current);

    try
    {
        _ccserverPrx->rpc_call_async(pc.SeqNo, "AysncTransfer", buf.c_str(), buf.length(), cb);
    }
    catch (TafException& e)
    {
        //本地的异常错误，例如到服务端连接全部失效等等
        //(超时和其他异常在doResponseException处理)
        //...
    }

	current->setResponse(false);
	/**/

    return 0;
}

/**
 * 做为客户端异步调用时，处理其他server(成功)的响应
 */
int TransImp::doResponse(ReqMessagePtr resp)
{
	//(1)如果有请求多个server，则根据objname判断是哪个server的resp
	//(2)如果到一个server有多个callback类型，根据type判断类型
	//...	
	if (resp->proxy->taf_name() == _ccserverObjName 
		&& resp->callback->getType() == "ServantCallback")
	{
		ServantCallback* cb = dynamic_cast<ServantCallback*>(resp->callback.get());

		vector<char>& buff = resp->response.sBuffer;

		if (!buff.empty())
		{
			CCProtocol::packs ps;

			uint32_t len = buff.size();

			ps.Decode((const char*)(&buff[0]), len);

			LOG->debug() << "[CBType:" << cb->getType() << ",FromObject:" << resp->proxy->taf_name() << "]<-S2C Cmd:" << ps.Cmd << ",Seq:" <<  ps.SeqNo << ",Len:" << ps.Len << endl;
			
            //发送响应给ccapi
			cb->getCurrent()->sendResponse((const char*)(&buff[0]), buff.size());
		}
	}
    return 0;
}

/**
 * 做为客户端异步调用时，处理其他server(本地异常或者超时)的响应
 */
int TransImp::doResponseException(ReqMessagePtr resp)
{
    //超时处理(注意:超时只表示规定时间内没有收到响应，不能确定对方的服务端一定没有处理）
    if (resp->response.iRet == JCEASYNCCALLTIMEOUT)
    {
        if (resp->proxy->taf_name() == _ccserverObjName 
            && resp->callback->getType() == "ServantCallback")
        {
            //重发等业务逻辑处理...
        }
    }
    return 0;
}

/**
 * 做为客户端异步调用时，处理其他server(没有找到request)的响应
 * 例如push类消息或者对端server迟到的异步响应
 */
int TransImp::doResponseNoRequest(ReqMessagePtr resp)
{
    //此时对端server的数据包在resp->response.buff里
    //...
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
