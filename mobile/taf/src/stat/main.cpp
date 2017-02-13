#include "StatServer.h"
#include <iostream>

using namespace std;

TC_Config* g_pconf;

StatServer g_app;

/**
 * @param argc
 * @param argv
 *
 * @return int
 */
int main( int argc, char* argv[] )
{
    try
    {
        //StatServer app;
        g_pconf = &g_app.getConfig();
        g_app.main( argc, argv );
        LOG->debug() << "main init succ" << endl;
        g_app.waitForShutdown();
    }
    catch ( exception& ex )
    {
        cout << ex.what() << endl;
    }

    return 0;
}


