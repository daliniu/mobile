#include "servant/Communicator.h"
#include "util/tc_common.h"
#include "util/tc_option.h"
#include "util/tc_thread_pool.h"

#include <sys/un.h>
#include <iostream>
#include "log/taf_logger.h"
#include "A.h"

using namespace std;
using namespace taf;
using namespace Test;

struct ACallback : public APrxCallback
{
	//回调函数
	virtual void callback_test(const string &ret)
	{
		int i = TC_Common::strto<int>(ret);
		if(i % 1000 == 0)
		{
			LOG->debug() << "recv callback_test:" << ret << endl;
		}
	}

	virtual void callback_test_exception(taf::Int32 ret)
	{
		LOG->debug() << "recv callback_test_exception:" << ret << endl;
	}
};

APrx _aPrx;

bool g_b = true;

void sync_call_test(bool hash)
{
	int i = 100000;
	while(g_b && i)
	{
		try
		{
			if(i % 1000 == 0)
			{
				//发起远程调用
				LOG->debug() << "sync_call:" << i << endl;
			}

			string s;

			if(hash)
			{
				s = _aPrx->taf_hash(i)->test(pthread_self(), i);
			}
			else
			{
				s = _aPrx->test(pthread_self(), i);
			}
		}
		catch(exception &ex)
		{
			LOG->error() << ex.what() << endl;
		}

//		sleep(1);
//      usleep(1000*10);			
    	i--;
	}
}

void async_call_test(bool hash)
{
	int i = 100000;

	while(i && g_b)
	{
		try
		{
			if(i % 1000 == 0)
			{
				LOG->debug() << "async_call:" << i << endl;
			}

			APrxCallbackPtr a = new ACallback;
			//发起远程调用
			if(hash)
			{
				_aPrx->taf_hash(i)->async_test(a, pthread_self(), i);
			}
			else
			{
				_aPrx->async_test(a, pthread_self(), i);
			}

			if(i%50 == 0)
				usleep(1000 * 10);
//            sleep(1);
		}
		catch(exception &ex)
		{
			LOG->debug() << ex.what() << endl;
		}

		--i;
	}
}

TC_ThreadPool tpoolA;

void batch_call(int thread, bool hash, bool async)
{
    tpoolA.init(thread);
    tpoolA.start();
	ostringstream s;

    LOG->debug() << "batch_call start time:" << TC_Common::now2str() << endl;
	
	s << "batch_call start time:" << TC_Common::now2str();
	typedef TC_Functor<void, TL::TLMaker<bool>::Result> functor_type;

    functor_type* functor = NULL;

	if(async)
	{
		functor = new functor_type(async_call_test);
	}
	else
	{
		functor = new functor_type(sync_call_test);
	}

    functor_type::wrapper_type wtA((*functor), hash);

    for(size_t i = 0; i < tpoolA.getThreadNum(); i++)
    {
        tpoolA.exec(wtA);
    }

	tpoolA.waitForAllDone();

	delete functor;
	LOG->debug() << s.str()<< endl;

    LOG->debug() << "batch_call stop time:" << TC_Common::now2str() << endl;
}

CommunicatorPtr _comm = new Communicator();

static void sighandler1( int sig_no )
{
	LOG->debug() << "terminate" << endl;

	_comm->terminate();

	g_b = false;

}

void usage(int argc, char *argv[])
{
    cout << "Usage:" << argv[0] << " --log-info --log-sync --udp --tcp --hash --async --thread=5" << endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    try
	{
		if(argc < 2)
		{
			usage(argc, argv);
		}

        signal(SIGINT, sighandler1);

		TafRollLogger::getInstance()->logger()->modFlag(0xffff, true);

		TC_Option option;
		option.decode(argc, argv);

		if(option.hasParam("log-info"))
		{
			TafRollLogger::getInstance()->logger()->setLogLevel(TC_RollLogger::INFO_LOG);
		}

		if(option.hasParam("log-sync"))
		{
			TafRollLogger::getInstance()->sync(true);
		}

		string obj;

		if(option.hasParam("udp"))
		{
			obj = "Test.AServer.AObj@udp -h 127.0.0.1 -p 22345:udp -h 127.0.0.1 -p 22346:udp -h 127.0.0.1 -p 22347";
       	}
		else
		{
			obj = "Test.AServer.AObj@tcp -h 127.0.0.1 -p 22345:tcp -h 127.0.0.1 -p 22346:tcp -h 127.0.0.1 -p 22347";
		}

		_aPrx = _comm->stringToProxy<APrx>(obj);

		int thread = TC_Common::strto<int>(option.getValue("thread"));
		if(thread < 1)
		{
			thread = 5;
		}
	  
		bool hash = option.hasParam("hash");
		bool async= option.hasParam("async");
	
		LOG->debug() << "start test, obj:" << obj << ", thread:" << thread << ", hash:" << hash << ", sync:" << !async << endl;
			 
		batch_call(thread, hash, async);
	}
	catch(exception &ex)
	{
        cout << "error:" << ex.what() << endl;
	}

	cout << "main ok" << endl;
	return 0;
}



