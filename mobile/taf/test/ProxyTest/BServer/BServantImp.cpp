#include "BServantImp.h"
#include "BServer.h"
#include "servant/Application.h"
#include "servant/Communicator.h"

using namespace std;
using namespace taf;

//////////////////////////////////////////////////////
void BServantImp::initialize()
{
	//initialize servant here:
	//...
    _pPrx = Application::getCommunicator()->stringToProxy<AServantPrx>("Test.AServer.AServantObj");
}
//////////////////////////////////////////////////////
void BServantImp::destroy()
{
}
  
class AServantCallback : public AServantPrxCallback
{

public:
	AServantCallback(JceCurrentPtr &current): _current(current){}
    virtual void callback_saysomething(const std::string& _ret)
	{ 	
		//assert(r == "hello word");
		AServant::async_response_saysomething(_current,_ret);
		
	}
    virtual void callback_saysomething_exception(taf::Int32 ret)
	{ 	
		//assert(ret == 0);
		cout << "callback exception:" << ret << endl; 
	}

	JceCurrentPtr _current;
};



int BServantImp::test(taf::JceCurrentPtr current) { return 0;}

string BServantImp::dohandle(const string &s, JceCurrentPtr current)
{
		//发起到另外一个服务的异步调用
		/*BServantPrx pPtr = BServer::getCommunicator()->stringToProxy<BServantPrx>("Test.BServer.BServantObj@tcp -h 127.0.0.1 -p 13655");
		BServantPrxCallback* cb = new BServantCallback(current);
		pPtr->async_dohandle(cb,s);*/


        Test::AServantPrxCallbackPtr cb = new AServantCallback(current);
		_pPrx->async_saysomething(cb,s);
		current->setResponse(false);
		//async_saysomething(AServantPrxCallbackPtr callback,const std::string & something,
		return "";

	  /*AServantPrx pPtr = AServer::getCommunicator()->stringToProxy<AServantPrx>("Test.AServer.AServantObj@tcp -h 127.0.0.1 -p 13610");
		AServantPrxCallback* cb = new AServantCallback(current);
		pPtr->async_saysomthing(cb,s);
		return 0;*/
}



