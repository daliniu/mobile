#ifndef _TafCaseServantImp_H_
#define _TafCaseServantImp_H_

#include "servant/Application.h"
#include "TafCaseServant.h"

/**
 *
 *
 */
class TafCaseServantImp : public Test::TafCaseServant
{
public:
	/**
	 *
	 */
	virtual ~TafCaseServantImp() {}

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
	virtual int test(taf::JceCurrentPtr current) { return 0;};
	
	virtual taf::Int32 getUser(const Test::UserInfo & inUser,Test::UserInfo &outUser,taf::JceCurrentPtr current);

	virtual taf::Int32 testUnsigned(taf::UInt8 ubInUin,taf::UInt16 usInUin,taf::UInt32 uiInUin,taf::UInt8 &ubOutUin,taf::UInt16 &usOutUin,taf::UInt32 &uiOutUin,taf::JceCurrentPtr current);
};
/////////////////////////////////////////////////////
#endif
