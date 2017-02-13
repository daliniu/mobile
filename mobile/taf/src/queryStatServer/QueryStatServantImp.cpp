#include "QueryStatServantImp.h"
#include "servant/Application.h"
#include "RequestDecoder.h"

#define DDLOG  FDLOG("inout")

using namespace std;
extern TC_Config * g_pconf;

extern QueryStatServer g_app;

//////////////////////////////////////////////////////
void queryStatServantImp::initialize()
{
	//initialize servant here:
	_proxy.init();
}

//////////////////////////////////////////////////////
void queryStatServantImp::destroy()
{
	//destroy servant here:
	//...
}

taf::Int32 queryStatServantImp::query(const std::string &sIn,std::string &result,taf::JceCurrentPtr current)
{
	try
	{
		int64_t tStart = TC_TimeProvider::getInstance()->getNowMs();
		string sUid = TC_Common::tostr(g_app.genUid()) + "|";

		DDLOG << sUid << "input:"  <<  sIn << endl;
		LOG->debug()<< sUid << "input:"  <<  sIn << endl;

		doQuery(sUid,sIn, result);

		int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();
		LOG->debug()<< sUid << "send result succ,size(B):"<< result.length()<<",timecost(ms):" <<(tEnd-tStart)<<endl;
		DDLOG << sUid << "send result succ,size(B):"<< result.length()<<",timecost(ms):" <<(tEnd-tStart)<<endl;

		DDLOG << sUid << "result:" <<  (result.size()> 64?result.substr(0, 64):result) << endl; // 只打印64字节,减少io
		LOG->debug()<< sUid << "result:" <<  (result.size()> 64?result.substr(0, 64):result) << endl;

		if (result.size()> 64)
		{
			LOGINFO(sUid << "result:" <<  result.substr(64) << endl); //用info级别打印大于64字节部分
		}
		return 0;
	}
	catch (exception &ex)
	{
		LOG->error()<< ex.what() << endl;
	}
	return 0;

}


int queryStatServantImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
	try
	{
		int64_t tStart	= TC_TimeProvider::getInstance()->getNowMs();

		const vector<char>& request = current->getRequestBuffer();

		string buf((const char*)(&request[0]), request.size());

		string sUid = TC_Common::tostr(g_app.genUid()) + "|";

		DDLOG << sUid << "input:" <<  buf << endl;
		LOG->debug()<< sUid << "input:" <<  buf << endl;

		string result = "";
		doQuery(sUid, buf, result);

		int64_t tEnd = TC_TimeProvider::getInstance()->getNowMs();

		current->sendResponse(result.c_str(), result.length());

		LOG->debug()<< sUid << "send result succ,size(B):"<< result.length()<<",timecost(ms):" <<(tEnd-tStart)<<endl;
		DDLOG << sUid << "send result succ,size(B):"<< result.length()<<",timecost(ms):" <<(tEnd-tStart)<<endl;
		DDLOG << sUid << "result:" <<  (result.size()> 64?result.substr(0, 64):result) << endl; // 只打印64字节

		LOG->debug()<< sUid << "result:" <<  (result.size()> 64?result.substr(0, 64):result) << endl;

		if (result.size()> 64)
		{
			LOGINFO(sUid << "result:" <<  result.substr(64) << endl); //用info级别打印大于64字节部分
		}
		return 0;
	}
	catch (exception &ex)
	{
		LOG->error()<< ex.what() << endl;
	}
	return 0;
}

int queryStatServantImp::doQuery(const string sUid, const string &sIn, string &sResult)
{
	try
	{
		size_t pos = sIn.rfind("}"); // find json end

		string s;

		if (pos != string::npos)
		{
			s = sIn.substr(0, pos+1);
		}
		else
		{
			throw TC_Exception("bad query string");
		}

		RequestDecoder decoder(s);

		int ret = decoder.decode();

		decoder.addUid(sUid); //传人uid，供打印使用

		LOG->debug()  << sUid <<  "decode jason ret:" << ret<< ", sqlPart: " << TC_Common::tostr(decoder.getSql()) <<  endl;

		string result = "";
		if ( ret == RequestDecoder::QUERY)
		{
			_proxy.startQueryThreadPool(decoder.getSql(), result);
		}
		else if (ret == RequestDecoder::TIMECHECK)
		{
			string lasttime = _proxy.getLastTime(decoder.getSql(), 0);
			result += "lasttime:" + lasttime + "\n";
		}
		else if (ret == RequestDecoder::CREATE)
		{
			_proxy.create(decoder.getSql() ,result);
		}
		else if (ret == RequestDecoder::INSERT)
		{
			int dbNumber = g_app.getActiveDbSize();

			vector<string>	vdata(dbNumber); //每个db的入库记录数组。

			size_t pos = sIn.find("start");

			if (pos != string::npos)
			{
				string data = sIn.substr(pos + 6);
				RequestDecoder::getData(data, vdata);

				LOG->debug()  << sUid << "getData vdata size:" <<  vdata.size() << endl;
				_proxy.StartInsertThreadPool(decoder.getSql(),vdata, result);
			}
			else
			{
				LOG->debug()  << sUid <<  "no data to insert." << endl;
				result	= "no data to insert.\n";
			}
		}
		else if (ret == RequestDecoder::DROP)
		{
			_proxy.deleteTable(decoder.getSql(), result);
		}
		else if (ret == RequestDecoder::ALTER)
		{
			_proxy.create(decoder.getSql() ,result, 1);
		}
		else if (ret == RequestDecoder::DESC)
		{
			_proxy.desc(decoder.getSql(), result);
		}
		else
		{
			LOG->error() << sUid  << "decode request failed\n" <<endl;
			result = "Ret:-1\ndecode request failed\n";
		}

		result += "endline\n";

		sResult = result;
		return 0;
	}
	catch (exception &ex)
	{
		sResult ="Ret:-1\n" +  string(ex.what()) + "\nendline\n";

		LOG->error()  << sUid << sResult << endl;
	}

	return -1;
}


