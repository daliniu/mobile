#include "util/tc_clientsocket.h"
#include "BatchPatchThread.h"
#include "NodeImp.h"
#include "RegistryProxy.h"
#include "NodeServer.h"

extern BatchPatch * g_BatchPatchThread;

void NodeImp::initialize()
{
    try
    {
        cout << "initialize NodeImp" << endl;
        _tNodeInfo      =_tPlatformInfo.getNodeInfo();
        _sDownLoadPath  =_tPlatformInfo.getDownLoadDir();
    }
    catch ( exception& e )
    {
        LOG->error() << e.what() << endl;
        exit( 0 );
    }
}

int NodeImp::destroyServer( const string& application, const string& serverName,string &result, JceCurrentPtr current )
{
    result = string(__FUNCTION__) + " ["+application + "." + serverName+"] ";
    LOG->debug() << result << endl;
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if ( pServerObjectPtr )
    {
        string s;
        CommandDestroy command(pServerObjectPtr);
        int iRet = command.doProcess(current,s, false);
        if ( iRet == 0 && ServerFactory::getInstance()->eraseServer(application, serverName) == 0)
        {
            pServerObjectPtr = NULL;
            result = result+"succ:" + s;
        }
        else
        {
            result = "error:"+s;
        }
        return iRet;
    }
    result += "server is  not exist";
    return -1;
}

int NodeImp::patch(const string& application, const string& serverName, const bool shutdown,const string & patchServer, string &result, JceCurrentPtr current)
{
    static TC_ThreadLock g_mutex;               //保护不重复发布的锁
    static std::set<std::string> g_setPatch;    //标记特定的服务是否在发布中

    int  iRet           = -1;
    string sVersion     = result;
    try
    {
        {
            TC_ThreadLock::Lock lock(g_mutex);
            if (g_setPatch.count(application + "_" + serverName) == 1)
            {
                result += "server(" + application + "_" + serverName + ") is patching, please wait to end";
                return -1;
            }

            g_setPatch.insert(application + "_" + serverName);
        }

        string s;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->loadServer(application, serverName, true, s);
        if (!pServerObjectPtr)
        {
            iRet    = -1;
            result  = result + "error,version " + sVersion + " " + s;
            LOG->error() << "NodeImp::patch|find server object " << application << "." << serverName << "|" << result << endl;
            goto EXIT;
        }

        if (pServerObjectPtr->getServerType() == "taf_java") //如果是java服务的话需要清空目录，备份目录
        {
            LOG->debug() << "NodeImp::patch|" << application << "." << serverName << "|taf_java|begin" << endl;
            int maxbaknum   = 5;
            string srcPath  = "/usr/local/app/taf/tafnode/data/" + application + "." + serverName + "/";
            string destPath = "/usr/local/app/taf/tafnode/tmp/" + application + "." + serverName + "/";

            if (!TC_File::isFileExistEx(srcPath, S_IFDIR)) //不存在这个目录，先创建
            {
                LOG->debug() << srcPath << " is not exist,create it... "<<endl;
                TC_File::makeDir(srcPath);
            }

            if (TC_File::makeDir(destPath)) //循环的更新bin_bak目录
            {
                for(unsigned int i = maxbaknum - 1; i >= 1; i--) 
                {
                    string destPathBak = destPath + "bin_bak" + TC_Common::tostr<int>(i) + "/";
                    if(!TC_File::isFileExistEx(destPathBak,S_IFDIR)) //不存在这个目录，可以继续循环
                    {
                        LOG->debug() << destPathBak << " is not exist,continue... "<<endl;
                        continue;
                    }
                    else //存在这个目录，进行替换:bak1-->bak2
                    {
                        string newFile = destPath + "bin_bak" + TC_Common::tostr<int>(i+1) + "/";
                        if(TC_File::makeDir(newFile)) 
                        {
                            TC_File::copyFile(destPathBak,newFile);
                            LOG->debug() << "copyFile:"<< destPathBak << " to "<< newFile << " finished!"<<endl;
                        }
                        else
                        {
                            LOG->debug() << "makeDir:"<< newFile << "error..." <<endl;
                        }
                    }
                }
            }
            
            //更新当前目录到/bin_bak1/目录
            string existFile    = srcPath + "bin/";
            string destPathBak  = destPath + "bin_bak1/";
            if(TC_File::makeDir(destPathBak))
            {
                if(!TC_File::isFileExistEx(existFile,S_IFDIR)) //不存在这个目录，先创建
                {
                   LOG->debug() << existFile << " is not exist,create it... "<<endl;
                   TC_File::makeDir(existFile);
                }
                TC_File::copyFile(existFile,destPathBak);
                LOG->debug()  << "copyFile:"<< existFile << " to "<< destPathBak << " finished!"<<endl;
                if(TC_File::removeFile(existFile,true) == 0) 
                {
                    LOG->debug() << "removeFile:"<< existFile << " finished!"<<endl;
                    if(TC_File::makeDir(existFile))
                    {
                        LOG->debug() << "makeDir:"<< existFile << " finished!"<<endl;
                    }
                }
            }

		    //保留指定文件在bin目录中的	
		    string sSavedFileName = "classes/autoconf";
	    	string sBackupSrc = destPathBak + sSavedFileName;
	    	string sBackupDest = existFile + sSavedFileName;
	    	if (TC_File::isFileExistEx(sBackupSrc,S_IFDIR))
	    	{
		  		if (!TC_File::isFileExistEx(sBackupDest,S_IFDIR))
		 	 	{
		       		if(!TC_File::makeDirRecursive(TC_File::simplifyDirectory(sBackupDest)))
					{
			    		LOG->error() << "failed to mkdir dest directory:" << sBackupDest << endl;
					}
		  		}
		  		TC_File::copyFile(sBackupSrc,sBackupDest,true);
	    	}
        }
        
        result = string(__FUNCTION__) + " [" + application + "." + serverName + "] ";
        LOG->debug() << result << endl;
        if (g_app.isValid(current->getIp()) == false)
        {
            result += " erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            goto EXIT;
        }

        s = sVersion;
        CommandPatch command(pServerObjectPtr, patchServer, _sDownLoadPath);
        if (command.canExecute(s) == ServerCommand::EXECUTABLE)
        {
            Node::async_response_patch(current, 0, s);
            current->setResponse(false);
            iRet = command.execute(s);
        }

        if (iRet == 0)
        {
            result = result + "ing,version " + sVersion;
        }
        else
        {
            result = result + "error,version " + sVersion + " " + s;
        }
    }
    catch (exception & e)
    {
        LOG->error() <<  __FUNCTION__ <<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }

EXIT:
    try
    {
        TC_ThreadLock::Lock lock(g_mutex);
        g_setPatch.erase(application + "_" + serverName);
    }
    catch (std::exception & ex)
    {
        LOG->error() << __FILE__ << "|" << __FUNCTION__ << "|g_setPatch erase(" << application << "_" << serverName << ") error:" << ex.what() << endl;
    }
    catch (...)
    {
        LOG->error() << __FILE__ << "|" << __FUNCTION__ << "|g_setPatch erase(" << application << "_" << serverName << ") error" << endl;
    }

    return iRet;
}

int NodeImp::patchPro(const taf::PatchRequest & req, string & result, JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__    << "|" 
                 << req.appname + "." + req.servername + "_" + req.nodename << "|"
                 << req.groupname   << "|"
                 << req.version     << "|"
                 << req.user        << "|"
                 << req.servertype  << "|"
                 << req.patchobj    << "|"
                 << req.md5         << endl; 

    try
    {
    	 //获取预发布服务
    	string sError("");
        ServerObjectPtr server = ServerFactory::getInstance()->loadServer(req.appname, req.servername, true, sError);
	if (!server)
	{
		result = "load " + req.appname + "." + req.servername + "|" + sError;
		LOG->error() << __FUNCTION__  << ":" << __LINE__ << "|" <<req.appname + "." + req.servername + "_" + req.nodename << "|" << result << endl;
		return -1;
	}

	{
		TC_ThreadRecLock::Lock lock(*server);
		ServerObject::InternalServerState  eState = server->getInternalState();
		if (server->toStringState(eState).find("ing") != string::npos)
	       {
	            result = "cannot patch the server ,the server state is " + server->toStringState(eState);
	            LOG->error() << __FUNCTION__  << ":" << __LINE__ << "|" << req.appname + "." + req.servername << "|" << result << endl;
	            return -1;
	        }


		//保存patching前的状态，patch完成后恢复
		LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|" <<req.appname + "." + req.servername + "_" + req.nodename << "|saved state:" << server->toStringState(eState) << endl;
		server->setLastState(eState);
		server->setState(ServerObject::BatchPatching);
		//将百分比初始化为0，防止上次patch结果影响本次patch进度
		server->setPatchPercent(0);

		LOG->debug() << req.appname + "." + req.servername + "_" + req.nodename << "|" << req.groupname   << "|preset success" << endl;
	}
	
        g_BatchPatchThread->push_back(req,server);
    }
    catch (std::exception & ex)
    {
        LOG->error() << __FUNCTION__ << "|" << req.appname + "." + req.servername + "_" + req.nodename << "|Exception:" << ex.what() << endl;
        result = ex.what();
        return -1;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << req.appname + "." + req.servername + "_" + req.nodename << "|Unknown Exception" << endl;
        result = "Unknown Exception";
        return -1;
    }

       LOG->debug() << __FUNCTION__    << "|" 
                 << req.appname + "." + req.servername + "_" + req.nodename << "|"
                 << req.groupname   << "|"
                 << req.version     << "|"
                 << req.user        << "|"
                 << req.servertype  << "|return success"<< endl; 

    return 0;
}

int NodeImp::patchSubborn(const string & patchServer, const string & application, const string & serverName, const string & srcFile, const string & dstFile, string & result, JceCurrentPtr current)
{
    string sError;
    int iRet = -1;

    try
    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );

        string s;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->loadServer(application, serverName, true, s);

        CommandDownload command(pServerObjectPtr, patchServer, srcFile, dstFile);
        if (command.canExecute(sError) == ServerCommand::EXECUTABLE)
        {
            if (pServerObjectPtr && !application.empty() && !serverName.empty())
            {
                Node::async_response_patchSubborn(current, 0, s);
                current->setResponse(false);
            }

            iRet = command.execute(sError);

            if (iRet == 0)
            {
                result = result + "succ";
            }
            else
            {
                result = result + "error, " + sError;
            }
        }
        else
        {
            result = result + "error," + sError;
        }
    }
    catch ( exception& e )
    {
        LOG->error() <<  __FUNCTION__ <<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }

    return iRet;
}

int NodeImp::addFile(const string &application,const string &serverName,const string &file, string &result, JceCurrentPtr current)
{
    result = string(__FUNCTION__)+" server ["+application + "." + serverName+"] file:"+file;
    LOG->debug() << result  << endl;
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if ( pServerObjectPtr )
    {
        string s;
        CommandAddFile command(pServerObjectPtr,file);
        int iRet = command.doProcess(current,s);
        if ( iRet == 0)
        {
            result = result+"succ:" + s;
        }
        else
        {
            result = "error:"+s;
        }
        return iRet;
    }
    result += "server is  not exist";
    return -1;
}

string NodeImp::getName( JceCurrentPtr current )
{
    LOG->debug() <<"into "<< string(__FUNCTION__) << endl;
    _tNodeInfo = _tPlatformInfo.getNodeInfo();
    return _tNodeInfo.nodeName;
}

LoadInfo NodeImp::getLoad( JceCurrentPtr current )
{
    LOG->debug() << string(__FUNCTION__) << endl;
    return  _tPlatformInfo.getLoadInfo();
}

int NodeImp::shutdown( string &result,JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__);
        LOG->debug() << "into "<< result << endl;
        if ( g_app.isValid(current->getIp()) == false )
        {
            result += "erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            return -1;
        }
        Application::terminate();
        return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__ <<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::stopAllServers( string &result,JceCurrentPtr current )
{
    try
    {
        LOG->debug() <<"into "<<__FUNCTION__<< endl;        
        if ( g_app.isValid(current->getIp()) == false )
        {
            result = " erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            return -1;
        }
        map<string, ServerGroup> mmServerList;
        mmServerList.clear();
        mmServerList = ServerFactory::getInstance()->getAllServers();
        for ( map<string, ServerGroup>::const_iterator it = mmServerList.begin(); it != mmServerList.end(); it++ )
        {
            for ( map<string, ServerObjectPtr>::const_iterator p = it->second.begin(); p != it->second.end(); p++ )
            {
                ServerObjectPtr pServerObjectPtr = p->second;
                if ( pServerObjectPtr )
                {
                    result =result+"stop server ["+it->first+"."+ p->first+"] ";
                    LOG->debug() << result << endl;
                    bool bByNode = true;
                    string s;                 
                    CommandStop command(pServerObjectPtr,bByNode);           
                    int iRet = command.doProcess(s);
                    if (iRet == 0)
                    {
                        result = result+"succ:"+s+"\n";
                    }
                    else
                    {
                        result = result+"error:"+s+"\n";
                    }
                }
            }
        } 
        return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__<<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__<<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    cout<<"stop succ"<<endl;
    return -1;
}

int NodeImp::loadServer( const string& application, const string& serverName,string &result, JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
        LOG->debug() << result<<endl;
        if ( g_app.isValid(current->getIp()) == false )
        {
            result += " erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            return -1;
        }
        string s;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->loadServer(application,serverName,false,s);
        if (!pServerObjectPtr)
        {
            result =result+"error::cannot load server description.\n"+s;
            return -1;
        }
        result = result+"succ"+s;
        return 0;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__<<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__<<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::startServer( const string& application, const string& serverName,string &result, JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__)+ " ["+application + "." + serverName+"] from "+ current->getIp()+" ";
        LOG->debug() << result << endl;
        if ( g_app.isValid(current->getIp()) == false )
        {
            result += " erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            return -1;
        }
        string s;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->loadServer( application, serverName,true,s);
        if ( pServerObjectPtr )
        {
            bool bByNode = true;
            CommandStart command(pServerObjectPtr,bByNode);
            int iRet = command.doProcess(s);
            if (iRet == 0 )
            {
                result = result+":server is activating, please wait ...\n"+s;
            }
            else
            {
                result = result+"error:"+s;
            }
            LOG->debug() <<result << endl;
            return iRet;
        }
        //设置服务状态enable
        pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
        if (pServerObjectPtr)
        {
            pServerObjectPtr->setEnabled(true);
        }

        result =result+"error::cannot load server description from regisrty.\n"+s;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__ << " catch exception:" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::stopServer( const string& application, const string& serverName,string &result, JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__)+" ["+application + "." + serverName+"] from "+ current->getIp()+" ";
        LOG->debug() <<result << endl;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
        if ( pServerObjectPtr )
        {
            string s;           
            bool bByNode = true;          
            CommandStop command(pServerObjectPtr,bByNode);           
            int iRet = command.doProcess(current,s);
            if (iRet == 0 )
            {
                result = result+"succ:"+s;
            }
            else
            {
                result = result+"error:"+s;
            }
            LOG->debug() <<result << endl;
            return iRet;
        }
        result += "server is  not exist";
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__<<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__<<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::notifyServer( const string& application, const string& serverName, const string &sMsg, string &result, JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__)+" ["+application + "." + serverName+"] '" + sMsg + "' ";
        LOG->debug() << result << endl;
        if ( g_app.isValid(current->getIp()) == false )
        {
            result += " erro:ip "+ current->getIp()+" is invalid";
            LOG->error() << result << endl;
            return -1;
        }
        if ( application == "taf" && serverName == "tafnode" )
        {
            AdminFPrx pAdminPrx;    //服务管理代理
            pAdminPrx = Application::getCommunicator()->stringToProxy<AdminFPrx>("AdminObj@"+ServerConfig::Local);
            result = pAdminPrx->notify(sMsg);
            return 0;
        }
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
        if ( pServerObjectPtr )
        {
            string s;
            CommandNotify command(pServerObjectPtr,sMsg);
            int iRet = command.doProcess(s);
            if (iRet == 0 )
            {
                result = result + "succ:" + s;
            }
            else
            {
                result = result + "error:" + s;
            }
            return iRet;
        }
        result += "server is not exist";
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__<<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__<<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::getServerPid( const string& application, const string& serverName,string &result,JceCurrentPtr current )
{
    result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
    LOG->debug() <<result  << endl;
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if ( pServerObjectPtr )
    {
        result += "succ";
        pServerObjectPtr->getServerDescriptor().display(LOG->info());
        return pServerObjectPtr->getPid();
    }
    result += "server not exist";
    return -1;
}

ServerState NodeImp::getSettingState( const string& application, const string& serverName,string &result, JceCurrentPtr current )
{
    result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if ( pServerObjectPtr )
    {
        result += "succ";
        return pServerObjectPtr->isEnabled()==true?taf::Active:taf::Inactive;
    }
    result += "server not exist";
    LOG->error() << __FUNCTION__ <<" "<<result<< endl;
    return taf::Inactive;
}

ServerState NodeImp::getState( const string& application, const string& serverName, string &result, JceCurrentPtr current )
{
    result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
    ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
    if ( pServerObjectPtr )
    {
        result += "succ";
        return pServerObjectPtr->getState();
    }
    result += "server not exist";
    LOG->error() << string(__FUNCTION__)<<" "<<result<< endl;
    return taf::Inactive;
}

int NodeImp::synState( const string& application, const string& serverName, string &result, JceCurrentPtr current )
{
    try
    {
        result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
        if ( pServerObjectPtr )
        {
            result += "succ";
            pServerObjectPtr->synState();
            return  0;
        }
        result += "server not exist";
        LOG->error() << __FUNCTION__ <<" "<<result<< endl;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__ <<" catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;
}

int NodeImp::getPatchPercent( const string& application, const string& serverName,PatchInfo &tPatchInfo, JceCurrentPtr current)
{
    string &result =  tPatchInfo.sResult;
    try
    {
        result = string(__FUNCTION__)+" ["+application + "." + serverName+"] ";
        LOG->debug() <<result  << endl;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( application, serverName );
		LOG ->debug() << result << "|get ServerObj" << endl;
        if ( pServerObjectPtr )
        {
            result += "succ";
            return pServerObjectPtr->getPatchPercent(tPatchInfo);
        }
        result += "server not exist";
        LOG->error() << __FUNCTION__ <<" "<< result<< endl;
    }
    catch ( exception& e )
    {
        LOG->error() << __FUNCTION__ << " catch exception :" << e.what() << endl;
        result += e.what();
    }
    catch ( ... )
    {
        LOG->error() << __FUNCTION__ <<" catch unkown exception" << endl;
        result += "catch unkown exception";
    }
    return -1;

}

taf::Int32 NodeImp::delCache(const  string &sFullCacheName, const std::string &sBackupPath, const std::string & sKey, std :: string &result, JceCurrentPtr current)
{
	try
	{
		LOG->debug() << "["<< __FUNCTION__  << "]" <<sFullCacheName << "|" << sBackupPath << "|" << sKey << endl;
		if (sFullCacheName.empty())
		{
			result = "cache server name is empty";
			throw runtime_error(result);
		}

		string sBasePath = "/usr/local/app/taf/tafnode/data/"+sFullCacheName+"/bin/";
		if (!TC_File::isFileExistEx(sBasePath, S_IFDIR))
		{
			result = "no such directory:" + sBasePath;
			//服务不存在没有发布过，返回成功
			//throw runtime_error(result);
			return 0;
		}

		key_t key;
		if (sKey.empty())
		{
			key = ftok(sBasePath.c_str(), 'D');
		}
		else
		{
			key = TC_Common::strto<key_t>(sKey);
		}
		LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|key=" << key << endl;	

		int shmid = shmget(key, 0, 0666);
		if (shmid == -1)
		{
			result = "failed to shmget " + sBasePath + "|key=" + TC_Common::tostr(key);
			//throw runtime_error(result);
			//如果获取失败则认为共享内存已经删除,直接返回成功
			LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|"  << sFullCacheName << "|" << result << endl;
			return 0;
		}
		
		shmid_ds *pShmInfo= new shmid_ds;
		int iRet = shmctl(shmid, IPC_STAT, pShmInfo);
		if (iRet != 0)
		{
			result = "failed to shmctl " + sBasePath + "|key=" + TC_Common::tostr(key) + "|ret=" + TC_Common::tostr(iRet);
			delete pShmInfo;
			pShmInfo = NULL;
			throw runtime_error(result);
		}

		if (pShmInfo->shm_nattch>=1)
		{
			result = "current attach count >= 1,please check it |" + sBasePath + "|key=" +TC_Common::tostr(key)+"|attch_count=" + TC_Common::tostr(pShmInfo->shm_nattch);
			delete pShmInfo;
			pShmInfo = NULL;
			throw runtime_error(result);
		};

		/*bool bBackup = true;
		if (pShmInfo->shm_segsz == 0){bBackup=false;}
		size_t iSize= pShmInfo->shm_segsz;*/
		delete pShmInfo;
		pShmInfo = NULL;

		//备份共享内存
		/*if (bBackup)
		{
			void *pData = NULL;
			pData  = shmat(shmid, NULL, 0);
			
			if (pData == (void*)-1)
			{
				result = "failed to attach shm|"  + sBasePath + "|key=" + TC_Common::tostr(key);
				throw runtime_error(result);
			}

			bool bSuccess = true;
			try
			{
				if (sBackupPath.empty())
				{
					result = "cache backup path is empty";
					throw runtime_error(result);			
				}

				if (!TC_File::isFileExistEx(sBackupPath, S_IFDIR))
				{
					if (TC_File::isFileExistEx(sBackupPath))
					{
						result = "cache backup path is a file:" + sBackupPath + "|" + sBasePath;
						throw runtime_error(result);
					}

					if (!TC_File::makeDirRecursive(sBackupPath))
					{
						result = "faild to mkdir backup path:" +sBackupPath;
						throw runtime_error(result);					
					}
				}

				string sBackupFile("");
				string::size_type lastCharPos = sBackupPath.length()-1; 
				if (sBackupPath.at(lastCharPos) != '/') 
				{
					sBackupFile = sBackupPath +  "/";
				}
				else 
				{
					sBackupFile = sBackupPath;
				}
				 sBackupFile = sBackupFile + sFullCacheName+ "_" + TC_Common::tostr(key) + ".cache_bakup";

				FILE *fp = fopen(sBackupFile.c_str(), "w+b");
				if(fp == NULL)
				{
					result = "failed to pen file:" + sBackupFile;
					throw runtime_error(result);
				}

				size_t WRITE_ONCE_SIZE = 1000000000; //一次写1G
				size_t ret = 0;
				size_t iLeftSize = iSize;
				while (iLeftSize > WRITE_ONCE_SIZE)
				{
					ret = fwrite(pData, 1, WRITE_ONCE_SIZE, fp);
					if (ret != WRITE_ONCE_SIZE)
					{
						result = "failed to write 1G file|ret=" + TC_Common::tostr(ret) + "|size=" + TC_Common::tostr(WRITE_ONCE_SIZE) + "|error=" + TC_Common::tostr(errno);
						throw runtime_error(result);
					}
					
					iLeftSize = iSize - WRITE_ONCE_SIZE;
					iSize = iLeftSize;
				}
				
				ret = fwrite(pData, 1, iLeftSize, fp);
				if (ret!=iLeftSize)
				{
					result = "failed to write file|ret=" + TC_Common::tostr(ret) + "|size=" +TC_Common::tostr(iLeftSize) + "|error=" + TC_Common::tostr(errno);
					throw runtime_error(result);
				}

				//删除共享内存
				ret =shmctl(shmid, IPC_RMID, NULL);
				if (ret !=0)
				{
					result = "failed to rm share memory|key=" + TC_Common::tostr(key) + "|shmid=" + TC_Common::tostr(shmid) + "|ret=" + TC_Common::tostr(ret);
					throw runtime_error(result);
				}
			}
			catch(exception &e)
			{
				bSuccess = false;
				result = TC_Common::tostr(__FUNCTION__) + ":"+ TC_Common::tostr(__LINE__)  + "|" + e.what();
				LOG->error() << result << endl;
				TAF_NOTIFY_ERROR(result);
			}

			int iRet = shmdt(pData);
			if (iRet != 0)
			{
				result = "failed to dettach share memory|key=" + TC_Common::tostr(key) + "|shmid=" + TC_Common::tostr(shmid) + "|ret=" + TC_Common::tostr(iRet);
				throw runtime_error(result);
			}

			if (!bSuccess)
			{
				LOG->debug() << "failed to delete cache:" << sFullCacheName << endl;
				return -1;
			}
			else
			{
				LOG->debug() << "success delete cache:" << sFullCacheName << endl;
			}
			
		}
		else
		{*/
			//删除共享内存
			int ret =shmctl(shmid, IPC_RMID, NULL);
			if (ret !=0)
			{
				result = "failed to rm share memory|key=" + TC_Common::tostr(key) + "|shmid=" + TC_Common::tostr(shmid) + "|ret=" + TC_Common::tostr(ret);
				throw runtime_error(result);
			}

			LOG->debug() << "success delete cache:" << sFullCacheName <<", no need backup it"<< endl;
		//}

		return 0;
		
			
		
	}
	catch ( exception& e )
	{
		result = TC_Common::tostr(__FUNCTION__) + ":"+ TC_Common::tostr(__LINE__)  + "|" + e.what();
		LOG->error() << result << endl;
		TAF_NOTIFY_ERROR(result);
	}
	
	return -1;
}


/*********************************************************************/





