#include "LogServer.h"
#include <iostream>

using namespace taf;

int main(int argc, char *argv[])
{
    try
    {
        LogServer app;
		app.main(argc, argv);
		app.waitForShutdown();
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}


