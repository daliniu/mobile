#include "AServant.h"
#include <string.h>
#include "util/tc_config.h"
#include "servant/Communicator.h"
#include <iostream>

using namespace std;
using namespace taf;
using namespace Test;

int g_count = 0;
int g_errorCount = 0;
TC_ThreadMutex mutex;

class ProxyCallBack : public AServantPrxCallback
{
public:
	ProxyCallBack(){}

	virtual ~ProxyCallBack(){}

	virtual void callback_saysomething(const std::string& ret)
	{
		TC_LockT<TC_ThreadMutex> lock(mutex);
		g_count++;
	}

	virtual void callback_saysomething_exception(taf::Int32 ret)
	{
		cout << "callback_getString_exception : ret = " << ret << endl;
		TC_LockT<TC_ThreadMutex> lock(mutex);
		g_errorCount++;
	}
};



int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		cout << "usage: " << argv[0] << " Count Speed" << endl;
		return -1;
	}
	int iCount = TC_Common::strto<int>(argv[1]);
	int iSpeed = TC_Common::strto<int>(argv[2]);

	string stringToProxy = "Test.AServer.AServantObj";
	CommunicatorPtr pComm = new Communicator;
	pComm->setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.194.147 -p 17890");
	AServantPrx proxyPrx = pComm->stringToProxy<AServantPrx>(stringToProxy);

	struct timeval tStart, tEnd;
	taf::Int32 count = 0;
	gettimeofday(&tStart, NULL);

	int iSend = 0;
	time_t tBegin = time(NULL);

	while(count < iCount)
	{
		AServantPrxCallbackPtr cb = new ProxyCallBack();
		try
		{
			proxyPrx->async_saysomething(cb, "hello world");
		}
		catch (const std::exception & ex)
		{
			cout << "async_getString exception: " << ex.what() << endl;
			TC_LockT<TC_ThreadMutex> lock(mutex);
			g_errorCount++;
		}
		count++;
		iSend++;		
		if(iSend > iSpeed)
		{
			time_t tNow = time(NULL);
			while(tNow <= tBegin)
			{
				usleep(1000);
				tNow = time(NULL);
			}
			tBegin = tNow;
			iSend = 0;
		}
	}
	cout << "send:" << count <<endl;
	while(true)
	{
		int sum = g_count + g_errorCount;
		cout << "response :" << "sum ="  << sum << ", succ = " << g_count << ",error = " << g_errorCount << endl;
		if( sum >= count )
			break;
		else
			usleep(100000);
	}
	gettimeofday(&tEnd, NULL);
	cout << TC_Common::tostr<taf::Int32>(1000000*(tEnd.tv_sec-tStart.tv_sec)+(tEnd.tv_usec-tStart.tv_usec)) << endl;
	
	return 0;
}
