#include "DbProxy.h"
#include "DbThread.h"
#include "util/tc_config.h"
#include "QueryStatServer.h"
#include <time.h>

///////////////////////////////////////////////////////////

extern QueryStatServer g_app;


string dateInc(const string&  sDate);
string tFlagInc(const string& stflag);

void query(int iThread, /*i, 0 ->3*/ const map<string,string>& mSqlPart, map<string, vector<Int64> > &result, string &sRes);


void insert(int iThread, /*i, 0 ->3*/ const map<string,string>& mSqlPart, const string &sData, string &result);

DbProxy::DbProxy()
{
}

DbProxy::~DbProxy()
{
	if(_queryPool)
	{
		_queryPool->stop();
		delete _queryPool;
	}
}

void DbProxy::init()
{
	_queryPool = new TC_ThreadPool();
	_queryPool->init(g_app.getThreadPoolSize());

	_queryPool->start();
	////////////////////

	LOG->debug() << "DbProxy init ok." << endl;

}
string DbProxy::makeResult(int iRet, const string& sRes)
{
	size_t act    = g_app.getActiveDbSize();
	int total     = g_app.getStatDbNumber();
	string result = "Ret:" + TC_Common::tostr(iRet) + "\n"
		   + "ActiveDb:" + TC_Common::tostr(act) + "\n"
		   + "TotalDb:" + TC_Common::tostr(total) + "\n"
		   + sRes;
	return result;
}

int DbProxy::createRespHead(const vector<string> &res, const string& sLasttime , string& result)
{

	int iRet = 0;
	string sRes;

	//检查查询返回值，如果一个线程失败，就返回失败。
	for (size_t i=0; i< res.size(); i++)
	{
		sRes += res[i] ;
		if ( res[i][4] != '0' && iRet == 0)
		{
			iRet = -1;
		}
	}

	int total = g_app.getStatDbNumber();
	result = "Ret:" + TC_Common::tostr(iRet) + "\n" +
		     "lasttime:" + sLasttime + "\n" +
		     "ActiveDb:" + TC_Common::tostr(res.size()) + "\n" +
		     "TotalDb:" + TC_Common::tostr(total) + "\n" +
		     sRes;

	return iRet;
}
int DbProxy::createRespData(const string& sUid, const vector<map<string, vector<Int64> > >& vDataList, const string& sHead,  string &result)
{
	// 组合多线程结果
	//map  first由goupby生成
	//map second 由index生成

	int64_t tStart = TC_TimeProvider::getInstance()->getNowMs();
	vector<map<string, vector<Int64> > >::const_iterator dataItr = vDataList.begin();

	map <string, vector<Int64> > mStatData;
	map <string, vector<Int64> >::iterator _it;

	for(size_t i=0;dataItr != vDataList.end();dataItr++,i++)
	{
		LOGINFO(sUid << "sum["<<i<<"].size"<< ":" << dataItr->size() << endl);
		for (map<string, vector<Int64> >::const_iterator it = dataItr->begin(); it != dataItr->end(); it++)
		{
			_it = mStatData.find(it->first);
			if (_it != mStatData.end())
			{
				const vector<Int64> &number1 = it->second;
				vector<Int64> &number2 = _it->second;
				// 相同key的值 求和,number1和number1的大小是一样的
				for (size_t j=0; j<number1.size(); j++)
				{
					number2[j] +=  number1[j];
				}
			}
			else
			{
				mStatData[it->first] = it->second;
			}
		}
	}

	result += sHead + "linecount:" + TC_Common::tostr(mStatData.size()) + "\n";

	//把 查询结果转换成一行一行的串
	/*
	* input :groupby, f_date, f_tflag
		  * input : index, succ_count, timeout_count
		  *all map <string, vector<Int64> >
		  *string =>>  f_date, f_tflag
		  *vector<Int64>  =>> succ_count, timeout_count
	*/
	_it = mStatData.begin();
	while(_it != mStatData.end())
	{
		string valueBuffer = "";
		vector<Int64>::iterator valueIt = _it->second.begin();
		while(valueIt != _it->second.end()) // value is vector int, need transfer to string;
		{
			valueBuffer += TC_Common::tostr(*valueIt) + ",";
			valueIt++;
		}

		result += _it->first + ",";
		result += valueBuffer + "\n";

		_it++;
	}

	LOGINFO("result:"<<result<<endl);
	int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();
	LOG->debug()<< sUid << "createRespData,size(B)"<< result.length()<<",timecost(ms):" <<(tEnd-tStart)<<endl;
	return 0;

}

int DbProxy::create(map<string, string>& sqlPart, string& result, int bAlter )
{
	string sRes = "";
	int iRet = 0;
	try
	{
		TC_Mysql tcMysql;
		vector<TC_DBConf> vDBConf = g_app.getActiveDbInfo();

		string dbName = sqlPart["dataid"];

		for (size_t iThread=0; iThread < vDBConf.size(); iThread++)
		{

			TC_DBConf tcDbConf = vDBConf[iThread];
			// first connect to mysql to create database
			tcDbConf._database = "mysql";
			tcMysql.init(tcDbConf);

   			string sql = "create database if not exists " + dbName +  ";" ;
   			LOG->debug() << sql << endl;
   			tcMysql.execute(sql);

			tcMysql.disconnect();

			tcDbConf._database = dbName;
   			tcMysql.init(tcDbConf);

			 LOG->debug()<< "iThread" << ":"  << iThread << ", "<< "dataid: " << sqlPart["dataid"] << endl;
			 if (bAlter == 0)
			 {
			 	if (0 != creatTable(sqlPart, &tcMysql ,sRes) &&  iRet == 0)
				{
					iRet = -1;
				}
			 }
			 else
			 {
				if (0 != alterTable(sqlPart, &tcMysql ,sRes) &&  iRet == 0)
				{
					iRet = -1;
				}
			 }
		}
	}
	catch(TC_Mysql_Exception & ex)
	{
		LOG->error()  << ex.what() << endl;
		sRes +=  string("exception ") + ex.what() + "\n";
	}
	catch(exception & ex)
	{
		LOG->error()  << ex.what() << endl;
		sRes +=  string("exception ") + ex.what() + "\n";
	}
	result = makeResult(iRet, sRes);
	return iRet;
}
int DbProxy::creatTable(map<string, string> &sqlPart, TC_Mysql *_pstMql, string&  result)
{
	string daytime = sqlPart["daytime"];
	string sRes    = "";
	int iRet       = 0;
	if (daytime.length() == 8)  // 20110610
	{
		int hour = 0;
		ostringstream	osHour;
		for (hour=0; hour < 24; hour++)
		{
			osHour	<< setw(2) << setfill('0') << hour ;
			string sTbName =  sqlPart["dataid"] + "_" + daytime + osHour.str();
			osHour.str(""); // 清空
			string sql = "CREATE TABLE if not exists `" + sTbName +  "` " + sqlPart["sql_part"] +
				         "DEFAULT CHARSET=gbk";
			if ( 0 != creatTb(sTbName, sql, _pstMql, sRes) && iRet == 0) //失败一次，返回失败
			{
				iRet = -1;
			}
		}
		result =  sRes;
		return iRet;
	}
	string sTbName =  sqlPart["dataid"] + "_" + daytime;
	string sql = "CREATE TABLE `" + sTbName +  "` " + sqlPart["sql_part"] + "DEFAULT CHARSET=gbk";
	iRet = creatTb(sTbName, sql, _pstMql, sRes);
	result = sRes;
	return iRet;
}
int DbProxy::dropTb(map<string, string> &sqlPart, TC_Mysql *_pstMql, string& result)
{
	string sql     = "";
	string daytime = sqlPart["daytime"];
	int iRet       = 0;
	if ( daytime == "null") // 删除db
	{
		sql = "DROP database IF EXISTS " + sqlPart["dataid"] +	";";
		LOG->debug() << sql << endl;
		iRet = mysql_real_query(_pstMql->getMysql(), sql.c_str(), sql.length());
		if (iRet != 0)
		{
			result = string(mysql_error(_pstMql->getMysql()));
		}
		return iRet;
	}

	if (daytime.length() == 8)  // 20110610，删除一天的表
	{
		int hour = 0;
		ostringstream	osHour;
		for (hour=0; hour < 24; hour++)
		{
			osHour	<< setw(2) << setfill('0') << hour ;
			string sTbName =  sqlPart["dataid"] + "_" + daytime + osHour.str();
			osHour.str(""); // 清空流缓存。
			sql = "DROP TABLE IF EXISTS `" + sTbName +  "` ";
			LOG->debug() << sql << endl;
			iRet = mysql_real_query(_pstMql->getMysql(), sql.c_str(), sql.length());
			if (iRet != 0)
			{
				result = string(mysql_error(_pstMql->getMysql()));
			}
		}
		return iRet;
	}
    //删除一个小时的表
	string sTbName =  sqlPart["dataid"] + "_" + daytime;
	sql = "DROP TABLE IF EXISTS " + sTbName +  " ; ";
	LOG->debug() << sql << endl;
	iRet = mysql_real_query(_pstMql->getMysql(), sql.c_str(), sql.length());

	if (iRet != 0)
	{
		result = string(mysql_error(_pstMql->getMysql()));
		LOG->error() << "mysql_real_query:" <<  sql << result << endl;
	}
	return iRet;
}

int DbProxy::creatTb(const string &sTbName, const string& sSql , TC_Mysql *pMysql, string& result)
{
	try
	{
		LOG->debug() << sSql << endl;

		TC_Mysql::MysqlData tTotalRecord = pMysql->queryRecord("show tables like '%"+sTbName+"%'");
		LOG->debug()<<__FUNCTION__<<"|show tables like '%"+sTbName+"%|affected:"<<tTotalRecord.size()<<endl;
		if (tTotalRecord.size() > 0)
		{
			result += "table " +  sTbName + " already exists.\n" ;
			return 0;
		}
		pMysql->execute(sSql);
		return 0;
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		result +=  string("exception: ") + ex.what() + "\n";
		return -1;
	}
}

int DbProxy::deleteTable(map<string, string> sqlPart, string& result)
{
	string sRes = "";
	int iRet    = 0;
	try
	{
		string dbName =  sqlPart["dataid"];
		vector<TC_DBConf> vDBConf = g_app.getActiveDbInfo();

		for (size_t iThread=0; iThread < vDBConf.size(); iThread++)
		{
			TC_Mysql tcMysql;
			TC_DBConf tcDbConf = vDBConf[iThread];
			tcDbConf._database = dbName;

			tcMysql.init(tcDbConf);

			 LOG->debug()<< "iThread" << ":"  << iThread << ", "<< "dataid: " << sqlPart["dataid"] << endl;
			 tcMysql.connect();
			 if ( 0 != dropTb(sqlPart, &tcMysql, sRes) && iRet == 0)
			 {
				iRet = -1;
			 }
		}
	}
	catch(TC_Mysql_Exception & ex)
	{
		LOG->debug()  << ex.what() << endl;
		sRes += string("exception ") +  ex.what();
		iRet = -1;
	}
	catch(exception & ex)
	{
		LOG->debug()  << ex.what() << endl;
		sRes += string("exception ") +  ex.what();
		iRet = -1;
	}
	result = makeResult(iRet, sRes);
	return 0;
}
int DbProxy::alterTable(map<string, string>   sqlPart, TC_Mysql *_pstMql, string&  result)
{
	try
	{
		string daytime = sqlPart["daytime"];
		if (daytime.length() == 8)	// 20110610
		{
			int hour = 0;
			ostringstream	osHour;
			for (hour=0; hour < 24; hour++)
			{
				osHour	<< setw(2) << setfill('0') << hour ;
				string sTbName =  sqlPart["dataid"] + "_" + daytime + osHour.str();
				osHour.str(""); // 清空
				string sql = "ALTER TABLE `" + sTbName +  "` " + sqlPart["sql_part"] ;
			    LOG->debug() << sql << endl;
				_pstMql->execute(sql);
			}
			return 0;
		}
		string sTbName =  sqlPart["dataid"] + "_" + daytime;
		string sql = "ALTER TABLE `" + sTbName +  "` " + sqlPart["sql_part"] ;
		LOG->debug() << sql << endl;
		_pstMql->execute(sql);
	}
	catch (TC_Mysql_Exception& ex)
	{
		LOG->error() << "exception: " << ex.what() << endl;
		result +=  string("exception: ") + ex.what() + "\n";
		return -1;
	}
	return 0;
}

int DbProxy::desc(map<string, string>& mSqlPart,  string&  result)
{
	int iRet    = 0;
	string sRes = "";

	try
	{
		TC_Mysql tcMysql;

		TC_DBConf tcDbConf = g_app.getActiveDbInfo()[0];

		string dbName =  mSqlPart["dataid"];
		tcDbConf._database = dbName;

		tcMysql.init(tcDbConf);


		LOG->debug()<< " DbProxy::desc: "<< "dataid: " << mSqlPart["dataid"] << endl;

		tcMysql.connect();

		string daytime = mSqlPart["daytime"];
		if ( daytime == "null") // 删除db
		{
			sRes = "daytime = null";
			iRet = -1;
		}

		string sTbNames = "";
		if (daytime.length() == 8)  // 20110610
		{
			int hour = 0;
			ostringstream	osHour;
			osHour	<< setw(2) << setfill('0') << hour ;
			sTbNames =  mSqlPart["dataid"] + "_" + daytime + osHour.str();
			osHour.str(""); // 清空流缓存。
		}
		else
		{
			sTbNames =	mSqlPart["dataid"] + "_" + daytime;
		}

		string sql = "show create table " + sTbNames +  " ;";

		LOG->debug() << sql << endl;

        taf::TC_Mysql::MysqlData res = tcMysql.queryRecord(sql);
        LOG->debug()<< sql <<" affected:"<<res.size()<<endl;

		vector<map<string, string> >& data = res.data();

		map<string, string>::iterator it =  data[0].begin();

		sRes = data[0]["Create Table"];
        while( it != data[0].end())
        {
            LOG->debug() << it->first  << " "  << it->second << endl;
			it ++;
        }
	}
	catch(TC_Mysql_Exception & ex)
	{
		LOG->debug()  << ex.what() << endl;
		sRes += string("exception ") +	ex.what();
		iRet = -1;
	}
	catch(exception & ex)
	{
		LOG->debug()  << ex.what() << endl;
		sRes += string("exception ") +	ex.what();
		iRet = -1;
	}
	result = makeResult(iRet, sRes);
	return iRet;
}

string DbProxy::getLastTime(const map<string,string>& mSqlPart,int iThreadId)
{
	string sUid	  = mSqlPart.find("uid")->second;

	string vsTbNamePre = mSqlPart.find("dataid")->second + "_";

	string min = "99999999999"; // 求最小的，初始使用很大的数据
	string tmp = "";

	vector<TC_DBConf> vDbInfo = g_app.getActiveDbInfo();

	int iThreads = g_app.getActiveDbSize();

	for (int i=0; i< iThreads; i++)
	{
		tmp = selectLastTime(sUid,i,vsTbNamePre,vDbInfo[i]);
		if (tmp < min)
		{
			min = tmp;
		}
	}

	LOG->debug() << sUid << "final lasttime:" << min << endl;
	return min;

}

int DbProxy::updateLastTime(const map<string, string>& mSqlPart)
{
	vector<TC_DBConf> vDbInfo = g_app.getActiveDbInfo();

	string dbName = mSqlPart.find("dataid")->second;

	string vsTbNamePre   = dbName + "_";

	int thread_number = g_app.getActiveDbSize();

	for (int i=0; i< thread_number; i++)
	{
		TC_Mysql tcMysql;

		TC_DBConf tcDbConf = vDbInfo[i];

		tcDbConf._database = dbName;

		tcMysql.init(tcDbConf);
		tcMysql.connect();

		if ( 0 != updateEcsStatus(mSqlPart.find("lasttime")->second, vsTbNamePre, &tcMysql))
		{
			updateEcsStatus(mSqlPart.find("lasttime")->second, vsTbNamePre, &tcMysql);
		}
	}
	return 0;
}

/**
 * 通过线程池进行并发查询
 */
void DbProxy::startQueryThreadPool(const map<string,string> &mSqlPart, string &sResult)
{
	typedef TC_Functor<void, TL::TLMaker<int, const map<string,string>&, map<string,vector<Int64> >&, string&>::Result> QueryFun;
	typedef QueryFun::wrapper_type QueryFunWrapper;

	try
	{
		string sUid = mSqlPart.find("uid")->second;

		int iThreads = g_app.getActiveDbSize();

		vector<string> res(iThreads);

		vector<map<string, vector<Int64> > >  vDataList(iThreads);

		QueryFun qeryCMD(query);

		int64_t tStart	= TC_TimeProvider::getInstance()->getNowMs();
		for(int i=0; i<iThreads; i++)
		{
			LOG->debug()  << sUid << "add iThread:" << i << endl;
			QueryFunWrapper fwrapper(qeryCMD, i, mSqlPart, vDataList[i], res[i]);

			_queryPool->exec(fwrapper);
		}

		//等待线程结束
		LOG->debug()  << sUid << "waitForAllDone..." << endl;
		bool b = _queryPool->waitForAllDone(-1); // wait all
		int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();
		LOG->debug()  << sUid << "waitForAllDone..." << b  <<", JobNum:" << _queryPool->getJobNum() << " ThreadNum:" << _queryPool->getThreadNum()<<",timecost(ms):" <<(tEnd-tStart)<<endl;

		// 返回ret code
		string sHead;

		string sLasttime = getLastTime(mSqlPart,0);

		if(createRespHead(res, sLasttime, sHead) != 0)
		{
			sResult = sHead;
			LOG->error() << "DbProxy::startQueryThreadPool query error:" << sHead << endl;
			return;
		}

		createRespData(sUid, vDataList,sHead, sResult);
	}
	catch (exception &ex)
	{
		sResult ="Ret:-1\n" + string(ex.what()) + "\n";
		LOG->error() << "DbProxy::startQueryThreadPool exception:" << ex.what() << endl;
	}
}

int DbProxy::updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql)
{
	string result = "";
	try
	{
	    TC_Mysql::RECORD_DATA rd;
		string sAppName   = sTbNamePre;
		string sCondition = "where  appname='"+sAppName+"' and action =0 and lasttime < '" + sLastTime+"'";

		rd["lasttime"]    = make_pair(TC_Mysql::DB_STR, sLastTime);

		int iRet          = pMysql->updateRecord("t_ecstatus", rd, sCondition);
		LOG->debug() << "DbProxy::updateEcsStatus iRet: " <<iRet <<"\n"<<pMysql->getLastSQL()<< endl;
		if (iRet == 0 )
		{
			rd["appname"]       = make_pair(TC_Mysql::DB_STR, sAppName);
			rd["checkint"]      = make_pair(TC_Mysql::DB_INT, TC_Common::tostr(10));
			rd["lasttime"]      = make_pair(TC_Mysql::DB_STR, sLastTime);
			iRet = pMysql->replaceRecord("t_ecstatus", rd);
		}
		if (iRet != 1)
		{
			LOG->error() << "DbProxy::updateEcsStatus erro: ret:" << iRet<<"\n"<<pMysql->getLastSQL()<< endl;
		}
	}
	catch (TC_Mysql_Exception& ex)
	{
		string sql = "CREATE TABLE `t_ecstatus` ( "
		"  `id` int(11) NOT NULL auto_increment, "
		"  `appname` varchar(64) NOT NULL default '', "
		"  `action` tinyint(4) NOT NULL default '0', "
		"  `checkint` smallint(6) NOT NULL default '10', "
		"  `lasttime` varchar(16) NOT NULL default '', "
		"   PRIMARY KEY  (`appname`,`action`), "
		"   UNIQUE KEY `id` (`id`) "
		" ) ENGINE=HEAP DEFAULT CHARSET=gbk";
		LOG->error() << "DbProxy::updateEcsStatus exception: " << ex.what() << endl;
		creatTb("t_ecstatus", sql, pMysql, result);
		return 1;
	}
	return 0;
}

void DbProxy::StartInsertThreadPool(const map<string,string> mSqlPart, const vector<string>& vdata, string &result)
{
	typedef TC_Functor<void, TL::TLMaker<int, const map<string, string>&,const string&, string &>::Result> InsertFun;
	typedef InsertFun::wrapper_type InsertFunWrapper;

	try
	{
		int iThreads = g_app.getActiveDbSize();
	    TC_ThreadPool tpool;
		tpool.init(iThreads);
		tpool.start();

		vector<string>  res(iThreads);

		InsertFun cmd(insert);

		for(int i=0; i<iThreads; i++)
		{
			LOG->debug() << "start thread:" << i << endl;

			InsertFunWrapper fwrapper(cmd, i, mSqlPart,vdata[i], res[i]);
			tpool.exec(fwrapper);
		}

		//等待线程结束
		LOG->debug() << "waitForAllDone..." << endl;
		bool b = tpool.waitForAllDone(-1); // wait all
		LOG->debug() << "waitForAllDone...b " << b << endl;

		int iRet		  = 0;
		int affected_rows = 0;

		string sRes = "";
		// 组合多线程结果
		for (int i=0; i< iThreads; i++)
		{
			sRes += res[i] + "\n";
			if ( res[i][4] != '0'  && iRet == 0)
			{
				iRet = -1;
			}
			string::size_type pos = res[i].find("affected_rows");
			if (pos != string::npos)
			{
				affected_rows += TC_Common::strto<int>( res[i].substr(pos + strlen("affected_rows:")));
			}
		}
		string sResult = makeResult(iRet, sRes);

		result = "affected_rows:" + TC_Common::tostr(affected_rows) + "\n" + sResult;

		LOG->debug() << result << endl;

		updateLastTime(mSqlPart);

	}
	catch (exception &ex)
	{
		LOG->error()<< ex.what() << endl;
	}
}


void query(int iThread, /*i, 0 ->3*/ const map<string,string>& mSqlPart, map<string, vector<Int64> > &result, string &sRes)
{
	string dateFrom   = "";
	string dateTo     = "";
	string tflagFrom  = "";
	string tflagTo    = "";
	string whereCond  = "";
	string groupCond  = "";
	string groupField = "";
	string selectCond = "";
	string sumField   = "";

	string sUid       = "";
	string ignoreKey  = " ";

	int64_t tStart = TC_TimeProvider::getInstance()->getNowMs();
	try
	{
		map<string, string> mTmpSqlart = mSqlPart;

		string sDbName = mTmpSqlart["dataid"];

        TC_Mysql tcMysql;

		TC_DBConf tcDbConf = g_app.getActiveDbInfo()[iThread];

		tcDbConf._database = sDbName;

		tcMysql.init(tcDbConf);

	    sUid = mTmpSqlart["uid"];
		LOG->debug() << sUid << "iThread:"  << iThread << endl;

		//dateFrom =>> 20111120
	  	dateFrom  = mTmpSqlart["date1"];
	  	dateTo    = mTmpSqlart["date2"];

		//tflagFrom =>> 2360
	  	tflagFrom = mTmpSqlart["tflag1"];
	  	tflagTo   = mTmpSqlart["tflag2"];

		// 输入tflag 条件检查
		if (tflagFrom.length()            != 4     ||
			tflagTo.length()              != 4     ||
			TC_Common::isdigit(tflagFrom) == false ||
			TC_Common::isdigit(tflagTo)   == false)
		{
		    sRes += "ret:-1 iDb:" + TC_Common::tostr(iThread) + " wrong tflag:" + tflagFrom + "|" + tflagTo +  "\n";
			LOG->error() << sUid << sRes << endl;
			return ;
		}

		//groupCond =>> "where slave_name like 'MTTsh2.BrokerServer' and f_tflag >='0000'  and f_tflag <='2360'  and f_date = '20111120'"
	  	whereCond = mTmpSqlart["whereCond"];

		//groupCond =>> "group by f_date, f_tflag"
	  	groupCond = mTmpSqlart["groupCond"];

		//sumField    =>> "succ_count, timeout_count";
	  	sumField  = mTmpSqlart["sumField"];

		//groupField  =>> "f_date, f_tflag"
	  	groupField = mTmpSqlart["groupField"];

        //selectCond =>> "succ_count, timeout_count, f_date, f_tflag"
		selectCond = sumField +"," + groupField;

		//日期格式20111019
		string::size_type pos = string::npos;
		string tmpDate = "f_date";
		if ((pos = selectCond.find(tmpDate, 0)) != string::npos)
		{
		    selectCond.replace(pos, tmpDate.length(), "DATE_FORMAT( f_date, '%Y%m%d') as f_date");
		}

		//不使用主键
		if ( sDbName == "taf")
		{
            ignoreKey = "  IGNORE INDEX ( PRIMARY ) ";
		}

		vector<string> vGroupField = TC_Common::sepstr<string>(groupField, ", ");
		vector<string> vSumField = TC_Common::sepstr<string>(sumField, ", ");
		string sTbNamePre = sDbName + "_";

		/* the result is as following format:
			+--------------------+-----------------+-----------------+-----------------+---------+
			| sum(timeout_count) | sum(succ_count) | sum(total_time) | sum(exce_count) | f_tflag |
			+--------------------+-----------------+-----------------+-----------------+---------+
			|				   0 |				 6 |			1477 |				 0 | 1410	 |
			|				   0 |				 8 |			2861 |				 0 | 1420	 |
			|				   0 |			   618 |		  102669 |				 0 | 1440	 |
			|				   0 |				 1 |			 161 |				 0 | 1450	 |
			+--------------------+-----------------+-----------------+-----------------+---------+
		*/
		//select range by f_date and f_tflag
		for(string day = dateFrom; day <= dateTo ; day = dateInc(day))
		{
			for(string tflag = tflagFrom; tflag <= tflagTo && (tflag.substr(0,2) < "24");tflag = tFlagInc(tflag))
			{
				//table name:taf_2012060723
				string sTbName = sTbNamePre + day + tflag.substr(0,2);

			    string sSql = "select " + selectCond + " from " + sTbName + " " + ignoreKey  + whereCond  + groupCond + " order by null;";

				LOGINFO(sUid <<"sSql:" << sSql << endl);

				//int64_t ttStart = TC_TimeProvider::getInstance()->getNowMs();
			    taf::TC_Mysql::MysqlData res = tcMysql.queryRecord(sSql);
				//LOG->debug() << sUid << " query tflag:" << tflag <<",timecost(ms):" <<(TC_TimeProvider::getInstance()->getNowMs()-ttStart)<<endl;

			    LOGINFO(sUid << "res.size:" << res.size() << endl);

				// result is key:value pair;
				//sKey 由groupby生成
				//value由index生成
				//int64_t t2Start = TC_TimeProvider::getInstance()->getNowMs();
				for(size_t	iRow = 0; iRow<res.size(); iRow++)
				{
					string sKey = "";
					for(size_t j=0; j<vGroupField.size(); j++)
					{
						sKey += sKey.empty()?"":",";
						sKey += res[iRow][vGroupField[j]];
					}

					map<string,vector<Int64> >::iterator itResult = result.find(sKey);
					if (itResult != result.end())
					{
						vector<Int64>& data = itResult->second;
						for (size_t j=0; j< vSumField.size() && j< data.size(); j++)
						{
							data[j] += TC_Common::strto<Int64>(res[iRow][vSumField[j]]);// 相同key的值 求和
						}
					}
					else
					{
						vector<Int64>& vRes = result[sKey];
						for(size_t j=0; j<vSumField.size(); j++)
						{
							vRes.push_back( TC_Common::strto<Int64>(res[iRow][vSumField[j]]));;
						}
					}
					LOGINFO("query iThread:" << iThread <<" {"<< sKey << ":" << TC_Common::tostr(result[sKey])<<"}"<< endl);
				}

				//LOG->debug() << sUid << "res.size(B):" << res.size()<< " proccess tflag:" << tflag <<",timecost(ms):" <<(TC_TimeProvider::getInstance()->getNowMs()-t2Start)<<endl;
				LOGINFO("query iThread :" << iThread << " day:"<<day<<" tflag:" << tflag << endl);
			}
		}  //day

		sRes =  "ret:0 iDb:" + TC_Common::tostr(iThread)  + "\n";
	}
	catch(TC_Mysql_Exception & ex)
	{
		sRes = "ret:-1 iDb:" + TC_Common::tostr(iThread) + string(" exception ") + ex.what() + "\n";
        LOG->error() << sUid <<"query: " << sRes << endl;
	}
	catch(exception & ex)
	{
		sRes =  "ret:-1 iDb:" + TC_Common::tostr(iThread)  +string(" exception ") + ex.what() + "\n";
        LOG->error() << sUid <<"query: " << sRes << endl;
	}
	int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();
	LOG->debug() << sUid << "exit query iThread :" << iThread <<",timecost(ms):" <<(tEnd-tStart)<<" res:"<<sRes<<endl;
}


string tFlagInc(const string& stflag)
{
	LOGINFO("tFlagInc tFlag:" << stflag << endl);
	int h = TC_Common::strto<int>(stflag.substr(0,2));
	int m = TC_Common::strto<int>(stflag.substr(2,2));

	h += 1;
	char buf[5];
	snprintf(buf,sizeof(buf),"%.2d%.2d",h,m);
	LOGINFO("tFlagInc return tFlag:" << string(buf)<< endl);

	return string(buf);
}

string dateInc(const string& sDate)
{
	// date :20110705
	LOGINFO("dateInc date:" << sDate << endl);

	struct tm *p;
	time_t timep;
	string ret   = "20991231"; // 返回大数
	struct tm tt = {0};
	int day      = 0;
	int mon      = 0;
	int year     = 0;
	try
	{
		year = TC_Common::strto<int>(sDate.substr(0, 4));
		mon  = TC_Common::strto<int>(sDate.substr(4, 2));
		day  = TC_Common::strto<int>(sDate.substr(6, 2));

		time(&timep);
		p=localtime_r(&timep, &tt);
		p->tm_mon  = mon -1;
		p->tm_mday = day +1;
		p->tm_year = year -1900 ;
		timep = mktime(p);
		ret = TC_Common::tm2str(timep, "%Y%m%d");
		LOGINFO("dateInc return date:" << ret << endl);
		return ret;
	}
	catch(exception & ex)
	{
		LOG->error() << string("exception ") +	ex.what();
		return ret;
	}
}
void DbProxy::getTimeInfo(time_t tTime,string &sDate,string &sFlag)
{
    string sTime,sHour,sMinute;
    time_t t    = tTime;
	int Interval = g_app.getInsertInterval();
    t           = (t/(Interval*60))*Interval*60; //要求必须为loadIntev整数倍
    t           = (t%3600 == 0?t-60:t);                           //要求将9点写作0860
    sTime       = TC_Common::tm2str(t,"%Y%m%d%H%M");
    sDate       = sTime.substr(0,8);
    sHour       = sTime.substr(8,2);
    sMinute     = sTime.substr(10,2);
    sFlag       = sHour +  (sMinute=="59"?"60":sMinute);    //要求将9点写作0860
}

string DbProxy::selectLastTime(const string& sUid, int iThread , const string& tbname, const TC_DBConf& tcDbInfo)
{
	try
	{
		TC_Mysql tcMysql;

		TC_DBConf tcDbConf = tcDbInfo;

		tcDbConf._database = TC_Common::trimright(tbname, "_");

		tcMysql.init(tcDbConf);

		int interval      = g_app.getInsertInterval();
		time_t now    = TC_TimeProvider::getInstance()->getNow();

		string sDate,sFlag;
		getTimeInfo((now - interval * 60 * 2), sDate, sFlag); // 排除历史过期数据

		string sLast = sDate + " " + sFlag;

		string sSql = "select min(lasttime) as lasttime  from t_ecstatus where appname like '" + tbname + "%' and lasttime > '" + sLast + "'" ;

		taf::TC_Mysql::MysqlData res = tcMysql.queryRecord(sSql);

		LOGINFO( sUid << "sSql:" << sSql << "lasttime:" <<  res[0]["lasttime"] << endl);

		if (res.size() > 0)
		{
			return res[0]["lasttime"];
		}
		return "";
	}
	catch(TC_Mysql_Exception & ex)
	{
		LOG->error()  <<"selectLastTime: sUid="<< sUid <<" "<< ex.what() << endl;
	}
	catch(exception& e)
	{
		LOG->error()  <<"selectLastTime: sUid="<< sUid <<" "<< e.what() << endl;
	}
	return "";
}


void insert(int iThread, /*i, 0 ->3*/ const map<string,string>& mSqlPart, const string &sData, string &result)
{
	map<string, string> mTmpSqlPart = mSqlPart;

	LOG->debug()<< "iThread" << ":" << iThread << ", "<< "dataid: "  << mTmpSqlPart["dataid"] << " sData.size:" << sData.size()<< endl;

	TC_Mysql tcMysql;


	TC_DBConf tcDbConf = g_app.getActiveDbInfo()[iThread];

	string dbName = mTmpSqlPart["dataid"];
	tcDbConf._database = dbName;

	tcMysql.init(tcDbConf);

	string sTbName = dbName + "_" + mTmpSqlPart["daytime"];

	string sDataValue = sData;

	string sErr = "";
	string sRes = "";

	size_t affected_rows = 0;

	int iRet = -1;
	int iExcTrytime = 1;
	int iConTryTime = 1;

AGAIN:
	try
	{
		tcMysql.connect();

	}catch(TC_Mysql_Exception& ex)
	{
		LOG->error()<< "connect to db:" << iThread <<" failure!!!" << endl;
		//连接只尝试两次
		if (iConTryTime == 1)
		{
			iConTryTime = 0;
			goto AGAIN;
		}
	}

	MYSQL *_pstMql = tcMysql.getMysql();

	string sSql = "replace " + sTbName + " values " + sDataValue + ";";
	if(_pstMql)
	{
		iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
		if(iRet != 0)
		{
			//再尝试一次
			iRet = mysql_real_query(_pstMql, sSql.c_str(), sSql.length());
		}

		if (iRet != 0)
		{
			sErr = "mysql_real_query:  " + string(mysql_error(_pstMql));

			sRes += sErr + "\n";

			LOG->error()<< sErr << " \nsql:" << sSql << endl;
			if (_pstMql != NULL)
			{
				mysql_close(_pstMql);
				_pstMql = NULL;
			}

			if (iExcTrytime == 1)
			{
				iExcTrytime = 0; // 清除重试标志
				goto AGAIN;
			}
		}
		else
		{
			affected_rows = mysql_affected_rows(_pstMql);
		}
	}

	result = "ret:" + TC_Common::tostr(iRet) + " affected_rows:" + TC_Common::tostr(affected_rows) +  sRes;
	if (_pstMql  != NULL)
	{
        mysql_close(_pstMql);
		_pstMql = NULL;
	}
}


