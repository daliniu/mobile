#ifndef __STAT_HASH_MAP_H_
#define __STAT_HASH_MAP_H_

#include "servant/StatF.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "StatQueryImp.h"

using namespace taf;

typedef JceHashMap<StatMicMsgHead, StatMicMsgBody, ThreadLockPolicy,FileStorePolicy> HashMap;
class StatHashMap;
extern TC_Config* g_pconf;
extern StatHashMap g_hashmap;

typedef std::map<taf::StatMicMsgHead, taf::StatMicMsgBody, std::less<taf::StatMicMsgHead>, __gnu_cxx::__pool_alloc<std::pair<taf::StatMicMsgHead const, taf::StatMicMsgBody> > > StatMsg;

class StatHashMap : public HashMap
{
public:


    /**
    * 增加数据
    * @param Key
    * @param Value
    *
    * @return int
    */
    int add(const taf::StatMicMsgHead &head, const taf::StatMicMsgBody &body)
    {
        StatMicMsgBody stBody;
        int ret = TC_HashMap::RT_OK;
        taf::JceOutputStream<BufferWriter> osk;
        head.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        string sv;
        time_t t = 0;
		{

			TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
            ret = this->_t.get(sk, sv,t);
        }
        if (ret < 0)
        {
            return -1;
        }
        //读取到数据了, 解包
        if (ret == TC_HashMap::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            stBody.readFrom(is);
        }
        stBody.count            += body.count;
        stBody.execCount        += body.execCount;
        stBody.timeoutCount     += body.timeoutCount;
        for(map<int,int>::const_iterator it = body.intervalCount.begin();it!= body.intervalCount.end();it++)
        {
            stBody.intervalCount[it->first] += it->second;
        }
        stBody.totalRspTime += body.totalRspTime;
        if(stBody.maxRspTime < body.maxRspTime)
        {
            stBody.maxRspTime = body.maxRspTime;
        }
        //非0最小值
        if( stBody.minRspTime == 0 || (stBody.minRspTime > body.minRspTime && body.minRspTime != 0))
        {
            stBody.minRspTime = body.minRspTime;
        }
        taf::JceOutputStream<BufferWriter> osv;
        stBody.writeTo(osv);
        string stemp(osv.getBuffer(), osv.getLength());
        vector<TC_HashMap::BlockData> vtData;
	    LOGINFO("StatHashMap::add sk.length:" << sk.length() << " v.length:" << stemp.length() << endl);
		{
			TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
		    int ret = 0  ;
			ret = this->_t.set(sk, stemp,true,vtData);
		    return   ret;
	    }
        return 0;
    }
};

#endif


