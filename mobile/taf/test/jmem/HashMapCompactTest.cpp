#include "HashMapTest.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_thread_pool.h"
#include "util/tc_file.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap_compact.h"

using namespace taf;

namespace c
{

//JceHashMapCompact<Test::MapKey, Test::MapValue, SemLockPolicy, ShmStorePolicy> g_hashmap;
//typedef JceHashMapCompact<Test::MapKey, Test::MapValue, SemLockPolicy, FileStorePolicy> HashMap;
typedef JceHashMapCompact<Test::MapKey, Test::MapValue, ThreadLockPolicy, FileStorePolicy> HashMap;


HashMap g_hashmap;

class TestToDoFunctor : public HashMap::ToDoFunctor
{
protected:

    virtual void erase(const TestToDoFunctor::DataRecord &data)
    {
        cout << "TestToDoFunctor::erase::" << data._key.s << ":" << data._value.s << ":" << data._dirty << ":" << data._iSyncTime << endl;
    }

    virtual void del(bool bExists, const TestToDoFunctor::DataRecord &data)
    {
        cout << "TestToDoFunctor::del::" << bExists << ":" << data._key.s << ":" << data._value.s << ":" << data._dirty << ":" << data._iSyncTime << endl;
    }

    virtual void sync(const TestToDoFunctor::DataRecord &data)
    {
        cout << "TestToDoFunctor::sync::" << data._key.s << ":" << data._value.s << ":" << data._dirty << ":" << data._iSyncTime << endl;
    }

    virtual void backup(const TestToDoFunctor::DataRecord &data)
    {
        cout << "TestToDoFunctor::backup::" << data._key.s << ":" << data._value.s << ":" << data._dirty << ":" << data._iSyncTime << endl;
    }

    virtual int get(TestToDoFunctor::DataRecord &data)
    {
        ostringstream s;
        s << rand();

        data._value.s   = s.str();
        data._iSyncTime = 0;
        cout << "TestToDoFunctor::gec::" << data._key.s << ":" << data._value.s << endl;
//        return TC_HashMapCompact::RT_NO_DATA;
        return TC_HashMapCompact::RT_OK;
    }
};

TestToDoFunctor g_todo;

void initHashMap()
{
    size_t n = 20480;

//  g_hashmap.initDataBlockSize(10, 40, 1.2);
    g_hashmap.initDataBlockSize(10, 20, 1.2);

    if(TC_File::isFileExist("hashmap-compact.dat"))
    {
        n = TC_File::getFileSize("hashmap-compact.dat");
    }
    g_hashmap.initStore("hashmap-compact.dat", n);
    g_hashmap.setAutoErase(true);
//    int ret = g_hashmap.initStore(3434, n);
//    g_hashmap.initLock(1019);
  //  g_hashmap.setSyncTime(5);
//    g_hashmap.setToDoFunctor(&g_todo);
}

void testAppend()
{
    cout << g_hashmap.getMapHead()._iMemSize << endl;
    size_t iNewSize = g_hashmap.getMapHead()._iMemSize * 2;

    g_hashmap.expand(iNewSize);
}

vector<pair<Test::MapKey, Test::MapValue> > prepare()
{
    cout << "start pareparing data" << endl;

    size_t count = 10000;
    vector<pair<Test::MapKey, Test::MapValue> > v;

    srand(time(NULL));

    vector<string> vs;
    for(size_t i = 0; i < 20; i++)
    {
        string tmp;
        size_t n = rand() % 300;
        for(size_t i = 0; i < n; i++)
        {
            tmp += "a";
        }
        vs.push_back(tmp + TC_Common::tostr(rand()));
    }

    for(size_t i = 0; i < count; i++)
    {
        pair<Test::MapKey, Test::MapValue> data;

        ostringstream s;
        s << i << "_" << rand();
        data.first.s    = s.str() + "_" + vs[rand() % vs.size()];
        data.second.s   = s.str() + "_" + vs[rand() % vs.size()];

        v.push_back(data);
    }

    cout << "pareparing data OK:" << v.size() << endl;
    return v;
}

void testSetAppend()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    for(size_t i = 0; i < 10000000; i++)
    {
        size_t index = rand() % v.size();

        {
//        unsigned int allChunk = g_hashmap.allBlockChunkCount();
//        unsigned int useChunk = g_hashmap.getMapHead()._iUsedChunk;

//        if ( useChunk * 10 >= allChunk * 9 )
        if(i % 1000 == 0)
        {
//            if(TC_File::isFileExist("test.hm"))
            {
                size_t totalSize = (size_t)TC_File::getFileSize("hashmap.dat") + 10240; 

        //        cout << "expand:" << totalSize << ":" << allChunk << ":" << useChunk << endl;//<< g_hashmap.desc() << endl;
                cout << "expand:" << totalSize << endl;
                int ret = g_hashmap.expand( totalSize );
                cout << ret << endl;
            } 
        }

        }
        g_hashmap.set(v[index].first, v[index].second);
    }
}

void testAppendThreadPool()
{
    TC_ThreadPool tpool;
    //4个线程
    tpool.init(10);

    TC_Functor<void> init(testSetAppend);
    TC_Functor<void>::wrapper_type iwt(init);
    //启动线程, 指定初始化对象,也可以没有初始化对象:tpool.start();
    tpool.start();

    for(size_t i = 0; i < tpool.getThreadNum(); i++)
    {
        tpool.exec(iwt);
    }

    tpool.waitForAllDone();
}

void testSetRand()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    for(size_t i = 0; i < 10000000; i++)
    {
        size_t index = rand() % v.size();

        int ret = g_hashmap.set(v[index].first, v[index].second);
        cout << "set:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
        Test::MapValue s;
        ret = g_hashmap.get(v[index].first, s);
//        assert(v[index].second == s);
    }

    cout << g_hashmap.size() << "|" << g_hashmap.dirtyCount() << endl;
}

void testSetRandOne()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    size_t index = rand() % v.size();

    int ret = g_hashmap.set(v[index].first, v[index].second);
    cout << "set:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
    Test::MapValue s;
    ret = g_hashmap.get(v[index].first, s);


    cout << g_hashmap.size() << "|" << g_hashmap.dirtyCount() << endl;
}

void testGetRand()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    for(int i = 0; i < 200000; i++)
    {
        size_t index = rand() % v.size();

        int ret = g_hashmap.get(v[index].first, v[index].second);
        cout << "get:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
        assert(ret == 0);
    }

    cout << g_hashmap.size() << "|" << g_hashmap.dirtyCount() << endl;
}

void testSetOne(const string &k, const string &v)
{
    pair<Test::MapKey, Test::MapValue> vv;
    vv.first.s  = k;
    vv.second.s = v;

    int ret = g_hashmap.set(vv.first, vv.second);
    cout << "set-taf:" << k << ":" << v << " ret=" << ret << endl;
}

void testSetOneFull(const string &k, const string &v, const uint8_t iVer, const uint32_t iExpire)
{
    pair<Test::MapKey, Test::MapValue> vv;
    vv.first.s  = k;
    vv.second.s = v;

    int ret = g_hashmap.set(vv.first, vv.second, true, iExpire, iVer);
    cout << "set-taf:" << k << ":" << v << " ret=" << ret << endl;
}

void testDel(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.del(mk);
    cout << "del:" << k << " ret=" << ret << endl;
}

void testErase(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.erase(mk);
    cout << "erase:" << k << " ret=" << ret << endl;
}

void testGet(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    Test::MapValue mv;
	uint32_t iSyncTime;
	uint32_t iExpireTime;
	uint8_t iVersion;
    int ret = g_hashmap.get(mk, mv, iSyncTime, iExpireTime, iVersion);
    cout << "get:" << k << ":" << mv.s << ":" << iSyncTime << ":" << iExpireTime << ":" << (int) iVersion << " ret=" << ret << endl;
}

void testSetKey(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.set(mk);
    cout << "set-key:" << k << " ret=" << ret << endl;
}

void testSetDirty(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.setDirty(mk);
    cout << "setdirty:" << k << " ret=" << ret << endl;
}

void testSetClean(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.setClean(mk);
    cout << "setdirty:" << k << " ret=" << ret << endl;
}

void testCheckDirty(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_hashmap.checkDirty(mk);
    cout << "checkdirty:" << k << " ret=" << ret << endl;
}

void testReadOnly(const string &k)
{
    if(k == "yes")
    {
        g_hashmap.setReadOnly(true);
    }
    else
    {
        g_hashmap.setReadOnly(false);
    }
    cout << "setReadOnly:" << k << endl;
}

void testAutoErase(const string &k)
{
    if(k == "yes")
    {
        g_hashmap.setAutoErase(true);
    }
    else
    {
        g_hashmap.setAutoErase(false);
    }
    cout << "setAutoErase:" << k << endl;
}

void testSync()
{
    g_hashmap.sync(time(NULL));
    cout << "testSync OK" << endl;
}

void testBackup()
{
    g_hashmap.backup();
    cout << "testBackup OK" << endl;
}

void testClear()
{
    g_hashmap.clear();
    cout << "testClear OK" << endl;
}

void testEraseAll(int radio)
{
    int ret = g_hashmap.erase(radio, true);
    cout << "testEraseAll:" << ret << endl;
}

struct C
{
    bool operator()(const Test::MapKey &k)
    {
        return true;
    }
};

void testGetHash(size_t i)
{
    vector<pair<Test::MapKey, Test::MapValue> > mvv;
    
    g_hashmap.getHash(i, mvv, C());
    for(size_t i = 0; i < mvv.size(); i++)
    {
        cout << mvv[i].first.s << ":" << mvv[i].second.s << endl;
    }
}

void testListBlock()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.begin();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testListRBlock()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.rbegin();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testSetListTime()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.beginSetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << n << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testSetListRTime()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.rbeginSetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testGetListTime()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.beginGetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testGetListRTime()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.rbeginGetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << ":" << k.s << ":" << v.s<< endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testListDirty()
{
    int n = 0;
    HashMap::lock_iterator it = g_hashmap.beginDirty();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            return;
        }

        k.display(cout);
        cout << "|";
        v.display(cout);
        cout << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testListHash()
{
    int n = 0;
    HashMap::hash_iterator it = g_hashmap.hashBegin();
    while(it != g_hashmap.hashEnd())
    {
        vector<pair<Test::MapKey, Test::MapValue> > vp;

        it->get(vp);

        for(size_t i = 0; i < vp.size(); i++)
        {
            cout << vp[i].first.displaySimple(cout);
            cout << "|";
            vp[i].second.displaySimple(cout);
            cout << endl;
        }
        cout << "**********************************" << endl;

        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testView()
{
    cout << g_hashmap.desc() << endl;
}

void testRecover(const string &s)
{
    cout << "recover:" << g_hashmap.recover(s == "yes") << endl;
}

void testSetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    HashMap::lock_iterator it = g_hashmap.beginSetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            ++it;
            continue;
        }

        k.display(cout);
        vk.push_back(k);

        ++it;
        ++n;
    }
    }

    for(size_t i = 0; i < vk.size(); i++)
    {
        g_hashmap.erase(vk[i]);
    }
}

void testRSetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    HashMap::lock_iterator it = g_hashmap.rbeginSetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            ++it;
            continue;
        }

        k.display(cout);
        vk.push_back(k);

        ++it;
        ++n;
    }
    }

    for(size_t i = 0; i < vk.size(); i++)
    {
        g_hashmap.erase(vk[i]);
    }
}


void testGetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    HashMap::lock_iterator it = g_hashmap.beginGetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            ++it;
            continue;
        }

        k.display(cout);
        vk.push_back(k);

        ++it;
        ++n;
    }
    }

    for(size_t i = 0; i < vk.size(); i++)
    {
        g_hashmap.erase(vk[i]);
    }
}

void testRGetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    HashMap::lock_iterator it = g_hashmap.rbeginGetTime();
    while(it != g_hashmap.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_HashMapCompact::RT_OK && ret != TC_HashMapCompact::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
            ++it;
            continue;
        }

        k.display(cout);
        vk.push_back(k);

        ++it;
        ++n;
    }
    }

    for(size_t i = 0; i < vk.size(); i++)
    {
        g_hashmap.erase(vk[i]);
    }
}

void testClearSet()
{

    g_hashmap.clear();
    cout << g_hashmap.desc() << endl;

    for(int i=0;i<1000;i++)
    {
        Test::MapKey k;
        Test::MapValue v;
        k.s = "111111111231423156463890909090909090909090909046666667"+TC_Common::tostr(i);
        v.s = "321y9645917hehclmwwfvopkgm,;'ml;ffbf b er ntr n"+TC_Common::tostr(i);       
        g_hashmap.set(k,v);
    }
   
}

void testBatchMark()
{
//    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    int64_t n = TC_Common::now2ms();

    Test::MapKey k;
    Test::MapValue v;
    k.s = "12345678901234567890";
    v.s = "111111111231423156463890909090909090909090909046666667111111111231423156463890909090909090909090909046666667";

    for(size_t i = 0; i < 1000000; i++)
    {
        g_hashmap.set(k, v);
    }

    int64_t m = TC_Common::now2ms();

    cout << (m - n) << endl;
}

void testGetExpire()
{
	int n = 0;
    HashMap::hash_iterator it = g_hashmap.hashBegin();
    while(it != g_hashmap.hashEnd())
    {
        vector<pair<Test::MapKey, Test::MapValue> > vp;

        it->getExpire(time(NULL), vp);

        for(size_t i = 0; i < vp.size(); i++)
        {
            cout << vp[i].first.displaySimple(cout);
            cout << "|";
            vp[i].second.displaySimple(cout);
            cout << endl;
        }
        //cout << "**********************************" << endl;

        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void usage(char *argv)
{
    cout << argv << " --help" << endl;
    cout << "\t --release" << endl;
    cout << "\t --view" << endl;
    cout << "\t --sync" << endl;
    cout << "\t --backup" << endl;
    cout << "\t --clear" << endl;
    cout << "\t --append" << endl;
    cout << "\t --set-append" << endl;
    cout << "\t --list-block" << endl;
    cout << "\t --list-rblock" << endl;
    cout << "\t --list-set" << endl;
    cout << "\t --list-rset" << endl;
    cout << "\t --list-get" << endl;
    cout << "\t --list-rget" << endl;
    cout << "\t --listd" << endl;
    cout << "\t --list-hash" << endl;
    cout << "\t --set-rand" << endl;
    cout << "\t --get-rand" << endl;
    cout << "\t --set-taf --key= --value= --ver= --expire=" << endl;
    cout << "\t --set-key= " << endl;
    cout << "\t --del=" << endl;
    cout << "\t --erase=" << endl;
    cout << "\t --get=" << endl;
    cout << "\t --gethash=" << endl;
    cout << "\t --eraseall=" << endl;
    cout << "\t --recover=yes|no" << endl;
    cout << "\t --readonly=yes|no" << endl;
    cout << "\t --autoerase=yes|no" << endl;
    cout << "\t --check-dirty=" << endl;
    cout << "\t --set-dirty=" << endl;
    cout << "\t --set-clean=" << endl;
    cout << "\t --erase-set" << endl;
    cout << "\t --erase-rset" << endl;
    cout << "\t --erase-get" << endl;
    cout << "\t --erase-rget" << endl;
    cout << "\t --batchmark" << endl;
	cout << "\t --get-expire" << endl;
}

}

int startHashMapCompactTest(int argc, char *argv[])
{
    try
    {
        TC_Option option;
        option.decode(argc, argv);

        map<string, string> m = option.getMulti();

        if(option.hasParam("help") || option.getMulti().empty())
        {
            c::usage(argv[0]);
            return 0;
        }

        c::initHashMap();

        if(option.hasParam("release"))
        {
//            g_hashmap.release();
            return 0;
        }

        while(true)
        {

        if(option.hasParam("set-rand"))
        {
            c::testSetRand();
            break;
        }
        else if(option.hasParam("set-one"))
        {
            c::testSetRandOne();
            break;
        }
        else if(option.hasParam("append"))
        {
            c::testAppend();
            break;
        }
        else if(option.hasParam("set-append"))
        {
            c::testAppendThreadPool();
            break;
        }
        else if(option.hasParam("get-rand"))
        {
            c::testGetRand();
        }
        else if(option.hasParam("set-taf"))
        {
			uint8_t iVer = 0;
			uint32_t iExpire = 0;
			if(option.hasParam("ver"))
				iVer = TC_Common::strto<uint32_t>(option.getValue("ver"));
			if(option.hasParam("expire"))
				iExpire = time(NULL) + TC_Common::strto<uint32_t>(option.getValue("expire"));

			c::testSetOneFull(option.getValue("key"), option.getValue("value"), iVer, iExpire);
            break;
        }
        else if(option.hasParam("del"))
        {
            c::testDel(option.getValue("del"));
            break;
        }
        else if(option.hasParam("erase"))
        {
            c::testErase(option.getValue("erase"));
            break;
        }
        else if(option.hasParam("eraseall"))
        {
            c::testEraseAll(TC_Common::strto<int>(option.getValue("eraseall")));
            c::testView();
            sleep(5);
        }
        else if(option.hasParam("get"))
        {
            c::testGet(option.getValue("get"));
            break;
        }
        else if(option.hasParam("set-key"))
        {
            c::testSetKey(option.getValue("set-key"));
            break;
        }
        else if(option.hasParam("readonly"))
        {
            c::testReadOnly(option.getValue("readonly"));
            break;
        }
        else if(option.hasParam("autoerase"))
        {
            c::testAutoErase(option.getValue("autoerase"));
            break;
        }
        else if(option.hasParam("check-dirty"))
        {
            c::testCheckDirty(option.getValue("check-dirty"));
            break;
        }
        else if(option.hasParam("set-dirty"))
        {
            c::testSetDirty(option.getValue("set-dirty"));
            break;
        }
        else if(option.hasParam("gethash"))
        {
            c::testGetHash(TC_Common::strto<size_t>(option.getValue("gethash")));
            break;
        }
        else if(option.hasParam("set-clean"))
        {
            c::testSetClean(option.getValue("set-clean"));
            break;
        }
        else if(option.hasParam("clean-set"))
        {
            c::testClearSet();
            break;
        }
        else if(option.hasParam("sync"))
        {
            c::testSync();
            sleep(5);
        }
        else if(option.hasParam("backup"))
        {
            c::testBackup();
        }
        else if(option.hasParam("list-block"))
        {
            c::testListBlock();
            break;
        }
        else if(option.hasParam("list-rblock"))
        {
            c::testListRBlock();
            break;
        }
        else if(option.hasParam("list-set"))
        {
            c::testSetListTime();
            break;
        }
        else if(option.hasParam("list-rset"))
        {
            c::testSetListRTime();
            break;
        }
        else if(option.hasParam("list-get"))
        {
            c::testGetListTime();
            break;
        }
        else if(option.hasParam("list-rget"))
        {
            c::testGetListRTime();
            break;
        }
        else if(option.hasParam("listd"))
        {
            c::testListDirty();
            break;
        }
        else if(option.hasParam("list-hash"))
        {
            c::testListHash();
            break;
        }
        else if(option.hasParam("view"))
        {
            c::testView();
            break;
        }
        else if(option.hasParam("clear"))
        {
            c::testClear();
            break;
        }
        else if(option.hasParam("erase-set"))
        {
            c::testSetErase();
            break;
        }                                    
        else if(option.hasParam("erase-rset"))
        {
            c::testRSetErase();
            break;
        }
        else if(option.hasParam("erase-get"))
        {
            c::testGetErase();
            break;
        }
        else if(option.hasParam("erase-rget"))
        {
            c::testRGetErase();
            break;
        }
        else if(option.hasParam("recover"))
        {
            c::testRecover(option.getValue("recover"));
            break;
        }
        else if(option.hasParam("batchmark"))
        {
            c::testBatchMark();
            break;
        }
		else if(option.hasParam("get-expire"))
        {
            c::testGetExpire();
            break;
        }
        else
        {
            c::usage(argv[0]);
            break;
        }
        }
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }
    cout << "end" << endl;

    return 0;
}


