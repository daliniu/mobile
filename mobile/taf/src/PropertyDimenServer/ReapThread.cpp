#include "ReapThread.h"
#include "util/tc_config.h"
#include "PropertyDimenServer.h"
#include "DbManager.h"
PropertyDimenReapThread::PropertyDimenReapThread()
{
    LOG->debug() << "begin PropertyDimenReapThread init" << endl;

    LOG->debug() << "PropertyDimenReapThread init ok." << endl;
}

PropertyDimenReapThread::~PropertyDimenReapThread()
{
    if (isAlive())
    {
        terminate();

        getThreadControl().join();
    }
}

void PropertyDimenReapThread::terminate()
{
    LOG->debug() << "[PropertyDimenReapThread terminate.]" << endl;

    _bTerminate = true;

    TC_ThreadLock::Lock lock(*this);

    notifyAll();
}

void PropertyDimenReapThread::run()
{
    string sDate,sFlag;
	LOG->info() << "PropertyDimenReapThread::run enter" << endl;
	int dbNumber = PropertyDimenDbManager::getInstance()->getDbNumber();
	vector<map<PropDimenHead,PropBody > > mAllStatMsg(dbNumber);
    while (!_bTerminate)
    {
        try
        {
            vector<string> vFiles;
            TC_File::listDirectory(PropertyDimenServer::g_sClonePatch,vFiles,false);
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

                getData(vFiles[i], mAllStatMsg);

                if( mAllStatMsg.size() > 0 )
                {
                    PropertyDimenDbManager::getInstance()->insert2MultiDbs( mAllStatMsg,sDate,sFlag);

                }

                TC_File::removeFile(vFiles[i],false);
            }
        }
        catch (exception& ex)
        {
            LOG->debug() << "PropertyDimenReapThread::run exception:"<< ex.what() << endl;
        }
        catch (... )
        {
            LOG->debug() << "PropertyDimenReapThread::run PropertyDimenReapThread unkonw exception catched" << endl;
        }
        TC_ThreadLock::Lock lock(*this);
        timedWait(REAP_INTERVAL);
    }
	LOG->info() << "PropertyDimenReapThread::run exit" << endl;
}

void PropertyDimenReapThread::getData(const string &sCloneFile, vector<map<PropDimenHead,PropBody> > &mAllStatMsg)
{
    try
    {
        PropertyHashMap tHashmap;
        tHashmap.initStore(sCloneFile.c_str(), g_hashmap.getMapHead()._iMemSize);
        LOG->debug() << "PropertyDimenReapThread::getData tHashmap Chunk:"<<tHashmap.allBlockChunkCount() <<"|"<<tHashmap.getMapHead()._iUsedChunk<<endl;
		int dbNumber = PropertyDimenDbManager::getInstance()->getDbNumber();
		int iCount = 0, dbSeq = 0;

		PropertyHashMap::lock_iterator it = tHashmap.beginSetTime();
        while ( it != tHashmap.end() )
        {
            PropDimenHead head;
            PropBody body;
            int ret = it->get( head, body );
            if ( ret < 0 )
            {
                continue;
            }

			PropertyDimenDbManager::getInstance()->filter(head, body);

			if (dbNumber > 0 && body.vInfo.size() > 0)
			{
				dbSeq = iCount%dbNumber;
				mAllStatMsg[dbSeq][head] = body;
			}
			iCount++;
            ++it;
        }
        LOG->debug() << "get total iCount:" << iCount << endl;
    }
    catch (exception& ex)
    {
        LOG->debug() << "PropertyDimenReapThread::getData exception:"<< ex.what() << endl;
		TAF_NOTIFY_ERROR("PropertyDimenReapThread::getData exception:"+string(ex.what()));
    }
}




