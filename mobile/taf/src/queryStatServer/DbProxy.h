#ifndef __DB_PROXY_H_
#define __DB_PROXY_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include <regex.h>

using namespace taf;

class DbProxy
{
public:
    /**
     *
     */
    DbProxy();
    ~DbProxy();
	void init();
public:
	void startQueryThreadPool(const map<string, string>& mSqlPart, string &sResult);

	void StartInsertThreadPool(const map<string,string> mSqlPart, const vector<string>& vdata, string &result);

	int create(map<string, string>& mSqlPart, string& result, int bAlter = 0);

	int deleteTable(map<string, string>   sqlPart, string& result);

	string getLastTime(const map<string,string>& mSqlPart,int iThreadId = 0);

	int desc(map<string, string>&	mSqlPart,  string&  result);
private:
	string makeResult(int iRet, const string& sRes);

	int createRespHead(const vector<string> &res, const string& sLasttime ,string& result);

	int createRespData(const string& sUid,const vector<map<string,vector<Int64> > >  &vDataList, const string& sHead,  string &result);

    string makeQueryResult(int iRet, string sLasttime , string& sRes);

    string selectLastTime(const string& sUid, int iThread , const string& tbname, const TC_DBConf& tcDbInfo);

	int dropTb(map<string, string>& sqlPart, TC_Mysql *_pstMql, string& result);

	int alter(map<string, string>   sqlPart);

	int alterTable(map<string, string>   sqlPart, TC_Mysql *_pstMql, string&  result);

	int creatTable(map<string, string>& sqlPart, TC_Mysql *_pstMql, string& result);

    int creatTb(const string &strTbName, const string& sql, TC_Mysql *pMysql , string& result);

	int updateLastTime(const map<string,string>& mSqlPart);

	int updateEcsStatus(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql);

	void getTimeInfo(time_t tTime,string &sDate,string &sFlag);
private:
	TC_ThreadPool *_queryPool;
};


#endif


