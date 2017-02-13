#include "NotifyServer.h"
#include <iostream>

using namespace taf;

TC_Config * g_pconf;

int main(int argc, char *argv[])
{
    try
    {
        NotifyServer app;
        g_pconf =  & app.getConfig();
        app.main(argc, argv);

        app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cerr<< ex.what() << endl;
    }

    return 0;


}


