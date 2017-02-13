#ifndef __PLAT_FORM_INFO_H_
#define __PLAT_FORM_INFO_H_
#include "Node.h"
#include <unistd.h>
#include "util/tc_config.h"
#include <iostream>
#include "log/taf_logger.h"
#include "servant/Application.h"

using namespace taf;
using namespace std;

extern TC_Config* g_pconf;
extern CommunicatorPtr g_pcomm;

class PlatformInfo
{
public:
    NodeInfo getNodeInfo() const;
    LoadInfo getLoadInfo() const;
    string getNodeName() const;
    string getDataDir() const;
    string getDownLoadDir() const; //文件下载目录
};

#endif

