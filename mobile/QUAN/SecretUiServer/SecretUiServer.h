#ifndef _SecretUiServer_H_
#define _SecretUiServer_H_

#include "BatchHttp.h"
#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class SecretUiServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~SecretUiServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
 
        BatchHttp& getActiveHttp() { return _tActiveHttp; }

private:
        BatchHttp _tActiveHttp;

};

extern SecretUiServer g_app;

////////////////////////////////////////////
#endif
