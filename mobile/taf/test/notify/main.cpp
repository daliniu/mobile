#include "util/tc_common.h"
#include "notify/Notify.h"

#include <iostream>

using namespace std;
using namespace taf;

int main(int argc, char *argv[])
{
    try
	{
		//==========================================
		Communicator _comm;
		//NotifyPrx notify = _comm.stringToProxy<NotifyPrx>("taf.onenotify.NotifyObj@tcp -h 172.25.38.67 -p 4435 -t 3000");
        NotifyPrx notify = _comm.stringToProxy<NotifyPrx>("taf.onenotify.NotifyObj@tcp -h 172.25.38.67 -p 54435 -t 3000");  //mine

        cout << time(NULL) << endl;
        for(int i=0; i< 123; i ++)
		    notify->notifyServer("taf.onenotify", "12345", "notify", "this is notify ---"+TC_Common::tostr(i) );

        cout << time(NULL) << endl;
        NotifyKey k;
        k.name = "taf.onenotify";
        k.ip = "172.25.38.67";
        NotifyInfo stInfo;

        for(int i=0;i< 7; i++)
        {
            k.page = i;
            cout<<notify->getNotifyInfo(k, stInfo) << endl;
        }


	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

    cout << "**********************" << endl;
	return 0;
}



