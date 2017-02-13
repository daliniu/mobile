#ifndef _BServantImp_H_
#define _BServantImp_H_

#include "servant/Application.h"
#include "BServant.h"
#include "AServant.h"


using namespace Test;

/**
 *
 *
 */
class BServantImp : public Test::BServant
{
public:
	/**
	 *
	 */
	virtual ~BServantImp() {}

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
	virtual int test(taf::JceCurrentPtr current);
	//virtual int test(const string &s, string &r, JceCurrentPtr current);
	//virtual taf::Int32 dohandle(const std::string & s,taf::JceCurrentPtr current);
	virtual string dohandle(const string &s, JceCurrentPtr current);

private:
	AServantPrx _pPrx;

};
/////////////////////////////////////////////////////
#endif
