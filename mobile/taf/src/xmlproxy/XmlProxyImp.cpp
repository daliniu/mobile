#include "XmlProxyImp.h"


int XmlProxyImp::doRequest(taf::JceCurrentPtr current, vector<char> & response)
{
	try
	{
		const vector<char> & vecRequest = current->getRequestBuffer();
		LOG->info() << "NOT DECODE:\n" << std::string(static_cast<const char *>(&vecRequest[0]), vecRequest.size()) << endl;

		TC_HttpRequest httpRequest;
		httpRequest.decode(static_cast<const char *>(&vecRequest[0]), vecRequest.size());

		std::string sProtocol = httpRequest.getHeader("Protocol");
		LOG->info() << __LINE__ << "Protocol:" << sProtocol << endl;
		LOG->info() << "HTTP DECODE:\n" << httpRequest.getContent() << endl;

		XmlProxyCallbackPtr cb = new XmlProxyCallback(this, current);
		cb->doRequest(httpRequest.getContent());
	}
	catch (exception & ex)
	{
		current->close();
		LOG->error() << "L:" << __LINE__ << " exception error:" << ex.what() << endl;
	}
	catch (...)
	{
		current->close();
		LOG->error() << "L:" << __LINE__ << " unkown error:" << endl;
	}

	return 0;
}

int XmlProxyImp::doResponse(ReqMessagePtr resp)
{
	XmlProxyCallback * cb = dynamic_cast<XmlProxyCallback *>(resp->callback.get());

	vector<char> & vecBuffer = resp->response.sBuffer;
	if (!vecBuffer.empty())
	{
		cb->doResponse(resp->request.iRequestId, vecBuffer);
	}

	return 0;
}

