#ifndef _LbsServer_H_
#define _LbsServer_H_

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class LbsServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~LbsServer() {};

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

extern LbsServer g_app;

////////////////////////////////////////////
#endif
