#include "MultiHashMapTest.h"
#include "jmem/jmem_multi_hashmap.h"
#include "util/tc_shm.h"
#include "util/tc_option.h"
#include <stdlib.h>
#include <iostream>
#include "util/tc_common.h"
#include <sys/time.h>

using namespace taf;

typedef JceMultiHashMap<struct MKey, struct UKey, struct SValue, SemLockPolicy, ShmStorePolicy> SHashMap;


SHashMap			g_hmap;
bool				g_bSilent = false;		// 是否为安静模式，不输出日志，可以取得更高的速度

class TestToDoFunctor : public SHashMap::ToDoFunctor
{
public:
    virtual void del(bool bExists, const DataRecord &data)
	{
		if(!g_bSilent)
		{
			cout << __FUNCTION__ << "|" << bExists << "|" << data._mkey.mkey << "|" 
				<< data._ukey.ukey << "|" << data._value.value << "|"
				<< (int)data._iVersion << "|" << data._dirty << endl;
		}
	}
	
    virtual void sync(const DataRecord &data)
	{
		if(!g_bSilent)
		{
			cout << __FUNCTION__ << "|" << data._mkey.mkey << "|" 
				<< data._ukey.ukey << "|" << data._value.value << "|"
				<< (int)data._iVersion << "|" << data._dirty << endl;
		}
	}
	
	virtual void erase(const DataRecord &data)
	{
		if(!g_bSilent)
		{
			cout << __FUNCTION__ << "|" << data._mkey.mkey << "|" 
				<< data._ukey.ukey << "|" << data._value.value << "|"
				<< (int)data._iVersion << "|" << data._dirty << endl;
		}
	}

	/*virtual int get(DataRecord &data)
	{
		if(!g_bSilent)
		{
			cout << __FUNCTION__ << "|" << data._mkey.mkey << "|" 
				<< data._ukey.ukey << endl;
		}

		return TC_Multi_HashMap::RT_NO_GET;
	}*/
};

TestToDoFunctor		g_todo;

class NullMatch
{
public:
	bool operator()(const MKey &mk, const UKey uk)
    {
		return true;
    }
	bool operator()(const MKey &mk)
    {
		return true;
    }
};

////////////////////////////////////////////////////////////////////
//

// 普通的功能测试
struct FunctionTest
{
	static void initMap()
	{
		size_t iMinSize = 10, iMaxSize = 40;
		g_hmap.initDataBlockSize(iMinSize, iMaxSize, 2.0);

#if __WORDSIZE == 64
		size_t iMemSize    = 1024*20;
		key_t key = ftok("/usr/local/app/hendy/platform/local/taf/jmem_multi_hashmap", 'F');
#else
		size_t iMemSize    = 1024*10;
		key_t key = ftok("/usr/local/app/hendy/platform/local/taf/jmem_multi_hashmap", 'F');
#endif
		
		cout << "init mem，shm key: 0x" << hex << key << dec << endl;
		
		g_hmap.initStore(key, iMemSize);
		g_hmap.initLock(key);
		
		g_hmap.setToDoFunctor(&g_todo);
		g_hmap.setSyncTime(30);
		
	}
};

// 大数据量性能测试
struct PerformanceTest
{
	static void initMap()
	{
		size_t iMinSize = 10, iMaxSize = 40;
		g_hmap.initDataBlockSize(iMinSize, iMaxSize, 2.0);
		
		size_t iMemSize    = 1073741824;		// 1G
		key_t key = ftok("/usr/local/app/hendy/platform/local/taf/jmem_multi_hashmap", 'P');
		
		cout << "init mem，shm key: 0x" << hex << key << dec << endl;
		
		g_hmap.initStore(key, iMemSize);
		g_hmap.initLock(key);		
		g_hmap.setToDoFunctor(&g_todo);
		
	}
};

// 内存大小测试，测试一些结构在不同的机器上所占用字节数
void testMemSize()
{
	cout << "sizeof(int)=" << sizeof(int) << endl;
	cout << "sizeof(long)=" << sizeof(long) << endl;
	cout << "sizeof(size_t)=" << sizeof(size_t) << endl;
	cout << "sizeof(time_t)=" << sizeof(time_t) << endl;
	cout << "sizeof(tagBlockHead)=" << sizeof(TC_Multi_HashMap::Block::tagBlockHead) << endl;
	cout << "sizeof(tagMainKeyHead)=" << sizeof(TC_Multi_HashMap::MainKey::tagMainKeyHead) << endl;
	cout << "sizeof(tagChunkHead)=" << sizeof(TC_Multi_HashMap::Block::tagChunkHead) << endl;
	cout << "sizeof(tagHashItem)=" << sizeof(TC_Multi_HashMap::tagHashItem) << endl;
	cout << "sizeof(tagMainKeyHashItem)=" << sizeof(TC_Multi_HashMap::tagMainKeyHashItem) << endl;
}

void usage(char *argv)
{
    cout << argv << " --help\tshow this help message" << endl;
	cout << "\t --mem\tview memory size of some structs" << endl;
	cout << "\t --f\tdo the functionality test(default option)" << endl;
	cout << "\t --p\tdo the performance test" << endl;
	cout << "\t --s\tsilent mode in performance test, no output" << endl;
    cout << "\t --release\trelease the allocated share memory" << endl;
    cout << "\t --view\tview statistic information of this map" << endl;
    cout << "\t --set [--mkey=] [--ukey=] [--value=] [--version] [--clean] [--full] [--tail]" << endl;
    cout << "\t --get [--mkey=] [--ukey=] [--bycount]\tbycount param used when get data under mkey by count method" << endl;
	cout << "\t --gethash [--hash]" << endl;
	cout << "\t --gethashm [--hash]" << endl;
    cout << "\t --del [--mkey=] [--ukey=]" << endl;
    cout << "\t --sync [--mkey] [--ukey] [--bytime] [--onlyone]" << endl;
    cout << "\t --erase [--mkey] [--ukey] [--ratio] [--force] [--chkdirty]" << endl;
    cout << "\t --isdirty [--mkey=] [--ukey=]" << endl;
    cout << "\t --listh\tlist data by hash index" << endl;
	cout << "\t --listb [--desc]\tlist data by block chain" << endl;
	cout << "\t --listg [--desc]\tlist data by get time chain" << endl;
	cout << "\t --lists [--desc]\tlist data by set time chain" << endl;
    cout << "\t --listd\tlist data by dirty data chain" << endl;
    cout << "\t --set-batch [--count] [--avgrec] [--proc]\tdo batch set operation, will set total count records with avgrec records per main key" << endl;
    cout << "\t --get-batch [--count] [--avgrec] [--proc]\tdo batch get operation" << endl;
	cout << "\t --chkmk [--mkey]\tcheck if main key exists" << endl;
	cout << "\t --setstate [--mkey] [--ukey] [--dirty] [--clean] [--part] [--full]\tcheck if main key exists" << endl;
	cout << "\t --chkbad [--repair]" << endl;
}

void set(const string &m, const string &u, const string &v, uint8_t version, bool dirty, bool full, bool head)
{
	MKey mk;
	UKey uk;
	SValue sv;

	mk.mkey = m;
	uk.ukey = u;
	sv.value = v;

	TC_Multi_HashMap::DATATYPE eType = (full ? TC_Multi_HashMap::FULL_DATA : TC_Multi_HashMap::PART_DATA);

	timeval tv_1, tv_2;
	gettimeofday(&tv_1, NULL);
	
	int ret = TC_Multi_HashMap::RT_OK;
	if(u.empty())
	{
		// 设置主key的only key
		ret = g_hmap.set(mk);
	}
	else
	{
		if(v.empty())
		{
			// 设置主键的only key
			ret = g_hmap.set(mk, uk, eType, head);
		}
		else
		{
			ret = g_hmap.set(mk, uk, sv, version, dirty, eType, head);
		}
	}
	
	gettimeofday(&tv_2, NULL);
	
	long lTime = 0;
	if(tv_2.tv_usec < tv_1.tv_usec)
	{
		lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
	}
	else
	{
		lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
	}
	
	//assert(ret == 0);
	cout << "set:" << m << "|" << u << "|" << v << "|" << ret << "|" << lTime << endl;
}

void get(const string &m, const string &u, bool byCount = false)
{
	MKey mk;
	UKey uk;
	mk.mkey = m;
	uk.ukey = u;
	SHashMap::Value v;

	if(!u.empty())
	{
		// 根据主键查询数据
		int ret = g_hmap.get(mk, uk, v);
		cout << "get:" << m << "|" << u << "|" << v._value.value << "|" 
			<< (int)v._iVersion << "|" << v._dirty << "|" << v._iSyncTime << "|" << ret << endl;
	}
	else
	{
		// 查主key下的所有数据
		size_t count = g_hmap.count(mk);
		cout << "count of " << m << ": " << count << endl;
		if(byCount)
		{
			vector<SHashMap::Value> vs;
			int ret = g_hmap.get(mk, vs);
			for(size_t i = 0; i < vs.size(); i ++)
			{
				cout << vs[i]._mkey.mkey << "|" << vs[i]._ukey.ukey << "|" << vs[i]._value.value 
					<< "|" << (int)vs[i]._iVersion << "|" << vs[i]._dirty << endl;
			}
			if(ret != 0)
			{
				cout << "get error: " << ret << endl;
			}
		}
		else
		{
			SHashMap::lock_iterator it = g_hmap.find(mk);
			while(it != g_hmap.end())
			{
				SHashMap::Value v;
				int ret = it->get(v);
				cout << v._mkey.mkey << "|" << v._ukey.ukey << "|" << v._value.value << "|" 
					<< (int)v._iVersion << "|" << v._dirty << "|" << v._iSyncTime << "|" << ret << endl;
				it ++;
			}
		}
	}
}

void sync(const string &m, const string &u)
{
	MKey mk;
	UKey uk;
	mk.mkey = m;
	uk.ukey = u;

	int ret = g_hmap.sync(mk, uk);

	cout << "sync:" << m << "|" << u << "|" << ret << endl;
}

// 按时间回写
void sync(bool bOnlyOne)
{
	int ret = TC_Multi_HashMap::RT_OK;
	if(bOnlyOne)
	{
		g_hmap.sync();
		ret = g_hmap.syncOnce(time(NULL));
	}
	else
	{
		ret = g_hmap.sync(time(NULL));
	}

	cout << "sync by time: " << ret << endl;
}

void erase(const string &m, const string &u, bool bForce)
{
	MKey mk;
	UKey uk;
	mk.mkey = m;
	uk.ukey = u;

	int ret = TC_Multi_HashMap::RT_OK;
	if(u.empty())
	{
		if(bForce)
		{
			ret = g_hmap.eraseByForce(mk);
		}
		else
		{
			ret = g_hmap.erase(mk);
		}
	}
	else
	{
		if(bForce)
		{
			ret = g_hmap.eraseByForce(mk, uk);
		}
		else
		{
			ret = g_hmap.erase(mk, uk);
		}
	}

	cout << "erase:" << m << "|" << u << "|" << (bForce ? "force" : "noforce") << "|" << ret << endl;
}

void erase(int ratio, bool bCheckDirty)
{
	int ret = g_hmap.erase(ratio, bCheckDirty);

	cout << "erase by ratio:" << ratio << "|" << (bCheckDirty ? "checkdirty" : "nocheckdirty") << "|" << ret << endl;
}

void listh()
{
	const TC_Multi_HashMap::tagMapHead &head = g_hmap.getMapHead();
	cout << "list hash count: " << head._iElementCount << endl;
	SHashMap::hash_iterator it = g_hmap.hashBegin();
	while(it != g_hmap.hashEnd())
	{
		vector<SHashMap::Value> vv;
		it->get(vv);
		++it;
		
		for(size_t i = 0; i < vv.size(); i ++)
		{
			cout << vv[i]._mkey.mkey << "|" << vv[i]._ukey.ukey << "|" << vv[i]._value.value 
				<< "|" << (int)vv[i]._iVersion << "|" << vv[i]._dirty << "|" << vv[i]._iSyncTime << endl;
		}
	}
}

void list(int type, bool asc = true)
{
	SHashMap::lock_iterator it = g_hmap.end();
	switch(type)
	{
	case 0:
		// 按block链遍历
		if(asc)
		{
			// 升序
			it = g_hmap.begin();
		}
		else
		{
			// 降序
			it = g_hmap.rbegin();
		}
		break;
	case 1:
		// 按get时间链遍历
		if(asc)
		{
			// 升序
			it = g_hmap.beginGetTime();
		}
		else
		{
			// 降序
			it = g_hmap.rbeginGetTime();
		}
		break;
	case 2:
		// 按set时间链遍历
		if(asc)
		{
			// 升序
			it = g_hmap.beginSetTime();
		}
		else
		{
			// 降序
			it = g_hmap.rbeginSetTime();
		}
		break;
	case 3:
		// 按脏数据链遍历
		it = g_hmap.beginDirty();
		break;
	default:
		return;
	}
	
	while(it != g_hmap.end())
	{
		SHashMap::Value v;
		it->get(v);
		++it;
		
		cout << v._mkey.mkey << "|" << v._ukey.ukey << "|" << v._value.value 
			<< "|" << (int)v._iVersion << "|" << v._dirty << "|" << v._iSyncTime << endl;
	}
}

void batchset(size_t iCount)
{
	MKey mk;
	UKey uk;
	SValue sv;

	int pid = getpid();

	timeval tv_1, tv_2;
	gettimeofday(&tv_1, NULL);

	for(size_t i = 0; i < iCount; i ++)
	{
		mk.mkey = TC_Common::tostr(rand()%1000000);		// 100万，避免主key过多重复，同时注意与chunk的大小匹配
		uk.ukey = TC_Common::tostr(rand()%65535);
		sv.value = mk.mkey + "_" + uk.ukey;

		timeval tv_1, tv_2;
		if(!g_bSilent)
		{
			gettimeofday(&tv_1, NULL);
		}
		
		int ret = g_hmap.set(mk, uk, sv, 0, true, TC_Multi_HashMap::AUTO_DATA, true);
		
		if(!g_bSilent)
		{
			gettimeofday(&tv_2, NULL);
			long lTime = 0;
			if(tv_2.tv_usec < tv_1.tv_usec)
			{
				lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
			}
			else
			{
				lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
			}
			
			//assert(ret == 0);
			cout << "set:" << pid << "|" << mk.mkey << "|" << uk.ukey << "|" << sv.value << "|" << ret << "|" << lTime << endl;
		}
	}

	gettimeofday(&tv_2, NULL);
	long lTime = 0;
	if(tv_2.tv_usec < tv_1.tv_usec)
	{
		lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
	}
	else
	{
		lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
	}
	cout << "batchset:" << pid << "|" << iCount << "|" << lTime << endl;
}

void batchset()
{
	vector<SHashMap::Value> vtSet;

	SHashMap::Value v;

	v._dirty = true, v._iVersion = 0, v._iSyncTime = 0;

	v._mkey.mkey = "1", v._ukey.ukey = "1", v._value.value = "11";
	vtSet.push_back(v);

	v._mkey.mkey = "1", v._ukey.ukey = "2", v._value.value = "12";
	vtSet.push_back(v);

	v._mkey.mkey = "2", v._ukey.ukey = "1", v._value.value = "21";
	vtSet.push_back(v);

	v._mkey.mkey = "3", v._ukey.ukey = "1", v._value.value = "31";
	vtSet.push_back(v);

	v._mkey.mkey = "4", v._ukey.ukey = "1", v._value.value = "41";
	vtSet.push_back(v);


	int ret = g_hmap.set(vtSet, TC_Multi_HashMap::AUTO_DATA, true, true);

	for(size_t i = 0; i < vtSet.size(); i ++)
	{
		cout << "set:" << vtSet[i]._mkey.mkey << "|" << vtSet[i]._ukey.ukey << "|" << vtSet[i]._value.value << "|" << ret << endl;
	}
}

void batchget(size_t iCount)
{
	MKey mk;
	UKey uk;
	SHashMap::Value v;

	int pid = getpid();

	timeval tv_1, tv_2;
	gettimeofday(&tv_1, NULL);
	
	for(size_t i = 0; i < iCount; i ++)
	{
		mk.mkey = TC_Common::tostr(rand()%1000000);		// 100万，避免主key过多重复，同时注意与chunk的大小匹配
		uk.ukey = TC_Common::tostr(rand()%65535);

		{
			timeval tv_1, tv_2;
			if(!g_bSilent)
			{
				gettimeofday(&tv_1, NULL);
			}
			
			int ret = g_hmap.get(mk, uk, v);
			
			if(!g_bSilent)
			{
				gettimeofday(&tv_2, NULL);
				long lTime = 0;
				if(tv_2.tv_usec < tv_1.tv_usec)
				{
					lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
				}
				else
				{
					lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
				}
				
				//assert(ret == 0);
				cout << "get:" << pid << "|" << mk.mkey << "|" << uk.ukey << "|" << v._value.value << "|" 
					<< (int)v._iVersion << "|" << v._dirty << "|" << v._iSyncTime << "|" << ret << endl;
			}
		}
	}
	
	gettimeofday(&tv_2, NULL);
	long lTime = 0;
	if(tv_2.tv_usec < tv_1.tv_usec)
	{
		lTime = (tv_2.tv_sec -1 - tv_1.tv_sec) * 1000000;
	}
	else
	{
		lTime = (tv_2.tv_sec - tv_1.tv_sec) * 1000000 + (tv_2.tv_usec - tv_1.tv_usec);
	}
	cout << "batchset:" << pid << "|" << iCount << "|" << lTime << endl;
}

int startMultiHashMapTest(int argc, char *argv[])
{
	srand(time(NULL));

	try
    {
        TC_Option option;
        option.decode(argc, argv);

        map<string, string> m = option.getMulti();

        if(option.hasParam("help") || option.getMulti().empty())
        {
            usage(argv[0]);
            return 0;
        }

		if(option.hasParam("mem"))
		{
			testMemSize();
			return 0;
		}
		
		g_bSilent = option.hasParam("s");

		// 随机设置和获取时也当作性能测试
		bool bPerfTest = option.hasParam("p");
		if(bPerfTest)
		{
			PerformanceTest::initMap();
		}
		else
		{
			FunctionTest::initMap();
		}
		
        if(option.hasParam("release"))
        {
            g_hmap.release();
            return 0;
        }

		if(option.hasParam("view"))
        {
            cout << g_hmap.desc() << endl;
        }
        else if(option.hasParam("set"))
        {
            if(option.getValue("mkey").empty())
			{
				usage(argv[0]);
				return 0;
			}

            string mk = option.getValue("mkey");
			string uk = option.getValue("ukey");
            string v = option.getValue("value");
			uint8_t version = option.getValue("version").empty() ? 0 : atoi(option.getValue("version").c_str());
			bool dirty = !option.hasParam("clean");
			bool full = option.getValue("full").empty() ? false : true;
			bool head = !option.hasParam("tail");
			
			set(mk, uk, v, version, dirty, full, head);
        }
        else if(option.hasParam("get"))
        {
            string mk;
			string uk;
            mk = option.getValue("mkey");
			uk = option.getValue("ukey");
			
			if(mk.empty())
			{
				usage(argv[0]);
				return 0;
			}
			
			get(mk, uk, option.hasParam("bycount"));
        }
		else if(option.hasParam("gethash"))
		{
			if(option.getValue("hash").empty())
			{
				usage(argv[0]);
				return 0;
			}
			vector<SHashMap::Value> vs;
			int ret = g_hmap.getHash(TC_Common::strto<size_t>(option.getValue("hash")), vs, NullMatch());
			cout << "count of hash " << option.getValue("hash") << ": " << vs.size() << ", ret=" << ret << endl;
			for(size_t i = 0; i < vs.size(); i ++)
			{
				cout << vs[i]._mkey.mkey << "|" << vs[i]._ukey.ukey << "|" << vs[i]._value.value 
					<< "|" << (int)vs[i]._iVersion << "|" << vs[i]._dirty << endl;
			}
		}
		else if(option.hasParam("gethashm"))
		{
			if(option.getValue("hash").empty())
			{
				usage(argv[0]);
				return 0;
			}
			map<MKey, vector<SHashMap::Value> > mv;
			int ret = g_hmap.getHashM(TC_Common::strto<size_t>(option.getValue("hash")), mv, NullMatch());
			cout << "count of mainkey hash " << option.getValue("hash") << ": " << mv.size() << ", ret=" << ret << endl;
			map<MKey, vector<SHashMap::Value> >::iterator it = mv.begin();
			while(it != mv.end())
			{
				vector<SHashMap::Value> &vs = it->second;
				for(size_t i = 0; i < vs.size(); i ++)
				{
					cout << vs[i]._mkey.mkey << "|" << vs[i]._ukey.ukey << "|" << vs[i]._value.value 
						<< "|" << (int)vs[i]._iVersion << "|" << vs[i]._dirty << endl;
				}
				it ++;
			}
		}
        else if(option.hasParam("del"))
        {
            MKey mk;
			UKey uk;
            mk.mkey = option.getValue("mkey");
			uk.ukey = option.getValue("ukey");
			if(mk.mkey.empty() || uk.ukey.empty())
			{
				usage(argv[0]);
				return 0;
			}
            int ret = g_hmap.del(mk, uk);
            cout << "del:" << mk.mkey << "|" << uk.ukey << "|" << ret << endl;
        }
        else if(option.hasParam("sync"))
        {
			string mk = option.getValue("mkey");
			string uk = option.getValue("ukey");
			bool bSyncByTime = option.hasParam("bytime");
			bool bSyncOnlyOne = option.hasParam("onlyone");

			if(!mk.empty() && !uk.empty())
			{
				sync(mk, uk);
			}
			else if(bSyncByTime)
			{
				sync(bSyncOnlyOne);
			}
			else
			{
				usage(argv[0]);
				return 0;
			}
        }
        else if(option.hasParam("erase"))
        {
			string mk = option.getValue("mkey");
			string uk = option.getValue("ukey");
            string ratio = option.getValue("ratio");
			bool bForce = option.hasParam("force");
			bool bCheckDirty = option.hasParam("chkdirty");
			if(!mk.empty())
			{
				erase(mk, uk, bForce);
			}
			else if(!ratio.empty())
			{
				erase(TC_Common::strto<int>(ratio), bCheckDirty);
			}
			else
			{
				usage(argv[0]);
				return 0;
			}
		}			
        else if(option.hasParam("isdirty"))
        {
            MKey mk;
			UKey uk;
            mk.mkey = option.getValue("mkey");
			uk.ukey = option.getValue("ukey");
			int ret = TC_Multi_HashMap::RT_OK;
			if(mk.mkey.empty())
			{
				usage(argv[0]);
				return 0;
			}
			if(uk.ukey.empty())
			{
				ret = g_hmap.checkDirty(mk);
			}
			else
			{
				ret = g_hmap.checkDirty(mk, uk);
			}
            cout << "isdirty:" << mk.mkey << "|" << uk.ukey << "|" << ret << endl;
        }
		else if(option.hasParam("chkmk"))
		{
			MKey mk;
			mk.mkey = option.getValue("mkey");
			if(mk.mkey.empty())
			{
				usage(argv[0]);
				return 0;
			}
			
			int ret = g_hmap.checkMainKey(mk);
			if(ret == TC_Multi_HashMap::RT_OK)
			{
				cout << "main key " << mk.mkey << " exists" << endl;
			}
			else if(ret == TC_Multi_HashMap::RT_ONLY_KEY)
			{
				cout << "main key " << mk.mkey << " exists, but only key" << endl;
			}
			else if(ret == TC_Multi_HashMap::RT_PART_DATA)
			{
				cout << "main key " << mk.mkey << " exists, and possible part data" << endl;
			}
			else if(ret == TC_Multi_HashMap::RT_NO_DATA)
			{
				cout << "main key " << mk.mkey << " not exists" << endl;
			}
			else
			{
				cout << "chkmk " << mk.mkey << " failed: " << ret << endl;
			}
		}
        else if(option.hasParam("listh"))
        {
			listh();
        }
        else if(option.hasParam("listb"))
        {
			if(option.hasParam("desc"))
			{
				list(0, false);
			}
			else
			{
				list(0, true);
			}
        }
        else if(option.hasParam("listg"))
        {
			if(option.hasParam("desc"))
			{
				list(1, false);
			}
			else
			{
				list(1, true);
			}
        }
        else if(option.hasParam("lists"))
        {
			if(option.hasParam("desc"))
			{
				list(2, false);
			}
			else
			{
				list(2, true);
			}
        }
        else if(option.hasParam("listd"))
        {
			list(3);
        }
		else if(option.hasParam("set-batch"))
		{
			string sCount = option.getValue("count");
			string sProcNum = option.getValue("proc");
			if(!sCount.empty())
			{
				if(!sProcNum.empty())
				{
					// 启多个进程
					int iProcNum = TC_Common::strto<int>(sProcNum);
					for(int i = 0; i < iProcNum - 1; i ++)
					{
						if(fork() == 0)
						{
							// 子进程
							break;
						}
					}
				}
				batchset(TC_Common::strto<size_t>(sCount));
			}
			else
			{
				// 不带参数的批量设置
				batchset();
			}
		}
		else if(option.hasParam("get-batch"))
		{
			string sCount = option.getValue("count");
			string sProcNum = option.getValue("proc");
			if(sCount.empty())
			{
				usage(argv[0]);
				return 0;
			}
			
			if(!sProcNum.empty())
			{
				// 启多个进程
				int iProcNum = TC_Common::strto<int>(sProcNum);
				for(int i = 0; i < iProcNum; i ++)
				{
					if(fork() == 0)
					{
						// 子进程
						break;
					}
				}
			}
			batchget(TC_Common::strto<size_t>(sCount));
		}
		else if(option.hasParam("setstate"))
		{
			MKey mk;
			UKey uk;
			mk.mkey = option.getValue("mkey");
			uk.ukey = option.getValue("ukey");
			bool bDirty = option.hasParam("dirty");
			bool bClean = option.hasParam("clean");
			bool bFull = option.hasParam("full");
			bool bPart = option.hasParam("part");
			if(mk.mkey.empty())
			{
				usage(argv[0]);
				return 0;
			}
			if(!uk.ukey.empty())
			{
				if(bDirty)
				{
					int ret = g_hmap.setDirty(mk, uk);
					cout << "setdirty:" << mk.mkey << "|" << uk.ukey << "|" << ret << endl;
				}
				else if(bClean)
				{
					int ret = g_hmap.setClean(mk, uk);
					cout << "setclean:" << mk.mkey << "|" << uk.ukey << "|" << ret << endl;
				}
				else
				{
					usage(argv[0]);
					return 0;
				}
			}
			else
			{
				if(bFull)
				{
					int ret = g_hmap.setFullData(mk, true);
					cout << "set full data:" << mk.mkey << "|" << ret << endl;
				}
				else if(bPart)
				{
					int ret = g_hmap.setFullData(mk, false);
					cout << "set part data:" << mk.mkey << "|" << ret << endl;
				}
				else
				{
					usage(argv[0]);
					return 0;
				}
			}
		}
		else if(option.hasParam("chkbad"))
		{
			size_t iCount = g_hmap.checkBadBlock(option.hasParam("repair"));
			cout << iCount << " bad blocks in total" << endl;
		}
        else
        {
            usage(argv[0]);
        }
	}
	catch(exception &e)
	{
		cerr << "error! " << e.what() << endl;
	}

	return 0;
}

