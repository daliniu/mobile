#ifndef __XMLPROXYIMP_H__
#define __XMLPROXYIMP_H__

#include "servant/Application.h"
#include "XmlProxyCallback.h"

class XmlProxyImp : public taf::Servant
{
public:
	virtual ~XmlProxyImp(){}
	
	virtual void initialize(){}

	virtual void destroy(){}


	virtual int doRequest(taf::JceCurrentPtr current, vector<char> & response);

	virtual int doResponse(ReqMessagePtr resp);
};

#endif

