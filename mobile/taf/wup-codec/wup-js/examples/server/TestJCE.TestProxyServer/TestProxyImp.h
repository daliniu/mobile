#ifndef _TestProxyImp_H_
#define _TestProxyImp_H_

#include "servant/Application.h"
#include "util/tc_http.h"
#include "util/tc_http_async.h"
#include "util/tc_cgi.h"
#include "TestProxyServer.h"
#include "UserInfo.h"

using namespace taf ;

class TestProxyImp : public taf::Servant
{
public:
	virtual ~TestProxyImp() {}

	virtual void initialize();

	virtual void destroy();

    virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

public:
	virtual int getUserInfo(const string &sUin, TestJCE::UserInfo & stUser);

	static void doSendResponse(taf::JceCurrentPtr current, const char * szBuff, size_t sizeLen, int code = 200, const string& about = "OK");
};

/////////////////////////////////////////////////////
#endif
