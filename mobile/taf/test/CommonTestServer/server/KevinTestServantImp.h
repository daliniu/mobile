#ifndef _KevinTestServantImp_H_
#define _KevinTestServantImp_H_

#include "servant/Application.h"
#include "KevinTestServant.h"

/**
 *
 *
 */
class KevinTestServantImp : public Test::KevinTestServant
{
public:
	/**
	 *
	 */
	virtual ~KevinTestServantImp() {}

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
	virtual int test(taf::JceCurrentPtr current) 
	{ 
		_rpproxy->report(1);

		LOG->debug() << "ssssssssssssssssss" << endl;			
			
		return 0;
	}

	
	
	
	virtual std::string setUser(const std::string & suin, const Test::stUserInfo_t & inUser,Test::stUserInfo_t &outUser,taf::JceCurrentPtr current);


	virtual std::string pay(const std::string & suin,std::string &sret,taf::JceCurrentPtr current);
	
private:
	PropertyReportPtr _rpproxy;

};
/////////////////////////////////////////////////////
#endif
