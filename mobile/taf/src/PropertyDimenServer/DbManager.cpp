#include "DbManager.h"
#include "ReapThread.h"
#include "util/tc_config.h"
#include "PropertyDimenServer.h"
#include "PropertyDimenHashMap.h"
///////////////////////////////////////////////////////////
//
extern TC_Config* g_pconf;
////////////////////
PropertyDimenDbManager::PropertyDimenDbManager()
{
	LOG->debug() << "PropertyDimenDbManager init begin." << endl;
	_sSql               = g_pconf->get("/taf<sql>", "");
	_sSqlStatus         = g_pconf->get("/taf<sqlStatus>", "");
	_sTbNamePre         = g_pconf->get("/taf/db<tbnamePre>","taf_property_dimen_");
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

	_eCutType = CUT_BY_HOUR;

	//初始化配置db连接
	// 均匀分布在多个数据库
	vector<string> vDb =g_pconf->getDomainVector("/taf/multidb");
	dbNumber = vDb.size();
	
	LOG->debug() << "PropertyDimenDbManager init multidb size:" <<dbNumber  << endl;
	for (int i=0; i< dbNumber; i++)
	{
		TC_DBConf tConf;
		tConf.loadFromMap(g_pconf->getDomainMap("/taf/multidb/" + vDb[i]));
		_vsTbNamePre.push_back(g_pconf->get("/taf/multidb/" + vDb[i] + "<tbname>",
			"t_propert_dimen_"));
		 TC_Mysql *_p = new TC_Mysql();
		 _p->init(tConf);
		_vpMysql.push_back(_p);
	}


    //读取过滤配置
	vector<string> listFilter = g_pconf->getDomainKey("/taf/filter");
	for (unsigned i = 0; i < listFilter.size(); i++)
	{
		vector<string> vOneFilter = TC_Common::sepstr<string>(listFilter[i], ";", true);
		if (vOneFilter.size() != 4)
		{
			LOG->error() << "PropertyDimenDbManager config format error. " << TC_Common::tostr(listFilter[i]) << endl;
			return ;
		}
        filterCond fc;
		fc.serverName      = TC_Common::trim(vOneFilter[0]);
		fc.propertyName    = TC_Common::trim(vOneFilter[1]);
		fc.policyName      = TC_Common::trim(vOneFilter[2]);
		fc.threshold       = TC_Common::trim(vOneFilter[3]);
		_vFilterCond.push_back(fc);
	}
	LOG->debug() << "PropertyDimenDbManager init filter size:" << _vFilterCond.size()  << endl;

	LOG->debug() << "PropertyDimenDbManager init ok." << endl;
}
///////////////////////////////////////////////////////////
//
int PropertyDimenDbManager::insert2Db( const map<PropDimenHead,ReportPropMsgBody>& statmsg, const string &strDate,const string &strFlag ,const string &sTbNamePre ,TC_Mysql *pMysql )
{
	int iCount = 0;
	ostringstream osSql;
	string strValues;

	string strTbName   = (sTbNamePre !=""?sTbNamePre:_sTbNamePre) + TC_Common::replace(strDate,"-","") + strFlag.substr(0,_eCutType*2);
	try
	{
		for(map<PropDimenHead,PropBody>::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
		{								
			const PropDimenHead& head = it->first;
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
					+ "'" + pMysql->escapeString(head.dimension1) + "',"
					+ "'" + pMysql->escapeString(head.dimension2) + "',"
					+ "'" + pMysql->escapeString(head.dimension3) + "',"
					+ "'" + pMysql->escapeString(head.dimension4) + "',"
					+ "'" + pMysql->escapeString(head.dimension5) + "',"
					+ "'" + pMysql->escapeString(head.dimension6) + "',"
					+ "'" + pMysql->escapeString(head.dimension7) + "',"
					+ "'" + TC_Common::tostr( body.vInfo[i].policy ) + "',"
					+ "'" + TC_Common::tostr( body.vInfo[i].value ) + "') ";
	
				if ( iCount == 0 )
				{
					osSql.str("");
					osSql << "insert ignore into " + strTbName + "  (f_date,f_tflag,master_name,master_ip,property_name,"
						"dimension1,dimension2,dimension3,dimension4,dimension5,dimension6,dimension7,policy,value) values ";
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
int PropertyDimenDbManager::getDbNumber()
{
	return dbNumber;
}


int PropertyDimenDbManager::insert2MultiDbs(vector<map<PropDimenHead,PropBody > > &statmsg, const string &sDate, const string &sFlag)
{
	startInsertThreadPool(sDate,  sFlag,  statmsg);
    return 0;

}
void ConInsert(int iThread, string sDate, string sFlag, map<PropDimenHead,PropBody >& msg)
{

	int iCheck = 0;
	string sIp = ServerConfig::LocalIp;
	string sLastTime = sDate+" "+sFlag;
	TC_Mysql * pMysql = PropertyDimenDbManager::getInstance()->getDbInfo(iThread);
	string sTbName = PropertyDimenDbManager::getInstance()->getTbName(iThread);


	iCheck = PropertyDimenDbManager::getInstance()->checkLastTime (sLastTime, sTbName	+ sIp, pMysql, iThread);
	if (iCheck == 0)  //=0,没有记录表示: 数据库记录的时间，比当前当前时间小
	{
		LOG->debug() << "insert " + sTbName + TC_Common::tostr(iThread) + " sDate:"+ sDate + 
		" sFlag:" +sFlag+ " affected:" <<msg.size() << endl;

		if (PropertyDimenDbManager::getInstance()->insert2Db(msg,sDate,sFlag, sTbName, pMysql) != 0)
		{
			PropertyDimenDbManager::getInstance()->insert2Db(msg,sDate,sFlag, sTbName, pMysql);
		}
		if (PropertyDimenDbManager::getInstance()->updateEcsStatus(sLastTime, sTbName	+ sIp, pMysql) != 0)
		{
			PropertyDimenDbManager::getInstance()->updateEcsStatus(sLastTime, sTbName	+ sIp, pMysql);
		}
	}
	
	msg.clear();
	return ;
}

///////////////////////////////////////////////////////////
//
void PropertyDimenDbManager::startInsertThreadPool(string sDate, string sFlag, vector<map<PropDimenHead,PropBody > > &statmsg)
{
	try
	{
	    int iThreads      = 0;
		
	    TC_ThreadPool tpool;
		
		iThreads = statmsg.size();
		tpool.init(iThreads + 1);
		tpool.start();

		//定义仿函数
		typedef void (*TpMem)(int, string , string , map<PropDimenHead, PropBody >&);
		TC_Functor<void, TL::TLMaker<int, string , string, map<PropDimenHead,PropBody > & >::Result> cmd(static_cast<TpMem>(&ConInsert));

        //添加对象到线程池执行。

		for(int i=0; i<iThreads; i++)
		{
			LOG->debug()  <<  "start thread:" << i << endl;
			TC_Functor<void, TL::TLMaker<int, string , string, map<PropDimenHead,PropBody > &>::Result>::wrapper_type fwrapper(cmd, i,  sDate, sFlag,statmsg[i]);
			tpool.exec(fwrapper);
		}

		//等待线程结束
		LOG->debug()  << "waitForAllDone..." << endl;
		bool b = tpool.waitForAllDone(-1); // wait all
		LOG->debug()  <<"waitForAllDone..." << b  <<", JobNum:" << tpool.getJobNum() << " ThreadNum:" << tpool.getThreadNum()<< endl;

	}
	catch (exception &ex)
	{
		LOG->error() << "DbProxy::startQueryThreadPool exception:" << ex.what() << endl;
	}
}


///////////////////////////////////////////////////////////
//
int PropertyDimenDbManager::creatTable(const string &sTbName, TC_Mysql *_pstMql)
{
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
int PropertyDimenDbManager::creatTb(const string &sTbName,TC_Mysql *pMysql)
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
int PropertyDimenDbManager::creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql)
{ 
	try
	{
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
int PropertyDimenDbManager::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
	try
	{
		string sAppName  = sTbNamePre != ""?sTbNamePre:_sTbNamePre;
		string sCondition = "where  appname='"+sAppName+"' and action =0";
		TC_Mysql::RECORD_DATA rd;
		rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
		int iRet = pMysql->updateRecord("t_ecstatus", rd,sCondition);
		LOG->debug() << "PropertyDimenDbManager::updateEcsStatus iRet: " <<iRet <<"\n"<<pMysql->getLastSQL()<< endl;
		if (iRet == 0 )
		{
			rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
			rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(PropertyDimenServer::g_iInsertInterval));
			rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
			iRet = pMysql->replaceRecord("t_ecstatus", rd);
		}
		if (iRet != 1)
		{
			LOG->debug() << "PropertyDimenDbManager::updateEcsStatus erro: ret:" << iRet<<"\n"<<pMysql->getLastSQL()<< endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "PropertyDimenDbManager::updateEcsStatus exception: " << ex.what() << endl;
		creatEscTb("t_ecstatus", _sSqlStatus, pMysql);
		return 1;
	}
	return 0;
}
///////////////////////////////////////////////////////////
//
int PropertyDimenDbManager::checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql, int seq)
{
	int iRet = 0;
	try
	{
		string sAppName  = sTbNamePre != ""?sTbNamePre:_sTbNamePre;
		string sCondition = "where  appname='" + sAppName + "' and lasttime >= '" + sLastTime + "'" ;
		iRet = pMysql->getRecordCount("t_ecstatus", sCondition);
		LOG->debug() << "PropertyDimenDbManager::checkLastTime seq:" << seq<< " iRet: " <<iRet <<" "<<pMysql->getLastSQL()<< endl;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "PropertyDimenDbManager::checkLastTime exception: " << ex.what() << endl;
		creatEscTb("t_ecstatus", _sSqlStatus, pMysql);
		return 0;
	}
	return iRet;
}

/////////////////////////////////////////////////////////
// 
bool PropertyDimenDbManager::filter(const PropDimenHead &head, PropBody &body)
{
	try
	{
		TC_LockT<TC_ThreadMutex> lock(*this);
        ostringstream os;
		
        os.str("");
		head.displaySimple(os);
		os << ", body:";
		body.displaySimple(os);
		LOG->info() << "PropertyDimenDbManager::filter start:" << os.str() << endl;
		for (size_t i = 0; i < _vFilterCond.size(); i++)
		{
			if ((head.moduleName == _vFilterCond[i].serverName || "*" == _vFilterCond[i].serverName)
            	&& head.propertyName == _vFilterCond[i].propertyName)
			{
				vector<taf::ReportPropInfo>::iterator it = body.vInfo.begin();
				while (it != body.vInfo.end())
				{
					if (it->policy == _vFilterCond[i].policyName
						&& TC_Common::strto<int>(it->value) <=  TC_Common::strto<int>(_vFilterCond[i].threshold))
					{
						//body.vInfo.erase(it); //vector erase 后，it自动++
						body.vInfo.clear();
						break ;
					}
					it++;
				}
				break;
			}
		} 
	
        os.str("");
		head.displaySimple(os);
		os << ", body:";
		body.displaySimple(os);
		LOG->info() << "PropertyDimenDbManager::filter resul:" << os.str() << endl;
	}
	catch (exception &ex)
	{
		LOG->error() << "PropertyDimenDbManager::filter exception:" << ex.what() << endl;
		return  false;
	}

	return true;
}

/////////////////////////////////////////////////////////
// 


