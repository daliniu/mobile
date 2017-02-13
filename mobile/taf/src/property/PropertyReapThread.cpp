#include "PropertyReapThread.h"
#include "util/tc_config.h"
#include "PropertyServer.h"
#include "PropertyDbManager.h"
PropertyReapThread::PropertyReapThread():_bTerminate(false)
{
}

PropertyReapThread::~PropertyReapThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void PropertyReapThread::terminate()
{
    LOG->debug() << "[PropertyReapThread terminate.]" << endl;

    _bTerminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void PropertyReapThread::run()
{
    string sDate,sFlag;
    map<PropHead,PropBody> mStatMsg;

	int dbNumber = PropertyDbManager::getInstance()->getDbNumber();
	vector<map<PropHead,PropBody > > mAllStatMsg(dbNumber);
    while (!_bTerminate)
    {
        try
        {
            vector<string> vFiles;
            TC_File::listDirectory(PropertyServer::g_sClonePatch,vFiles,false);
            sort(vFiles.begin(),vFiles.end());
            for(size_t i= 0; i< vFiles.size(); i++)
            {
                string sFileName = TC_File::extractFileName(TC_File::excludeFileExt(vFiles[i]));
                if(sFileName.length() != 12 || TC_Common::isdigit(sFileName) == false)
                {
                      LOG->debug() << "file name  invalid |" << vFiles[i] << endl;
                      continue;
                }
                sDate = sFileName.substr(0,8);
                sFlag = sFileName.substr(8,4);

                getData(vFiles[i],mStatMsg, mAllStatMsg);
                if ( mStatMsg.size() > 0 )
                {
                    if(PropertyDbManager::getInstance()->insert2Db( mStatMsg,sDate,sFlag) != 0)
                    {
                        PropertyDbManager::getInstance()->insert2Db( mStatMsg,sDate,sFlag);
                    }
                }

                PropertyDbManager::getInstance()->updateEcsStatus(sDate+" "+sFlag);
                PropertyDbManager::getInstance()->flush(sDate,sFlag);

                if( mAllStatMsg.size() > 0 )
                {
                    PropertyDbManager::getInstance()->insert2MultiDbs( mAllStatMsg,sDate,sFlag);

                }

                TC_File::removeFile(vFiles[i],false);
            }
        }
        catch (exception& ex)
        {
            LOG->debug() << "PropertyReapThread::run exception:"<< ex.what() << endl;
        }
        catch (... )
        {
            LOG->debug() << "PropertyReapThread::run PropertyReapThread unkonw exception catched" << endl;
        }
        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }
}

void PropertyReapThread::getData(const string &sCloneFile, map<PropHead,PropBody> &mStatMsg, vector<map<PropHead,PropBody> > &mAllStatMsg)
{
    try
    {
        mStatMsg.clear();
        PropertyHashMap tHashmap;
        tHashmap.initStore(sCloneFile.c_str(), g_hashmap.getMapHead()._iMemSize);
        LOG->debug() << "PropertyReapThread::getData tHashmap Chunk:"<<tHashmap.allBlockChunkCount() <<"|"<<tHashmap.getMapHead()._iUsedChunk<<endl;
		int dbNumber = PropertyDbManager::getInstance()->getDbNumber();
		int iCount = 0, dbSeq = 0;

		PropertyHashMap::lock_iterator it = tHashmap.beginSetTime();
        while ( it != tHashmap.end() )
        {
            PropHead head;
            PropBody body;
            int ret = it->get( head, body );
            if ( ret < 0 )
            {
                continue;
            }

			if (dbNumber > 0)
			{
				dbSeq = iCount%dbNumber;
				mAllStatMsg[dbSeq][head] = body;
			}
			iCount++;
            ///if(PropertyDbManager::getInstance()->filter(head,body) == true)
            {
                //++it;
                //continue;
            }
            mStatMsg.insert(make_pair( head, body));
            ++it;
        }
        LOG->debug() << "get total size |" << mStatMsg.size() << " iCount:" << iCount << endl;
    }
    catch (exception& ex)
    {
        LOG->debug() << "PropertyReapThread::getData exception:"<< ex.what() << endl;
    }
}


