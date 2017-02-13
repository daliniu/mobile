#ifndef _TestAddConfig_H_
#define _TestAddConfig_H_

#include <iostream>
#include "servant/Application.h"
#include "wbl.h"
#include "wbl_comm.h"
#include "wbl_config_file.h"

using namespace taf;

/**
 *
 **/
class TestAddConfig : public Application
{
public:
	/**
	 *
	 **/
	virtual ~TestAddConfig() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern TestAddConfig g_app;

////////////////////////////////////////////
#endif
