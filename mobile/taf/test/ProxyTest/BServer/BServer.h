#ifndef _BServer_H_
#define _BServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class BServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~BServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern BServer g_app;

////////////////////////////////////////////
#endif
