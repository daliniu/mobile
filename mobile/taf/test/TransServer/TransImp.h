#ifndef _TRANS_IMP_H_
#define _TRANS_IMP_H_

#include "servant/ServantProxy.h"
#include "servant/Servant.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace taf;

/////////////////////////////////////////////////////////////////////////
class TransImp : public Servant
{

public:
    /**
     * 构造函数
     */
    TransImp();

public:
    /**
     * 对象初始化
     */
    virtual void initialize();

    /**
     * 做为服务端，处理客户端的主动请求
     * @param current 
     * @param response 
     * @return int 
     */
    virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

    /**
     * 做为客户端异步调用时，处理其他server(成功)的响应
     * @param resp 
     * @return int 
     */
    virtual int doResponse(ReqMessagePtr resp);

    /**
     * 做为客户端异步调用时，处理其他server(本地异常或者超时)的响应
     * @param resp 
     * @return int 
     */
    virtual int doResponseException(ReqMessagePtr resp);

    /**
     * 做为客户端异步调用时，处理其他server(没有找到request)的响应
     * 例如push类消息或者对端server迟到的异步响应
     * @param resp 
     * @return int 
     */
    virtual int doResponseNoRequest(ReqMessagePtr resp);

    /**
     * 对象销毁
     */
	virtual void destroy();

protected:
    /**
     * ccserver的对象名称
     */
    string _ccserverObjName;

	/**
	 * ccserver的对象地址
	 */
    string _ccserverObjHost;

	/**
	 * 访问ccserver的代理
	 */
	ServantPrx _ccserverPrx;
};
///////////////////////////////////////////////////////////////////////////////
#endif
