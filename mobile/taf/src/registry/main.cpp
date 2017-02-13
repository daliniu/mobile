#include "RegistryServer.h"
#include <iostream>

using namespace taf;

RegistryServer g_app;
TC_Config * g_pconf;

int main(int argc, char *argv[])
{
    try
    {
        g_pconf =  & g_app.getConfig();
        g_app.main(argc, argv);

        g_app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cerr<< ex.what() << endl;
    }

    return 0;
}


