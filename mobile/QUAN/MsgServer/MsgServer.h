#ifndef _MsgServer_H_
#define _MsgServer_H_
#include "BatchHttp.h"
#include <iostream>
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class MsgServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~MsgServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

        void initHashMap();

		BatchHttp& getMsgHttp() { return _tMsgHttp; }

	BatchHttp _tMsgHttp;
	string _sPushUrl;
};

extern MsgServer g_app;

////////////////////////////////////////////
#endif
