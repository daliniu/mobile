#ifndef __DESTROY_COMMAND_H_
#define __DESTROY_COMMAND_H_

#include "ServerCommand.h"

/**
 * 销毁服务
 * 
 */
class CommandDestroy : public ServerCommand
{
public:
    CommandDestroy(const ServerObjectPtr &pServerObjectPtr,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
    
private:
    bool    _bByNode;
    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr; 
private:
    StatExChangePtr _pStatExChange;    
};

//////////////////////////////////////////////////////////////
//
inline CommandDestroy::CommandDestroy(const ServerObjectPtr &pServerObjectPtr,bool bByNode)
:_bByNode(bByNode)
,_pServerObjectPtr(pServerObjectPtr)
{ 
    _tDesc      = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandDestroy::canExecute(string &sResult)
{  
    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );
    
    LOG->debug() << _tDesc.application<< "." << _tDesc.serverName << " beging destroyed------|"<< endl;    

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();

    if (eState != ServerObject::Inactive)
    {
        sResult = "server state is not Inactive. the curent state is " + _pServerObjectPtr->toStringState( eState );
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }
    
    _pStatExChange = new StatExChange(_pServerObjectPtr, ServerObject::Destroying, ServerObject::Destroyed);
      
    return EXECUTABLE;
}


//////////////////////////////////////////////////////////////
//
inline int CommandDestroy::execute(string &sResult)
{   

    try
    {
        string sServerDir  = _pServerObjectPtr->getServerDir();
        string sServerId = _pServerObjectPtr->getServerId();
		
        if(TC_File::isFileExistEx(sServerDir, S_IFDIR))
        {
            TC_File::removeFile(sServerDir,true);
        }
	else
	{
		LOG->error() << "[" << sServerId << "]'s server path doesnot exist:" << sServerDir << endl;
	}

	//删除服务日志目录
	//>>added by spinnerxu@20110728
	string sLogPath = _pServerObjectPtr->getLogPath();
	if (sLogPath.empty())
	{//使用默认路径
		sLogPath = "/usr/local/app/taf/app_log/";
	}

	vector<string> vtServerNames = TC_Common::sepstr<string>(sServerId,".");
	if (vtServerNames.size() != 2)
	{
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|parse serverid error:" << sServerId << "|" << vtServerNames.size()  << endl;
		sResult = "failed to remove log path, server name error:" + sServerId;
		//return -1;
 	}
	else
	{
		sLogPath += vtServerNames[0] + "/" + vtServerNames[1];
		sLogPath = TC_File::simplifyDirectory(sLogPath);
		if (TC_File::isFileExistEx(sLogPath, S_IFDIR))
		{
			if (0 == TC_File::removeFile(sLogPath,true))
			{
				LOG->debug() << "remove log path success:" << sLogPath << "|" << sServerId << endl;
			}
			else
			{
				LOG->error() << "failed to remove log path:" << sLogPath << "|" << sServerId << endl;
			}
		}
		else
		{
			LOG->debug() << "[" << sServerId << "]'s log path doesnot exist:" << sLogPath << endl;
		}
	}
	//<<
    }
    catch (exception& e)
    {
        sResult =  "ServerObject::destroy Exception:"  + string(e.what());
        LOG->error() <<sResult;
        return -1;
    }
    return 0; 
}
#endif 
