#include "ReapSSDThread.h"
#include "util/tc_config.h"
#include "StatServer.h"


ReapSSDThread::ReapSSDThread()
{
    ////////////////////
    _bTerminate = false;
	_iCurWeight = 0;
	_iLastSq = -1;
    LOG->debug() << "begin ReapThread ok" << endl;
}

ReapSSDThread::~ReapSSDThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void ReapSSDThread::terminate()
{
    LOG->debug() << "[ReapSSDThread terminate.]" << endl;

    _bTerminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void ReapSSDThread::run()
{
    string sDate,sTime;

	int dbNumber = StatDbManager::getInstance()->getDbNumber();

	vector<StatMsg> vAllStatMsg(dbNumber);

    while (!_bTerminate)
    {
        try
        {
            //获取clone文件列表
            vector<string> vFiles;

            TC_File::listDirectory(g_app.getClonePath(),vFiles,false);

            sort(vFiles.begin(),vFiles.end());

			for(size_t i= 0; i< vFiles.size(); i++)
            {
                string sFileName = TC_File::extractFileName(TC_File::excludeFileExt(vFiles[i]));
                if(sFileName.length() != 12 || TC_Common::isdigit(sFileName) == false)
                {
                      LOG->debug() << "ReapSSDThread::run  invalid name |" << vFiles[i] << endl;
                      continue;
                }

                getData(vFiles[i],vAllStatMsg);

                if(vAllStatMsg.size() > 0)
                {
	                sDate   = sFileName.substr(0,8);
               		sTime   = sFileName.substr(8,4);
                    StatDbManager::getInstance()->insert2MultiDbs(vAllStatMsg,sDate,sTime);
                }

                TC_File::removeFile(vFiles[i],false);
            }

        }
        catch ( exception& ex )
        {
            LOG->debug() << "ReapSSDThread::run exception:"<< ex.what() << endl;

        }
        catch (... )
        {
            LOG->debug() << "ReapSSDThread::run ReapSSDThread unkonw exception catched" << endl;
        }

        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }
}

int ReapSSDThread::getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight)
{
	while (true){

		_iLastSq = (_iLastSq + 1) % iMaxDb;

		if (_iLastSq == 0)
		{
			_iCurWeight = _iCurWeight - iGcd;
			if (_iCurWeight <= 0)
			{
				_iCurWeight = iMaxW;
				if(_iCurWeight == 0)
				{
					return 0;
				}
			}
		}

		if (vDbWeight[_iLastSq] >= _iCurWeight)
		{
			return _iLastSq;
		}
	}
}

void ReapSSDThread::getData(const string &sCloneFile,vector<StatMsg> &vAllStatMsg)
{
    try
    {
        HashMap tHashmap;

        tHashmap.initStore(sCloneFile.c_str(), g_hashmap.getMapHead()._iMemSize);
        LOG->debug() << "ReapSSDThread::getData tHashmap Chunk:" << tHashmap.allBlockChunkCount() <<"|"<<tHashmap.getMapHead()._iUsedChunk<<endl;

		int iCount = 0,dbSeq=0;

		//获取db个数
		int dbNumber = StatDbManager::getInstance()->getDbNumber();

		vector<int> vDbWeight;
		int iGcd = 0,iMaxW = 0;

		StatDbManager::getInstance()->getDbWeighted(iGcd,iMaxW,vDbWeight);

		bool bEnable = StatDbManager::getInstance()->IsEnableWeighted();

		HashMap::lock_iterator it = tHashmap.beginSetTime();

        while ( it != tHashmap.end() )
        {
            StatMicMsgHead head;
            StatMicMsgBody body;
            int ret = it->get( head, body );
            if ( ret < 0 )
            {
                ++it;
                continue;
            }

			if (g_app.IsEnableMTTDomain()) // MTTdomain需要过滤
			{
				if (StatDbManager::getInstance()->filter(head, body))
				{
					++it;
					continue; //不入库
				}
			}

			if (dbNumber > 0)
			{
				if(bEnable)//按权重入库
				{
					dbSeq = getIndexWithWeighted(dbNumber,iGcd,iMaxW,vDbWeight);
					LOGINFO("ReapSSDThread::getIndexWithWeighted |" << dbSeq << endl);
				}
				else
				{
					dbSeq = iCount % dbNumber;
				}

				vAllStatMsg[dbSeq][head] = body;
			}

			iCount++;

            ++it;
        }
        LOG->debug() << "ReapSSDThread::getData get total size |" << iCount << endl;
    }
    catch (exception& ex)
    {
        LOG->debug() << "ReapSSDThread::getData exception:"<< ex.what() << endl;
    }
}
