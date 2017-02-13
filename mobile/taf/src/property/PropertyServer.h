#ifndef __PROPERTY_SERVER_H_
#define __PROPERTY_SERVER_H_

#include "servant/Application.h"
#include "PropertyReapThread.h"

using namespace taf;

class PropertyServer : public Application
{
    
public:
    static void getTimeInfo(time_t &tTime,string &sDate,string &sFlag);
    
        
protected:
    /**
     * 初始化, 只会进程调用一次
     */
    virtual void initialize();

    /**
     * 析够, 每个进程都会调用一次
     */
    virtual void destroyApp();
    
public:
    static string g_sClonePatch;
    //数据库插入间隔
    static int g_iInsertInterval;

private:

    void initHashMap();
    
    PropertyReapThread* _pReapThread;
    


};

#endif

