#include "BServant.h"
#include "servant/Communicator.h"
#include "wbl.h"
#include "wbl/thread_pool.h"
#include <iostream>

using namespace std;
using namespace Test;
using namespace taf;
using namespace wbl;

class Test1
{
public:
	Test1();
	~Test1();
	void  th_dohandle(int excut_num);
private:
	Communicator _comm;
	BServantPrx prx;
};

Test1::Test1()
{
	 //Test::AServantPrx ptr;
     //Application::getCommunicator()->stringToProxy("Test.BServer.BServantObj@tcp -h 10.148.144.147 -p 13655", prx);
	 _comm.stringToProxy("Test.BServer.BServantObj@tcp -h 10.148.144.147 -p 13655",prx);
}

Test1::~Test1()
{
	
}

void Test1::th_dohandle(int excut_num)
{
	struct timeval tStart, tEnd;
	taf::Int32 count = 0;
	gettimeofday(&tStart,NULL);

	for(int i=0; i<excut_num; i++) 
	{
		string r = prx->dohandle("hello world");
		count++;
		//taf::Int32 iRet = proxyPrx->dohandle("hello word");
	}
	gettimeofday(&tEnd, NULL);	
	cout << TC_Common::tostr<taf::Int32>(1000000*(tEnd.tv_sec-tStart.tv_sec)+(tEnd.tv_usec-tStart.tv_usec)) << endl;
	cout << "succ:" << count <<endl;
}

int main(int argc,char ** argv)
{
		/*Communicator _comm;
	
		try
		{
			BServantPrx prx;
			_comm.stringToProxy("Test.BServer.BServantObj@tcp -h 10.148.144.147 -p 13655",prx);*/

		Test1 test1;
        try
        {
			taf::Int32 threads = TC_Common::strto<taf::Int32>(string(argv[1]));
			thread_pool tp;
            if(tp.start(threads))
            {
                cout << "init tp succ" << endl;
            }
			taf::Int32 times = TC_Common::strto<taf::Int32>(string(argv[2]));
			
			for(int i = 0; i<threads; i++) 
			{
				tp.execute_memfun(test1,&Test1::th_dohandle,times);
				cout<<"***********"<<endl;
			}
            tp.wait(); 
        }catch(exception &e)
        {
            cout<<e.what()<<endl;
        }
        catch(...)
        {
            
        }
    /*}
    catch(std::exception& e)
    {
        cerr<<"std::excepton:"<<e.what()<<std::endl;
    }
    catch(...)
    {
        cout<<"unknow exception."<<std::endl;
    }*/
	
    return 0;
}
