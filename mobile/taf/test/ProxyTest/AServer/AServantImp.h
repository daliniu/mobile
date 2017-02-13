#ifndef _AServantImp_H_
#define _AServantImp_H_

#include "servant/Application.h"
#include "AServant.h"

/**
 *
 *
 */
class AServantImp : public Test::AServant
{
public:
	/**
	 *
	 */
	virtual ~AServantImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	/**
	 *
	 */
	//virtual int test(taf::JceCurrentPtr current) { return 0;};
	virtual int test(taf::JceCurrentPtr current); 
    virtual string saysomething(const std::string& s, taf::JceCurrentPtr current);
};
/////////////////////////////////////////////////////
#endif
