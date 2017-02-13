#include "servant/Application.h"
#include "Patch.h"

using namespace taf;

class PatchServer : public Application
{
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 每个进程都会调用一次
     */
    virtual void destroyApp();
};


