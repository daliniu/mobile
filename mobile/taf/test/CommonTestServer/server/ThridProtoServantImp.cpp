#include "ThridProtoServantImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void ThridProtoServantImp::initialize()
{
	//initialize servant here:
	//...

}

//////////////////////////////////////////////////////
void ThridProtoServantImp::destroy()
{
	//destroy servant here:
	//...
}


int ThridProtoServantImp::doRequest(JceCurrentPtr current, vector<char> &buffer)
{
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|ID:" << current->getRequestId() << "|Len:" << current->getRequestBuffer().size() << "|Report:" << (ServerConfig::ReportFlow?"true":"false") << endl;

	
	int iLen = sizeof(int) + sizeof(unsigned int) + 100;
	buffer.resize(iLen);
	
	
	iLen = ::htonl(iLen);
	memcpy((char *)&buffer[0], (char *)&iLen, sizeof(int));
	
	const vector<char> & vecBuff = current->getRequestBuffer();
	unsigned int iRequestID = 0;
	memcpy((char *)&iRequestID, (const char *)&vecBuff[sizeof(int)], sizeof(unsigned int));
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|ReqeustID:" << ::ntohl(iRequestID) << endl;

	memcpy((char *)&buffer[sizeof(int)], (char *)&iRequestID, sizeof(unsigned int));
	
	return 0;
}
