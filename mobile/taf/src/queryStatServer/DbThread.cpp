#include "DbThread.h"
#include "DbProxy.h"
#include "util/tc_config.h"
#include "QueryStatServer.h"


DBThread::DBThread()
{
    ////////////////////
    _bTerminate         = false;
    LOG->debug() << "begin DBThread ok" << endl;
}

DBThread::~DBThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void DBThread::terminate()
{
    LOG->debug() << "[DBThread terminate.]" << endl;

    _bTerminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void DBThread::run()
{
	LOG->debug() << "DBThread::run " << endl;
    while (!_bTerminate)
    {
        try
        {
			tryConnect();
        }
        catch ( exception& ex )
        {
            LOG->debug() << "DBThread::run exception:"<< ex.what() << endl;

        }
        catch (... )
        {
            LOG->debug() << "DBThread::run DBThread unkonw exception catched" << endl;
        }
        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }
}


int DBThread::connect(MYSQL** pstMql,const TC_DBConf& tcConf)
{

    *pstMql = mysql_init(NULL);
    if (mysql_real_connect(*pstMql,tcConf._host.c_str(), tcConf._user.c_str(), tcConf._password.c_str(), tcConf._database.c_str(), tcConf._port, NULL, tcConf._flag) == NULL)
    {
		LOG->debug() << "[TC_Mysql::connect]: mysql_real_connect: " + string(mysql_error(*pstMql)) + ", erron:" <<mysql_errno(*pstMql) <<" port:"<< tcConf._port << endl;
		return -1;
    }
    return 0;
}
void DBThread::tryConnect()
{
    try
    {
		size_t dbNumber = g_app.getStatDbNumber();

		vector<TC_DBConf> vDbInfo = g_app.getStatDbInfo();

		vector<TC_DBConf> vAtivedbInfo;

		for (size_t i = 0; i < dbNumber; i++)
		{
	 		TC_DBConf tcDBConf = vDbInfo[i];

			//modify database
			tcDBConf._database = "test";

			MYSQL* _pstMql;

			int iRet = connect(&_pstMql,tcDBConf);

			if (_pstMql != NULL)
			{
				mysql_close(_pstMql);
				_pstMql = NULL;
			}

		    if (iRet != 0)
		    {
				continue;
			}

			vAtivedbInfo.push_back(vDbInfo[i]);;
		}

		g_app.setActiveDb(vAtivedbInfo);

		LOGINFO("active db:" << g_app.dumpDbInfo(g_app.getActiveDbInfo()) << endl);
    }
    catch (exception& ex)
    {
        LOG->debug() << "DBThread::tryConnect exception:"<< ex.what() << endl;
    }
}



