#ifndef __STAT_QUERY_HASH_MAP_IMP_H_
#define __STAT_QUERY_HASH_MAP_IMP_H_

#include "StatQuery.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"


using namespace taf;

typedef JceHashMap<StatQueryKey,StatQueryValue, ThreadLockPolicy, FileStorePolicy> QueryHashMap;
class StatQueryHashMap;
extern TC_Config* g_pconf;
extern StatQueryHashMap g_master_hashmap;
extern StatQueryHashMap g_slave_hashmap;

class StatQueryHashMap : public QueryHashMap
{
public:
    StatQueryHashMap()
    :_iMaxPageSize(20)
    ,_iMaxPageNum(10)
    {      
    };
    ~StatQueryHashMap()
    {
    };
     /**
     * load page info 
     * @return void
     */
     void intPageInfo(int pageSize,int pageNum)
    {
        _iMaxPageSize   = pageSize;  
        _iMaxPageNum    = pageNum;          
    };

public:
    /**
    * 增加数据
    * @param Key
    * @param Value
    *
    * @return int
    */
    int add(taf::StatQueryKey &key, const taf::StatQueryData &data)
    {
        StatQueryValue tValue;
        int ret = TC_HashMap::RT_OK;
        taf::JceOutputStream<BufferWriter> osk;
        key.writeTo(osk);
        string sk(osk.getBuffer(), osk.getLength());
        TC_LockT<ThreadLockPolicy::Mutex> lock(ThreadLockPolicy::mutex());
        string sv;
        time_t t = 0;
        ret = this->_t.get(sk, sv,t);

        if (ret < 0)
        {
            return -1;
        }
        //读取到数据了, 解包
        if (ret == TC_HashMap::RT_OK)
        {
            taf::JceInputStream<BufferReader> is;
            is.setBuffer(sv.c_str(), sv.length());
            tValue.readFrom(is);
        }
        //LOG->debug() << "------------------------|page|"<<key.page<<"|nextpage|"<<tValue.nextpage<<"|size|"<<tValue.dataList.size()<<endl;  
        if(tValue.dataList.size()< _iMaxPageSize)
        {
            tValue.dataList.push_back(data);
        }
        else
        {   
           
           key.page = (tValue.nextpage+1)%_iMaxPageNum;
           if(key.page == 0)
           {
               key.page ++;
           }
           //LOG->debug() << "------------------------|page|"<<key.page<<"|nextpage|"<<tValue.nextpage<<"|size|"<<tValue.dataList.size()<<endl;  
           taf::JceOutputStream<BufferWriter> osk;
           key.writeTo(osk);
           string sk(osk.getBuffer(), osk.getLength());
           taf::JceOutputStream<BufferWriter> osv;
           tValue.writeTo(osv);
           string stemp(osv.getBuffer(), osv.getLength());
           vector<TC_HashMap::BlockData> vtData;
           this->_t.set(sk, stemp,true,vtData);        
           
           tValue.nextpage = key.page;         
           key.page = 0;           
           tValue.dataList.clear();
           tValue.dataList.push_back(data);                                   
        }
        //LOG->debug() << "------------------------|page|"<<key.page<<"|nextpage|"<<tValue.nextpage<<"|size|"<<tValue.dataList.size()<<endl;  
        taf::JceOutputStream<BufferWriter> osv;
        tValue.writeTo(osv);
        string stemp(osv.getBuffer(), osv.getLength());
        vector<TC_HashMap::BlockData> vtData;
        return  this->_t.set(sk, stemp,true,vtData);
    }
private:
    unsigned int _iMaxPageSize;
    unsigned int _iMaxPageNum;
};

#endif


