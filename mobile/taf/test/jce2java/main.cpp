#include "TestServer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        Server app;
        app.main(argc, argv);
        app.waitForShutdown();
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}


