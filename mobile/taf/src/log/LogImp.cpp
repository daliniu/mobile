#include "LogImp.h"
#include "log/taf_logger.h"

GlobeInfo   g_globe;

TC_DayLogger &GlobeInfo::makeDayLogger(const string &app, const string &server, const string &logname, const string &format,const string& ip)
{
	string sLogPath = _log_path +  "/" + app + "/" + server + "/" + logname;
	LOG->debug() << "makeDayLogger|sLogPath=" <<sLogPath<<"|"<<format<< endl;
    TC_DayLogger *p = new TC_DayLogger();
    p->init(sLogPath, format);
    p->setupThread(&_group);
    //所有标识都不要
    p->modFlag(0xffff, false);
    _loggers[logname][ip] = p;
    _formats[logname][ip] = format;
    return (*p);
}

TC_DayLogger &GlobeInfo::makeDayLogger(const LogInfo & info, const string &logname, const string &format,const string& ip)
{
	string sLogPath = getRealLogName(info);

	LOG->debug() << "makeDayLoggerEx|sLogPath=" <<sLogPath<<"|"<<format<< endl;
	TC_DayLogger *p = new TC_DayLogger();

	p->init(sLogPath, format,info.bHasSufix,info.sConcatStr);
	p->setSeparator(info.sSepar);
	p->enableSqareWrapper(info.bHasSquareBracket);
	p->setupThread(&_group);
	//所有标识都不要
	p->modFlag(0xffff, false);
	_loggers[logname][ip] = p;
	_formats[logname][ip] = format;
	return (*p);

}
string GlobeInfo::getRealLogName(const LogInfo & info)
{
	string sRealLogname;
	if(info.bHasAppNamePrefix)
	{
		sRealLogname = info.sFilename.empty() ? (info.appname +"." + info.servername) : \
			(info.appname +"." + info.servername + info.sConcatStr + info.sFilename);
	}
	else
	{
		sRealLogname = info.sFilename;
	}

	string setDivision = getSetGoodFormat(info.setdivision);

	string sLogPath = _log_path +  "/" + info.appname + "/" + setDivision  + "/" + info.servername + "/" + sRealLogname;

	return sLogPath;
}

string GlobeInfo::getLogName(const LogInfo & info)
{
	string sLogname = TC_Common::tostr<bool>(info.bHasSufix) + info.sConcatStr + TC_Common::tostr<bool>(info.bHasAppNamePrefix) + info.sConcatStr +\
						TC_Common::tostr<bool>(info.bHasSquareBracket) + info.sConcatStr + info.sSepar + info.sConcatStr +\
						info.setdivision + info.sConcatStr + info.appname + info.sConcatStr + info.servername + info.sConcatStr + info.sFilename;

	return sLogname;
}

bool GlobeInfo::HasSameFormat(const string& logname,const string& format,string& ip)
{
	map<string,string>::iterator  itFormat = _formats[logname].begin();
	bool bHasFormat = false;		 //是否有创建过的format
	for(; itFormat != _formats[logname].end();itFormat++)
	{
		if(itFormat->second == format)	  //找到创建过的format
		{
			bHasFormat = true;
			ip = itFormat->first; //为创建过相同format的节点ip
			break;
		}
	}
	return bHasFormat;
}

bool GlobeInfo::IsLoggerAttached(const string& logname,const string& sExcludeIp,const TC_DayLogger* pLogger)
{
	map<string,TC_DayLogger*>::iterator  itLogger = _loggers[logname].begin();
	bool bIsAttached = false;		 //是否有ip在用该logger实例
	for(; itLogger != _loggers[logname].end();itLogger++)
	{
		if(itLogger->first != sExcludeIp && itLogger->second == pLogger)
		{
			bIsAttached = true;
			LOG->debug() << "IsLoggerAttached|" << itLogger->first<< endl;
			break;
		}
	}
	return bIsAttached;
}

TC_DayLogger& GlobeInfo::getLogger(const LogInfo & infoOrg,const string& ip)
{
    //替换app名称
	LogInfo info = infoOrg;
	map<string,string>::const_iterator it = _mAppIdcRelation.find(info.appname);
	if(it != _mAppIdcRelation.end() )
	{
		info.appname  =  it->second;
	}
    
	string format = info.sFormat;
	if(binary_search(_vHourlist.begin(),_vHourlist.end(),(info.appname + "." + info.servername)) != false)
	{
		format = "%Y%m%d%H";
	}

	string setDivision = getSetGoodFormat(info.setdivision);

	TC_File::makeDirRecursive(_log_path + "/"  + info.appname + "/" + setDivision + "/" + info.servername);

	string logname = getLogName(info);

	//LOG->debug() <<"getLogger=" <<logname<<"|"<<format<< endl;

	Lock lock(*this);
	map<string, map<string,TC_DayLogger*> >::iterator itLogName = _loggers.find(logname);
	if( itLogName == _loggers.end())	 //没有创建过的log
	{
		return makeDayLogger(info, logname, format,ip);
	}

	map<string,TC_DayLogger*>::iterator  itIp = itLogName->second.find(ip);
	if(itIp == itLogName->second.end())   //没有创建过的ip节点
	{
		string sSameIp;
		if(HasSameFormat(logname,format,sSameIp))		  //有创建过的format,把新的节点ip划分到同组
		{
			_loggers[logname][ip] = _loggers[logname][sSameIp];//使用具有相同format的logger实例
			_formats[logname][ip] = format;
			return (*(_loggers[logname][ip]));
		}
		else  //没有创建过该format,新建立一个logname下的format组
		{
			return makeDayLogger(info, logname, format,ip);
		}

	}
	else  //该logname下的这个ip节点创建过
	{
		if(_formats[logname][ip] != format)  //该ip节点的日志格式变化了
		{
			string sSameIp;
			TC_DayLogger* pOldLogger = _loggers[logname][ip];
			if(HasSameFormat(logname,format,sSameIp))//已经有该格式，归入同组
			{
				_loggers[logname][ip] = _loggers[logname][sSameIp];
				_formats[logname][ip] = format;

				if(pOldLogger) //删除旧的loger
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return (*(_loggers[logname][ip]));
			}
			else
			{
				_loggers[logname][ip] = NULL;
				_formats[logname][ip] = "";
				if(!IsLoggerAttached(logname,ip,pOldLogger))//可能有多个节点引用这个loger，这里需要判断
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return makeDayLogger(info, logname, format,ip);
			}
		}
	}
	//没有改变格式
	return (*(_loggers[logname][ip]));

}

TC_DayLogger& GlobeInfo::getLogger(const string &appOrg, const string &server, const string &file, const string &sformat,const string& ip)
{

    //替换app名称
	string app = appOrg;
	map<string,string>::const_iterator it = _mAppIdcRelation.find(app);
	if(it != _mAppIdcRelation.end() )
	{
		app  =  it->second;
	}
	
	TC_File::makeDirRecursive(_log_path + "/"  + app + "/" + server);

	string format = sformat;
	if(binary_search(_vHourlist.begin(),_vHourlist.end(),(app + "." + server)) != false)
	{
		format = "%Y%m%d%H";
	}


	string logname = file.empty() ? (app + "." + server) : (app + "." + server + "_" + file);
	//LOG->debug() <<"getLogger=" <<logname<<"|"<<format<< endl;

	Lock lock(*this);
	map<string, map<string,TC_DayLogger*> >::iterator itLogName = _loggers.find(logname);
	if( itLogName == _loggers.end())     //没有创建过的log
	{
		return makeDayLogger(app, server, logname, format,ip);
	}

	map<string,TC_DayLogger*>::iterator  itIp = itLogName->second.find(ip);
	if(itIp == itLogName->second.end())   //没有创建过的ip节点
	{
		string sSameIp;
		if(HasSameFormat(logname,format,sSameIp))         //有创建过的format,把新的节点ip划分到同组
		{
			_loggers[logname][ip] = _loggers[logname][sSameIp];//使用具有相同format的logger实例
			_formats[logname][ip] = format;
			return (*(_loggers[logname][ip]));
		}
		else  //没有创建过该format,新建立一个logname下的format组
		{
			return makeDayLogger(app, server, logname, format,ip);
		}

	}
	else  //该logname下的这个ip节点创建过
	{
		if(_formats[logname][ip] != format)  //该ip节点的日志格式变化了
		{
			string sSameIp;
			TC_DayLogger* pOldLogger = _loggers[logname][ip];
			if(HasSameFormat(logname,format,sSameIp))//已经有该格式，归入同组
			{
				_loggers[logname][ip] = _loggers[logname][sSameIp];
				_formats[logname][ip] = format;

				if(pOldLogger) //删除旧的loger
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return (*(_loggers[logname][ip]));
			}
			else
			{
				_loggers[logname][ip] = NULL;
				_formats[logname][ip] = "";
				if(!IsLoggerAttached(logname,ip,pOldLogger))//可能有多个节点引用这个loger，这里需要判断
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return makeDayLogger(app, server, logname, format,ip);
			}
		}
	}
	//没有改变格式
	return (*(_loggers[logname][ip]));
}
string GlobeInfo::getSetGoodFormat(const string& sSetDivision)
{
	vector<string> vtSetDivisions = TC_Common::sepstr<string>(sSetDivision,".");
	if(vtSetDivisions.size() != 3)
	{
		return "";
	}
	else
	{
		if(vtSetDivisions[2] == "*")
		{
			return string(vtSetDivisions[0] + vtSetDivisions[1]);
		}
		else
		{
			return string(vtSetDivisions[0] + vtSetDivisions[1] + vtSetDivisions[2]);
		}
	}
}

//////////////////////////////////////////
//

void LogImp::initialize()
{

}



void LogImp::logger(const string &app, const string &server, const string &file, const string &format, const vector<string> &buffer, taf::JceCurrentPtr current)
{

    TC_DayLogger &dl = g_globe.getLogger(app, server, file, format,current->getIp());
    //记录日志
    for(size_t i = 0; i < buffer.size(); i++)
    {
        dl.any() << current->getIp() << "|" << buffer[i];
    }
}

void LogImp::loggerbyInfo(const LogInfo & info,const vector<std::string> & buffer,taf::JceCurrentPtr current)
{
    TC_DayLogger &dl = g_globe.getLogger(info,current->getIp());

    //记录日志
    for(size_t i = 0; i < buffer.size(); i++)
    {
        dl.any() << current->getIp() << info.sSepar << buffer[i];
    }
}
