#ifndef __COMMAND_DOWNLOAD_H__
#define __COMMAND_DOWNLOAD_H__

#include "ServerCommand.h"
#include "patch/taf_patch.h"

class CommandDownload : public ServerCommand
{
public:
    CommandDownload(ServerObjectPtr pServerObjectPtr, const string & sPatchServer, const string & srcFile, const string & dstFile);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);

private:
    string _sPatchServer;
    string _sSrcFile;
    string _sDstFile;

    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr; 
    StatExChangePtr     _pStatExChange;
};

//////////////////////////////////////////////////////////////
//
inline CommandDownload::CommandDownload(ServerObjectPtr pServerObjectPtr, const string & patchServer, const string & srcFile, const string & dstFile)
:_sPatchServer(patchServer)
,_sSrcFile(srcFile)
,_sDstFile(dstFile)
,_pServerObjectPtr(pServerObjectPtr)
{
    if (_pServerObjectPtr) _tDesc  = _pServerObjectPtr->getServerDescriptor();
}

//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandDownload::canExecute(string &sResult)
{ 
    if (!_pServerObjectPtr) return EXECUTABLE;

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();
    if (_pServerObjectPtr->toStringState(eState).find("ing") != string::npos)
    {
        sResult = "cannot patch the server ,the server state is " + _pServerObjectPtr->toStringState(eState);
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }

    _pStatExChange = new StatExChange(_pServerObjectPtr, ServerObject::Patching, eState);

    _pServerObjectPtr->setPatchPercent(0);
    return EXECUTABLE;
}

//////////////////////////////////////////////////////////////
//
inline int CommandDownload::execute(string &sResult)
{   
    int iRet = -1;

    try
    {
        PatchPrx pPtr = Application::getCommunicator()->stringToProxy<PatchPrx>(_sPatchServer);
        pPtr->taf_timeout(60000);//发布服务超时时间设置为1分钟

        PatchClientInterfacePtr p = new PatchClientInterface(_pServerObjectPtr);

        TafPatch tafPatch;
        tafPatch.init(pPtr, _sSrcFile, _sDstFile, false);
        tafPatch.setRemove(false);
        tafPatch.downloadSubborn(p); 

        if (_pServerObjectPtr) _pServerObjectPtr->setPatched(true);
        sResult = "download " + _sSrcFile + ",succ";
        iRet = 0;
    } 
    catch (exception& e)
    {
        sResult += e.what();
    }
    catch (const std::string& e)
    {
        sResult += e;
    }
    catch (...)
    {
        sResult += "catch unkwon exception";
    }

    if (_pServerObjectPtr) _pServerObjectPtr->setPatchResult(sResult, iRet==0?true:false);
    LOG->error() << __FILE__ << "|" << __LINE__ << "|S:" << _sSrcFile << "|D:" << _sDstFile << "|R:" << sResult << endl;

    return iRet;
}

#endif

