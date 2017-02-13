#ifndef _SecretServer_H_
#define _SecretServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class SecretServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~SecretServer() {};

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

extern SecretServer g_app;

////////////////////////////////////////////
#endif
