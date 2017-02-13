#include "ServerObject.h"
#include "RegistryProxy.h"
#include "util/tc_clientsocket.h"
#include "servant/Communicator.h"
#include "NodeServer.h"

ServerObject::ServerObject( const ServerDescriptor& tDesc)
:_bTafServer(true)
,_bLoaded(false)
,_bPatched(true)
,_bNoticed(false)
,_uNoticeFailTimes(0)
,_bEnSynState(true)
,_pid(0)
,_eState(ServerObject::Inactive)
,_bLimitStateUpdated(false)
{
    //60秒内最多启动10次，达到10次启动仍失败后,每隔600秒再重试一次
    _pActivatorPtr  = new Activator(60,10,600);

    //服务相关修改集中放到setServerDescriptor函数中
     setServerDescriptor(tDesc);

	_pServiceLimitResource = new ServerLimitResource(5,10,60,_sApplication,_sServerName);

}

void ServerObject::setServerDescriptor( const ServerDescriptor& tDesc )
{
    _tDesc          = tDesc;
    _sApplication   = tDesc.application;
    _sServerName    = tDesc.serverName;
    _sServerId      = _sApplication+"."+_sServerName;
    _tDesc.settingState == "active"?_bEnabled = true:_bEnabled = false;

    time_t now = TC_TimeProvider::getInstance()->getNow();
    _mAdapterKeepAliveTime.clear();

     map<string, AdapterDescriptor>::const_iterator itAdapters;
     for( itAdapters = _tDesc.adapters.begin(); itAdapters != _tDesc.adapters.end(); itAdapters++)
     {
         _mAdapterKeepAliveTime[itAdapters->first] = now;
     }
     _mAdapterKeepAliveTime["AdminAdapter"] = now;
     setLastKeepAliveTime(now);
}

bool ServerObject::isAutoStart()
{
    Lock lock(*this);
    if(toStringState(_eState).find( "ing" ) != string::npos)  //正处于中间态时不允许重启动
    {
		LOG->debug() << _sApplication << "." << _sServerName <<" not allow to restart in (ing) state"<<endl;
        return false;
    }
    if(!_bEnabled||!_bLoaded || !_bPatched || _pActivatorPtr->isActivatingLimited())
    {
		if(_pActivatorPtr->isActivatingLimited())
		{
			LOG->debug() << _sApplication << "." << _sServerName <<" not allow to restart in limited state"<<endl;
		}
        return false;
    }
    return true;
}

ServerState ServerObject::getState()
{
    Lock lock(*this);
    LOGINFO(_sApplication << "." << _sServerName <<"'s state is "<<toStringState(_eState)<<endl);
    return toServerState( _eState );
}

//////////////////////////////////////////////////////////////////


void ServerObject::setState(InternalServerState eState, bool bSynState)
{
    Lock lock(*this);

    if (_eState == eState  && _bNoticed == true)
    {
        return;
    }
    if ( _eState != eState )
    {
        LOG->debug() << _sApplication << "." << _sServerName << " State changed! old State:" << toStringState( _eState ) << " new State:" << toStringState( eState ) << endl;
        _eState = eState;
    }
    if(bSynState == true)
    {
        synState();
    }
}

void ServerObject::setLastState(InternalServerState eState)
{
    Lock lock(*this);

    _eLastState = eState;
}

bool ServerObject::isScriptFile(const string &sFileName)
{
    if(TC_Common::lower(TC_File::extractFileExt(sFileName)) == "sh" ) //.sh文件为脚本
    {
        return true;
    }
    if(TC_File::extractFileName(_sStartScript) == sFileName )
    {
        return true;
    }
    if(TC_File::extractFileName(_sStopScript) == sFileName )
    {
        return true;
    }
    if(TC_File::extractFileName(_sMonitorScript) == sFileName )
    {
        return true;
    }
    return false;
}

//同步更新 重启服务 需要准确通知主控状态时调用
void ServerObject::synState()
{
    Lock lock(*this);
    try
    {
        ServerStateInfo tServerStateInfo;
        tServerStateInfo.serverState    = (IsEnSynState()?toServerState(_eState):taf::Inactive);
        tServerStateInfo.processId      = _pid;
        //根据uNoticeFailTimes判断同步还是异步更新服务状态
        //防止主控超时导致阻塞服务上报心跳
        if(_uNoticeFailTimes < 3)
        {
            AdminProxy::getInstance()->getRegistryProxy()->updateServer( _tNodeInfo.nodeName,  _sApplication, _sServerName, tServerStateInfo);
        }
        else
        {
            AdminProxy::getInstance()->getRegistryProxy()->async_updateServer(NULL, _tNodeInfo.nodeName,  _sApplication, _sServerName, tServerStateInfo);
        }
        _bNoticed = true;
        _uNoticeFailTimes = 0;
    }
    catch (exception &e)
    {
        _bNoticed = false;
        _uNoticeFailTimes ++;
        LOG->error() << "ServerObject::synState "<<_sApplication<<"."<<_sServerName<<" error times:"<<_uNoticeFailTimes<<" reason:"<< e.what() << endl;
    }
}

//异步同步状态
void ServerObject::asyncSynState()
{
    Lock lock(*this);
    try
    {
        ServerStateInfo tServerStateInfo;
        tServerStateInfo.serverState    = (IsEnSynState()?toServerState(_eState):taf::Inactive);
        tServerStateInfo.processId      = _pid;
        AdminProxy::getInstance()->getRegistryProxy()->async_updateServer( NULL, _tNodeInfo.nodeName,  _sApplication, _sServerName, tServerStateInfo);
    }
    catch (exception &e)
    {
        LOG->error() << "ServerObject::async_synState "<<_sApplication<<"."<<_sServerName<<" error:" << e.what() << endl;
    }
}


ServerState ServerObject::toServerState(InternalServerState eState) const
{
    switch (eState)
    {
        case Inactive:
        {
            return taf::Inactive;
        }
        case Activating:
        {
            return taf::Activating;
        }
        case Active:
        {
            return taf::Active;
        }
        case Deactivating:
        {
            return taf::Deactivating;
        }
        case Destroying:
        {
            return taf::Destroying;
        }
        case Destroyed:
        {
            return taf::Destroyed;
        }
        default:
        {
            return taf::Inactive;
        }
    }
}

string ServerObject::toStringState(InternalServerState eState) const
{
    switch (eState)
    {
        case Inactive:
        {
            return "Inactive";
        }
        case Activating:
        {
            return "Activating";
        }
        case Active:
        {
            return "Active";
        }
        case Deactivating:
        {
            return "Deactivating";
        }
        case Destroying:
        {
            return "Destroying";
        }
        case Destroyed:
        {
            return "Destroyed";
        }
        case Loading:
        {
            return "Loading";
        }
        case Patching:
        {
            return "Patching";
        }
        case BatchPatching:
        {
            return "BatchPatching";
        }
        case AddFilesing:
        {
            return "AddFilesing";
        }
        default:
        {
            ostringstream os;
            os << "state " << eState;
            return os.str();
        }
    };
}

int ServerObject::checkPid()
{
    Lock lock(*this);
    if(_pid != 0)
    {
        int iRet = _pActivatorPtr->sendSignal(_pid, 0);
        if (iRet == 0)
        {
            return 0;
        }
    }
    return -1;
}

void ServerObject::keepAlive(pid_t pid,const string &adapter)
{
    Lock lock(*this);
    if (pid <= 0)
    {
        LOG->error()<< _sApplication << "." << _sServerName << " pid "<<pid<<" error, pid <= 0"<<endl;
        return;
    }
    else
    {
        LOG->debug() << "keepAlive|"<<_sServerType<< "|pid|" << pid
            <<"|server|"<<_sApplication << "." << _sServerName <<"|"<<adapter<< endl;
    }
    time_t now  = TC_TimeProvider::getInstance()->getNow();
    setLastKeepAliveTime(now,adapter);
    //setLastKeepAliveTime(now);
    setPid(pid);

    //心跳不改变正在转换期状态(Activating除外)
    if(toStringState(_eState).find("ing") == string::npos || _eState == ServerObject::Activating)
    {
         LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|" << _tDesc.application
            << "|" << _tDesc.serverName << "|" << toStringState(_eState) << "|" << _eState << endl;
        setState(ServerObject::Active);
    }
    else
    {
         LOG->debug() << _sApplication << "." << _sServerName << " State no need changed to active!  State:" << toStringState( _eState ) << endl;
    }

    if(!_bLoaded)
    {
        _bLoaded = true;
    }
    if(!_bPatched)
    {
        _bPatched = true;
    }
}

void ServerObject::setLastKeepAliveTime(int t,const string& adapter)
{
    Lock lock(*this);
    _tKeepAliveTime = t;
    map<string, time_t>::iterator it1 = _mAdapterKeepAliveTime.begin();
    if(adapter.empty())
    {
        while (it1 != _mAdapterKeepAliveTime.end() )
        {
            it1->second = t;
            it1++;
        }
        return;
    }
    map<string, time_t>::iterator it2 = _mAdapterKeepAliveTime.find(adapter);
    if( it2 != _mAdapterKeepAliveTime.end())
    {
        it2->second = t;
    }
    else
    {
        LOG->error()<<adapter<<" not registed "<< endl;
    }
}

int ServerObject::getLastKeepAliveTime(const string &adapter)
{
    if(adapter.empty())
    {
        return _tKeepAliveTime;
    }
    map<string, time_t>::const_iterator it = _mAdapterKeepAliveTime.find(adapter);
    if( it != _mAdapterKeepAliveTime.end())
    {
        return it->second;
    }
    return -1;
}


bool ServerObject::isTimeOut(int iTimeout)
{
    Lock lock(*this);
    time_t now = TC_TimeProvider::getInstance()->getNow();
    if(now - _tKeepAliveTime > iTimeout)
    {
        LOG->error()<<"server time  out "<<now - _tKeepAliveTime<<"|>|"<<iTimeout<< endl;
        return true;
    }
    map<string, time_t>::const_iterator it = _mAdapterKeepAliveTime.begin();
    while (it != _mAdapterKeepAliveTime.end() )
    {
        if(now - it->second > iTimeout)
        {
            LOG->error()<<"server "<< it->first<<" time  out "<<now - it->second<<"|>|"<<iTimeout
            <<"|"<<TC_Common::tostr(_mAdapterKeepAliveTime)<< endl;
            return true;
        }
        it++;
    }
    return false;
}

void ServerObject::setVersion( const string & version )
{
    Lock lock(*this);
    try
    {
        _sVersion = version;
        AdminProxy::getInstance()->getRegistryProxy()->async_reportVersion(NULL,_sApplication, _sServerName, _tNodeInfo.nodeName, version);

    }catch(...)
    {
        _bNoticed = false;
    }
}

void ServerObject::setPid(pid_t pid)
{
    Lock lock(*this);
    if (pid == _pid)
    {
        return;
    }
    LOG->debug()<< _sApplication << "." << _sServerName << " pid changed! old pid:" << _pid << " new pid:" << pid << endl;
    _pid = pid;
}

void ServerObject::setPatchPercent(const int iPercent)
{
    //下载结束仍需要等待本地copy  进度最多设置99% 本地copy结束后设置为100%
    _tPatchInfo.iPercent      = iPercent>99?99:iPercent;
    _tPatchInfo.iModifyTime   = TC_TimeProvider::getInstance()->getNow();
}

void ServerObject::setPatchResult(const string &sPatchResult,const bool bSucc)
{
    Lock lock(*this);
    _tPatchInfo.sResult = sPatchResult;
    _tPatchInfo.bSucc = bSucc;
}

void ServerObject::setPatchVersion(const string &sVersion)
{
    Lock lock(*this);
    _tPatchInfo.sVersion = sVersion;
}

string ServerObject::getPatchVersion()
{
    Lock lock(*this);
    return _tPatchInfo.sVersion;
}

int ServerObject::getPatchPercent(PatchInfo &tPatchInfo)
{
    LOG->debug() << "Into " << __FUNCTION__ << ":" << __LINE__ << "|"<< _sApplication << "_" << _sServerName << "|"<< _sServerId<< endl;
    Lock lock(*this);
    LOG->debug() << __FUNCTION__ << ":" << __LINE__  << "|"<< _sApplication << "_" << _sServerName << "|"<< _sServerId << "get lock" << endl;

    tPatchInfo              = _tPatchInfo;
    //是否正在发布
    tPatchInfo.bPatching    = (_eState == ServerObject::Patching)?true:false;

    if (tPatchInfo.bSucc == true || _eState == ServerObject::Patching || _eState == ServerObject::BatchPatching)
    {
        LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|" << _tDesc.application
            << "|" << _tDesc.serverName << "|succ:" << (tPatchInfo.bSucc?"true":"fasle") << "|" << _eState << endl;
        return 0;
    }

    LOG->error() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|" << _tDesc.application
           << "|" << _tDesc.serverName << "|succ:" << (tPatchInfo.bSucc?"true":"fasle") << "|" << _eState << endl;
    return -1;
}

string ServerObject::decodeMacro(const string& value) const
{
    string tmp = value;
    map<string,string>::const_iterator it = _mMacro.begin();
    while(it != _mMacro.end())
    {
        tmp = TC_Common::replace(tmp, "${" + it->first + "}", it->second);
        ++it;
    }
    return tmp;
}

void ServerObject::setMacro(const map<string,string>& mMacro)
{
    Lock lock(*this);
    map<string,string>::const_iterator it1 = mMacro.begin();
    for(;it1!= mMacro.end();++it1)
    {
        map<string,string>::iterator it2 = _mMacro.find(it1->first);

        if(it2 != _mMacro.end())
        {
            it2->second = it1->second;
        }
        else
        {
            _mMacro[it1->first] = it1->second;
        }
    }
}

void ServerObject::setScript(const string &sStartScript,const string &sStopScript,const string &sMonitorScript )
{

    _sStartScript    = TC_File::simplifyDirectory(TC_Common::trim(sStartScript));
    _sStopScript     = TC_File::simplifyDirectory(TC_Common::trim(sStopScript));
    _sMonitorScript  = TC_File::simplifyDirectory(TC_Common::trim(sMonitorScript));
}

bool ServerObject::getRemoteScript(const string &sFileName)
{
    string sFile = TC_File::simplifyDirectory(TC_Common::trim(sFileName));
    if(!sFile.empty())
    {
       CommandAddFile commands(this,sFile);
       commands.doProcess(); //拉取脚本
       return true;
    }
    return false;
}

void ServerObject::doMonScript()
{
    try
    {
        string sResult;
        time_t tNow = TC_TimeProvider::getInstance()->getNow();
        if(TC_File::isAbsolute(_sMonitorScript) == true) //监控脚本
        {
            if(_eState == ServerObject::Activating||tNow - _tKeepAliveTime > ServantHandle::HEART_BEAT_INTERVAL)
            {
                 map<string,string> mResult;
                 _pActivatorPtr->doScript(_sServerId,_sMonitorScript,sResult,mResult);
                 if(mResult.find("pid") != mResult.end() && TC_Common::isdigit(mResult["pid"]) == true)
                 {
                     LOG->debug() << __FUNCTION__ << ":" << __LINE__ << ":" << "|" << _sServerId << "|"<< mResult["pid"] << endl;
                     keepAlive(TC_Common::strto<int>(mResult["pid"]));
                 }
            }
        }
    }
    catch (exception &e)
    {
        LOG->error() << "ServerObject::doMonScript error:" << e.what() << endl;
    }
}

void ServerObject::checkServer(int iTimeout)
//checkServer时对服务所占用的内存上报到主控
{
    try
    {
        string sResult;

        //pid不存在属于服务异常
        LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|" << _sServerId<<"|" << ServerObject::toStringState(_eState) << "|" << _pid << endl;
        if( (_eState != ServerObject::Inactive && _eState != ServerObject::Deactivating && _eState != ServerObject::Activating)&&
				checkPid() != 0)
        {
            sResult = "[alarm] down,pid not exist";
            LOG->debug()<<_sServerId<<" "<<sResult << "|pid|" << _pid  << endl;
            CommandStop command(this);
            command.doProcess();
        }

        if(  _eState == ServerObject::Activating  && _pid != 0 &&  checkPid() != 0)
        {
            sResult = "[alarm] down,activating server pid not exist";
            LOG->debug()<<_sServerId<<" "<<sResult << "|pid|" << _pid  << endl;
            CommandStop command(this);
            command.doProcess();
        }


        //正在运行程序心跳上报超时。
        int iRealTimeout = _iTimeout > 0?_iTimeout:iTimeout;
        if( _eState != ServerObject::Inactive && isTimeOut(iRealTimeout))
        {
            sResult = "[alarm] zombie process,no keep alive msg for " + TC_Common::tostr(iRealTimeout) + " seconds";
            LOG->debug()<<_sServerId<<" "<<sResult<< endl;
            CommandStop command(this);
            command.doProcess();
        }

        //启动服务
        if( _eState == ServerObject::Inactive && isAutoStart() == true)
        {
            CommandStart command(this);
            command.doProcess();
            sResult = sResult == ""?"[alarm] down, server is inactive":sResult;
            LOG->debug()<<_sServerId<<" "<<sResult<< endl;
            FDLOG()<<_sServerId<<" "<<sResult<< endl;

			//配置了coredump检测才进行操作
			if(_tLimitStateInfo.bEnableCoreLimit)
			{
				_pServiceLimitResource->addExcStopRecord();
			}

            g_app.reportServer(_sServerId,sResult);
        }

        //add by edwardsu
        char sTmp[64] ={0};
        sprintf(sTmp, "%u", _pid);
        string spid = string(sTmp);

        string filename = "/proc/" + spid + "/statm";
        string stream;
        stream = TC_File::load2str(filename);
        if(!stream.empty())
        {
           LOG->debug()<<"filename:"<<filename<<",stream:"<<stream<<endl;
           //stream = stream.substr(stream.find_first_of(" ")+1,stream.length());
          // stream = stream.substr(0,stream.find_first_of(" "));//上报虚拟内存
          //>>改成上报物理内存
          vector<string> vtStatm = TC_Common::sepstr<string>(stream, " ");
	  if (vtStatm.size() < 2)
	  {
	  	LOG->error() << __FUNCTION__ << ":" << __LINE__ << "|cannot get server[" + _sServerId  + "] physical mem size|stream|" << stream  << endl;
	  }
	  else
	  {
	  	stream = vtStatm[1];
	  }
          //<<modified by spinnerxu@20110725
          // LOG->debug()<<"filename:"<<filename<<",mem size:"<<stream<<endl;
           if(TC_Common::isdigit(stream))
           {
                REPORT_MAX(_sServerId, _sServerId+".memsize", TC_Common::strto<int>(stream) * 4);
                //LOG->debug()<<"report_max("<<_sServerId<<".memsize,"<<TC_Common::strto<int>(stream)*4<<")OK."<<endl;
                return;
           }
           else
           {
                LOG->debug()<<"stream : "<<stream<<" ,is not a digit."<<endl;
           }
       }
       else
       {
            LOG->debug()<<"stream is empty: "<<stream<<endl;
       }
       //end add
    }
    catch(exception &ex)
    {
        LOG->error() << "ServerObject::checkServer error:" << ex.what() << endl;
    }
    catch(...)
    {
        LOG->error() << "ServerObject::checkServer unknown error" << endl;
    }
}

void ServerObject::checkCoredumpLimit()
{
	//server is inactive or core limit not enable
	if(_eState != ServerObject::Active || !_tLimitStateInfo.bEnableCoreLimit)
	{
		LOGINFO("checkCoredumpLimit:server is inactive or disable corelimit"<<endl);
		return;
	}

	Lock lock(*this);
	if(_tLimitStateInfo.eCoreType == EM_AUTO_LIMIT)
	{
		bool bNeedClose=false;
	    int iRet =_pServiceLimitResource->IsCoreLimitNeedClose(bNeedClose);

		bool bNeedUpdate = (iRet==2)?true:false;

		//server restart or node config reload or server coredump close state expired and needclose
		//restart and need close core(because core is open in default)
		if(_bLimitStateUpdated && bNeedClose)
		{
			_bLimitStateUpdated = setServerCoreLimit(true)?false:true;//设置成功后再屏蔽更新
			_tLimitStateInfo.bCloseCore = bNeedClose;
		}
		//core limit expired time,need open core limit
		else if(bNeedUpdate && !bNeedClose)
		{

			setServerCoreLimit(false);

			_tLimitStateInfo.bCloseCore = bNeedClose;
		}
	}
	//manual config,just set once when config update or server restart
	else
	{
		if(_bLimitStateUpdated)
		{
			_bLimitStateUpdated = setServerCoreLimit(_tLimitStateInfo.bCloseCore)?false:true; //设置成功后再屏蔽更新
		}
	}
}

bool ServerObject::setServerCoreLimit(bool bCloseCore)
{
	string sResult;
	const string sCmd = (bCloseCore)?("taf.closecore yes"):("taf.closecore no");
	CommandNotify command(this,sCmd);
	int iRet = command.doProcess(sResult);
	LOG->debug()<<"checkCoredumpLimit|"<<_sServerId<<"|"<<sCmd<<"|"<<sResult<<endl;
	return (iRet==0);

}

//服务重启时设置
void ServerObject::setLimitInfoUpdate(bool bUpdate)
{
	Lock lock(*this);
    _bLimitStateUpdated = true;
}

//手动配置有更新时调用
void ServerObject::setServerLimitInfo(const ServerLimitInfo& tInfo)
{
	Lock lock(*this);
	_tLimitStateInfo = tInfo;
	_pServiceLimitResource->setLimitCheckInfo(tInfo.iMaxExcStopCount,tInfo.iCoreLimitTimeInterval,tInfo.iCoreLimitExpiredTime);
	_bLimitStateUpdated = true;
}
