#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "util/tc_parsepara.h"
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class TCServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~TCServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

protected:
    template<typename T>
    void bind(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(getEpollServer().get());

        //设置adapter名称, 唯一
        lsPtr->setName(str);
        //设置绑定端口
        lsPtr->setEndpoint(str);
        //设置最大连接数
        lsPtr->setMaxConns(20000);
        //设置启动线程数
        lsPtr->setHandleNum(10);
        //设置协议解析器
        lsPtr->setProtocol(&T::parse);

        //绑定对象
        getEpollServer()->bind(lsPtr);

        //设置逻辑处理器
        lsPtr->setHandle<T>();
    }
};

extern TCServer g_app;

////////////////////////////////////////////
#endif
