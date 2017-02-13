#ifndef _LOG_SERVER_H
#define _LOG_SERVER_H

#include "servant/Application.h"

using namespace taf;

class LogServer : public Application
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
private:
	bool loadLogFormat(const string& command, const string& params, string& result);
};

#endif

