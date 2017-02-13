#ifndef _queryStatServantImp_H_
#define _queryStatServantImp_H_

#include "servant/Application.h"
#include "QueryStatServant.h"
#include "QueryStatServer.h"

#include "util/tc_common.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "JSON_parser.h"
#include "DbProxy.h"

/**
 *
 *
 */
class queryStatServantImp : public taf::queryData
{
public:
	/**
	 *
	 */
	virtual ~queryStatServantImp() {}

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
	virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);
	virtual taf::Int32 query(const std::string & sIn, std::string &result,taf::JceCurrentPtr current);
private:
	int doQuery(const string sUid, const string &sIn, string &sResult);
private:
	DbProxy _proxy;
};


/////////////////////////////////////////////////////
#endif
