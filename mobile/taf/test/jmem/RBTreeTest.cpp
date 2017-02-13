#include "RBTreeTest.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_thread_pool.h"
#include "util/tc_file.h"
#include "util/tc_option.h"
#include "jmem/jmem_rbtree.h"

using namespace taf;

namespace b
{

//JceRBTree<Test::MapKey, Test::MapValue, SemLockPolicy, ShmStorePolicy> g_rbtree;
typedef JceRBTree<Test::MapKey, Test::MapValue, SemLockPolicy, FileStorePolicy> RBTree;
//typedef JceRBTree<Test::MapKey, Test::MapValue, ThreadLockPolicy, FileStorePolicy> RBTree;

RBTree g_rbtree;

class TestToDoFunctor : public RBTree::ToDoFunctor
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
        cout << "TestToDoFunctor::get::" << data._key.s << ":" << data._value.s << endl;
//        return TC_RBTree::RT_NO_DATA;
        return TC_RBTree::RT_OK;
    }
};

TestToDoFunctor g_todo;

struct RBTreeLess
{
    bool operator()(const string &k1, const string &k2)
    {
        Test::MapKey tk;
        taf::JceInputStream<BufferReader> is;
        is.setBuffer(k1.c_str(), k1.length());
        tk.readFrom(is);

        Test::MapKey tv;
        is.setBuffer(k2.c_str(), k2.length());
        tv.readFrom(is);
                            
        return TC_Common::strto<int>(tk.s) < TC_Common::strto<int>(tv.s);
    }
};

void initRBTree()
{
    size_t n = 1024*1040*100;

//  g_rbtree.initDataBlockSize(10, 40, 1.2);
    g_rbtree.initDataBlockSize(10, 50, 1.2);

    if(TC_File::isFileExist("rbtree.dat"))
    {
        n = TC_File::getFileSize("rbtree.dat");
    }
    g_rbtree.initStore("rbtree.dat", n);
//    int ret = g_rbtree.initStore(3434, n);
    g_rbtree.initLock(2324);
    g_rbtree.setLessFunctor(RBTreeLess());
    g_rbtree.setSyncTime(5);
    g_rbtree.setToDoFunctor(&g_todo);
    g_rbtree.setAutoErase(true);
}

void testAppend()
{
    cout << g_rbtree.getMapHead()._iMemSize << endl;
    size_t iNewSize = g_rbtree.getMapHead()._iMemSize * 2;

    g_rbtree.expand(iNewSize);
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
        size_t n = rand() % 10;
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
//        unsigned int allChunk = g_rbtree.allBlockChunkCount();
//        unsigned int useChunk = g_rbtree.getMapHead()._iUsedChunk;

//        if ( useChunk * 10 >= allChunk * 9 )
        if(i % 1000 == 0)
        {
//            if(TC_File::isFileExist("test.hm"))
            {
                size_t totalSize = (size_t)TC_File::getFileSize("rbtree.dat") + 10240; 

        //        cout << "expand:" << totalSize << ":" << allChunk << ":" << useChunk << endl;//<< g_rbtree.desc() << endl;
                cout << "expand:" << totalSize << endl;
                int ret = g_rbtree.expand( totalSize );
                cout << ret << endl;
            } 
        }

        }
        g_rbtree.set(v[index].first, v[index].second);
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

void testSendRand()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    for(size_t i = 0; i < 10000000; i++)
    {
        size_t index = rand() % v.size();
        v[index].first.s = TC_Common::tostr(i*2);

        int ret = g_rbtree.set(v[index].first, v[index].second);
        cout << "set:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
        Test::MapValue s;
        ret = g_rbtree.get(v[index].first, s);
//        assert(v[index].second == s);
    }

    cout << g_rbtree.size() << "|" << g_rbtree.dirtyCount() << endl;
}

void testSetRandOne()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    size_t index = rand() % v.size();

    int ret = g_rbtree.set(v[index].first, v[index].second);
    cout << "set:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
}

void testGetRand()
{
    vector<pair<Test::MapKey, Test::MapValue> > v = prepare();

    for(int i = 0; i < 200000; i++)
    {
        size_t index = rand() % v.size();

        int ret = g_rbtree.get(v[index].first, v[index].second);
        cout << "get:" << v[index].first.s << ":" << v[index].second.s << " ret=" << ret << endl;
        assert(ret == 0);
    }

    cout << g_rbtree.size() << "|" << g_rbtree.dirtyCount() << endl;
}

void testSetOne(const string &k, const string &v)
{
    pair<Test::MapKey, Test::MapValue> vv;
    vv.first.s  = k;
    vv.second.s = v;

    int ret = g_rbtree.set(vv.first, vv.second);
    cout << "set-taf:" << k << ":" << v << " ret=" << ret << endl;
}

void testFind(const string &c)
{
    Test::MapKey kk;
    kk.s = c;
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.find(kk);
    if(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
}

void testLowerBound(const string &c)
{
    Test::MapKey kk;
    kk.s = c;
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.lower_bound(kk);
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testUpperBound(const string &c)
{
    Test::MapKey kk;
    kk.s = c;
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.upper_bound(kk);
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testEqualRange(const string &c1, const string &c2)
{
    Test::MapKey kk1;
    kk1.s = c1;

    Test::MapKey kk2;
    kk2.s = c2;

    int n = 0;
    pair<RBTree::lock_iterator, RBTree::lock_iterator> pit = g_rbtree.equal_range(kk1, kk2);
    RBTree::lock_iterator it = pit.first;

    while(it != pit.second)
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testDel(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.del(mk);
    cout << "del:" << k << " ret=" << ret << endl;
}

void testErase(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.erase(mk);
    cout << "erase:" << k << " ret=" << ret << endl;
}

void testGet(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    Test::MapValue mv;
    int ret = g_rbtree.get(mk, mv);
    cout << "get:" << k << ":" << mv.s << " ret=" << ret << endl;
}

void testSetKey(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.set(mk);
    cout << "set-key:" << k << " ret=" << ret << endl;
}

void testSetDirty(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.setDirty(mk);
    cout << "setdirty:" << k << " ret=" << ret << endl;
}

void testSetClean(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.setClean(mk);
    cout << "setdirty:" << k << " ret=" << ret << endl;
}

void testCheckDirty(const string &k)
{
    Test::MapKey mk;
    mk.s = k;
    int ret = g_rbtree.checkDirty(mk);
    cout << "checkdirty:" << k << " ret=" << ret << endl;
}

void testReadOnly(const string &k)
{
    if(k == "yes")
    {
        g_rbtree.setReadOnly(true);
    }
    else
    {
        g_rbtree.setReadOnly(false);
    }
    cout << "setReadOnly:" << k << endl;
}

void testAutoErase(const string &k)
{
    if(k == "yes")
    {
        g_rbtree.setAutoErase(true);
    }
    else
    {
        g_rbtree.setAutoErase(false);
    }
    cout << "setAutoErase:" << k << endl;
}

void testSync()
{
    g_rbtree.sync(time(NULL));
    cout << "testSync OK" << endl;
}

void testBackup()
{
    g_rbtree.backup();
    cout << "testBackup OK" << endl;
}

void testClear()
{
    g_rbtree.clear();
    cout << "testClear OK" << endl;
}

void testEraseAll(int radio)
{
    int ret = g_rbtree.erase(radio);
    cout << "testEraseAll:" << ret << endl;
}

void testRBTreeList()
{
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.begin();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
        {
            cout << "error:" << ret << endl;
        }
        else
        {
            k.display(cout);
            cout << "|";
            v.display(cout);
        }
        cout << n << "----------------" << endl;
        ++it;
        ++n;
    }
    cout << "count:" << n << endl;
}

void testRBTreeNoLockList()
{
    int n = 0;
    RBTree::nolock_iterator it = g_rbtree.nolock_begin();
    while(it != g_rbtree.nolock_end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testRBTreeRNoLockList()
{
    int n = 0;
    RBTree::nolock_iterator it = g_rbtree.nolock_rbegin();
    while(it != g_rbtree.nolock_end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testRRBTreeList()
{
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.rbegin();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testSetListTime()
{
    int n = 0;
    RBTree::lock_iterator it = g_rbtree.beginSetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
    RBTree::lock_iterator it = g_rbtree.rbeginSetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
    RBTree::lock_iterator it = g_rbtree.beginGetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
    RBTree::lock_iterator it = g_rbtree.rbeginGetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
    RBTree::lock_iterator it = g_rbtree.beginDirty();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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

void testView()
{
    cout << g_rbtree.desc() << endl;
}

void testSetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    RBTree::lock_iterator it = g_rbtree.beginSetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
        g_rbtree.erase(vk[i]);
    }
}

void testRSetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    RBTree::lock_iterator it = g_rbtree.rbeginSetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
        g_rbtree.erase(vk[i]);
    }
}


void testGetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    RBTree::lock_iterator it = g_rbtree.beginGetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
        g_rbtree.erase(vk[i]);
    }
}

void testRGetErase()
{
    vector<Test::MapKey> vk;
    int n = 0;
    {
    RBTree::lock_iterator it = g_rbtree.rbeginGetTime();
    while(it != g_rbtree.end())
    {
        Test::MapKey k;
        Test::MapValue v;

        int ret = it->get(k, v);
        if(ret != TC_RBTree::RT_OK && ret != TC_RBTree::RT_ONLY_KEY)
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
        g_rbtree.erase(vk[i]);
    }
}

void testClearSet()
{

    g_rbtree.clear();
    cout << g_rbtree.desc() << endl;

    for(int i=0;i<1000;i++)
    {
        Test::MapKey k;
        Test::MapValue v;
        k.s = "111111111231423156463890909090909090909090909046666667"+TC_Common::tostr(i);
        v.s = "321y9645917hehclmwwfvopkgm,;'ml;ffbf b er ntr n"+TC_Common::tostr(i);       
        g_rbtree.set(k,v);
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
        g_rbtree.set(k, v);
    }

    int64_t m = TC_Common::now2ms();

    cout << (m - n) << endl;
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
    cout << "\t --set-one" << endl;
    cout << "\t --find=" << endl;
    cout << "\t --lower-bound=" << endl;
    cout << "\t --upper-bound=" << endl;
    cout << "\t --equal-range --key1= --key2=" << endl;
    cout << "\t --nolock-list" << endl;
    cout << "\t --nolock-rlist" << endl;
    cout << "\t --list" << endl;
    cout << "\t --rlist" << endl;
    cout << "\t --list-set" << endl;
    cout << "\t --list-rset" << endl;
    cout << "\t --list-get" << endl;
    cout << "\t --list-rget" << endl;
    cout << "\t --listd" << endl;
    cout << "\t --set-rand" << endl;
    cout << "\t --get-rand" << endl;
    cout << "\t --set-taf --key= --value=" << endl;
    cout << "\t --set-key= " << endl;
    cout << "\t --del=" << endl;
    cout << "\t --erase=" << endl;
    cout << "\t --get=" << endl;
    cout << "\t --eraseall=" << endl;
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
}

}

int startRBTreeTest(int argc, char *argv[])
{
    try
    {
        cout << "start" << endl;
        TC_Option option;
        option.decode(argc, argv);

        map<string, string> m = option.getMulti();

        if(option.hasParam("help") || option.getMulti().empty())
        {
            b::usage(argv[0]);
            return 0;
        }

        b::initRBTree();

        if(option.hasParam("release"))
        {
//            g_rbtree.release();
            return 0;
        }

        while(true)
        {

        if(option.hasParam("set-rand"))
        {
            b::testSendRand();
            break;
        }
        else if(option.hasParam("append"))
        {
            b::testAppend();
            break;
        }
        else if(option.hasParam("set-append"))
        {
            b::testAppendThreadPool();
            break;
        }
        else if(option.hasParam("get-rand"))
        {
            b::testGetRand();
        }
        else if(option.hasParam("set-taf"))
        {
            b::testSetOne(option.getValue("key"), option.getValue("value"));
            break;
        }
        else if(option.hasParam("find"))
        {
            b::testFind(option.getValue("find"));
            break;
        }
        else if(option.hasParam("upper-bound"))
        {
            b::testUpperBound(option.getValue("upper-bound"));
            break;
        }
        else if(option.hasParam("lower-bound"))
        {
            b::testLowerBound(option.getValue("lower-bound"));
            break;
        }
        else if(option.hasParam("equal-range"))
        {
            b::testEqualRange(option.getValue("key1"), option.getValue("key2"));
            break;
        }
        else if(option.hasParam("del"))
        {
            b::testDel(option.getValue("del"));
            break;
        }
        else if(option.hasParam("erase"))
        {
            b::testErase(option.getValue("erase"));
            break;
        }
        else if(option.hasParam("eraseall"))
        {
            b::testEraseAll(TC_Common::strto<int>(option.getValue("eraseall")));
            b::testView();
            sleep(5);
        }
        else if(option.hasParam("get"))
        {
            b::testGet(option.getValue("get"));
            break;
        }
        else if(option.hasParam("set-key"))
        {
            b::testSetKey(option.getValue("set-key"));
            break;
        }
        else if(option.hasParam("set-one"))
        {
            b::testSetRandOne();
            break;
        }
        else if(option.hasParam("readonly"))
        {
            b::testReadOnly(option.getValue("readonly"));
            break;
        }
        else if(option.hasParam("autoerase"))
        {
            b::testAutoErase(option.getValue("autoerase"));
            break;
        }
        else if(option.hasParam("check-dirty"))
        {
            b::testCheckDirty(option.getValue("check-dirty"));
            break;
        }
        else if(option.hasParam("set-dirty"))
        {
            b::testSetDirty(option.getValue("set-dirty"));
            break;
        }
        else if(option.hasParam("set-clean"))
        {
            b::testSetClean(option.getValue("set-clean"));
            break;
        }
        else if(option.hasParam("clean-set"))
        {
            b::testClearSet();
            break;
        }
        else if(option.hasParam("sync"))
        {
            b::testSync();
            sleep(5);
        }
        else if(option.hasParam("backup"))
        {
            b::testBackup();
        }
        else if(option.hasParam("list"))
        {
            b::testRBTreeList();
            break;
        }
        else if(option.hasParam("nolock-list"))
        {
            b::testRBTreeNoLockList();
            break;
        }
        else if(option.hasParam("nolock-rlist"))
        {
            b::testRBTreeRNoLockList();
            break;
        }
        else if(option.hasParam("rlist"))
        {
            b::testRRBTreeList();
            break;
        }
        else if(option.hasParam("list-set"))
        {
            b::testSetListTime();
            break;
        }
        else if(option.hasParam("list-rset"))
        {
            b::testSetListRTime();
            break;
        }
        else if(option.hasParam("list-get"))
        {
            b::testGetListTime();
            break;
        }
        else if(option.hasParam("list-rget"))
        {
            b::testGetListRTime();
            break;
        }
        else if(option.hasParam("listd"))
        {
            b::testListDirty();
            break;
        }
        else if(option.hasParam("view"))
        {
            b::testView();
            break;
        }
        else if(option.hasParam("clear"))
        {
            b::testClear();
            break;
        }
        else if(option.hasParam("erase-set"))
        {
            b::testSetErase();
            break;
        }                                    
        else if(option.hasParam("erase-rset"))
        {
            b::testRSetErase();
            break;
        }
        else if(option.hasParam("erase-get"))
        {
            b::testGetErase();
            break;
        }
        else if(option.hasParam("erase-rget"))
        {
            b::testRGetErase();
            break;
        }
        else if(option.hasParam("batchmark"))
        {
            b::testBatchMark();
            break;
        }
        else
        {
            b::usage(argv[0]);
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


