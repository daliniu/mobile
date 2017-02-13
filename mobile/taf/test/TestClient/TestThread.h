#ifndef _TEST_THREAD_H_
#define _TEST_THREAD_H_

#include <vector>
#include "util/tc_thread.h"
#include "TestCase.h"
#include "TestBasePrxCallbackImp.h"

using namespace std;
using namespace taf;

/////////////////////////////////////////////////////////////////
template<class T>
struct TestThread : public taf::TC_Thread
{
public:
    TestThread(typename T::proxy_type proxy) : _proxy(proxy) {}

    virtual ~TestThread() {}

public:
    void clearTestCase() { _allTestCase.clear(); }

    void addTestCase(T* t) { _allTestCase.push_back(t); }

    T* getRandomTestCase()
    {
        if (!_allTestCase.empty())
        {
            return _allTestCase[rand() % _allTestCase.size()];
        }
        return NULL;
    }

    virtual void run() = 0;

protected:
    typename T::proxy_type _proxy;

    vector<T*> _allTestCase;
};
/////////////////////////////////////////////////////////////////
template<class T>
struct TestSyncThread : public TestThread<T>
{
    TestSyncThread(typename T::proxy_type proxy) : TestThread<T>(proxy) {}

    virtual void run()
    {
        uint32_t account = 0;

        //for (;;)
        {
            //sleep(1);

            int count = 100;

            for(int i = 0; i < count; ++i)
            {
                try
                {
                    T* t = TestThread<T>::getRandomTestCase();

                    if (t)
                    {
                        t->doTest(TestThread<T>::_proxy, ++account, NULL);
                    }
                }
                catch (exception& e)
                {
                    cout << pthread_self() << ":exception:" << e.what() << endl;
                }
                catch (...)
                {
                    cout << pthread_self() << ":exception." << endl;
                }
            }
        }
    }
};
/////////////////////////////////////////////////////////////////
template<class T>
struct TestAsyncThread : public TestThread<T>
{
    TestAsyncThread(typename T::proxy_type proxy) : TestThread<T>(proxy) {}

    virtual void run()
    {
        uint32_t account = 0;

        //for (;;)
        {
            //sleep(1);

            int count = 100;

            for(int i = 0; i < count; ++i)
            {
                try
                {
                    T* t = TestThread<T>::getRandomTestCase();

                    if (t)
                    {
                        typename T::callback_proxy_type cb = new TestBasePrxCallbackImp();

                        t->doTest(TestThread<T>::_proxy, ++account, cb);
                    }
                }
                catch (exception& e)
                {
                    cout << pthread_self() << ":exception:" << e.what() << endl;
                }
                catch (...)
                {
                    cout << pthread_self() << ":exception." << endl;
                }
            }
        }
    }
};
/////////////////////////////////////////////////////////////////
#endif
