#include "DbThread.h"
#include "DbProxy.h"
#include "util/tc_config.h"
#include "QueryStatServer.h"
#include "RequestDecoder.h"
extern int decodejson( const char * inxxx, map<string, vector<string> > * req);

static void print_indention();

static size_t s_Level = 0;

static const char* s_pIndention = "  ";

static int s_IsKey = 0;

static string skey = "";



RequestDecoder::RequestDecoder(const string& input):_sInput(input)
{
}
RequestDecoder::~RequestDecoder()
{
	_mReq.clear();
}

int RequestDecoder::composeSqlPartition()
{
	vector<string> vConditions = _mReq["filter"];

	if(vConditions.empty())
	{
		return -1;
	}

	string date1,date2;

	int ret = find_field(vConditions, "f_date", date1);
	date1 = TC_Common::trim(date1, " ''");
	date1 = TC_Common::trim(date1, " ");
	if (ret == EQ || ret ==-1)
	{
		LOG->debug() << "f_date EQ " << date1<< endl;
		date2 = date1;
	}
	else //NOT EQ, find  again
	{
		ret = find_field(vConditions, "f_date", date2);
		date2 = TC_Common::trim(date2, " ''");
		date2 = TC_Common::trim(date2, " ");
		if (date1 < date2)
		{
			LOG->debug() << "v1 < v2:" << date1<< " to:" <<TC_Common::strto<int>(date1) << endl;
		}
		else
		{
			LOG->debug() << "v2 < v1:" << date2 <<" to:" <<TC_Common::strto<int>(date2)<< endl;
			date1.swap(date2);
		}
	}

	_mSql["date1"] = date1;
	_mSql["date2"] = date2;
	//get date end

	//get hour
	string tflag1,tflag2;
	ret = find_field(vConditions, "f_tflag", tflag1);
	tflag1 = TC_Common::trim(tflag1, " \"'");
	if (ret == EQ || ret == -1)
	{
		LOG->debug() << "f_tflag EQ " << tflag1<< endl;
		tflag2 = tflag1;
	}
	else //NOT EQ, find  again
	{
		ret = find_field(vConditions, "f_tflag", tflag2);
		tflag2 = TC_Common::trim(tflag2, " \"'");
		if (tflag1 < tflag2)
		{
			LOG->debug() << "v1 < v2:" << tflag1<< " to:" <<TC_Common::strto<int>(tflag1) << endl;
		}
		else
		{
			LOG->debug() << "v2 < v1:" << tflag2 <<" to:" <<TC_Common::strto<int>(tflag2)<< endl;
			tflag1.swap(tflag2);
		}
	}
	_mSql["tflag1"] = tflag1;
	_mSql["tflag2"] = tflag2;
	//get hour end

	vConditions = _mReq["filter"];
	if (!vConditions.empty())
	{
		string whereCond = "";
		vector<string>::iterator it = vConditions.begin();
		while(it != vConditions.end())
		{
			whereCond += (whereCond.empty()?"":" and ") + *it ;
	 		it++;
		}

		_mSql["whereCond"] = " where " + whereCond;
	}


	vConditions = _mReq["groupby"];
	if (!vConditions.empty())
	{
		string groupCond = "";
		string groupField = "";
		vector<string>::iterator it = vConditions.begin();
		while(it != vConditions.end())
		{
			groupCond += (groupCond.empty()?"":", ") + *it;
			groupField += (groupField.empty()?"":", ") + *it;
			it++;
		}
		_mSql["groupCond"] = " group by " + groupCond;
		_mSql["groupField"] = groupField;
	}

	string sumField = "";
	vConditions = _mReq["indexs"];
	if (vConditions.empty())
	{
		sumField = " sum(succ_count) ";
	}
	else
	{
		vector<string>::iterator it = vConditions.begin();
		while(it != vConditions.end())
		{
			sumField += string((sumField.empty()?"":", ")) + " sum(" + *it + ")" ;
	 		it++;
		}
	}

	_mSql["sumField"]= sumField;
	return 0;
}

int RequestDecoder::parseCondition(const string& sCond, string& value)
{
	string::size_type pos =0;
	pos= sCond.find(">=");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+2);
		return GE;
	}

	pos = sCond.find("<=");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+2);
		return LE;
	}

	pos = sCond.find("!=");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+2);
		return NE;
	}
	pos = sCond.find("<");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+1);
		return LT;
	}

	pos = sCond.find(">");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+1);
		return GT;
	}

	pos = sCond.find("=");
	if (pos != string::npos )
	{
		value = sCond.substr(pos+1);
		return EQ;
	}
	// ==
	return 0;
}

int RequestDecoder::find_field(vector<string>& vCond, const string& field /*f_tflag, f_date*/, string& value )
{
	vector<string>::iterator it = vCond.begin();
	while(it != vCond.end()){

		string::size_type pos = it->find(field);

		if(pos != string::npos)
		{
			string temp = *it;
			vCond.erase(it);
			return parseCondition(temp, value);
		}
		it++;
	}
	return -1;
}

/*

name : indexs, filter, groupby
*/

int RequestDecoder::decodeDataid()
{

	string::size_type pos = _sInput.find("dataid");
	if (pos == string::npos)
	{
		return -1;
	}
	string::size_type endpos = _sInput.find(",", pos);
	_mSql["dataid"] = _sInput.substr(pos + 9, endpos - pos -10);
	LOG->debug() << "decodeDataid: " << _mSql["dataid"] <<endl;
	return 0;
}

int RequestDecoder::decodeMethod()
{

	string::size_type pos = _sInput.find("method");
	if (pos == string::npos)
	{
		return -1;
	}
	string::size_type endpos = _sInput.find(",", pos);
	if (endpos!=string::npos)
	{
		_mSql["method"] = _sInput.substr(pos + 9, endpos - pos -10);
	}
	else
	{
		_mSql["method"] = "timecheck"; //
	}
	LOG->debug() << "decodeMethod: " << _mSql["method"] <<endl;
	return 0;
}

int RequestDecoder::decodeArray(const string& arr, vector<string> &vFields)
{

	string::size_type pos1 = arr.find_first_of("[");
	if(pos1 == string::npos)
	{
		LOG->debug() << ("paramh '" + arr + "' is invalid!" );
		return -1;
	}
	string::size_type pos2 = arr.find_first_of("]");
	if(pos2 == string::npos)
	{
		LOG->debug() << ("paramh '" + arr + "' is invalid!" );
		return -1;
	}
	vFields = TC_Common::sepstr<string>(arr.substr(pos1, pos2 -pos1-1), ",");
	vector<string>::iterator it = vFields.begin();
	while(it != vFields.end())
	{

		LOG->debug() << "indexs bt " << *it << endl;
		string s = TC_Common::trim(*it, "[]\"\"");
		*it = s;
		LOG->debug() << "indexs at " << *it << endl;
		it++;
	}
	return 0;


}



int RequestDecoder::printd(void* ctx, int type, const JSON_value* value)
{
	map<string, vector<string> >  *req = (map<string, vector<string> > *)ctx;

	switch(type) {
	case JSON_T_ARRAY_BEGIN:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("[\n");
		++s_Level;
		break;
	case JSON_T_ARRAY_END:
		//assert(!s_IsKey);
		s_IsKey =0;
		if (s_Level > 0) --s_Level;
		print_indention();
		LOGINFO("]\n");
		break;
   case JSON_T_OBJECT_BEGIN:
	   if (!s_IsKey) print_indention();
	   s_IsKey = 0;
	   LOGINFO("{\n");
	   ++s_Level;
		break;
	case JSON_T_OBJECT_END:
		//assert(!s_IsKey);
		if (s_Level > 0) --s_Level;
		print_indention();
		LOGINFO("}\n");
		break;
	case JSON_T_INTEGER:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("integer: " << value->vu.integer_value<< endl);
		break;
	case JSON_T_FLOAT:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO(value->vu.float_value <<endl); /* We wanted stringified floats */
		break;
	case JSON_T_NULL:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("null\n");
		break;
	case JSON_T_TRUE:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("true\n");
		break;
	case JSON_T_FALSE:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("false\n");
		break;
	case JSON_T_KEY:
		s_IsKey = 1;
		print_indention();
		LOGINFO("key = " << value->vu.str.value <<  ", value = " << endl);
		{
			vector<string> v;
			v.push_back(string(value->vu.str.value));
			(*req)["tmpkey"] = v; // req map结构的tmpkey传递key value
		}
		break;
	case JSON_T_STRING:
		if (!s_IsKey) print_indention();
		s_IsKey = 0;
		LOGINFO("string: " <<  value->vu.str.value<< endl);
		(*req)[ (*req)["tmpkey"].back()].push_back(string(value->vu.str.value));
		break;
	default:
		LOGINFO("RequestDecoder not support type:" << TC_Common::tostr(type) << endl);
		//assert(0);
		break;
	}

	return 1;
}


map<string, string>& RequestDecoder::getSql()
{
	return _mSql;
}


int RequestDecoder::decode()
{
	if (decodejson(_sInput.c_str(),  &_mReq) != 0)
	{
		return -1; //fail
	}

	try
	{
		if (_mReq["method"].size())
		{
			_mSql["method"] = _mReq["method"].front();
		}

		if (_mReq["dataid"].size())
		{
			_mSql["dataid"] = _mReq["dataid"].front();
		}

		if (_mSql["method"] == "timecheck")
		{
			return TIMECHECK; // 查看最后入库时间
		}
		else if (_mSql["method"] == "create")
		{
			_mSql["sql_part"] = _mReq["sql_part"].front();
			_mSql["daytime"] = _mReq["daytime"].front();
			return CREATE;
		}
		else if (_mSql["method"] == "insert")
		{
			_mSql["daytime"] = _mReq["daytime"].front();
			_mSql["lasttime"] = _mReq["lasttime"].front();
			return INSERT;
		}
		else if (_mSql["method"] == "drop")
		{
			if (_mReq["daytime"].size() > 0)
			{
				_mSql["daytime"] = _mReq["daytime"].front();
			}
			else
			{
				_mSql["daytime"] = "null";
			}
			return DROP;
		}
		else if (_mSql["method"] == "alter")
		{
			_mSql["sql_part"] = _mReq["sql_part"].front();
			_mSql["daytime"] = _mReq["daytime"].front();
			return ALTER;
		}
		else if (_mSql["method"] == "desc")
		{
			_mSql["daytime"] = _mReq["daytime"].front();
			return DESC;
		}
		else if(_mSql["method"] == "query")
		{
			composeSqlPartition();
			return QUERY;
		}

	}
	catch (exception& ex)
	{
		LOG->error() << "RequestDecoder::decode exception:"<< ex.what() << endl;
		return -1;
	}
	return -1;
}

//传入sUid，供打印使用
int RequestDecoder::addUid(const string& sUid)
{
    _mSql["uid"] = sUid;
	return 0;
}
static void print_indention()
{
    size_t i;

    for (i = 0; i < s_Level; ++i) {
        printf("%s", s_pIndention);
    }
}

string RequestDecoder::getLine(const char** ppChar)
{
	string sTmp;

	sTmp.reserve(512);

	while((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
	{
		sTmp.append(1, (**ppChar));
		(*ppChar)++;
	}

	if((**ppChar) == '\r')
    {
		(*ppChar)++;   /* pass the char '\n' */
    }

	(*ppChar)++;

	return sTmp;
}

void RequestDecoder::getData(const string &sdata, vector<string>  &vdata)
{
    try
    {
		const char *pBuf =sdata.c_str();

		const char **p = &pBuf;

		int iCount = 0, dbSeq = 0;
		int dbNumber = g_app.getActiveDbSize();
        while ( 1 )
        {

            string line = getLine(p);
            if ( line == ""  || line  == "data line end")
            {
				break;
			}
			if (dbNumber > 0)
			{
				dbSeq = iCount%dbNumber;
				if (iCount >= dbNumber)
				{
					line = ", " + line;
				}
				vdata[dbSeq]  += line;
			}
			iCount++;
        }
		LOG->debug() <<"total size |" << iCount <<"|vData:"<< TC_Common::tostr(vdata) << endl;
    }
    catch (exception& ex)
    {
        LOG->error() << "DBThread::getData exception:"<< ex.what() << endl;
    }
}

