#ifndef __PATCH_COMMAND_H_
#define __PATCH_COMMAND_H_

#include "ServerCommand.h"
#include "patch/taf_patch.h"

/**
 * 自patchserver下载服务
 * 
 */

class PatchClientInterface;
typedef TC_AutoPtr<PatchClientInterface> PatchClientInterfacePtr;

/**
 * patch的日志提示
 */
class PatchClientInterface : public TafPatchNotifyInterface
{
public:
    PatchClientInterface(ServerObjectPtr pServerObjectPtr)
    :_iTotalSize(0)
    ,_iCurrentSize(0)
    ,_pServerObjectPtr(pServerObjectPtr) 
    {};

    virtual void onDownload(const FileInfo &fi)
    {
        LOG->debug() << "------------------------------------" << endl;
        if (fi.size < 0 )
        {
            throw runtime_error(" no such file or directory ("+fi.path +") on patch server." );
        }
        LOG->debug() << "downloading [" << fi.path << "] [" << fi.size*1.0/1024 << "K]" << endl;
    }
    virtual void onDownloading(const FileInfo &fi, size_t pos, const string &dest)
    {
        LOG->debug() << "downloading [" << fi.path << " ->] [" << dest << "] [" << pos*100/fi.size << "/100%]" << endl;
        LOG->debug() << "downloading ok [" <<(_iCurrentSize+pos)*1.0/1024<<"K vs "<<_iTotalSize*1.0/1024<<"K "<< (_iCurrentSize+pos)*100.0/_iTotalSize << "/100%]" << endl;
        if (_pServerObjectPtr)
        {
            _pServerObjectPtr->setPatchPercent((int)((_iCurrentSize+pos)*100.0/_iTotalSize));
        }
    }
    virtual void onDownloadOK(const FileInfo &fi, const string &dest)
    {
        LOG->debug() << "download ok [" << fi.path << "] -> [" << dest << "]" << endl;
        _iCurrentSize+=fi.size;
        if (_pServerObjectPtr)
        {
            _pServerObjectPtr->setPatchPercent((int)(_iCurrentSize*100.0/_iTotalSize));
        }
    }

    virtual void onListFile()
    {
        LOG->debug() << "listing files" << endl;
    }

    virtual void onListFileOK(const vector<FileInfo> &vf)
    {
        for (unsigned i=0; i< vf.size();i++)
        {
            _iTotalSize += vf[i].size;
            LOG->debug() << "LIST:" << vf[i].path << "|SIZE:" << vf[i].size << "|MD5:" << vf[i].md5 << endl;
        }
        LOG->debug() << "list ok total [" << vf.size() << "] files ["<< _iTotalSize <<"] size "<< endl;
    }

    virtual void onRemoveLocalDirectory(const string &sDir)
    {
        LOG->debug() << "------------------------------------" << endl;
        LOG->debug() << "remove local directory [" << sDir << "]" << endl;
    }
    virtual void onRemoveLocalDirectoryOK(const string &sDir)
    {
        LOG->debug() << "remove local directory ok [" << sDir << "]"<< endl;
        LOG->debug() << "------------------------------------" << endl;
    }
    virtual void onSetExecutable(const FileInfo &fi)
    {
        LOG->debug() << "executable  [" << fi.path << "]" << endl;
    }
    virtual void onDownloadAllOK(const vector<FileInfo> &vf, const time_t timeBegin, const time_t timeEnd)
    {
        if (_pServerObjectPtr)
        {
            _pServerObjectPtr->setPatchPercent(100);
        }
        LOG->debug() << "------------------------------------" << endl;
        LOG->debug() << "download all files succ." << endl;
        if (_pServerObjectPtr)
        {
            ServerDescriptor svrDesc  = _pServerObjectPtr->getServerDescriptor();
            string sReport = "download " + svrDesc.application + "."+ svrDesc.serverName + " all files complete, version:" + _pServerObjectPtr->getPatchVersion() + ", begin:" + TC_Common::tostr<time_t>(timeBegin) + ", end:" + TC_Common::tostr<time_t>(timeEnd);

            g_app.reportServer(svrDesc.application + "." + svrDesc.serverName, sReport);
        }
    }

    virtual void onReportTime(const string & sFile, const time_t timeBegin, const time_t timeEnd)
    {
        if (_pServerObjectPtr)
        {
            ServerDescriptor svrDesc  = _pServerObjectPtr->getServerDescriptor();
            string sReport = "download '" + sFile +  "', version:" + _pServerObjectPtr->getPatchVersion() + ", begin:" + TC_Common::tostr<time_t>(timeBegin) + ", end:" + TC_Common::tostr<time_t>(timeEnd);

            g_app.reportServer(svrDesc.application + "." + svrDesc.serverName, sReport);
        }
    }
private:
    int     _iTotalSize;
    int     _iCurrentSize;   
    ServerObjectPtr     _pServerObjectPtr;             
};

class CommandPatch : public ServerCommand
{
public:
    CommandPatch(const ServerObjectPtr &pServerObjectPtr,const string &sPatchServer,const string &sDownLoadPath,bool bShutDown=false,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);

    /**
     * 设置发布版本号和发布者的信息
     */
    int updatePatchResult();

private:
    bool    _bByNode;
    bool    _bShutDown;
    string  _sPatchServer;
    string  _sPatchVersion;
    string  _sPatchUser;
    string  _sDownLoadPath;

    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr; 
private:
    StatExChangePtr _pStatExChange;
};

//////////////////////////////////////////////////////////////
//
inline CommandPatch::CommandPatch(const ServerObjectPtr &pServerObjectPtr,const string &sPatchServer,const string &sDownLoadPath,bool bShutDown,bool bByNode)
:_bByNode(bByNode)
,_bShutDown(bShutDown)
,_sPatchServer(sPatchServer)
,_sPatchVersion("")
,_sDownLoadPath(sDownLoadPath)
,_pServerObjectPtr(pServerObjectPtr)
{ 
    _tDesc  = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandPatch::canExecute(string &sResult)
{ 
    LOG->debug() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|" << sResult << endl;

    //1.6.5版本之后，version字段传递版本号和发布者
    string sVersion = sResult;
    string sUser    = "";
    std::string::size_type pos = sResult.find(",");
    if (pos != std::string::npos)
    {
        sVersion    = sResult.substr(0, pos);
        sUser       = sResult.substr(pos + 1);
    }
    LOG->debug() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|V:" << sVersion << "|U:" << sUser << endl;


    //设置发布信息
    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );

    LOG->debug()<<"patch begining "<<  _tDesc.application + "." + _tDesc.serverName<<endl;

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();

    if (_bShutDown == true && eState != ServerObject::Inactive)
    {
        sResult = "cannot patch the server,the server state is "+ _pServerObjectPtr->toStringState(eState);;
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }
    if (_pServerObjectPtr->toStringState(eState).find("ing") != string::npos)
    {
        sResult = "cannot patch the server ,the server state is " + _pServerObjectPtr->toStringState(eState);
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }
    _pStatExChange  = new StatExChange(_pServerObjectPtr, ServerObject::Patching, eState);


    //1.5.0版本后发布时自result携带版本信息
    //清空旧记录
    _sPatchVersion  = sVersion;
    _sPatchUser     = sUser;
    _pServerObjectPtr->setPatchVersion(_sPatchVersion);
    _pServerObjectPtr->setPatchPercent(0);
    _pServerObjectPtr->setPatchResult("");
    return EXECUTABLE;
}


//////////////////////////////////////////////////////////////
//
inline int CommandPatch::updatePatchResult()
{
    try
    {
        LOG->debug() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << endl;

        //服务发布成功，向主控发送UPDDATE命令
        string sRigistry  = g_app.getConfig().get("/taf/node<registryObj>", "taf.tafregistry.RegistryObj");
        RegistryPrx proxy = Application::getCommunicator()->stringToProxy<RegistryPrx>(sRigistry);
        if (!proxy)
        {
            std::string sResult = "patch succ but update version and user fault, get registry proxy fail";

            LOG->error() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|" << sResult << endl;
            g_app.reportServer(_tDesc.application + "." + _tDesc.serverName, sResult);

            return -1;
        }

        //向主控发送命令，设置该服务的版本号和发布者
        struct PatchResult patch;
        patch.sApplication  = _tDesc.application;
        patch.sServerName   = _tDesc.serverName;
        patch.sNodeName     = _tDesc.nodeName;
        patch.sVersion      = _sPatchVersion;
        patch.sUserName     = _sPatchUser;

        int iRet = proxy->updatePatchResult(patch);
        LOG->debug() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|Update:" << iRet << endl;
    }
    catch (exception& e)
    {
        LOG->error() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|Exception:" << e.what() << endl;
        return -1;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << _tDesc.application + "." + _tDesc.serverName << "|Unknown Exception" << endl;
        return -1;
    }

    return 0;
}


inline int CommandPatch::execute(string &sResult)
{   
    int iRet = -1;
    try
    {
        string sExePath         = _pServerObjectPtr->getExePath();
        string sLocalPach       = _sDownLoadPath+"/"+ _tDesc.application + "." + _tDesc.serverName;
        string sRemotePath      = "/" + _tDesc.application + "/" + _tDesc.serverName;
        TC_File::removeFile(sLocalPach,true);
        if (!TC_File::makeDirRecursive( sLocalPach ))
        {
            sResult =  "cannot create dir: " + sLocalPach;
        }
        else
        {
            TafPatch tafPatch;
            PatchPrx pPtr = Application::getCommunicator()->stringToProxy<PatchPrx>(_sPatchServer);
            //发布服务超时时间设置为1分钟
            pPtr->taf_timeout(60000);
            tafPatch.init(pPtr, sRemotePath, sLocalPach);
            tafPatch.setRemove(false);
            PatchClientInterfacePtr p = new PatchClientInterface(_pServerObjectPtr);
            tafPatch.download(p); 

            //------------------下载完毕以后，判断是否需要解压--------------------            
            vector<string> vec_files;
            TC_File::listDirectory(sLocalPach,vec_files, true);
            for( vector<string>::iterator iter = vec_files.begin(); iter != vec_files.end(); iter++ ) 
            {
                string filename = *iter;
                if(TC_File::extractFileExt(filename) == "tgz") 
                {
                    string cmd = "tar xzfv " + filename + " -C " + sLocalPach;
                    system(cmd.c_str());
                    LOG->debug() << "filename:" << filename << ", unzip ok." <<endl;
                    //解压完毕，删除tgz文件
                    TC_File::removeFile(filename,true);

                    //------------------解压完毕，拷贝目录到exepath-------------------------      
                    vector<string> vec;      
                    TC_File::listDirectory(sLocalPach, vec, false);//列出当前目录
                    for(vector<string>::iterator iter = vec.begin(); iter != vec.end(); iter++) 
                    {
                        string tmppath = *iter;
                        if(TC_File::isFileExist(tmppath, S_IFDIR))//如果这个路径为目录，则认为是TGZ包解压出来的那个目录
                        {
                            sLocalPach = tmppath;
                            break;
                        }
                    }
                }
            }
            sResult = "copy path " +sLocalPach+ " to " +sExePath;
            LOG->debug() << sResult << endl;
            TC_File::copyFile(sLocalPach,sExePath,true);
            _pServerObjectPtr->setPatched(true);  
            sResult = " patch "+_tDesc.application + "." + _tDesc.serverName+" succ,version "+_sPatchVersion;
            LOG->debug() << sResult<< endl;
            iRet = 0;

            if (!_sPatchVersion.empty() && !_sPatchUser.empty()) updatePatchResult();
        }
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
    _pServerObjectPtr->setPatchResult(sResult, iRet==0?true:false);
    LOG->error() <<sResult << endl;

    g_app.reportServer(_tDesc.application + "." + _tDesc.serverName, sResult);

    return iRet;
}



#endif
