#ifndef _GetSecretServer_H_
#define _GetSecretServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class GetSecretServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~GetSecretServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

        void initHashMap();
};

extern GetSecretServer g_app;

////////////////////////////////////////////
#endif
