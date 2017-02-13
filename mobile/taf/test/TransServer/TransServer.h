#ifndef _TransServer_H_
#define _TransServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "servant/Application.h"

using namespace taf;

class TransServer : public Application
{
public:
	/**
	 * 析构函数
	 **/
	virtual ~TransServer() {};

	/**
	 * 服务初始化
	 **/
	virtual void initialize();

	/**
	 * 服务销毁
	 **/
	virtual void destroyApp();
};
////////////////////////////////////////////
#endif
