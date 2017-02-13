#ifndef _UserInfoServer_H_
#define _UserInfoServer_H_

#include <iostream>
#include "servant/Application.h"
#include "CommHashMap.h"

#include "AsyncWork.h"

using namespace taf;

/**
 *
 **/
class UserInfoServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~UserInfoServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

        template <typename T>
        static void initHashMap(TC_Config &tConf,const string &name,T &hashmap)
        {
            LOG->debug() << __FUNCTION__<<" "<< name << " begin" << endl;

            string sConfigPath = "/main/hashmap/"+name;

            string sCacheFile  	= ServerConfig::DataPath + "/" + tConf.get(sConfigPath+"<file>",name);
            string sCachePath 	= TC_File::extractFilePath(sCacheFile);

            int iMinBlock       = TC_Common::strto<int>(tConf.get(sConfigPath+"<minBlock>","150"));
            int iMaxBlock       = TC_Common::strto<int>(tConf.get(sConfigPath+"<maxBlock>","200"));
            float iFactor       = TC_Common::strto<float>(tConf.get(sConfigPath+"<factor>","1.5"));
            size_t iSize   	= TC_Common::toSize(tConf.get(sConfigPath+"<size>"), 1024*1024*20);

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

        AsyncMaster mAsyncMaster;
        typedef JceHashMap<QUAN::CacheKey,QUAN::FriendListCacheValue, ThreadLockPolicy, FileStorePolicy> ActiveFriendsHashMapType;
        typedef JceHashMap<QUAN::CacheKey,QUAN::FollowerListCacheValue, ThreadLockPolicy, FileStorePolicy> ActiveFollowersHashMapType;
        typedef JceHashMap<QUAN::CacheKey,QUAN::UserInfoCacheValue, ThreadLockPolicy, FileStorePolicy> UserInfoHashMapType;

        ActiveFriendsHashMapType    mActiveFriendsHashMap;
        ActiveFollowersHashMapType  mActiveFollowersHashMap;
        UserInfoHashMapType         mUserInfoHashMap;
};

extern UserInfoServer g_app;

////////////////////////////////////////////
#endif
