#ifndef __REQUEST_DECODE_H__
#define __REQUEST_DECODE_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"

#include "JSON_parser.h"


class RequestDecoder
{
public:
	static void getData(const string &sdata,  vector<string> &vdata);
	static string getLine(const char** ppChar);
	static int printd(void* ctx, int type, const JSON_value* value);
public:
	enum Op
	{
		SET,
		ADD,
		SUB,
		EQ,
		NE,
		GT,
		LT,
		LE,
		GE,
		LIMIT,
	};

	enum METHOD
	{
		QUERY = 0,
		TIMECHECK,
		CREATE,
		INSERT,
		DROP,
		ALTER,
		DESC,
	};
	RequestDecoder(const string& input);

	~RequestDecoder();

	int parseCondition(const string& sCond, string& value);

	/**
	 * 从vCond中解析出field对应的值
	 *
	 */
	int find_field(vector<string>& vCond, const string& field /*f_tflag, f_date*/, string& value );

	int decode();

	int decodeDataid();

	int decodeMethod();

	int decodeArray(const string& arr, vector<string> &vFields);

	int addUid(const string& sUid);

	map<string, string>& getSql();

	int composeSqlPartition();

private:
	//待解析的json请求串
	string _sInput;

	//解析json请求串得到的键值对
	map<string, vector<string> >  _mReq;

	//解析json得到的数据库查询条件值
	map<string, string> _mSql;
};
#endif

