#ifndef __DB_MANAGER_H_
#define __DB_MANAGER_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/PropertyF.h"
#include "PropertyDB.h"
#include "PropertyDimenHashMap.h"
#include <regex.h>

using namespace taf;

struct filterCond
{
    filterCond()
    {
    
    };

    ~filterCond()
    {

    };

    std::string serverName;
	std::string propertyName;
	std::string policyName;
	std::string threshold;
};

class PropertyDimenDbManager : public TC_Singleton<PropertyDimenDbManager>,public TC_ThreadMutex 
{
public:
    enum CutType
    {
        CUT_BY_DAY      = 0,
        CUT_BY_HOUR     = 1,
        CUT_BY_MINUTE   = 2,
    };

public:
    /**
     *
     */
    PropertyDimenDbManager();
    ~PropertyDimenDbManager(){}

public:
	int creatTable(const string &sTbName, TC_Mysql *_pstMql = NULL);

    int creatTb(const string &strTbName,TC_Mysql *pMysql = NULL);

    int insert2Db(const map<PropDimenHead,PropBody>& statmsg,const string &sDate,const string &sFlag,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int updateEcsStatus(const string &sLastTime,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);
	
	int checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql, int seq);
	
	int creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql = NULL);

    int insert2MultiDbs(vector<map<PropDimenHead,PropBody > > &statmsg,const string &sDate, const string &sFlag);
	
	void startInsertThreadPool(string sDate, string sFlag, vector<map<PropDimenHead,PropBody > > &statmsg);
	int	getDbNumber();
	TC_Mysql * getDbInfo(int i) { return  _vpMysql[i]; };
	string getTbName(int i) { return _vsTbNamePre[i]; };
	
	bool filter(const PropDimenHead &head, PropBody &body);
private:



private:
    vector<filterCond>  _vFilterCond;
    
private:

    string              _sSql;              //创建表
    string              _sSqlStatus;              //创建表t_ecstatus
    string              _sTbNamePre;        //表前缀
    string              _sConcerns;         // 告警关注人
    int                 _iMaxInsertCount;   //一次最大插入条数
    CutType             _eCutType;          //分表类型    
    int                 _iInterval;         //执行ReapSql时间间隔
    map<string,string>  _mReapSql;          //定时执行sql  
    

    vector<TC_Mysql *> _vpMysql; 
	vector<string> _vsTbNamePre;
	int dbNumber;
};

#endif


