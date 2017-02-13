#ifndef _XmlProxyServer_H_
#define _XmlProxyServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class XmlProxyServer : public Application
{
public:
    /**
     *
     **/
    virtual ~XmlProxyServer() {};

    /**
     *
     **/
    virtual void initialize();

    /**
     *
     **/
    virtual void destroyApp();


	/** 
	 * 响应WEB管理台发送过来的loadproxy命令 
	 * 
	 * @author kevintian (2010-7-9)
	 * 
	 * @param command 
	 * @param params 
	 * @param result 
	 * 
	 * @return bool 
	 */
	bool cmdLoadProxy(const string& command, const string& params, string& result);
};

extern XmlProxyServer g_app;

////////////////////////////////////////////
#endif
