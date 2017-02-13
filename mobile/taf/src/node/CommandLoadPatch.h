#ifndef __LOADPATCH_COMMAND_H_
#define __LOADPATCH_COMMAND_H_

#include "CommandPatch.h"
#include "patch/taf_patch.h"
#include "NodeDescriptor.h"

class PatchClientInterface;
typedef TC_AutoPtr<PatchClientInterface>    PatchClientInterfacePtr;

class CommandLoadPatch : public ServerCommand
{
public:
    CommandLoadPatch(const ServerObjectPtr & server, const std::string & sDownloadPath, const taf::PatchRequest & request);

    ExeStatus canExecute(std::string &sResult);

    int execute(std::string &sResult);

    int updatePatchResult();

    int download(const std::string & sRemotePath, const std::string & sLoadPath, const std::string & sLoadFile, std::string & sResult);
private:
	void backupfiles();
private:
    taf::PatchRequest   _PatchRequest;

    std::string         _sDownloadPath;

    std::string         _sLocalPath;

    ServerObjectPtr     _pServerObjectPtr;

private:
    StatExChangePtr _pStatExChange;
};


//////////////////////////////////////////////////////////////
//
inline CommandLoadPatch::CommandLoadPatch(const ServerObjectPtr & server, const std::string & sDownloadPath, const taf::PatchRequest & request)
: _pServerObjectPtr(server)
{
    _sDownloadPath  = sDownloadPath + "/BatchPatchingLoad";
    _sLocalPath     = sDownloadPath + "/BatchPatching";
    _PatchRequest   = request;
}

inline ServerCommand::ExeStatus CommandLoadPatch::canExecute(string & sResult)
{
    LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << sResult << endl;

    //设置发布信息
    TC_ThreadRecLock::Lock lock(*_pServerObjectPtr);

    /*ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();
    if (_pServerObjectPtr->toStringState(eState).find("ing") != string::npos)
    {
        sResult = "cannot patch the server ,the server state is " + _pServerObjectPtr->toStringState(eState);
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << sResult << endl;
        return DIS_EXECUTABLE;
    }*/

    ServerObject::InternalServerState eState = _pServerObjectPtr->getLastState();
    LOG->debug() << __FUNCTION__ << ":" << __LINE__ << " old state :" << _pServerObjectPtr->toStringState( eState )  << endl;

    _pStatExChange  = new StatExChange(_pServerObjectPtr, ServerObject::BatchPatching, eState);

    _pServerObjectPtr->setPatchVersion(_PatchRequest.version);
    _pServerObjectPtr->setPatchPercent(0);
    _pServerObjectPtr->setPatchResult("");

    return EXECUTABLE;
}

inline int CommandLoadPatch::updatePatchResult()
{
    try
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << endl;

        //服务发布成功，向主控发送UPDDATE命令
        string sRigistry  = g_app.getConfig().get("/taf/node<registryObj>", "taf.tafregistry.RegistryObj");
        RegistryPrx proxy = Application::getCommunicator()->stringToProxy<RegistryPrx>(sRigistry);
        if (!proxy)
        {
            std::string sResult = "patch succ but update version and user fault, get registry proxy fail";

            LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << sResult << endl;
            g_app.reportServer(_PatchRequest.appname + "." + _PatchRequest.servername, sResult);

            return -1;
        }

        //向主控发送命令，设置该服务的版本号和发布者
        struct PatchResult patch;
        patch.sApplication  = _PatchRequest.appname;
        patch.sServerName   = _PatchRequest.servername;
        patch.sNodeName     = _PatchRequest.nodename;
        patch.sVersion      = _PatchRequest.version;
        patch.sUserName     = _PatchRequest.user;

        int iRet = proxy->updatePatchResult(patch);
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Update:" << iRet << endl;
    }
    catch (exception& e)
    {
        LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Exception:" << e.what() << endl;
        return -1;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Unknown Exception" << endl;
        return -1;
    }

    return 0;
}


inline int CommandLoadPatch::download(const std::string & sRemotePath, const std::string & sLoadPath, const std::string & sLoadFile, std::string & sResult)
{
    static  TC_ThreadLock md5_mutex;                                                        //保护不重复发布的锁
    static  std::map<std::string, pair<TC_ThreadLock *, int> >              g_MapMd5Path;   //标记特定的服务是否在发布中
    typedef std::map<std::string, pair<TC_ThreadLock *, int> >::iterator    iterator;

    int  iRet                   = 0;
    bool bLoading               = true;
    TC_ThreadLock * load_lock   = NULL;
    {
        TC_ThreadLock::Lock lock(md5_mutex);

        if (TC_MD5::md5file(sLoadFile) == _PatchRequest.md5)
        {
			LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << " has same md5!"<<endl;
            return 0;
        }

        iterator it = g_MapMd5Path.find(_PatchRequest.md5);
        if (it == g_MapMd5Path.end())
        {
            bLoading    = false;
            load_lock   = new TC_ThreadLock();
            g_MapMd5Path.insert(std::make_pair(_PatchRequest.md5, make_pair(load_lock, 1)));
        }
        else
        {
            bLoading    = true;
            load_lock   = it->second.first;
            it->second.second++;
        }
    }

    if (bLoading == true)
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|wait loading before" << endl;
        TC_ThreadLock::Lock LoadLock(*load_lock);

        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|wait loading begin" << endl;
        load_lock->wait();
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|wait loading end" << endl;

        iRet = 0;
        goto FREE;
    }

    LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|load file before" << endl;
    TC_File::removeFile(sLoadPath, true);
    if (!TC_File::makeDirRecursive(sLoadPath))
    {
        sResult =  "cannot create dir: " + sLoadPath;
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|load file error|" << sResult << endl;

        iRet = -1;
        goto FREE_AND_NOTIFY;
    }

    try
    {
        //到PATCH下载文件
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|load file loading" << endl;
        PatchPrx proxy = Application::getCommunicator()->stringToProxy<PatchPrx>(_PatchRequest.patchobj);
        proxy->taf_timeout(60000);

        PatchClientInterfacePtr p = new PatchClientInterface(_pServerObjectPtr);

        TafPatch tafPatch;
        tafPatch.init(proxy, sRemotePath, sLoadPath);
        tafPatch.setRemove(false);
        tafPatch.download(p);
    }
    catch (std::exception & ex)
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|Exception:" << ex.what() << endl;
    }
    catch (...)
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|Unknown Exception" << endl;
    }

FREE_AND_NOTIFY:
    //通知释放
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|load file notifyAll" << endl;
        TC_ThreadLock::Lock LoadLock(*load_lock);

        load_lock->notifyAll();
    }

FREE:
    LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|load file remove md5" << endl;
    TC_ThreadLock::Lock lock(md5_mutex);
    iterator it = g_MapMd5Path.find(_PatchRequest.md5);
    if (it != g_MapMd5Path.end() && (--it->second.second == 0))
    {
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << "|load file remove md5:" << it->second.second << endl;
        delete it->second.first;
        g_MapMd5Path.erase(_PatchRequest.md5);
    }

    return 0;
}

inline int CommandLoadPatch::execute(string &sResult)
{
    LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << _PatchRequest.md5 << endl;

    int iRet = -1;
    try
    {
        string sServerName  = _PatchRequest.groupname.empty()?_PatchRequest.servername:_PatchRequest.groupname;
        string sLoadPath    = _sDownloadPath + "/" + _PatchRequest.appname + "." + sServerName;
        string sLoadFile    = sLoadPath + "/" + _PatchRequest.appname + "." + sServerName + ".tgz";

        if (download("/TAFBatchPatching/" + _PatchRequest.appname + "/" + sServerName, sLoadPath, sLoadFile, sResult) != 0)
        {
			sResult = "download file faiure";
            goto EXIT;
        }

        //判断下载是否正确 并解压
        if (TC_File::isFileExist(sLoadFile) == false)
        {
			sResult ="file not exist";
            LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|file not exist:" << sLoadFile << endl;
			goto EXIT;
		}

		if(TC_MD5::md5file(sLoadFile) != _PatchRequest.md5)
        {
			sResult = "file md5 error";
            LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|file md5 error:" << sLoadFile << endl;
            goto EXIT;
        }

        string sLocalPach   = _sLocalPath + "/" + _PatchRequest.appname + "." + _PatchRequest.servername;
        TC_File::removeFile(sLocalPach, true);
        if (!TC_File::makeDirRecursive(sLocalPach))
        {
			sResult = "makeDirRecursive fault";
            LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|makeDirRecursive fault" << endl;
            goto EXIT;
        }
        string cmd          = "tar xzfv " + sLoadFile + " -C " + sLocalPach;
        system(cmd.c_str());
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|unzip:" << cmd <<endl;

        //移动目录重新命名文件
        string sSrcFile     = sLocalPach + "/" + sServerName + "/" + sServerName;
        string sDstFile     = sLocalPach + "/" + sServerName + "/" + _PatchRequest.servername;
        rename(sSrcFile.c_str(), sDstFile.c_str());
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|rename:" << sSrcFile << " " << sDstFile << endl;

		//检查是否需要备份bin目录下的文件夹
		backupfiles();

        TC_File::copyFile(sLocalPach + "/" + sServerName, _pServerObjectPtr->getExePath(), true);


        //设置发布结果
        _pServerObjectPtr->setPatched(true);

        sResult = "patch " + _PatchRequest.appname + "." + _PatchRequest.servername + " succ, version " + _PatchRequest.version;
        iRet    = 0;
        LOG->debug() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|" << sResult << endl;

        //设置发布状态
        updatePatchResult();
    }
    catch (exception& e)
    {
        sResult += e.what();
        LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Exception:" << e.what() << endl;
    }
    catch (...)
    {
        sResult += "catch unkwon exception";
        LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Unknown Exception" << endl;
    }

EXIT:
    _pServerObjectPtr->setPatchResult(sResult, iRet==0?true:false);

    g_app.reportServer(_PatchRequest.appname + "." + _PatchRequest.servername, sResult);

    return iRet;
}

inline void CommandLoadPatch:: backupfiles()
{
	try
	{
	    if (_pServerObjectPtr->getServerType() == "taf_java") //如果是java服务的话需要清空目录，备份目录
	    {
	        int maxbaknum   = 5;
	        string srcPath  = "/usr/local/app/taf/tafnode/data/" + _PatchRequest.appname + "." + _PatchRequest.servername + "/";
	        string destPath = "/usr/local/app/taf/tafnode/tmp/" + _PatchRequest.appname + "." + _PatchRequest.servername + "/";

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
	               LOG->debug() << existFile << " backup file is not exist,create it... "<<endl;
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
		    vector<string> vFileNames = TC_Common::sepstr<string>(_pServerObjectPtr->getBackupFileNames(), ";|");
			for(vector<string>::size_type i = 0;i< vFileNames.size();i++)
			{
		    	string sBackupSrc = destPathBak + vFileNames[i];
		    	string sBackupDest = existFile + vFileNames[i];
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
					LOG->debug()<<"Backup:"<<sBackupSrc<<" to "<<sBackupDest<<" succ"<<endl;
		    	}
			}
	    }
	}catch(exception& e)
	{
		LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Exception:" << e.what() << endl;

	}catch(...)
	{
		LOG->error() << __FUNCTION__ << "|" << _PatchRequest.appname + "." + _PatchRequest.servername << "|Unknown Exception" << endl;
	}
}

#endif

