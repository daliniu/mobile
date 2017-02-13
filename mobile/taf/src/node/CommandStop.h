#ifndef __STOP_COMMAND_H_
#define __STOP_COMMAND_H_

#include "ServerCommand.h"

class CommandStop : public ServerCommand
{
public:
    enum
    {
        STOP_WAIT_INTERVAL  = 3,        /*服务关闭等待时间 秒*/
        STOP_SLEEP_INTERVAL = 100000,   /*微妙*/
    };
public:
    CommandStop(const ServerObjectPtr &pServerObjectPtr,bool bByNode = false,bool bGenerateCore = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);

private:
    bool   _bGenerateCore;
    bool    _bByNode;
    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandStop::CommandStop(const ServerObjectPtr &pServerObjectPtr,bool bByNode,bool bGenerateCore)
:_bGenerateCore(bGenerateCore)
,_bByNode(bByNode)
,_pServerObjectPtr(pServerObjectPtr)
{
    _tDesc      = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandStop::canExecute(string &sResult)
{
    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );

    LOG->debug() << _tDesc.application<< "." << _tDesc.serverName << " beging deactivate------|byRegistry|"<<_bByNode<< endl;

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();

    if(_bByNode)
    {
        _pServerObjectPtr->setEnabled(false);
    }

    if (eState == ServerObject::Inactive)
    {
        _pServerObjectPtr->synState();
        sResult = "server state is Inactive. ";
        LOG->debug() << sResult << endl;
        return NO_NEED_EXECUTE;
    }
    if (eState == ServerObject::Destroying)
    {
        sResult = "server state is Destroying. ";
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if (eState == ServerObject::Patching || eState == ServerObject::BatchPatching)
    {
        PatchInfo tPatchInfo;
        _pServerObjectPtr->getPatchPercent(tPatchInfo);
        //状态为Pathing时 10秒无更新才允许stop
        std::string sPatchType = eState == ServerObject::BatchPatching?"BatchPatching":"Patching";
        time_t iTimes          = eState == ServerObject::BatchPatching?10*60:10;
        if( TC_TimeProvider::getInstance()->getNow() - tPatchInfo.iModifyTime < iTimes)
        {
            sResult = "server is " + sPatchType + " " + TC_Common::tostr(tPatchInfo.iPercent) + "%, please try again later.....";
            LOG->debug() << "CommandStop::canExecute|" << sResult << endl;
            return DIS_EXECUTABLE;
        }
        else
        {
            LOG->debug() << "server is patching "<<tPatchInfo.iPercent<<"% ,and no modify info for "<<TC_TimeProvider::getInstance()->getNow() - tPatchInfo.iModifyTime<<"s"<< endl;
        }
    }

    _pServerObjectPtr->setState(ServerObject::Deactivating);

    return EXECUTABLE;

}


//////////////////////////////////////////////////////////////
//
inline int CommandStop::execute(string &sResult)
{
    try
    {
        pid_t pid = _pServerObjectPtr->getPid();
	LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|pid|" << pid << endl;
        if(pid != 0)
        {
            string f = "/proc/"+TC_Common::tostr(pid)+"/status";
            LOG->debug()<<"print the server status :"<<f<<endl;
            LOG->debug()<<TC_File::load2str(f)<<endl;
        }

       string sStopScript   = _pServerObjectPtr->getStopScript();
       //配置了脚本或者非taf服务
       if(!sStopScript.empty()|| _pServerObjectPtr->isTafServer()== false)
       {
           map<string,string> mResult;
           string sServerId     = _pServerObjectPtr->getServerId();
           _pServerObjectPtr->getActivator()->doScript(sServerId,sStopScript,sResult,mResult);
       }
       else
       {
           AdminFPrx pAdminPrx;    //服务管理代理
           pAdminPrx = Application::getCommunicator()->stringToProxy<AdminFPrx>("AdminObj@"+_pServerObjectPtr->getLocalEndpoint().toString());
           pAdminPrx->async_shutdown(NULL);
       }

    }
    catch (exception& e)
    {
        LOG->debug() <<_tDesc.application<< "." << _tDesc.serverName<<" shut down result:|" << e.what() << "|use kill -9 for check" << endl;
        pid_t pid = _pServerObjectPtr->getPid();
        _pServerObjectPtr->getActivator()->deactivate(pid);
    }
    catch (...)
    {
        LOG->debug() <<_tDesc.application<< "." << _tDesc.serverName<<" shut down fail:|" << "|use kill -9 for check" << endl;
        pid_t pid = _pServerObjectPtr->getPid();
        _pServerObjectPtr->getActivator()->deactivate(pid);
    }

    //等待STOP_WAIT_INTERVAL秒
    time_t tNow = TC_TimeProvider::getInstance()->getNow();
    int iStopWaitInterval = STOP_WAIT_INTERVAL;
    try
    {
        //服务停止,超时时间自己定义的情况
        TC_Config conf;
        conf.parseFile(_pServerObjectPtr->getConfigFile());
        iStopWaitInterval = TC_Common::strto<int>(conf["/taf/application/server<deactivating-timeout>"])/1000;
        if(iStopWaitInterval < STOP_WAIT_INTERVAL)
        {
            iStopWaitInterval = STOP_WAIT_INTERVAL;
        }
        if(iStopWaitInterval > 60)
        {
            iStopWaitInterval = 60;
        }

    }
    catch (...)
    {
    }
    while(TC_TimeProvider::getInstance()->getNow()- iStopWaitInterval < tNow)
    {
        if ( _pServerObjectPtr->checkPid() != 0)  //如果pid已经不存在
        {
            _pServerObjectPtr->setPid(0);
            _pServerObjectPtr->setState(ServerObject::Inactive);
			LOG->debug() <<_tDesc.application<< "." << _tDesc.serverName<<"server already stop"<< endl;
            return 0;
        }
        LOGINFO(_tDesc.application<< "." << _tDesc.serverName<<" deactivating usleep "<<int(STOP_SLEEP_INTERVAL)<< endl);
        usleep(STOP_SLEEP_INTERVAL);
    }


    LOG->debug() <<_tDesc.application<< "." << _tDesc.serverName<<" shut down timeout ( used "<<iStopWaitInterval<< "'s),use kill -9" << endl;

    //仍然失败。用kill -9，再等待STOP_WAIT_INTERVAL秒。
    pid_t pid = _pServerObjectPtr->getPid();
    if(_bGenerateCore == true)
    {
        _pServerObjectPtr->getActivator()->deactivateAndGenerateCore(pid);
    }
    else
    {
         _pServerObjectPtr->getActivator()->deactivate(pid);
    }

    tNow = TC_TimeProvider::getInstance()->getNow();
    while(TC_TimeProvider::getInstance()->getNow()-STOP_WAIT_INTERVAL < tNow)
    {
        if (_pServerObjectPtr->checkPid() != 0)  //如果pid已经不存在
        {
            _pServerObjectPtr->setPid(0);
            _pServerObjectPtr->setState(ServerObject::Inactive);
            return 0;
        }
        usleep(STOP_SLEEP_INTERVAL);
    }
    sResult ="server pid "+TC_Common::tostr(pid)+" is still exist";
    return  -1;
}
#endif
