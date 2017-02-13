#include "BServant.h"
#include "AServant.h"
#include "servant/Communicator.h"
#include <iostream>

using namespace std;
using namespace Test;
using namespace taf;

int main(int argc,char ** argv)
{
    Communicator _comm;
	_comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.194.147 -p 17890");

    try
    {
        BServantPrx prx;
		AServantPrx a_prx;
		
        _comm.stringToProxy("Test.BServer.BServantObj",prx);
		_comm.stringToProxy("Test.AServer.AServantObj",a_prx);
        try
        {
			for(int i = 0; i < 10; i++)
			{
				string  ret2=prx->taf_hash(1)->dohandle("hello world");
				prx->taf_clear_hash();
				string r = a_prx->saysomething("hello world");
			}
            
        }catch(exception &e)
        {
            cout<<"abc"<<e.what()<<endl;
        }
        catch(...)
        {
            
        }
    }
    catch(std::exception& e)
    {
        cerr<<"std::excepton:"<<e.what()<<std::endl;
    }
    catch(...)
    {
        cout<<"unknow exception."<<std::endl;
    }
	
    return 0;
}
