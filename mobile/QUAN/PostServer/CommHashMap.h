#ifndef _COMMHASHMAP_H_
#define _COMMHASHMAP_H_

#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_singleton.h"
#include "servant/Application.h"

/**
 *
 *
 */

class CommHashMap : public TC_Singleton<CommHashMap>
{
    public:

        CommHashMap(unsigned long long iMaxMemSize = 5368709120)
            :_iMaxMemSize(iMaxMemSize)
        {
        }

        template <typename M>
            void initHashMap(TC_Config &tConf,const string &name,M &hashmap)
            {
                LOG->debug() << __FUNCTION__<<" "<< name << " begin" << endl;

                string sConfigPath = "/main/hashmap/"+name;

                string sCacheFile   = ServerConfig::DataPath + "/" + tConf.get(sConfigPath+"<file>",name);
                string sCachePath   = TC_File::extractFilePath(sCacheFile);

                int iMinBlock       = TC_Common::strto<int>(tConf.get(sConfigPath+"<minBlock>","150"));
                int iMaxBlock       = TC_Common::strto<int>(tConf.get(sConfigPath+"<maxBlock>","200"));
                float iFactor       = TC_Common::strto<float>(tConf.get(sConfigPath+"<factor>","1.5"));
                size_t iSize        = TC_Common::toSize(tConf.get(sConfigPath+"<size>"), 1024*1024*20);

                if(!TC_File::makeDirRecursive(sCachePath) )
                {
                    LOG->error()<<"cannot create hashmap file "<<sCachePath<<endl;
                    exit(-1);
                }

                try
                {
                    LOG->info()<<"initDataBlockSize size: "<< iMinBlock << ", " << iMaxBlock << ", " << iFactor <<endl;

                    hashmap.initDataBlockSize(iMinBlock,iMaxBlock,iFactor);
                    if(TC_File::isFileExist(sCacheFile))
                    {
                        iSize = TC_File::getFileSize(sCacheFile);
                    }
                    hashmap.initStore( sCacheFile.c_str(), iSize );
                    hashmap.setAutoErase(true);
                    LOG->info()<< "\n" <<  hashmap.desc();

                    LOG->debug() << __FUNCTION__<<" "<< name << " end" << endl;
                }
                catch(TC_HashMap_Exception &e)
                {
                    TC_File::removeFile(sCacheFile,false);
                    throw runtime_error(e.what());
                }
            }



        template <typename M,typename K, typename V> 
            int addHashMap(M &tJceMap,K &tKey,V &tValue)
            {
                int iRet = -1;
                float rate =  (tJceMap.getMapHead()._iUsedChunk) * 1.0/tJceMap.allBlockChunkCount();

                if (rate >0.9  && (tJceMap.getMapHead()._iMemSize*2 < _iMaxMemSize) )
                {
                    tJceMap.expand(tJceMap.getMapHead()._iMemSize * 2);
                    LOG->debug() <<__FILE__<<"::"<< __FUNCTION__ <<" hashmap expand to "<<tJceMap.getMapHead()._iMemSize<<endl;
                }

                iRet = tJceMap.set( tKey, tValue );

                if ( iRet !=  TC_HashMap::RT_OK )
                {
                    LOG->debug() <<__FILE__<<"::"<< __FUNCTION__<<" set hashmap recourd erro|" << iRet<< endl;
                }

                return iRet;
            }

        template <typename M,typename K, typename V> 
            int getHashMap(M &tJceMap,K &tKey,V &tValue)
            {
                int iRet = -1;

                iRet = tJceMap.get( tKey, tValue );

                if ( iRet !=  TC_HashMap::RT_OK && iRet != TC_HashMap::RT_NO_DATA)
                {
                    LOG->debug() <<__FILE__<<"::"<< __FUNCTION__<<" get hashmap recourd erro|" << iRet<< endl;

                } 

                else
                {
                    iRet = 0;
                }

                return iRet;
            }

        template <typename M,typename K> 
            int eraseHashMap(M &tJceMap,K &tKey)
            {
                int iRet=-1;
                iRet=tJceMap.erase(tKey);
                if(iRet==TC_HashMap::RT_OK || iRet==TC_HashMap::RT_ONLY_KEY || TC_HashMap::RT_NO_DATA)
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
            }

        template <typename M>
            void dump2file(M &tJceMap,const string &sFilePre,int iDumpInterv = 3600*24)
            {
                time_t tTimeNow         = TC_TimeProvider::getInstance()->getNow();

                static time_t g_tLastDumpTime   = 0;

                if (tTimeNow - g_tLastDumpTime > iDumpInterv)
                {
                    static  TC_ThreadLock g_mutex;
                    TC_ThreadLock::Lock  lock( g_mutex );
                    if (tTimeNow - g_tLastDumpTime > iDumpInterv)
                    {

                        string sFile = sFilePre +"_" +TC_Common::tm2str(tTimeNow,"%Y%m%d%H%M") + ".txt";

                        int iRet = tJceMap.dump2file(sFile,true);
                        if (iRet != 0)
                        {
                            TC_File::removeFile(sFile,false);
                            LOG->error()<<__FILE__<<"::"<< __FUNCTION__<<"|"<< sFile <<"|ret|"<<iRet<< endl;
                        }
                        LOG->debug()<<__FILE__<<"::"<< __FUNCTION__<<"|" << sFile << " TimeInterv:" << iDumpInterv << " now:" << tTimeNow << " last:" << g_tLastDumpTime << endl;
                    }
                }
            }

    private:

        unsigned long long _iMaxMemSize;

};
/////////////////////////////////////////////////////
#endif
