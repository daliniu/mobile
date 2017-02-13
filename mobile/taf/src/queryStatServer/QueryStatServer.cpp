#include "QueryStatServer.h"
#include "QueryStatServantImp.h"

using namespace std;

QueryStatServer g_app;

TC_Config * g_pconf;

struct JsonProtocol
{
	static int parse(string &in, string &out)
	{
		LOGINFO(in<<endl);

		//string::size_type jsonStart = in.find("{");
		string::size_type jsonEnd = in.find("}");

		string::size_type insert = in.find("insert");

		string::size_type dEnd = in.find("data line end");

		if (jsonEnd != string::npos )
		{
			if (insert == string::npos) // 不是插入数据
			{
				out = in;
				in = "";
				return TC_EpollServer::PACKET_FULL;   //返回1表示收到的包已经完全
			}
			if (dEnd ==  string::npos) // 没有收到插入数据结尾
			{
				return TC_EpollServer::PACKET_LESS;
			}
			else // 收到插入数据结尾
			{
				out = in;
				in = "";
				return TC_EpollServer::PACKET_FULL;   //返回1表示收到的包已经完全
			}
		}

		return TC_EpollServer::PACKET_ERR;		//返回-1表示收到包协议错误，框架会自动关闭当前连接
	}
};

/////////////////////////////////////////////////////////////////
void QueryStatServer::initialize()
{
	//initialize application here:

	vector<string> v_dblist;

	g_pconf->getDomainVector("/taf/db", v_dblist);

	_dbNumber = v_dblist.size();
	LOG->debug()<< "getDomainVector stat db size:" << _dbNumber << endl;

	for(size_t i=0; i<_dbNumber;i++)
	{
		TC_DBConf tcDBConf;

		string path= "/taf/db/" + v_dblist[i];
		tcDBConf.loadFromMap(g_pconf->getDomainMap(path));

		LOG->debug()<< v_dblist[i] << ":"<< TC_Common::tostr(g_pconf->getDomainMap(path)) << endl;

		_mDbInfo.push_back(tcDBConf);
	}

	_pDBThread = new DBThread();

	_pDBThread->start();

	//QueryStatObj 非taf方式的接口查询， QueryObj taf接口查询
	addServant<queryStatServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".QueryStatObj");
	addServant<queryStatServantImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".NoTafObj");
	addServantProtocol(ServerConfig::Application + "." + ServerConfig::ServerName + ".NoTafObj", &JsonProtocol::parse);

	_insertInterval = TC_Common::strto<int>(g_pconf->get("/taf<interval>","10"));

	_ThreadPoolSize = TC_Common::strto<int>(g_pconf->get("/taf<threadpoolsize>","20"));
}
int QueryStatServer::getInsertInterval() const
{
	return _insertInterval;
}

size_t QueryStatServer::getStatDbNumber() const
{
	return _dbNumber;
}

void QueryStatServer::addActiveDb(const TC_DBConf& db)
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	_mActiveDbInfo.push_back(db);
}

void QueryStatServer::setActiveDb(const vector<TC_DBConf> &vDbInfo)
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	_mActiveDbInfo = vDbInfo;
}

void QueryStatServer::clearActiveDb()
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	_mActiveDbInfo.clear();
}

size_t QueryStatServer::getActiveDbSize() const
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	return  _mActiveDbInfo.size();
}

vector<TC_DBConf> QueryStatServer::getActiveDbInfo() const
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	return _mActiveDbInfo;
}

vector<TC_DBConf> QueryStatServer::getStatDbInfo() const
{
	TC_LockT<TC_ThreadMutex> lock(*this);
	return _mDbInfo;
}

uint32_t QueryStatServer::genUid()
{
	TC_LockT<TC_ThreadMutex> lock(*this);

	while (++_uniqId == 0);

	return _uniqId;
}

int QueryStatServer::getThreadPoolSize() const
{
	return _ThreadPoolSize;
}
string QueryStatServer::dumpDbInfo(const vector<TC_DBConf>& vDbInfo) const
{
	ostringstream os;

	os <<endl;
	for(size_t i = 0; i < vDbInfo.size();i++)
	{
		os << "[charset]=[" <<vDbInfo[i]._charset <<"] "
		   << "[dbhost]=[" <<vDbInfo[i]._host <<"] "
		   << "[dbpass]=[" <<vDbInfo[i]._password <<"] "
		   << "[dbport]=[" <<TC_Common::tostr(vDbInfo[i]._port) <<"] "
		   << "[dbuser]=[" <<vDbInfo[i]._user <<"]"
		   <<endl;
	}
	return os.str();
}

/////////////////////////////////////////////////////////////////
void
QueryStatServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	try
	{

        g_pconf =  & g_app.getConfig();
		g_app.main(argc, argv);
		TafTimeLogger::getInstance()->enableRemote("inout",false);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}
	return -1;
}
/////////////////////////////////////////////////////////////////


