#include "servant/Communicator.h"
#include "servant/Application.h"
#include "util/tc_option.h"
#include <iostream>

using namespace std;
using namespace taf;

bool cmdSetLogLevel(const string& params)
{
    string level = TC_Common::trim(params);
    int ret = TafRollLogger::getInstance()->logger()->setLogLevel(level);
    if(ret != 0)
    {
        cout<<"set log level [" + level + "] error"<<endl;
    }
    return true;
}

void parse(int argc, char *argv[])
{
    TC_Option op;
    op.decode(argc, argv);
    //直接输出编译的TAF版本
    if(op.hasParam("version"))
    {
        cout << "TAF:" << TAF_VERSION << endl;
        exit(0);
    }
    
    string level = op.getValue("logLevel");
    if(level != "")
    {
        cmdSetLogLevel(level);
    }
}

int main(int argc, char* argv[])
{
    Communicator _comm;
    if(argc < 2)
    {
        cout<<"\n\n";
        cout<<"Usege "<<argv[0]<<"  sProxy [--logLevel=INFO|ERROR|WARN|DEBUG|NONE]\n\n";
        cout<<"ie:\n\n";
        cout<<argv[0]<<" \"Coin.AccountServer.AccountObj@tcp -h  127.0.0.1 -p 29985\" \n\n"; 
        cout<<argv[0]<<" \"Coin.AccountServer.AccountObj@tcp -h  127.0.0.1 -p 29985\" --logLevel=INFO\n\n"; 
        exit(-1);  
    }
    parse(argc,argv);
    typedef taf::TC_AutoPtr<ServantProxy> ServantProxyPtr;
    ServantProxyPtr pPrx;
    int64_t tNow = TC_Common::now2ms();
    int iRet = -1;
    try
    {
        _comm.stringToProxy(argv[1], pPrx);
        pPrx->taf_ping();
        iRet = 0;
    } 
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }
    catch(...)
    {
    }
    cout<<"\nping_status|"<<string(iRet==0?"Succ":"Fail")<<"|"<<argv[1]<<"|Time Use:"<<TC_Common::now2ms() -tNow<<"\n"<<endl;                
    return 0;
}


/////////////////////////////////////////////////////////////////
