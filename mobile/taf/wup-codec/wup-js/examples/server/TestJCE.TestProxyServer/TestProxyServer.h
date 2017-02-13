#ifndef _TestProxyServer_H_
#define _TestProxyServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

class TestProxyServer : public Application
{
public:
	virtual ~TestProxyServer() {};

	virtual void initialize();

	virtual void destroyApp();
};

extern TestProxyServer g_app;
////////////////////////////////////////////
#endif
