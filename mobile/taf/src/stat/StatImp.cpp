#include "StatImp.h"
#include "StatServer.h"

StatHashMap g_hashmap;
extern StatQueryHashMap g_slave_hashmap;
extern StatQueryHashMap g_master_hashmap;
///////////////////////////////////////////////////////////
//

void StatImp::initialize()
{
}


///////////////////////////////////////////////////////////
//
int StatImp::reportMicMsg( const map<taf::StatMicMsgHead, taf::StatMicMsgBody>& statmsg,
                           bool bFromClient,
                           taf::JceCurrentPtr current )
{
    LOGINFO("report---------------------------------access size:" << statmsg.size() << "|bFromClient:" <<bFromClient << endl);


    for ( map<StatMicMsgHead, StatMicMsgBody>::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
    {
        StatMicMsgHead head = it->first;
        const StatMicMsgBody &body = it->second;

        if(bFromClient)
        {
            head.masterIp   = current->getIp();  //自己获取主调ip

            head.slaveName = getNameFromTaServerName(head.slaveName);

            if (!g_app.IsEnableMTTDomain()) //MTTDomain 不切断slaveName，缺省非MTTDomain
            {
            	head.slaveName  = getSlaveName(head.slaveName);
            }
        }
        else
        {
            head.slaveIp = current->getIp();
        }

		map<string, string>::iterator iit;
		iit =  g_app.getVirtualMasterIp().find(getSlaveName(head.slaveName));
		if( iit != g_app.getVirtualMasterIp().end())
		{
			head.masterIp	= iit->second; //按 slaveName来匹配，填入假的主调ip，减小入库数据量
		}

		//如果不是info等级的日志级别，就别往里走了
		ostringstream os;
		if(LOG->IsNeedLog(TafRollLogger::INFO_LOG))
		{
			os.str("");
			head.displaySimple(os);
			body.displaySimple(os);
		}

		//三个数据都为0时不入库
        if(body.count == 0 && body.execCount == 0 && body.timeoutCount == 0)
        {
            LOGINFO(os.str()<<"|zero"<<endl);
            continue;
        }

        int iAddHash        = addHashMap(head,body);
        int iAddQueryHash   = addQueryHashMap(head,body);

        LOGINFO(os.str()<<"|"<<iAddHash<<endl);
    }

    return 0;
}


int StatImp::reportSampleMsg(const vector<StatSampleMsg> &msg,taf::JceCurrentPtr current )
{
    LOGINFO("sample---------------------------------access size:" << msg.size() << endl);

    for(unsigned i=0; i<msg.size();i++)
    {
        StatSampleMsg sample = msg[i];
        sample.masterIp = current->getIp();
		sample.slaveName = getNameFromTaServerName(sample.slaveName);
        ostringstream os;
        sample.displaySimple(os);
        FDLOG()<<os.str()<<endl;
    }
    return 0;
}

///////////////////////////////////////////////////////////
//
int StatImp::addHashMap( const StatMicMsgHead &head, const StatMicMsgBody &body )
{
	//dump数据到文件
    dump2file();

    float rate =  (g_hashmap.getMapHead()._iUsedChunk) * 1.0/g_hashmap.allBlockChunkCount();

    if(rate >0.9)
    {
        g_hashmap.expand(g_hashmap.getMapHead()._iMemSize * 2);
        LOG->debug() << "StatImp::addHashMap hashmap expand to "<<g_hashmap.getMapHead()._iMemSize<<endl;
    }

    int iRet = g_hashmap.add( head, body );
    if ( iRet != 0 )
    {
        LOG->debug() << "StatImp::addHashMap set g_hashmap recourd erro|" << iRet<< endl;
        return iRet;
    }
    return iRet;
}

///////////////////////////////////////////////////////////
//
int StatImp::addQueryHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body)
{
    StatQueryKey    tKey;
    StatQueryData   tData;

    tKey.name       = head.masterName;
    tKey.ip         = head.masterIp;
    tData.curTime   = TC_TimeProvider::getInstance()->getNow();
    tData.head      = head;
    tData.body      = body;
	int iRet = 0;
	try
	{
	    //复制map
	    for(map<int,int>::const_iterator it = body.intervalCount.begin();it!= body.intervalCount.end();it++)
	    {
	        tData.body.intervalCount[it->first] = it->second;
	    }
	    iRet = g_master_hashmap.add(tKey,tData);
	    if ( iRet != 0 )
	    {
	        LOG->error() << "StatImp::addQueryHashMap set g_master_hashmap recourd erro|" << iRet<< endl;
	        return iRet;
	    }
	    tKey.name       = head.slaveName;
	    tKey.ip         = head.slaveIp;
	    iRet = g_slave_hashmap.add(tKey,tData);
	    if ( iRet != 0 )
	    {
	        LOG->error() << "StatImp::addQueryHashMap set g_slave_hashmap recourd erro|" << iRet<< endl;
	        return iRet;
	    }
	}
	catch (exception &ex)
	{
		LOG->error() << "StatImp::addQueryHashMap exception:" << string(ex.what()) << endl;
	}
	catch (...)
	{
		LOG->error() << "StatImp::addQueryHashMap unknown exception:" << endl;
	}
    return iRet;
}

string StatImp::getSlaveName(const string& sSlaveName)
{
    string::size_type pos =  sSlaveName.find(".");
    if(pos != string::npos)
    {
        pos = sSlaveName.find(".",pos+1);
        if(pos != string::npos)
        {
            return sSlaveName.substr(0,pos);
        }
    }
    return  sSlaveName;
}
string StatImp::getNameFromTaServerName(const string& sSlaveName)
{
	if(!sSlaveName.empty())
	{
		//规定#开头的为taserver服务的名字
		if( '#' == sSlaveName[0])
		{
			//taserver服务名规范: #servantName#routerName#idc
			string::size_type pos =  sSlaveName.substr(1).find_first_of('#');
			//增加tas_前缀
			return "tas_" + sSlaveName.substr(1,pos);
		}
	}

	return sSlaveName;
}

void StatImp::dump2file()
{
    static string g_sDate;
    static string g_sFlag;

    time_t tTimeNow         = TC_TimeProvider::getInstance()->getNow();
    time_t tTimeInterv      = g_app.getInserInterv() *60;//second

    static time_t g_tLastDumpTime   = 0;

    if(g_tLastDumpTime == 0)
    {
        g_app.getTimeInfo(g_tLastDumpTime,g_sDate,g_sFlag);
    }

    if(tTimeNow - g_tLastDumpTime > tTimeInterv)
    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );
        if(tTimeNow - g_tLastDumpTime > tTimeInterv)
        {
            g_app.getTimeInfo(g_tLastDumpTime,g_sDate,g_sFlag);

            string sFile = g_app.getClonePath() + "/" + g_sDate + g_sFlag + ".txt";

            int iRet = g_hashmap.dump2file(sFile,true);
            if(iRet != 0)
            {
                TC_File::removeFile(sFile,false);
                LOG->error() << "StatImp::dump2file |" << sFile <<"|ret|"<<iRet<< endl;
                return;
            }
            LOG->debug() << "StatImp::dump2file |" << sFile << " TimeInterv:" << tTimeInterv << " now:" << tTimeNow << " last:" << g_tLastDumpTime << endl;
        }
    }
}
