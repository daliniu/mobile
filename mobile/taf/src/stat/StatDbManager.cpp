#include "StatDbManager.h"
#include "util/tc_config.h"
#include "StatServer.h"
#include "StatHashMap.h"
///////////////////////////////////////////////////////////
//

StatDbManager::StatDbManager()
{
	LOG->debug() << "begin StatDbManager init" << endl;

	////////////////////

	_sSql               = (*g_pconf)["/taf<sql>"];
	_sSqlStatus         = g_pconf->get("/taf<sqlStatus>", "");
	_mReapSql           = g_pconf->getDomainMap("/taf/reapSql");
	_sTbNamePre         = g_pconf->get("/taf/db<tbnamePre>","t_stat_realtime_");
	_iInterval          = TC_Common::strto<int>(g_pconf->get("/taf/reapSql<interval>","5"));
	_iMaxInsertCount    = TC_Common::strto<int>(g_pconf->get("/taf/reapSql<maxInsertCount>","1000"));
	//默认不使用权重
	_EnableWeighted   = TC_Common::strto<bool>(g_pconf->get("/taf/<enWeighted>","0"));

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

	vector<string> vDb =g_pconf->getDomainVector("/taf/multidb");

	_dbNumber = vDb.size();

	LOG->debug() << "StatDbManager init multidb size:" <<_dbNumber  << endl;

	for (int i=0; i< _dbNumber; i++)
	{
		TC_DBConf tConf;
		tConf.loadFromMap(g_pconf->getDomainMap("/taf/multidb/" + vDb[i]));
		_vsTbNamePre.push_back(g_pconf->get("/taf/multidb/" + vDb[i] + "<tbname>",
											  "t_stat_0" + TC_Common::tostr(i) + "_"));
		//默认值为1
		_vDbWeighted.push_back(TC_Common::strto<int>(g_pconf->get("/taf/multidb/" + vDb[i] + "<weighted>","1")));

		 TC_Mysql *pMysql = new TC_Mysql();
		 pMysql->init(tConf);
		_vpSSDThreadMysql.push_back(pMysql);
	}

	string s;
	if (loadDbRouter(s) != 0)
	{
		throw runtime_error("StatDbManager::initialize error :" + s);
	}

	//读取过滤配置
	vector<string> listFilter = g_pconf->getDomainKey("/taf/filter");
	for (unsigned i = 0; i < listFilter.size(); i++)
	{
		vector<string> vOneFilter = TC_Common::sepstr<string>(listFilter[i], ";", true);
		if (vOneFilter.size() != 1)
		{
			LOG->error() << "StatDbManager config format error. " << TC_Common::tostr(listFilter[i]) << endl;
			return ;
		}
		StatFilterCond fc;
		fc.threshold	 = TC_Common::trim(vOneFilter[0]);
		_vFilterCond.push_back(fc);
	}
	LOG->debug() << "StatDbManager init filter size:" << _vFilterCond.size()  << endl;

	LOG->debug() << "StatDbManager init ok." << endl;
}
StatDbManager::~StatDbManager()
{
	vector<TC_Mysql*>::iterator it = _vpSSDThreadMysql.begin();
	while(it != _vpSSDThreadMysql.end())
	{
		if(*it)
		{
			delete *it;
		}
	}

	map<string,DbInfo*>::const_iterator itm = _mDbMysql.begin();
	while(itm != _mDbMysql.end())
	{
		if(itm->second)
		{
			delete itm->second;
		}
	}

}


///////////////////////////////////////////////////////////
//
bool StatDbManager::IsEnableWeighted()
{
	return _EnableWeighted;
}

void StatDbManager::getDbWeighted(int& iGcd,int& iMaxW,vector<int>& vDbWeighted)
{
	vDbWeighted = _vDbWeighted;

	int gcd = 0;
	int iMax = 0;
	for(size_t i = 0;i < vDbWeighted.size(); i++)
	{
		gcd = getGcd(gcd,vDbWeighted[i]);

		if(vDbWeighted[i] > iMax)
		{
			iMax = vDbWeighted[i];
		}
	}

	iGcd = gcd;

	iMaxW = iMax;
}

int StatDbManager::getGcd (int a, int b)
{
	int c;
	while(a != 0)
	{
		 c = a;
		 a = b%a;
		 b = c;
	}

	return b;
}

int StatDbManager::genRandOrder()
{
	ostringstream os;
	_vRandOrder.clear();
	srand((unsigned)time(NULL));
	int iValue = 0;
	size_t i = 0;
	while(1)
	{
		int iFound = 0;
		iValue =  rand()%(_dbNumber );
		for (i=0; i<_vRandOrder.size(); i++)
		{
			if (iValue == _vRandOrder[i])
			{
				iFound	= 1;
				break;
		    }
		}
		if ( iFound == 0)
		{
		    _vRandOrder.push_back(iValue);
		}
		else
		{
			continue;
		}
		if (_vRandOrder.size() ==	(size_t)_dbNumber  )
		{
			break;
		}
	}

	vector<int>::iterator it = _vRandOrder.begin();
	while(it != _vRandOrder.end() )
	{
	    os << *it++ << " " ;
	}
	LOG->debug() << "randorder: " << os.str() << endl;
	return 0;

}
int StatDbManager::loadDbRouter(string & sResult)
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
				LOG->error() << "[StatDbManager::loadDb ] config format error. " << TC_Common::tostr(vDbRouter[i]) << endl;
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
			LOG->debug() << "[StatDbManager::loadDb succ ] " << TC_Common::tostr(vDbRouter[i]) << endl;
		}

		sResult =  " loadDb ok size ["+TC_Common::tostr(vDbRouter.size())+"]";
		LOG->debug() << "[StatDbManager::loadDb]"<<sResult<<endl;
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
		LOG->error() << "[StatDbManager::loadDb] error:" << string(ex.what()) << endl;
		sResult = "load router config error:" + string(ex.what());
		return  -1;
	}
	return -1;
}


regex_t StatDbManager::initReg(const string& s)
{
	static const int iFlags = REG_EXTENDED | REG_ICASE;
	regex_t reg;
	if (regcomp(&reg,s.c_str(),iFlags))
	{

		throw runtime_error(string("StatDbManager::initReg: regular error:")+s);
	}
	return reg;
}
inline bool StatDbManager::matchReg(const regex_t& regex,const string& value)
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
int StatDbManager::getDbRouter(const string & sName,Db& tDb)
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
		LOG->error() << "[StatDbManager::getDbConfig] error:" << ex.what() << endl;
	}
	return -1;
}

///////////////////////////////////////////////////////////
//

int StatDbManager::insert2Db(const StatMsg &statmsg,const string &sDate,
						 const string &sFlag,const string &sTbNamePre,TC_Mysql *pMysql)
{
	int iCount  = 0;
	ostringstream osSql;
	string strValues;

	string sTbName  = (sTbNamePre !=""?sTbNamePre:_sTbNamePre) + TC_Common::replace(sDate,"-","") + sFlag.substr(0,_eCutType*2);

	try
	{
		for (StatMsg::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
		{
			const StatMicMsgHead& head      = it->first;
			const StatMicMsgBody& body      = it->second;

			string strIntervCount;
			int iTemp = 0;
			for (map<int,int>::const_iterator it = body.intervalCount.begin();it != body.intervalCount.end();it++)
			{
				if (iTemp != 0)
				{
					strIntervCount += ",";
				}
				strIntervCount = strIntervCount + TC_Common::tostr(it->first) + "|" + TC_Common::tostr(it->second);
				iTemp++;
			}

			int iAveTime = 0;
			if ( body.count != 0 )
			{
				iAveTime = body.totalRspTime/body.count;
				iAveTime == 0?iAveTime=1:iAveTime=iAveTime;
			}

			string sVersion;
			string sMasterName      = head.masterName;
			string::size_type pos   =  head.masterName.find("@");
			if (pos != string::npos)
			{
				sMasterName   = head.masterName.substr(0,pos);
				sVersion      = head.masterName.substr(pos+1);
			}

			//组织sql语句
			strValues = " ('" + sDate+ "',"
						+ "'" + sFlag + "',"
						+ "'" + pMysql->escapeString(sMasterName) + "',"
						+ "'" + pMysql->escapeString(head.slaveName) + "',"
						+ "'" + pMysql->escapeString(head.interfaceName) + "',"
						+ "'" + pMysql->escapeString(sVersion) + "',"
						+ "'" + pMysql->escapeString( head.masterIp ) + "',"
						+ "'" + pMysql->escapeString( head.slaveIp ) + "', "
						+ TC_Common::tostr<taf::Int32>( head.slavePort ) + ","
						+ TC_Common::tostr<taf::Int32>( head.returnValue ) + ","
						+ "'" + pMysql->escapeString( head.slaveSetName ) + "',"
						+ "'" + pMysql->escapeString( head.slaveSetArea ) + "',"
						+ "'" + pMysql->escapeString( head.slaveSetID ) + "',"
						+ TC_Common::tostr<taf::Int32>( body.count ) + ","
						+ TC_Common::tostr<taf::Int32>( body.timeoutCount ) + ","
						+ TC_Common::tostr<taf::Int32>( body.execCount ) + ","
						+ TC_Common::tostr<taf::Int64>( body.totalRspTime ) + ","
						+ "'" + strIntervCount + "',"
						+ TC_Common::tostr<taf::Int32>(iAveTime) + ","
						+ TC_Common::tostr<taf::Int32>( body.maxRspTime ) + ","
						+ TC_Common::tostr<taf::Int32>( body.minRspTime ) + " ) ";

			if ( iCount == 0 )
			{
				osSql.str("");
				osSql << "insert ignore into " + sTbName + "  (f_date,f_tflag,master_name,slave_name,interface_name,taf_version,master_ip,slave_ip,slave_port,return_value,set_name, set_area,set_id, succ_count,timeout_count,exce_count,total_time,interv_count,ave_time,maxrsp_time, minrsp_time) values ";
				osSql << strValues;
			}
			else
			{
				osSql << "," + strValues;
			}

			iCount ++;
			if ( iCount >= _iMaxInsertCount )
			{
				usleep(100);
				pMysql->execute(osSql.str());
				LOG->debug() << "insert "+ sTbName+" affected:" << iCount << endl;
				iCount = 0;
			}
		}

		if ( iCount != 0 )
		{
			pMysql->execute(osSql.str());
			LOG->debug() << "insert "+sTbName+" affected:" << iCount << endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		string err = string (ex.what());
		if (std::string::npos == err.find( "Duplicate"))
		{
			creatTable(sTbName,pMysql);
		}
		return 1;
	}
	catch (exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////
//

int StatDbManager::creatTable(const string &sTbName, TC_Mysql *_pstMql)
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



///////////////////////////////////////////////////////////
//

int StatDbManager::creatTb(const string &sTbName,TC_Mysql *pMysql)
{
	try
	{
		string sSql = TC_Common::replace(_sSql, "${TABLE}",sTbName);
		LOG->debug() << sSql << endl;


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


///////////////////////////////////////////////////////////
//

int StatDbManager::creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql)
{
	try
	{
		TC_Mysql::MysqlData tTotalRecord = pMysql->queryRecord("show tables like '%"+sTbName+"%'");
		LOG->debug()<<__FUNCTION__<<"|show tables like '%"+sTbName+"%|affected:"<<tTotalRecord.size()<<endl;
		if (tTotalRecord.size() > 0)
		{
			return 0;
		}
		LOG->debug() << sSql << endl;
		pMysql->execute(sSql);
		return 0;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		return 1;
	}
}


///////////////////////////////////////////////////////////
//

int StatDbManager::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
	try
	{
		string sAppName  = sTbNamePre != ""?sTbNamePre:_sTbNamePre;
		string sCondition = "where  appname='"+sAppName+"' and action =0";
		TC_Mysql::RECORD_DATA rd;
		rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
		int iRet = pMysql->updateRecord("t_ecstatus", rd,sCondition);
		LOG->debug() << "StatDbManager::updateEcsStatus iRet: " <<iRet <<" "<<pMysql->getLastSQL()<< endl;
		if (iRet == 0 )
		{
			rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
			rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(g_app.getInserInterv()));
			rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
			iRet = pMysql->replaceRecord("t_ecstatus", rd);
		}
		if (iRet != 1)
		{
			LOG->debug() << "StatDbManager::updateEcsStatus erro: ret:" << iRet<<" "<<pMysql->getLastSQL()<< endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		string sql = _sSqlStatus;
		LOG->error() << "StatDbManager::updateEcsStatus exception: " << ex.what() << endl;
		creatEscTb("t_ecstatus", sql, pMysql);
		return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////
//

int StatDbManager::insert2MultiDbs(vector<StatMsg> &vStatmsg, const string &sDate, const string &sFlag)
{
	string sIp = ServerConfig::LocalIp;

	string sRandOrder = g_app.getRandOrder();

	string sLastTime  = sDate + " " + sFlag;

	string sTbNamePre = "";
	TC_Mysql * pMysql = NULL;

	int iSize = vStatmsg.size();
	//这里有条件保证iSize  <= _dbNumber
	for (int j=0; j < iSize && j < _dbNumber; j++)
	{
		if (sRandOrder == "")
		{
			sRandOrder = "0";
		}

		int i = (j + TC_Common::strto<int>(sRandOrder)) % _dbNumber;

		sTbNamePre = _vsTbNamePre[i];

		pMysql = _vpSSDThreadMysql[i];

		if (checkLastTime(sLastTime,  sTbNamePre + sIp, pMysql) == 0)  //=0,没有记录表示: 数据库记录的时间，比当前当前时间小
		{
			LOG->debug() << "insert " << sTbNamePre << i <<  " sDate:" << sDate
						 << " sFlag:" << sFlag << " affected:" << vStatmsg[i].size() << endl;

			if ( insert2Db(vStatmsg[i],sDate,sFlag, sTbNamePre, pMysql) != 0)
			{
				if ( insert2Db(vStatmsg[i],sDate,sFlag, sTbNamePre, pMysql) != 0 )
				{
					sendAlarmSMS();
				}
			}

	 		if (updateEcsStatus(sLastTime, sTbNamePre  + sIp , pMysql) != 0)
	 		{
	 			updateEcsStatus(sLastTime, sTbNamePre  + sIp , pMysql);
	 		}
		}
		//删除已经入库的记录
		vStatmsg.at(i).clear();
	}
	return 0;
}

///////////////////////////////////////////////////////////
//

int StatDbManager::sendAlarmSMS()
{
	string errInfo = " ERROR:" + ServerConfig::LocalIp +  ":统计入库失败，请及时处理!";
	TAF_NOTIFY_ERROR(errInfo);

	LOG->error() << "TAF_NOTIFY_ERROR " << errInfo << endl;

	return 0;
}


///////////////////////////////////////////////////////////
//

int StatDbManager::getDbNumber()
{
	return _dbNumber;
}


///////////////////////////////////////////////////////////
//


int StatDbManager::checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
	int iRet = 0;
	try
	{
		string sAppName  = sTbNamePre != ""?sTbNamePre:_sTbNamePre;
		string sCondition = "where  appname='" + sAppName + "' and lasttime >= '" + sLastTime + "'" ;
		iRet = pMysql->getRecordCount("t_ecstatus", sCondition);
		LOG->debug() << "StatDbManager::checkLastTime iRet: " <<iRet <<" "<<pMysql->getLastSQL()<< endl;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "StatDbManager::checkLastTime exception: " << ex.what() << endl;
		creatEscTb("t_ecstatus", _sSqlStatus, pMysql);
		return 0;
	}
	return iRet;
}

/////////////////////////////////////////////////////////
//
bool StatDbManager::filter(const StatMicMsgHead &head,const StatMicMsgBody &body)
{
	try
	{
		TC_LockT<TC_ThreadMutex> lock(*this);

 		Int64 iCount = body.count + body.timeoutCount + body.execCount;
		LOGINFO("StatDbManager::filter count:" << iCount << endl);
		for (size_t i = 0; i < _vFilterCond.size(); i++)
		{
			 if (iCount <  TC_Common::strto<int>(_vFilterCond[i].threshold)) // 小于阀值就被过滤
			 {
				if(LOG->IsNeedLog(TafRollLogger::INFO_LOG))
				{
				 	ostringstream os;
					head.displaySimple(os);
					os << ", body:";
					body.displaySimple(os);
					LOGINFO("StatDbManager::filted :" << os.str() << endl);
					return true;
				}
			 }
		}
	}
	catch (exception &ex)
	{
		LOG->error() << "StatDbManager::filter exception:" << ex.what() << endl;
		return  false;
	}
	return false;
}

