#include <iostream>
#include "StatQuery.h"
#include "servant/Communicator.h"
#include "servant/Application.h"

#include "servant/StatF.h"
#include "servant/PropertyF.h"
#include "servant/StatReport.h"
#include "util/tc_option.h"

#include <cmath>

using namespace std;
using namespace taf;

taf::Communicator _comm;
    typedef  map<StatMicMsgHead, StatMicMsgBody>        MapStatMicMsg;
string g_statIp;
void printNow()
{
	char c[1024]  = {0};	
	int len = 1024;
	int n = 0;
	struct timeval t;
	TC_TimeProvider::getInstance()->getNow(&t);

	tm tt;
	localtime_r(&t.tv_sec, &tt);
	n += snprintf(c + n, len-n, "%04d-%02d-%02d %02d:%02d:%02d.%03ld|", 
				  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, t.tv_usec/1000);
	LOG->debug() << "now " << string(c) << endl;
}

void* testStat(void* arg)
{
    
    pthread_detach(pthread_self());   //将该线程变为脱离的，确保程序退出后释放资源
    MapStatMicMsg       _mStatMicMsgClient;

    StatMicMsgHead head;
    StatMicMsgBody body;
    
    head.masterName     = "m";
      
    head.masterIp       = "Ip"; 
        
    head.slaveName      = "s" ;//服务端version不需要上报
        
    head.slaveIp        = "sIp";   

    head.interfaceName  = "i";
    
    head.slavePort      = 44;

    head.returnValue    = 0;
    //head.setArea        = "testsetArea";
    //head.setID          = "testsetID";
    
    body.count = 1;
    body.totalRspTime = 100;
	string sName = "head.slaveName";


   int tmp = syscall(SYS_gettid);
	
	printNow();
    int i = 1000;
    while(i)
    {
    	StatFPrx statPrx = NULL;
        statPrx = _comm.stringToProxy<StatFPrx>("taf.tafstat.StatObj@tcp -h " + g_statIp + " -p 23333");
        //for(int j=0; j<20; j++)
        {
    	    head.slaveName = sName + TC_Common::tostr(i) + TC_Common::tostr(tmp);
	    _mStatMicMsgClient[head] = body;
	}
    	//statPrx->async_reportMicMsg(NULL,_mStatMicMsgClient,true);
    	statPrx->reportMicMsg(_mStatMicMsgClient,true);
    	_mStatMicMsgClient.clear();
        //LOG->debug() << "report:" << i << endl;
        //usleep(100);
        i--;
    }
    LOG->debug() << "send msg out" << endl;
	printNow();
    sleep(3);
}

void getStat()
{
    StatQueryPrx pPtr = _comm.stringToProxy<StatQueryPrx>("taf.tafstat.StatQueryObj");
    ostringstream os;
    StatQueryKey     tKey;
    StatQueryValue   tValue;
    tKey.name   = "head.slaveName";
    tKey.ip     = "22.11.11.22";
    pPtr->query(tKey,true,tValue);

    tKey.displaySimple(os);
    os<<"|";
    tValue.displaySimple(os);
    if(tValue.dataList.size()<1)
    {
       cout <<"no data"<<endl;
       return;
    }
    cout<<"|curpage|"<<tKey.page<<"|nextpage|"<<tValue.nextpage<<"|size|"<<tValue.dataList.size()
        <<"|"<<TC_Common::tm2str(tValue.dataList[0].curTime)<<endl;
    //cout<<os.str()<<endl;
    os.str("");
    int iEndPage = tValue.nextpage;
    do
    {
       tKey.page = tValue.nextpage;
       pPtr->query(tKey,true,tValue);
       cout<<"|curpage|"<<tKey.page<<"|nextpage|"<<tValue.nextpage<<"|size|"<<tValue.dataList.size()
       <<"|"<<TC_Common::tm2str(tValue.dataList[0].curTime)
       <<"|"<<tValue.dataList[0].body.maxRspTime
       <<"|"<<tValue.dataList[0].body.minRspTime
       <<"|"<<tValue.dataList[0].body.totalRspTime
       <<"|"<<tValue.dataList[0].body.count<<endl;
       tKey.displaySimple(os);
       os<<"|";
       tValue.displaySimple(os);
      // cout<<os.str()<<endl;
       os.str("");
    }  while(tValue.nextpage !=  iEndPage);
    LOG->debug() << "finish" << endl;
}
void usage(int argc, char *argv[])
{
    cout << "Usage:" << argv[0] << " --statip=172.27.34.213 --thread=5" << endl;
    exit(0);
}


int main( int argc, char* argv[] )
{
    try
    { 
        //TafRollLogger::getInstance()->logger()->setLogLevel("ERROR");   
	if(argc < 2)
	{
		usage(argc, argv);
	}

        int n= 10;
	TC_Option option;
	option.decode(argc, argv);
	if(option.hasParam("statip"))
	{
	    g_statIp = option.getValue("statip");
	}
	else
	{
	    g_statIp = "127.0.0.1";
	}

	if(option.hasParam("thread"))
	{
	    n = TC_Common::strto<int>(option.getValue("thread"));
	}
	else
	{
	    n = 10;
	}

        _comm.setProperty("modulename", "taf.tafstat");
        _comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.34.213 -p 17890");
        _comm.setProperty("stat", "taf.tafstat.StatObj");
        _comm.setProperty("report-interval", "1000");
        _comm.setProperty("sync-invoke-timeout", "600000");
        _comm.setProperty("async-invoke-timeout", "600000");
		pthread_t id[n];
		for (int i=0; i<n; i++)
		{
			pthread_create(&id[i], NULL, testStat, NULL);
		}
        //testStat();

		for (int i=0; i<n; i++)
		{
			pthread_join(id[i], NULL);
		}
        //getStat();
    }
    catch ( exception& ex )
    {
        cout << ex.what() << endl;
    }
    LOG->debug() << "finish" << endl;
    return 0;
}


