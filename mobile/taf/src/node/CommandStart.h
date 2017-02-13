#ifndef __START_COMMAND_H_
#define __START_COMMAND_H_

#include "ServerCommand.h"

class CommandStart : public ServerCommand
{
public:
    enum
    {
        START_WAIT_INTERVAL  = 3,        /*服务启动等待时间 秒*/
        START_SLEEP_INTERVAL = 100000,   /*微妙*/
    };
public:
    CommandStart(const ServerObjectPtr &pServerObjectPtr,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
private:
	bool startNormal(string &sResult);
	bool startByScript(string &sResult);

private:
    bool    _bByNode;
    string  _sExeFile;
    string  _sLogPath;
    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr;
};

//////////////////////////////////////////////////////////////
//
inline CommandStart::CommandStart(const ServerObjectPtr &pServerObjectPtr,bool bByNode)
:_bByNode(bByNode)
,_pServerObjectPtr(pServerObjectPtr)
{
    _sExeFile   = _pServerObjectPtr->getExeFile();
    _sLogPath   = _pServerObjectPtr->getLogPath();
    _tDesc      = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandStart::canExecute(string &sResult)
{

    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );

    LOG->debug() << _tDesc.application<< "." << _tDesc.serverName << " beging activate------|byRegistry|"<<_bByNode<< endl;

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();

    if(_bByNode)
    {
        _pServerObjectPtr->setEnabled(true);
    }
    else if(!_pServerObjectPtr->isEnabled())
    {
        sResult = "server is disabled";
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if(eState == ServerObject::Active)
    {
        _pServerObjectPtr->synState();
        sResult = "server is already Active";
        LOG->debug() << sResult << endl;
        return NO_NEED_EXECUTE;
    }

    if (eState != ServerObject::Inactive)
    {
        sResult = "server state is not Inactive. the curent state is " + _pServerObjectPtr->toStringState( eState );
        LOG->debug() << sResult << endl;
        return DIS_EXECUTABLE;
    }

    if(TC_File::isAbsolute(_sExeFile) == true &&!TC_File::isFileExistEx(_sExeFile) && _pServerObjectPtr->getStartScript().empty())
    {
        _pServerObjectPtr->setPatched(false);
        sResult      = "The server exe patch " + _sExeFile+" is not exist." ;
        LOG->debug()<< sResult <<endl;
        return DIS_EXECUTABLE;
    }
    _pServerObjectPtr->setPatched(true);

    _pServerObjectPtr->setState( ServerObject::Activating,false); //此时不通知regisrty。checkpid成功后再通知

    return EXECUTABLE;
}

inline bool CommandStart::startByScript(string &sResult)
{
	pid_t iPid = -1;
	bool bSucc = false;
	string sStartScript     = _pServerObjectPtr->getStartScript();
	string sMonitorScript   = _pServerObjectPtr->getMonitorScript();

    string sServerId    = _pServerObjectPtr->getServerId();
    iPid = _pServerObjectPtr->getActivator()->activate(sServerId,sStartScript,sMonitorScript,sResult);

 	vector<string> vtServerName =  TC_Common::sepstr<string>(sServerId, ".");
 	if (vtServerName.size()!=2)
  	{
  		sResult = sResult + "|failed to get pid for  " + sServerId + ",server id error";
		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|" << sResult << endl;
		throw runtime_error(sResult);
  	}

 	//默认使用启动脚本路径
 	string sFullExeFileName = TC_File::extractFilePath(sStartScript) + vtServerName[1];
 	if (!TC_File::isFileExist(sFullExeFileName))
 	{
 		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|file name " << sFullExeFileName << " error, use exe file" << endl;

		//使用exe路径
		sFullExeFileName = _pServerObjectPtr->getExeFile();
		if (!TC_File::isFileExist(sFullExeFileName))
		{
			sResult = sResult + "|failed to get full exe file name|" + sFullExeFileName;
			LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|" << sResult << endl;
			throw runtime_error(sResult);
		}
 	}
    time_t tNow = TC_TimeProvider::getInstance()->getNow();
    int iStartWaitInterval = START_WAIT_INTERVAL;
    try
    {
        //服务启动,超时时间自己定义的情况
        TC_Config conf;
        conf.parseFile(_pServerObjectPtr->getConfigFile());
        iStartWaitInterval = TC_Common::strto<int>(conf.get("/taf/application/server<activating-timeout>","3000"))/1000;
        if(iStartWaitInterval < START_WAIT_INTERVAL)
        {
            iStartWaitInterval = START_WAIT_INTERVAL;
        }
        if(iStartWaitInterval > 60)
        {
            iStartWaitInterval = 60;
        }

    }
    catch (...)
    {
    }

 	string sPidFile = "/usr/local/app/taf/tafnode/util/" + sServerId + ".pid";
 	string sGetServerPidScript = "ps -ef | grep -v 'grep' |grep -iE ' " + sFullExeFileName + " '| awk '{print $2}' > " + sPidFile;

    while((TC_TimeProvider::getInstance()->getNow()- iStartWaitInterval) < tNow)
    {
		//注意:由于是守护进程,不要对sytem返回值进行判断,始终wait不到子进程
 		system(sGetServerPidScript.c_str());
     	string sPid = TC_Common::trim(TC_File::load2str(sPidFile));
      	if (TC_Common::isdigit(sPid))
      	{
      		iPid = TC_Common::strto<int>(sPid);
		  	_pServerObjectPtr->setPid(iPid);
           	if( _pServerObjectPtr->checkPid() == 0 )
            {
                bSucc = true;
				break;
            }
      	}

        LOG->debug() <<_tDesc.application<< "." << _tDesc.serverName<<" activating usleep "<<int(iStartWaitInterval)<< endl;
        usleep(START_SLEEP_INTERVAL);
    }

   	if( _pServerObjectPtr->checkPid() != 0 )
    {
  		sResult = sResult + "|get pid for server[" + sServerId + "],pid is not digit";
  		LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|" << sResult << endl;
        if (TC_File::isFileExist(sPidFile))
       	{
       		TC_File::removeFile(sPidFile,false);
       	}
		throw runtime_error(sResult);
  	}

    if (TC_File::isFileExist(sPidFile))
   	{
   		TC_File::removeFile(sPidFile,false);
   	}
	return bSucc;
}

inline bool CommandStart::startNormal(string &sResult)
{
	pid_t iPid = -1;
	bool bSucc  = false;
	string sRollLogFile;
	vector<string> vEnvs;
	vector<string> vOptions;
	string sConfigFile      = _pServerObjectPtr->getConfigFile();
	string sLogPath         = _pServerObjectPtr->getLogPath();
	string sServerDir       = _pServerObjectPtr->getServerDir();
	string sLibPath         = _pServerObjectPtr->getLibPath();
	string sExePath         = _pServerObjectPtr->getExePath();

	//环境变量设置
	vector<string> vecEnvs = TC_Common::sepstr<string>(_pServerObjectPtr->getEnv(), ";|");
	for (vector<string>::size_type i = 0; i < vecEnvs.size(); i++)
	{
	    vEnvs.push_back(vecEnvs[i]);
	}

    {
        vEnvs.push_back("LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"+ sExePath+":"+sLibPath);
    }

    //生成启动脚本
    std::ostringstream osStartStcript;
    osStartStcript << "#!/bin/sh" << std::endl;
    for (vector<string>::size_type i = 0; i < vEnvs.size(); i++)
    {
        osStartStcript << "export " << vEnvs[i] << std::endl;
    }

    osStartStcript << std::endl;
	if(_pServerObjectPtr->getServerType() == "taf_java")
	{
	    //java服务
	    string sClassPath       = _pServerObjectPtr->getClassPath();
	    string sJarList         = sExePath+"/classes";
	    vector<string> vJarList;
	    TC_File::scanDir(sClassPath,vJarList);

	    sJarList = sJarList + ":" + sClassPath + "/*";

	    vOptions.push_back( "-Dconfig=" +sConfigFile);
	    string s = _pServerObjectPtr->getJvmParams()+ " -cp "+sJarList+" "+_pServerObjectPtr->getMainClass();
	    vector<string> v = TC_Common::sepstr<string>(s," \t");
	    vOptions.insert( vOptions.end(), v.begin(), v.end() );

		osStartStcript << _sExeFile << " " << TC_Common::tostr(vOptions)<< endl;
	}
	else
	{
	    //c++服务
	    vOptions.push_back( "--config=" +sConfigFile);
	    osStartStcript << _sExeFile << " " << TC_Common::tostr(vOptions) << " &" << endl;
	}
	//保存启动方式到bin目录供手工启动
	TC_File::save2file(sExePath + "/taf_start.sh", osStartStcript.str());
	TC_File::setExecutable(sExePath + "/taf_start.sh", true);

	if(sLogPath != "")
	{
	    sRollLogFile = sLogPath+"/"+_tDesc.application+"/"+_tDesc.serverName+"/"+_tDesc.application+"."+_tDesc.serverName+".log";
	}

	const string sPwdPath = sLogPath != "" ? sLogPath : sServerDir; //pwd patch 统一设置至log目录 以便core文件发现 删除
	iPid = _pServerObjectPtr->getActivator()->activate(_sExeFile, sPwdPath,sRollLogFile, vOptions, vEnvs );
	if(iPid == 0)  //child process
	{
	    return false;
	}

	_pServerObjectPtr->setPid(iPid);

	bSucc =(_pServerObjectPtr->checkPid() == 0)? true : false;

	return bSucc;
}
//////////////////////////////////////////////////////////////
//
inline int CommandStart::execute(string &sResult)
{
    try
    {
		bool bSucc  = false;
		//set stdout & stderr
		_pServerObjectPtr->getActivator()->setRedirectPath(_pServerObjectPtr->getRedirectPath());

		if(!_pServerObjectPtr->getStartScript().empty() || _pServerObjectPtr->isTafServer() == false )
		{
			bSucc = startByScript(sResult);
		}
		else
		{
			bSucc = startNormal(sResult);
		}

        if( bSucc == true)
        {
            _pServerObjectPtr->synState();
            _pServerObjectPtr->setLastKeepAliveTime(TC_TimeProvider::getInstance()->getNow());
			_pServerObjectPtr->setLimitInfoUpdate(true);
            LOG->debug() << _tDesc.application<< "." << _tDesc.serverName << "_" << _tDesc.nodeName << " start succ " << sResult << endl;
            return 0;
        }
    }
    catch (exception& e)
    {
        sResult = e.what();
    }
    catch (const std::string& e)
    {
        sResult = e;
    }
    catch (...)
    {
        sResult = "catch unkwon exception";
    }
    LOG->error() << _tDesc.application<< "." << _tDesc.serverName<< " start  failed :" << sResult << endl;
    _pServerObjectPtr->setPid(0);
    _pServerObjectPtr->setState(ServerObject::Inactive);
    return -1;
}
#endif
