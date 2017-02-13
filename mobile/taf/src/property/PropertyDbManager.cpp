#include "PropertyDbManager.h"
#include "PropertyReapThread.h"
#include "util/tc_config.h"
#include "PropertyServer.h"
#include "PropertyHashMap.h"
///////////////////////////////////////////////////////////
//

PropertyDbManager::PropertyDbManager()
{
	LOG->debug() << "begin PropertyDbManager init" << endl;

	////////////////////

	_sSql               = (*g_pconf)["/taf<sql>"];
	_sSqlStatus         = g_pconf->get("/taf<sqlStatus>", "");
	_mReapSql           = g_pconf->getDomainMap("/taf/reapSql");
	_sTbNamePre         = g_pconf->get("/taf/db<tbnamePre>","t_property_realtime_");
	_sConcerns          = g_pconf->get("/taf/<concerns>","  ");
	_iInterval          = TC_Common::strto<int>(g_pconf->get("/taf/reapSql<interval>","5"));
	_iMaxInsertCount    = TC_Common::strto<int>(g_pconf->get("/taf/reapSql<maxInsertCount>","1000"));


    if (_sSqlStatus == "")
    {
		_sSqlStatus = "CREATE TABLE `t_ecstatus` ( "
		"  `id` int(11) NOT NULL auto_increment, "
		"  `appname` varchar(64) NOT NULL default '', "
		"  `action` tinyint(4) NOT NULL default '0', "
		"  `checkint` smallint(6) NOT NULL default '10', "
		"  `lasttime` varchar(16) NOT NULL default '', "
		"   PRIMARY KEY  (`appname`,`action`), "
		"   UNIQUE KEY `id` (`id`) "
		" ) ENGINE=HEAP DEFAULT CHARSET=gbk";
	}


	string sCutType     = g_pconf->get("/taf/reapSql<CutType>","hour");
	if (sCutType == "day")
	{
		_eCutType = CUT_BY_DAY;
	}
	else if (sCutType == "minute")
	{
		_eCutType = CUT_BY_MINUTE;
	}
	else
	{
		_eCutType = CUT_BY_HOUR;
	}

	//初始化配置db连接
	//默认db
	TC_DBConf tConf;
	tConf.loadFromMap(g_pconf->getDomainMap("/taf/db"));
	_pDefaultMysql = new TC_Mysql();
	_pDefaultMysql->init(tConf);

	// 均匀分布在多个数据库
	vector<string> vDb =g_pconf->getDomainVector("/taf/multidb");
	dbNumber = vDb.size();
	
	LOG->debug() << "PropertyDbManager init multidb size:" <<dbNumber  << endl;
	for (int i=0; i< dbNumber; i++)
	{
		TC_DBConf tConf;
		tConf.loadFromMap(g_pconf->getDomainMap("/taf/multidb/" + vDb[i]));
		_vsTbNamePre.push_back(g_pconf->get("/taf/multidb/" + vDb[i] + "<tbname>",
			"t_propert_0" + TC_Common::tostr(i) + "_"));
		 TC_Mysql *_p = new TC_Mysql();
		 _p->init(tConf);
		_vpMysql.push_back(_p);
	}
	string s;
	if (loadDbRouter(s) != 0)
	{
		throw runtime_error("PropertyDbManager::initialize error :" + s);
	}

	LOG->debug() << "PropertyDbManager init ok." << endl;
}


int PropertyDbManager::loadDbRouter(string & sResult)
{
	try
	{
		_vDbConfig.clear();

		vector<string> vDbRouter = g_pconf->getDomainKey("/taf/dbrouter");
		for (unsigned i = 0; i < vDbRouter.size(); i++)
		{
			vector<string> vOneDb = TC_Common::sepstr<string>(vDbRouter[i], ";", true);
			if (vOneDb.size() < 6)
			{
				LOG->error() << "[PropertyDbManager::loadDb ] config format error. " << TC_Common::tostr(vDbRouter[i]) << endl;
				return -1;
			}

			TC_LockT<TC_ThreadMutex> lock(*this);
			string sModule      = TC_Common::trim(vOneDb[0]);
			string sDbhost      = TC_Common::trim(vOneDb[1]);
			string sDbName      = TC_Common::trim(vOneDb[2]);
			string sTbName      = TC_Common::trim(vOneDb[3]);
			string sDbUser      = TC_Common::trim(vOneDb[4]);
			string sDbPass      = TC_Common::trim(vOneDb[5]);
			int iDbport         = vOneDb.size()>6?TC_Common::strto<int>(vOneDb[6]):3306;

			_vDbConfig.push_back(sModule);

			TC_Mysql *pMysql = new TC_Mysql();
			pMysql->init(sDbhost,sDbUser,sDbPass,sDbName,"",iDbport);
			DbInfo *pDbInfo         = new DbInfo();
			pDbInfo->tDb.sModule    = sModule;
			pDbInfo->tDb.sIp        = sDbhost;
			pDbInfo->tDb.sDbName    = sDbName;
			pDbInfo->tDb.sTbName    = sTbName;
			pDbInfo->tDb.sUser      = sDbUser;
			pDbInfo->tDb.sPass      = sDbPass;
			pDbInfo->tDb.sPort      = iDbport;
			pDbInfo->pMysql         = pMysql;
			pDbInfo->tRegex         = initReg(sModule);
			if (_mDbMysql[sModule])
			{
				delete _mDbMysql[sModule];
			}

			_mDbMysql[sModule]  = pDbInfo;
			LOG->debug() << "[PropertyDbManager::loadDb succ ] " << TC_Common::tostr(vDbRouter[i]) << endl;
		}

		sResult =  " loadDb ok size ["+TC_Common::tostr(vDbRouter.size())+"]";
		LOG->debug() << "[PropertyDbManager::loadDb]"<<sResult<<endl;

		//打印加载的信息到日志，以备查验
		std::string sDbConfig;
		for (std::vector<std::string>::size_type i = 0; i < _vDbConfig.size(); i++)
		{
			map<string,DbInfo*>::const_iterator it = _mDbMysql.find(_vDbConfig[i]);
			if (it == _mDbMysql.end())
			{
				LOG->error() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|not found mysql handle for " << _vDbConfig[i] << endl;
				continue;
			}
			
			sDbConfig += "\nLOAD DB>" + _vDbConfig[i] + " --> H:" + it->second->tDb.sIp + " DB:" + it->second->tDb.sDbName + " TB:" + it->second->tDb.sTbName;
		}

		LOG->debug() << sDbConfig << endl;
		return 0;
	}
	catch (exception &ex)
	{
		LOG->error() << "[PropertyDbManager::loadDb] error:" << string(ex.what()) << endl;
		sResult = "load router config error:" + string(ex.what());
		return  -1;
	}
	return -1;
}


regex_t PropertyDbManager::initReg(const string& s)
{
	static const int iFlags = REG_EXTENDED | REG_ICASE;
	regex_t reg;
	if (regcomp(&reg,s.c_str(),iFlags))
	{
		//正则表达式错误
		throw runtime_error(string("PropertyDbManager::initReg: regular error:")+s);
	}
	return reg;
}

void PropertyDbManager::releaseReg()
{
	/*
	map<string,regex_t>::iterator it;
	for(it=_rules.begin();it!=_rules.end();it++) 
	{
		regfree(&(*it).second);
	}
	*/
}

inline bool PropertyDbManager::matchReg(const regex_t& regex,const string& value)
{
	regmatch_t stRegMatch;
	if (regexec(&(regex),value.c_str(), 1, &stRegMatch, 0) != 0)
	{
		return false;
	}
	if (stRegMatch.rm_so == 0 && stRegMatch.rm_eo == (int)value.length())
	{
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////
// 
int PropertyDbManager::flush(const string &sDate, const string &sFlag)
{
	try
	{
		ostringstream os;
		TC_LockT<TC_ThreadMutex> lock(*this);
		map<string,DbInfo*>::const_iterator it = _mDbMysql.begin();
		while (it != _mDbMysql.end())
		{

			if (it->second->mMasterTableMsg.size() > 0 )
			{
				string sMasterTbName = it->second->tDb.sTbName; /*+"master_";*/

				os.str("");

				it->second->tDb.displaySimple(os);

				LOG->debug() <<"begin insert2masterdb ["<<it->second->mMasterTableMsg.size()<<"]"<<os.str()<<endl;

				if (insert2Db(it->second->mMasterTableMsg,sDate,sFlag,sMasterTbName,it->second->pMysql) != 0)
				{
					insert2Db(it->second->mMasterTableMsg,sDate,sFlag,sMasterTbName,it->second->pMysql);
				}
				it->second->mMasterTableMsg.clear();
				updateEcsStatus(sDate+" "+sFlag,sMasterTbName,it->second->pMysql);
			}
			++it;
		}
		return 0;

	}
	catch (exception &ex)
	{
		LOG->error() << "[PropertyDbManager::routerByMaster] error:" << ex.what() << endl;
		return  -1;
	}
	return -1;
}

/////////////////////////////////////////////////////////
// 
bool PropertyDbManager::filter(const PropHead &head, const PropBody &body)
{
	try
	{
		bool bMasterMatched = false;
		TC_LockT<TC_ThreadMutex> lock(*this);

		for (std::vector<std::string>::size_type i = 0; i < _vDbConfig.size(); i++)
		{
			map<string,DbInfo*>::const_iterator it = _mDbMysql.find(_vDbConfig[i]);
			if (it == _mDbMysql.end())
			{
				LOG->error() << __FILE__ << "|" << __LINE__ << "|not found mysql handle for " << _vDbConfig[i] << endl;
				continue;
			}

			string sMasterName      = head.moduleName;
			string::size_type pos   =  sMasterName.find("@");
			if (pos != string::npos)
			{
				sMasterName   = sMasterName.substr(0,pos);
			}
			if (bMasterMatched == false && matchReg(it->second->tRegex,sMasterName) == true)
			{
				it->second->mMasterTableMsg[head] = body;
				bMasterMatched = true;
				return true;
			}
			++it;
		} 
	}
	catch (exception &ex)
	{
		LOG->error() << "[PropertyDbManager::routerByMaster] error:" << ex.what() << endl;
		return  false;
	}

	return false;
}

/////////////////////////////////////////////////////////
// 
int PropertyDbManager::getDbRouter(const string & sName,Db& tDb)
{
	try
	{
		TC_LockT<TC_ThreadMutex> lock(*this);

		for (std::vector<std::string>::size_type i = 0; i < _vDbConfig.size(); i++)
		{
			map<string,DbInfo*>::const_iterator it = _mDbMysql.find(_vDbConfig[i]);
			if (it == _mDbMysql.end())
			{
				LOG->error() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|not found mysql handle for " << _vDbConfig[i] << endl;
				continue;
			}

			if (matchReg(it->second->tRegex, sName) == true)
			{
				tDb = it->second->tDb;
				return 0;
			}
		} 
	}
	catch (exception &ex)
	{
		LOG->error() << "[PropertyDbManager::getDbConfig] error:" << ex.what() << endl;
	}
	return -1;
}


///////////////////////////////////////////////////////////
//
/*
insert2Db(const StatMsg &statmsg,const string &sDate, 
						 const string &sFlag,const string &sTbNamePre,TC_Mysql *pMysql)

*/
int PropertyDbManager::insert2Db( const map<PropHead,PropBody>& statmsg, const string &strDate,const string &strFlag ,const string &sTbNamePre ,TC_Mysql *pMysql )
{
	int iCount = 0;
	ostringstream osSql;
	string strValues;
	//string strTbName   = _sTbNamePre+ strDate + strFlag.substr(0,_eCutType*2);
	if (!pMysql)
	{
		pMysql = _pDefaultMysql;
	}
	string strTbName   = (sTbNamePre !=""?sTbNamePre:_sTbNamePre) + TC_Common::replace(strDate,"-","") + strFlag.substr(0,_eCutType*2);
	try
	{
		for(map<PropHead,PropBody>::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
		{								
			const PropHead& head = it->first;
			const PropBody& body = it->second;		  
			//组织sql语句
			for(size_t i = 0; i < body.vInfo.size(); i++)
			{
				strValues = /*" (FROM_UNIXTIME(" +TC_Common::tostr( head.time )+ "),"*/
					" ('" + strDate + "',"
					+ "'" + strFlag + "',"
					+ "'" + head.moduleName + "',"
					+ "'" + head.ip + "',"
					+ "'" + pMysql->escapeString(head.propertyName) + "',"
					+ "'" + head.setName + "',"
					+ "'" + head.setArea + "',"
					+ "'" + head.setID + "',"
					+ "'" + TC_Common::tostr( body.vInfo[i].policy ) + "',"
					+ "'" + TC_Common::tostr( body.vInfo[i].value ) + "') ";
	
				if ( iCount == 0 )
				{
					osSql.str("");
					osSql << "insert ignore into " + strTbName + "  (f_date,f_tflag,master_name,master_ip,property_name,set_name, set_area,set_id,policy,value) values ";
					osSql << strValues;
				}
				else
				{
					osSql<< "," + strValues;
				}	
				iCount ++;
			}
			if ( iCount >= _iMaxInsertCount )
			{
			    usleep(100);
				pMysql->execute(osSql.str());
				LOG->debug() << "insert affected:" << iCount << endl;
				iCount = 0;
			}
		}
		if ( iCount != 0 )
		{
			pMysql->execute(osSql.str());
			LOG->debug() << "insert affected:" << iCount << endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		creatTable(strTbName, pMysql);
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
	catch (exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
	return 0;
}
int PropertyDbManager::getDbNumber()
{
	return dbNumber;
}


int PropertyDbManager::insert2MultiDbs(vector<map<PropHead,PropBody > > &statmsg, const string &sDate, const string &sFlag)
{

	string sTbName = "";
	int iSize = statmsg.size();
	int iRet =0;
	string sIp = ServerConfig::LocalIp;

	for (int i=0; i < iSize && i < dbNumber; i++)
	{
		//sTbName = sTbNamePre + TC_Common::tostr(i) + "_";
		LOG->debug() << "insert "+_vsTbNamePre[i] + " sDate:"+ sDate + 
		" sFlag:" +sFlag+ " affected:" << statmsg[i].size() << endl;
		if (insert2Db(statmsg[i],sDate,sFlag, _vsTbNamePre[i], _vpMysql[i]) != 0)
		{
			iRet = insert2Db(statmsg[i],sDate,sFlag, _vsTbNamePre[i], _vpMysql[i]);

			if ( iRet != 0 )
			{
				sendAlarmSMS();
			}
		}
		if (updateEcsStatus(sDate+" "+sFlag, _vsTbNamePre[i]  + sIp , _vpMysql[i]) != 0)
		{
			updateEcsStatus(sDate+" "+sFlag, _vsTbNamePre[i]  + sIp , _vpMysql[i]);
		}
		statmsg.at(i).clear();
	}
	return 0;
}
///////////////////////////////////////////////////////////
//

int PropertyDbManager::sendAlarmSMS()
{
	static time_t lasttime = 0;  // 静态变量， 线程安全问题?
	static int count = 0;
	#define COUNT_MAX 2
	time_t current =  time(NULL);
	if (lasttime !=0 && lasttime + 600 > current && count > COUNT_MAX) // 20分钟发送2条
	{
		return 0;
	}
	if (count > COUNT_MAX)
	{
        count = 0;
	}
	count++;
	lasttime = current;
	string errInfo = " ERROR:" + ServerConfig::LocalIp +  ":统计入库失败，请及时处理!";
	TAF_NOTIFY_ERROR(errInfo);
	LOG->debug() << "TAF_NOTIFY_ERROR " << errInfo << endl;

	return 0;
}

int PropertyDbManager::creatTable(const string &sTbName, TC_Mysql *_pstMql)
{
	if(_eCutType == CUT_BY_DAY)
	{
		creatTb(sTbName,  _pstMql) ;	
		return 0;
	}
	string daytime = sTbName.substr(0, sTbName.length() -2 ); // 去掉小时
	int iRet = 0;
	int hour = 0;
	ostringstream	osHour;
	for (hour=0; hour < 24; hour++)
	{
		osHour	<< setw(2) << setfill('0') << hour ;
		string sName =   daytime + osHour.str();
		osHour.str(""); // 清空
		creatTb(sName,  _pstMql) ;
	}
	return iRet;	
}

int PropertyDbManager::creatTb(const string &sTbName,TC_Mysql *pMysql)
{ 
	try
	{
		string sSql = TC_Common::replace(_sSql, "${TABLE}",sTbName);
		LOG->debug() << sSql << endl;

		if (!pMysql)
		{
			pMysql = _pDefaultMysql;
		}
		TC_Mysql::MysqlData tTotalRecord = pMysql->queryRecord("show tables like '%"+sTbName+"%'");
		LOG->debug()<<__FUNCTION__<<"|show tables like '%"+sTbName+"%|affected:"<<tTotalRecord.size()<<endl;
		if (tTotalRecord.size() > 0)
		{
			return 0;
		}
		pMysql->execute(sSql);
		return 0;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
}

int PropertyDbManager::creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql)
{ 
	try
	{
		LOG->debug() << sSql << endl;

		if (!pMysql)
		{
			pMysql = _pDefaultMysql;
		}
		TC_Mysql::MysqlData tTotalRecord = pMysql->queryRecord("show tables like '%"+sTbName+"%'");
		LOG->debug()<<__FUNCTION__<<"|show tables like '%"+sTbName+"%|affected:"<<tTotalRecord.size()<<endl;
		if (tTotalRecord.size() > 0)
		{
			return 0;
		}
		pMysql->execute(sSql);
		return 0;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
}



int PropertyDbManager::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
	try
	{
		if (!pMysql)
		{
			pMysql = _pDefaultMysql;
		}

		string sAppName  = sTbNamePre != ""?sTbNamePre:_sTbNamePre;
		string sCondition = "where  appname='"+sAppName+"' and action =0";
		TC_Mysql::RECORD_DATA rd;
		rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
		int iRet = pMysql->updateRecord("t_ecstatus", rd,sCondition);
		LOG->debug() << "PropertyDbManager::updateEcsStatus iRet: " <<iRet <<"\n"<<pMysql->getLastSQL()<< endl;
		if (iRet == 0 )
		{
			rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
			rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(PropertyServer::g_iInsertInterval));
			rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
			iRet = pMysql->replaceRecord("t_ecstatus", rd);
		}
		if (iRet != 1)
		{
			LOG->debug() << "PropertyDbManager::updateEcsStatus erro: ret:" << iRet<<"\n"<<pMysql->getLastSQL()<< endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "PropertyDbManager::updateEcsStatus exception: " << ex.what() << endl;
		creatEscTb("t_ecstatus", _sSqlStatus, pMysql);
		return 1;
	}
	return 0;
}




