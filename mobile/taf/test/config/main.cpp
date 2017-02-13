#include "util/tc_common.h"
#include "Config.h"

#include <iostream>

using namespace std;
using namespace taf;

int main(int argc, char *argv[])
{
    try
	{
		//==========================================
		Communicator _comm;
		ConfigProxyPtr configPtr = _comm.stringToProxy<ConfigProxyPtr>("ConfigObj@tcp -h 10.1.36.39 -p 8890 -t 3000");

		//cout<<configPtr->loadConfig("Comm", "DbServer", "Db.conf", "")<<endl;
		cout<<configPtr->loadConfig("taf", "RegistryServer","10.1.36.39","Registry.conf")<<endl;


	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

    cout << "**********************" << endl;
	return 0;
}



