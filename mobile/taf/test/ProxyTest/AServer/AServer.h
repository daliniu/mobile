#ifndef _AServer_H_
#define _AServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class AServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~AServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern AServer g_app;

////////////////////////////////////////////
#endif
