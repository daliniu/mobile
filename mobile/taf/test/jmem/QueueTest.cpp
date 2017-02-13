#include "QueueTest.h"

using namespace taf;

//JceQueue<Test::QueueElement1, SemLockPolicy, ShmStorePolicy> g_queue;
JceQueue<Test::QueueElement1, SemLockPolicy, FileStorePolicy> g_queue;

class PushThread : public TC_Thread
{
    /**
     * 运行
     */
protected:
    virtual void run()
    {
        Test::QueueElement1 qe;
        while(true)
        {
            bool b;
            {
            //    TC_ThreadLock::Lock l(g_lock);
                qe.s = TC_Common::nowtime2str();
                qe.ss = "new";
                b = g_queue.push_back(qe);
            }

            if(!b)
            {
                cout << "push full" << endl;
                sleep(1);
                continue;
            }
            qe.ii++;
        }
    }
};

class PopThread : public TC_Thread
{
    /**
     * 运行
     */
protected:
    virtual void run()
    {
        Test::QueueElement1 qe;
        while(true)
        {
            bool b;
            {
                b = g_queue.pop_front(qe);
            }
            if(!b)
            {
                cout << "pop empty" << endl;
                sleep(1);
                continue;
            }
            cout << qe.ii << ":" << qe.s << ":" << qe.ss << endl;
        }
    }
};

int startQueueTest()
{
	try
	{
        size_t n = 1024*1024*10;

//        g_queue.initStore(1019, n);
        g_queue.initStore("test.jq", n);
        g_queue.initLock(1019);

        PushThread pt1;
        PushThread pt2;
        PushThread pt3;
        pt1.start();
        pt2.start();
        pt3.start();

        PopThread tt1;
        PopThread tt2;
        PopThread tt3;
        tt1.start();
        tt2.start();
        tt3.start();

        pt1.getThreadControl().join();
	}
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


