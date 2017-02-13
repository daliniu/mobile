#ifndef __CIRCLESHAREDCACHE_H__
#define __CIRCLESHAREDCACHE_H__

#include "util/tc_singleton.h"
#include "util/tc_common.h"
#include "CommHashMap.h"
#include "Circle.h"
#include "Base.h"

class CircleSharedCache:public taf::TC_Singleton<CircleSharedCache>
{
    public:
        CircleSharedCache()
        {
            TC_Config tConf;
            tConf.parseFile(ServerConfig::BasePath + "CircleServer.conf");

            CommHashMap::getInstance()->initHashMap(tConf,"circleInfoCache",mCircleInfoCache);
            CommHashMap::getInstance()->initHashMap(tConf,"circleLbsCache",mCircleLbsCache);
            CommHashMap::getInstance()->initHashMap(tConf,"circleUserCache",mCircleUserCache);
            CommHashMap::getInstance()->initHashMap(tConf,"circleUnlockCache",mCircleUnlockCache);
            CommHashMap::getInstance()->initHashMap(tConf,"circleLocationCache",mCircleLocationCache);
        }

        void UpdateCircle(taf::Int64 lCircleId,const QUAN::CircleInfo& info)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            QUAN::CircleValue value;
            value.tCircleInfo=info;
            value.iTimespan=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->addHashMap(mCircleInfoCache,key,value)!=0)
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
        }

        bool GetCircle(taf::Int64 lCircleId,QUAN::CircleInfo& info)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            QUAN::CircleValue value;

            int now=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->getHashMap(mCircleInfoCache,key,value)==0)
            {
                if ( now - value.iTimespan < 3600 * 24 )
                {
                    info=value.tCircleInfo; 
                    return true;
                }
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
            return false;
        }

        void ClearCircle(taf::Int64 lCircleId)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            if(CommHashMap::getInstance()->eraseHashMap(mCircleInfoCache,key))
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<" FAILED"<<endl;
            }
        }

        void UpdateCircleLbs(taf::Int32 x,taf::Int32 y,const vector<QUAN::CircleLbsInfo>& vCircleLbsInfo)
        {
            QUAN::CircleKey key;
            key.id=((taf::Int64)x<<32)+y;

            QUAN::CircleLbsValue value;
            value.vCircleLbsInfo=vCircleLbsInfo;
            value.iTimespan=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->addHashMap(mCircleLbsCache,key,value)!=0)
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
        }

        bool GetCircleLbs(taf::Int32 x,taf::Int32 y,vector<QUAN::CircleLbsInfo>& vCircleLbsInfo)
        {
            QUAN::CircleKey key;
            key.id=((taf::Int64)x<<32)+y;

            QUAN::CircleLbsValue value;

            int now=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->getHashMap(mCircleLbsCache,key,value)==0)
            {
                if ( now - value.iTimespan < 3600 * 24 )
                {
                    vCircleLbsInfo=value.vCircleLbsInfo; 
                    return true;
                }
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
            return false;
        }

        void ClearCircleLbs(taf::Int32 x,taf::Int32 y)
        {
            QUAN::CircleKey key;
            key.id=((taf::Int64)x<<32)+y;

            if(CommHashMap::getInstance()->eraseHashMap(mCircleLbsCache,key))
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<" FAILED"<<endl;
            }
        }


        void UpdateCircleUser(taf::Int64 lUId,const vector<taf::Int64>& vConcern,const vector<taf::Int64>& vAccessable)
        {
            QUAN::CircleKey key;
            key.id=lUId;

            QUAN::CircleUserValue value;
            value.vConcern=vConcern;
            value.vAccessable=vAccessable;
            value.iTimespan=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->addHashMap(mCircleUserCache,key,value)!=0)
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
        }

        bool GetCircleUser(taf::Int64 lUId,vector<taf::Int64>& vConcern,vector<taf::Int64>& vAccessable)
        {
            QUAN::CircleKey key;
            key.id=lUId;

            QUAN::CircleUserValue value;

            int now=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->getHashMap(mCircleUserCache,key,value)==0)
            {
                if ( now - value.iTimespan < 3600 * 24 )
                {
                    vConcern=value.vConcern;
                    vAccessable=value.vAccessable;
                    return true;
                }
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
            return false;
        }

        void ClearCircleUser(taf::Int64 lUId)
        {
            QUAN::CircleKey key;
            key.id=lUId;

            if(CommHashMap::getInstance()->eraseHashMap(mCircleUserCache,key))
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<" FAILED"<<endl;
            }
        }

        void UpdateCircleLocation(taf::Int64 lCircleId,const vector<QUAN::CircleLocation>& vLocation)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            QUAN::CircleLocationValue value;
            value.iTimespan=TC_TimeProvider::getInstance()->getNow();
            value.vLocation=vLocation;

            if(CommHashMap::getInstance()->addHashMap(mCircleLocationCache,key,value)!=0)
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
        }
        
        bool GetCircleLocation(taf::Int64 lCircleId,vector<QUAN::CircleLocation>& vLocation)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            QUAN::CircleLocationValue value;

            int now=TC_TimeProvider::getInstance()->getNow();

            if(CommHashMap::getInstance()->getHashMap(mCircleLocationCache,key,value)==0)
            {
                if ( now - value.iTimespan < 3600 * 24 )
                {
                    vLocation=value.vLocation;
                    return true;
                }
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
            return false;
        }

        void ClearCircleLocation(taf::Int64 lCircleId)
        {
            QUAN::CircleKey key;
            key.id=lCircleId;

            if(CommHashMap::getInstance()->eraseHashMap(mCircleLocationCache,key))
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<" FAILED"<<endl;
            }
        }
        
        void UpdateCircleUnlock(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 iLockNum)
        {
            QUAN::CircleUnlockKey key;
            key.lUId=lUId;
            key.lCircleId=lCircleId;

            QUAN::CircleUnlockValue value;
            value.iTimespan=TC_TimeProvider::getInstance()->getNow();
            value.iLockNum=iLockNum;

            if(CommHashMap::getInstance()->addHashMap(mCircleUnlockCache,key,value)!=0)
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
        }

        bool GetCircleUnlock(taf::Int64 lUId,taf::Int64 lCircleId,taf::Int32 &iLockNum)
        {
            QUAN::CircleUnlockKey key;
            key.lUId=lUId;
            key.lCircleId=lCircleId;

            QUAN::CircleUnlockValue value;
            if(CommHashMap::getInstance()->getHashMap(mCircleUnlockCache,key,value)==0)
            {
                int now=TC_TimeProvider::getInstance()->getNow();
                if ( now - value.iTimespan < 3600 * 24 )
                {
                    iLockNum=value.iLockNum;
                    return true;
                }
            }
            else
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<"=>"<<JCETOSTR(value)<<" FAILED"<<endl;
            }
            return false;
        }

        void ClearCircleUnlock(taf::Int64 lUId,taf::Int64 lCircleId)
        {
            QUAN::CircleUnlockKey key;
            key.lUId=lUId;
            key.lCircleId=lCircleId;

            if(CommHashMap::getInstance()->eraseHashMap(mCircleUnlockCache,key))
            {
                LOG->error()<<__FUNCTION__<<"|KEY:"<<JCETOSTR(key)<<" FAILED"<<endl;
            }
        }
        
    protected:
        typedef JceHashMap<QUAN::CircleKey, QUAN::CircleValue, ThreadLockPolicy, ShmFileStorePolicy> CircleInfoCacheHashMap;
        CircleInfoCacheHashMap mCircleInfoCache;

        typedef JceHashMap<QUAN::CircleKey, QUAN::CircleLbsValue, ThreadLockPolicy, ShmFileStorePolicy> CircleLbsCacheHashMap;
        CircleLbsCacheHashMap mCircleLbsCache;

        typedef JceHashMap<QUAN::CircleKey, QUAN::CircleUserValue, ThreadLockPolicy, ShmFileStorePolicy> CircleUserCacheHashMap;
        CircleUserCacheHashMap mCircleUserCache;

        typedef JceHashMap<QUAN::CircleKey, QUAN::CircleLocationValue, ThreadLockPolicy, ShmFileStorePolicy> CircleLocationCacheHashMap;
        CircleLocationCacheHashMap mCircleLocationCache;

        typedef JceHashMap<QUAN::CircleUnlockKey, QUAN::CircleUnlockValue, ThreadLockPolicy, ShmFileStorePolicy> CircleUnlockCacheHashMap;
        CircleUnlockCacheHashMap mCircleUnlockCache;
};


#endif
