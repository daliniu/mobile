#ifndef _ThridProtoServantImp_H_
#define _ThridProtoServantImp_H_

#include "servant/Application.h"

/**
 *
 *
 */
class ThridProtoServantImp : public taf::Servant
{
public:
	/**
	 *
	 */
	virtual ~ThridProtoServantImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	virtual int doRequest(JceCurrentPtr current, vector<char> &buffer);
};
/////////////////////////////////////////////////////
#endif
