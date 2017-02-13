#ifndef _KevinTestServer_H_
#define _KevinTestServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class KevinTestServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~KevinTestServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern KevinTestServer g_app;

////////////////////////////////////////////
#endif
