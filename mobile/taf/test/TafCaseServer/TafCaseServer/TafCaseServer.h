#ifndef _TafCaseServer_H_
#define _TafCaseServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class TafCaseServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~TafCaseServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern TafCaseServer g_app;

////////////////////////////////////////////
#endif
