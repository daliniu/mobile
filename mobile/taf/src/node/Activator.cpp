#include <sys/wait.h>
#include "Activator.h"
#include "NodeServer.h"
#include "log/taf_logger.h"

static pid_t *childpid = NULL;/* ptr to array allocated at run-time */
#define SHELL   "/bin/sh"

pid_t Activator::activate( const string& strExePath,
                           const string& strPwdPath,
                           const string &strRollLogPath,
                           const vector<string>& vOptions,
                           vector<string>& vEnvs )
{
    addActivatingRecord();
    if(isActivatingLimited() == true)
    {
        LOG->error()<<"The server "<< strExePath <<" activating is limited! it will not auto start until after "+TC_Common::tostr(_iPunishInterval) +" seconds"<<endl;
    }
    if ( strExePath.empty() )
    {
        throw runtime_error( "The server executable path is empty." );
    }
    if(TC_File::isAbsolute(strExePath) == true && !TC_File::isFileExistEx(strExePath))
    {
        throw runtime_error( "The server patch " + strExePath+" is not exist." );
    }
    if(!TC_File::canExecutable(strExePath))
    {
        TC_File::setExecutable(strExePath,true);
    }
    vector<string> vArgs;
    vArgs.push_back( strExePath);
    vArgs.insert( vArgs.end(), vOptions.begin(), vOptions.end() );

    LOG->debug() << "activating server path " << strExePath << " " <<TC_Common::tostr(vArgs)<< endl;
    //
    // Activate and create.
    // Convert to standard argc/argv.
    //
    int argc = static_cast<int>( vArgs.size() );
    char** argv = static_cast<char**>( malloc( ( argc + 1 ) * sizeof( char* ) ) );
    int i = 0;
    for ( vector<string>::const_iterator p = vArgs.begin(); p != vArgs.end(); ++p, ++i )
    {
        assert( i < argc );
        argv[i] = strdup( p->c_str() );
    }
    assert( i == argc );
    argv[argc] = 0;

    //
    // Current directory
    //
    const char* pwdCStr = strPwdPath.c_str();

    pid_t pid = fork();
    if ( pid == -1 )
    {
        LOG->debug() << strPwdPath << "|fork child process  catch exception|errno=" << errno << endl;
        throw runtime_error( "fork child process  catch exception" );
    }

    if ( pid == 0 ) // Child process.
    {
        //
        // Close all file descriptors, except for standard input,
        // standard output, standard error
        //
        int maxFd = static_cast<int>( sysconf( _SC_OPEN_MAX ) );
        for ( int fd = 3; fd < maxFd; ++fd )
        {
            close( fd );
        }

        //server stdcout 日志在滚动日志显示
		if(_sRedirectPath != "")
		{
			if(( freopen64(_sRedirectPath.c_str(), "ab", stdout)) != NULL && ( freopen64(_sRedirectPath.c_str(), "ab", stderr)) != NULL)
			{
				cout <<argv[0]<<" redirect stdout and stderr  to " << _sRedirectPath << endl;
			}
			else
			{
				cout << argv[0]<<" cannot redirect stdout and stderr  to log file" << _sRedirectPath << "|errno=" <<strerror(errno) << endl;
			}

		}
		else
		{
	        if(strRollLogPath != "" && ( freopen64(strRollLogPath.c_str(), "ab", stdout)) != NULL && ( freopen64(strRollLogPath.c_str(), "ab", stderr)) != NULL)
	        {
	            cout <<argv[0]<<" redirect stdout and stderr  to " << strRollLogPath << endl;
	        }
	        else
	        {
	            cout << argv[0]<<" cannot redirect stdout and stderr  to log file" << strRollLogPath << "|errno=" <<strerror(errno) << endl;
	        }
		}

		for_each(vEnvs.begin(), vEnvs.end(), EnvironmentEval());

        //
        // Change working directory.
        //
        if ( strlen( pwdCStr ) != 0 )
        {
            if ( chdir( pwdCStr ) == -1 )
            {
                cerr<<argv[0]<<" cannot change working directory to " << pwdCStr << "|errno=" << errno << endl;
            }
        }

        if ( execvp( argv[0], argv ) == -1 )
        {
            cerr <<"cannot execute " << argv[0] << "|errno=" << strerror(errno) << endl;
        }
		exit(0);
    }
    else // Parent process.
    {
        for ( i = 0; argv[i]; i++ )
        {
            free( argv[i] );
        }
        free( argv );
    }
    return pid;
}


pid_t Activator::activate(const string &strServerId,
                          const string &strStartScript,
                          const string &strMonitorScript,
                          string &strResult)
{
    addActivatingRecord();
    if(isActivatingLimited() == true)
    {
        LOG->error()<<"The server "<<strServerId<<":"<< strStartScript  <<" activating is limited! it will not auto start until after "+TC_Common::tostr(_iPunishInterval) +" seconds"<<endl;
    }
    if ( strStartScript.empty() )
    {
        throw runtime_error( "The script file "+strStartScript+" is empty." );
    }
    if(!TC_File::isFileExistEx(strStartScript))
    {
        throw runtime_error( "The script file: " + strStartScript+" is not exist." );
    }
    map <string,string> mResult;
    if(doScript(strServerId,strStartScript,strResult,mResult) == false)
    {
        throw runtime_error("run script file "+ strStartScript+ " error :"+strResult);
    }
    pid_t pid = -1;
    if(!strMonitorScript.empty() && TC_File::isFileExistEx(strMonitorScript))
    {
        string s;
        mResult.clear();
        if(doScript(strServerId,strMonitorScript,s,mResult) == false)
        {
            throw runtime_error("run script "+ strMonitorScript+ " error :"+s);
        }
    }

    //无法获取业务服务的pid,所以这里屏蔽掉
    //>>modified by spinnerxu@20111017
    /*if(mResult.find("pid") != mResult.end() && TC_Common::isdigit(mResult["pid"]) == true)
    {
        pid =  TC_Common::strto<int>(mResult["pid"]);
    }

    if(pid <= 0 )
    {
        throw runtime_error("run script "+ strMonitorScript+ " error pid invalid:"+strResult);
    }*/
    //<<
    return pid;
}

int Activator::deactivate( int pid )
{
    if ( pid != 0 )
    {
        return sendSignal( pid, SIGKILL );
    }
    return -1;
}

int Activator::deactivateAndGenerateCore( int pid )
{
    if ( pid != 0 )
    {
        return sendSignal( pid, SIGABRT );
    }
    return -1;
}

int Activator::sendSignal( int pid, int signal ) const
{
    assert( pid );
    int ret = ::kill( static_cast<pid_t>( pid ), signal );
    if ( ret != 0 && errno != ESRCH )
    {
        LOG->error() << "send signal "<<signal<<" to pid "<<pid<<" catch exception|" << errno << endl;
        return -1;
    }
    return ret;
}

bool Activator::isActivatingLimited ()
{
    unsigned uLen = _vActivatingRecord.size();

    //惩罚时间段内禁止启动
    if(_bLimited == true && uLen>0 && TC_TimeProvider::getInstance()->getNow()-_vActivatingRecord[uLen-1] < _iPunishInterval)
    {
        return true;
    }
    return false;
}
void Activator::addActivatingRecord()
{
    time_t tNow     = TC_TimeProvider::getInstance()->getNow();
    unsigned uLen   = _vActivatingRecord.size();

    if(uLen > 0 )
    {
        if(tNow - _vActivatingRecord[uLen -1] < _iTimeInterval)
        {
            _iCurCount++;
        }

        //完成惩罚时间,自动解除惩罚
        if( _bLimited == true && tNow - _vActivatingRecord[uLen-1] > _iPunishInterval*1.5)
        {
            _iCurCount  = 0;
            _bLimited   = false;
            _vActivatingRecord.clear();
        }

        if(_iCurCount > _iMaxCount)
        {
            _bLimited = true;
        }
    }
    _vActivatingRecord.push_back(tNow);
}

bool Activator::doScript(const string &strServerId,
                         const string &strScript,
                         string &strResult,
                         map <string,string> &mResult,
                         const string &sEndMark)
{
    LOG->info()<<"doScript "<<strScript<<" begin----"<<endl;

    if(!TC_File::isFileExistEx(strScript))
    {
        strResult = "The script file: " + strScript+" is not exist";
        return false;
    }
    string sRealEndMark = sEndMark;
    if(sRealEndMark == "")
    {
        sRealEndMark = "end-"+TC_File::extractFileName(strScript);
    }
    LOG->info()<<"doScript "<<strScript<<" endMark----"<<sRealEndMark<<endl;

    if(!TC_File::canExecutable(strScript))
    {
        TC_File::setExecutable(strScript,true);
    }
	string sRedirect ="";
	if(_sRedirectPath != "")
	{
		sRedirect =  " 2>&1 >>" +_sRedirectPath;
	}

    string sCmd = strScript + sRedirect + " " + strServerId + " &";
    FILE  *fp =  popen2(sCmd.c_str(),"r");
    if(fp == NULL)
    {
        strResult = "popen script file: "+ strScript +" error ";
        return false;
    }

    int flags;
    if((flags=fcntl(fileno(fp), F_GETFL, 0)) < 0)
    {
        strResult = "fcntl get error.script file: "+ strScript;
		fflush(fp);
		pclose2(fp);
        return false;
    }
    flags |= O_NONBLOCK;
    if( fcntl(fileno(fp), F_SETFL, flags) < 0 )
    {
        strResult = "fcntl set error.script file: "+ strScript;
		fflush(fp);
		pclose2(fp);

        return false;
    }
    //strResult = "";

    char c;
    time_t tNow = TC_TimeProvider::getInstance()->getNow();
    while(TC_TimeProvider::getInstance()->getNow()- 2 < tNow)
    {
        usleep(200000);
        while((c=fgetc(fp))!=EOF )
        {
            strResult += c;
        }
        if(sRealEndMark == "" || strResult.find(sRealEndMark) != string::npos)
        {
            LOG->info()<<"doScript "<<sCmd<<"|sEndMark "<<sRealEndMark <<" finded|"<<strResult<<endl;
            break;
        }
    }
    strResult = TC_Common::replace(strResult,"\n","\r\n");
    LOG->info()<<"doScript "<<sCmd<<" result:"<<strResult<<endl;
    fflush(fp);
    pclose2(fp);
    mResult = parseScriptResult(strServerId,strResult);
    return true;
}

map <string,string> Activator::parseScriptResult(const string &strServerId,const string &strResult)
{
    map <string,string> mResult;
    vector<string> vResult = TC_Common::sepstr<string>(strResult,"\n");
    for(unsigned i=0; i<vResult.size();i++)
    {
        string::size_type pos = vResult[i].find('=');
        if( pos != string::npos)
        {
            string sName    = vResult[i].substr(0, pos);
            string sValue   = vResult[i].substr(pos+1);
            sName           = TC_Common::lower(TC_Common::trim(sName));
            sValue          = TC_Common::trim(sValue);
            mResult[sName]  = sValue;
            if(sName == "notify" )
            {
               g_app.reportServer(strServerId,sValue);
            }
        }
    }
    return mResult;
}

FILE* Activator::popen2(const char *cmdstring, const char *type)
{
    int     i, pfd[2];
    pid_t   pid;
    FILE    *fp;
    /*only allow "r" or "w" */
    if((type[0] != 'r' && type[0] != 'w') || type[1] != 0)
    {
        errno = EINVAL;     /* required by POSIX.2 */
        return(NULL);
    }
    int maxfd = static_cast<int>(sysconf( _SC_OPEN_MAX ));
    if(childpid == NULL)
    {  /* first time through */
       /* allocate zeroed out array for child pids */
        childpid = new pid_t [maxfd];
        memset(childpid,0,sizeof(pid_t)*maxfd);
        //char** envArray = new char*[envCount];
        if (childpid  == NULL)
        {
            return(NULL);
        }
    }

    if (pipe(pfd) < 0)
    {
        return(NULL);   /* errno set by pipe() */
    }

    if ( (pid = fork()) < 0)
    {
        return(NULL);   /* errno set by fork() */
    }
    else if (pid == 0)
    {                            /* child */
        if (*type == 'r')
        {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            dup2(pfd[1], STDERR_FILENO);
            close(pfd[1]);
        }
        else
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }
        /* close all descriptors opened in parent process*/
        for (i = 3; i < maxfd; i++)
        {
            if(i != pfd[0]&& i!=  pfd[1])
            {
                close(i);
            }
        }
        execl(SHELL, "sh", "-c", cmdstring, (char *) 0);
        _exit(127);
    }
    /* parent */
    if (*type == 'r')
    {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL)
        {
            return(NULL);
        }
    }
    else
    {
        close(pfd[0]);
        if ( (fp = fdopen(pfd[1], type)) == NULL)
        {
            return(NULL);
        }
    }
    childpid[fileno(fp)] = pid; /* remember child pid for this fd */
    return(fp);
}

int Activator::pclose2(FILE *fp)
{
    int     fd, stat;
    pid_t   pid;

    if(childpid == NULL)
    {
        return(-1);     /* popen() has never been called */
    }
    fd = fileno(fp);
    if ((pid = childpid[fd]) == 0)
    {
        return(-1);     /* fp wasn't opened by popen() */
    }
    childpid[fd] = 0;
    if (fclose(fp) == EOF)
    {
        return(-1);
    }

    while(waitpid(pid, &stat, 0) < 0)
    {
        if(errno != EINTR)
        {
            return(-1); /* error other than EINTR from waitpid() */
        }
    }
    return(stat);   /* return child's termination status */
}
