#include "PatchServer.h"
#include <iostream>

using namespace std;

TC_Config *g_conf;

/**
 * 说明
 * 1 需要配置一个路径
 * 2 如果子目录是link目录, 则不进入link目录遍历
 * 3 如果文件是link, 则读取link的文件
 * 4 如果同步的文件是link目录, 则进入link目录遍历, 下面的规则如1
 * @param argc
 * @param argv
 *
 * @return int
 */
int main(int argc, char *argv[])
{
    try
    {
        PatchServer app;
        g_conf = &app.getConfig();
        app.main(argc, argv);
        app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


