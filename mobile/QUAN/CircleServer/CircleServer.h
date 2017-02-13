#ifndef _CircleServer_H_
#define _CircleServer_H_

#include <iostream>
#include "servant/Application.h"

#include "UniqID.h"

using namespace taf;

/**
 *
 **/
class CircleServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~CircleServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

        MDW::UniqIDPrx getUniqIDProxy()
        {
            return _pUniqIDPrx;
        }

protected:
        MDW::UniqIDPrx _pUniqIDPrx;
    
};

extern CircleServer g_app;

////////////////////////////////////////////
#endif
