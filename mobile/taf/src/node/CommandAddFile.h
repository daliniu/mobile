#ifndef __ADDFILE_COMMAND_H_
#define __ADDFILE_COMMAND_H_

#include "ServerCommand.h"

/**
 *自config服务下载文件 
 *
 */
class CommandAddFile : public ServerCommand
{
    
public:
    CommandAddFile(const ServerObjectPtr &pServerObjectPtr,const string &sFile,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
private:
    int getScriptFile(string &sResult);
    bool                _bByNode;
    string              _sFile; 
    string              _sServerId;               
    ServerDescriptor    _tDesc;
    StatExChangePtr     _pStatExChange;
    ServerObjectPtr     _pServerObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandAddFile::CommandAddFile(const ServerObjectPtr &pServerObjectPtr,const string &sFile,bool bByNode)
:_bByNode(bByNode)
,_sFile(sFile)
,_pServerObjectPtr(pServerObjectPtr)
{ 
    _tDesc      = _pServerObjectPtr->getServerDescriptor();
    _sServerId  = _pServerObjectPtr->getServerId();
}

//////////////////////////////////////////////////////////////////////////////
// 
inline ServerCommand::ExeStatus CommandAddFile::canExecute(string &sResult)
{
  
    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );
    
    LOG->debug() << _tDesc.application<< "." << _tDesc.serverName << " beging addFilesing------|"<< endl;    

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();
    
    if(_tDesc.application == "" || _tDesc.serverName == "" )
    {
        sResult = "app or server name is empty";
        LOG->debug()<< sResult <<endl;
        return DIS_EXECUTABLE;
    }
    if(_sFile.empty())
    {
        sResult = "file is empty. file:"+_sFile;
        LOG->debug()<< sResult <<endl;
        return DIS_EXECUTABLE;
    }
    //设当前状态为正在AddFilesing
    _pStatExChange = new StatExChange(_pServerObjectPtr,ServerObject::AddFilesing, eState);
    
    return EXECUTABLE;      
}
 
//////////////////////////////////////////////////////////////
//
inline int CommandAddFile::execute(string &sResult)
{
  try
  {       
        //若File不合法采用默认路径
        if( TC_File::isAbsolute(_sFile ) ==  false )
        {
            _sFile = _pServerObjectPtr->getExePath()+"/"+_sFile;
        }
        _sFile              = TC_File::simplifyDirectory(TC_Common::trim(_sFile));

        string sFilePath    = TC_File::extractFilePath(_sFile); 
        
        string sFileName    = TC_File::extractFileName(_sFile);      
        //创建目录
        if(!TC_File::makeDirRecursive(sFilePath ) )
        {
            sResult =  "cannot create dir:" + sFilePath + " erro:"+ strerror(errno);
            LOG->error() <<sResult<< endl;
            return -1;
        }
        bool bRet = false;
        if(_pServerObjectPtr->isScriptFile(sFileName) == true)
        {
            bRet = getScriptFile(sResult);
        }  
        else
        {
            TafRemoteConfig tTafRemoteConfig;
            tTafRemoteConfig.setConfigInfo(Application::getCommunicator(), ServerConfig::Config,_tDesc.application, _tDesc.serverName, sFilePath,_tDesc.setId);
            bRet = tTafRemoteConfig.addConfig(sFileName,sResult);
            g_app.reportServer(_pServerObjectPtr->getServerId(),sResult); 
        }
        return bRet;               
  } 
  catch(exception &e)
  {
      sResult = "get file:"+_sFile+" from configServer fail."+ e.what();
  }
  LOG->error() <<sResult<< endl;
  return -1;
}

inline int CommandAddFile::getScriptFile(string &sResult)
{
    try
    {     
        string sFileName    = TC_File::extractFileName(_sFile); 
                  
        ConfigPrx pPtr = Application::getCommunicator()->stringToProxy<ConfigPrx>(ServerConfig::Config);
        string sStream;
		int ret;
		if(_tDesc.setId.empty())
		{
			ret = pPtr->loadConfig(_tDesc.application, _tDesc.serverName, sFileName, sStream);
		}
		else
		{
			struct ConfigInfo confInfo;
			confInfo.appname = _tDesc.application;
			confInfo.servername = _tDesc.serverName;
			confInfo.filename = sFileName;
			confInfo.setdivision = _tDesc.setId;
			ret = pPtr->loadConfigByInfo(confInfo,sStream);
		}

        if (ret != 0)
        {
            sResult = "[fail] get remote file :" + sFileName;
            LOG->error() <<sResult<< endl;
            g_app.reportServer(_sServerId,sResult); 
            return -1;
        }
        sStream = _pServerObjectPtr->decodeMacro(sStream);
        string sTem;
        string::size_type pos = sStream.find('\n');
        while(pos != string::npos)
        {            
            sTem        = sTem + TC_Common::trim(sStream.substr(0,pos)," \r\t") +"\n";
            sStream     = sStream.substr(pos+1); 
            pos         = sStream.find('\n');     
        }               
        sStream = sTem + sStream;
        string sEndMark = "echo 'end-"+sFileName+"'";
        if(sStream.find(sEndMark) == string::npos)
        {
            sStream = sStream + "\n"+sEndMark ;   //tafnode 生成的脚本都以 'echo 'end filename'' 结束 
        }                   
        if(TC_File::isFileExist(_sFile) && TC_File::load2str(_sFile) != sStream)
        {
            TC_File::copyFile(_sFile,_sFile+".bak");
        }
        ofstream out(_sFile.c_str());
        if(!out.good())
        {
            sResult = "cannot create file: " + _sFile + " erro:"+ strerror(errno);;
            LOG->error() << sResult << endl;
            return -1;
        }
        out<<sStream;
        out.close();  
        sResult = "[succ] get remote file :"+sFileName;
        g_app.reportServer(_pServerObjectPtr->getServerId(),sResult); 
        LOG->debug() << sResult << endl;
        return 0;          
  } 
  catch(exception &e)
  {
      sResult = "get file:"+_sFile+" from configServer fail."+ e.what();
  }  
  return -1; 
}
#endif 
