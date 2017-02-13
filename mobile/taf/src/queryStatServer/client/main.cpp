#include "servant/Communicator.h"
#include "util/tc_common.h"
#include "util/tc_monitor.h"
#include <sys/un.h>
#include <iostream>
#include "log/taf_logger.h"

#include "queryStatServant.h"

#include "util/tc_mysql.h"

using namespace std;
using namespace taf;

CommunicatorPtr _comm = new Communicator();

static int _grid = 0;
static string obj = "taf.QueryStatServer.QueryStatObj@tcp -h 10.135.4.100 -p 10079 -t 600000";

int syncCall(queryDataPrx pPrx, int c)
{
	//发起远程调用
	for (int i = 0; i < c; ++i)
	{
		string r;

		string sInput =
			"{\"indexs\":[\"succ_count\",\"timeout_count\",\"total_time\"], "
			"\"filter\":[\"slave_name like '%taf.%'\",\"f_tflag >='0030'\",\"f_tflag <='2350'\","
			"\"f_date <= '20120605'\",\"f_date >= '20120605'\"],"
			"\"method\":\"query\",\"dataid\":\"taf\","
			"\"groupby\":[\"f_date\",\"f_tflag\",\"slave_name\",\"master_name\",\"interface_name\"]"
			"}" ;

		cout << "send:" << sInput << endl;
		taf::Int32 ret;
		try
		{
			ret = pPrx->query(sInput,r);
		}
		catch(exception& e)
		{
			cout << "exception:" << e.what() << endl;
		}

		cout << "ret:" << ret << "recv:" << r.size() <<" "<<r.substr(64)<< endl;
	}
	return 0;
}

void asyncCall(queryDataPrx pPrx, int c)
{
	//发起远程调用
	for (int i = 1; i < c; ++i)
	{
	/*
		queryStatPrxCallbackPtr p = new queryStatCallback;

		ostringstream os; os << "AccountCenter.UserAccountServer";

		try
		{
			pPrx->async_query(p, os.str());
		}
		catch(exception& e)
		{
			cout << "exception:" << e.what() << endl;
		}

      */
	}
}

int main(int argc, char *argv[])
{
    try
	{
        if (argc < 5)
        {
            cout << "Usage:" << argv[0] << " <config file> <count> <sync:0|async:1> <fork num>" << endl;
			//exit(0);
        }

        TC_Config conf;

        //conf.parseFile(argv[1]);

		//_comm->setProperty(conf);
        _comm->setProperty("sync-invoke-timeout", "600000");
        _comm->setProperty("async-invoke-timeout", "600000");
		queryDataPrx pPrx = _comm->stringToProxy<queryDataPrx>(obj);

		int c = 1;
		timeval tv1, tv2;
		gettimeofday(&tv1, NULL);

		int t = 0;

		if (t == 0)
		{
		    syncCall(pPrx, c);
		}
		else if (t == 1)
		{
			asyncCall(pPrx, c);
		}

		pid_t pid = getpid();
		gettimeofday(&tv2, NULL);

		cout << "(pid:" << pid << ")"
			 << "(" << (t == 0 ? "sync" : "async") << ")"
			 << "(count:" << c << ")"
			 << "(use ms:" << (tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000 << ")"
			 << endl;

		///sleep(3);
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}
	cout << "main return." << endl;

	return 0;
}



