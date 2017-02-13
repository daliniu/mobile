#ifndef __SERVER_COMMAND_H_
#define __SERVER_COMMAND_H_

#include "ServerObject.h"
#include "NodeServer.h"

class StatExChange;
typedef TC_AutoPtr<StatExChange> StatExChangePtr;

class ServerCommand : public TC_HandleBase
{
public:
    enum ExeStatus
    {   
        EXECUTABLE,        //可以运行
        DIS_EXECUTABLE,    //不可以运行
        NO_NEED_EXECUTE     //不需要运行
    };
public:
    virtual ~ServerCommand(){}
public:
    virtual ExeStatus canExecute(string &sResult){ return EXECUTABLE;}
    virtual int execute(string &sResult) = 0;
    virtual int doProcess()
    {
        string s ;
        return doProcess(s);
    }
    virtual int doProcess(string &sResult)
    {
        ExeStatus t = canExecute(sResult);
        if(t == DIS_EXECUTABLE)
        {
            return -1;
        }else if(t == NO_NEED_EXECUTE)
        {
            return 0;
        }
        return execute(sResult);
    }
    virtual int doProcess(const JceCurrentPtr current,string &sResult, bool bValid=true)
    {
        if( bValid && g_app.isValid(current->getIp()) == false )
        {
            sResult = " erro:ip "+ current->getIp()+" is invalid";
            return -1;
        }
       return doProcess(sResult);
    }      
};


class  StatExChange : public TC_HandleBase
{
public:

    /**
    * 构造函数
    * @param pServerObjectPtr ServerObject指针
    * @param eState1 状态1,构造时设置pServerObjectPtr 状态为state1
    * @param eState2 状态2,析构时设置pServerObjectPtr 状态为state2
    */
    StatExChange( const ServerObjectPtr& pServerObjectPtr, ServerObject::InternalServerState eState1, ServerObject::InternalServerState eState2 )
    {
        _pServerObjectPtr = pServerObjectPtr;
        _eState1 = eState1;
        _eState2 = eState2;
        _pServerObjectPtr->setState( _eState1 );
    }

    /**
     * 析够
     */
    ~StatExChange()
    {
        _pServerObjectPtr->setState( _eState2 );
    }

private:
    ServerObject::InternalServerState _eState1;
    ServerObject::InternalServerState _eState2;
    ServerObjectPtr _pServerObjectPtr;
};

#endif
