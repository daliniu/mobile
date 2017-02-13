#include "servant/Communicator.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include <sys/un.h>
#include <iostream>
#include "TestThread.h"

using namespace std;
using namespace taf;

string obj = "Test.TestServer.TestBaseObj@tcp -h 127.0.0.1 -p 22322";

///////////////////////////////////////////////////
void testAsync(CommunicatorPtr& comm, int num, vector<TC_Thread*>& join)
{
    srand(time(NULL));

    TestBasePrx tbp = comm->stringToProxy<TestBasePrx>(obj);

    for (int i = 0; i < num; ++i)
    {
        TestAsyncThread<TestBaseCase>* tst = new TestAsyncThread<TestBaseCase>(tbp);

        tst->addTestCase(new TestMapByte());
        tst->addTestCase(new TestMapInt());
        tst->addTestCase(new TestMapStr());
        tst->addTestCase(new TestMapIntFloat());
        tst->addTestCase(new TestMapIntDouble());
        tst->addTestCase(new TestMapIntStr());
        tst->addTestCase(new TestVectorByte());
        tst->addTestCase(new TestVectorInt());
        tst->addTestCase(new TestVectorStr());
        tst->addTestCase(new TestVectorShort());
        tst->addTestCase(new TestVectorFloat());
        tst->addTestCase(new TestVectorDouble());

        tst->start();

        join.push_back(tst);
    }
}

void testSync(CommunicatorPtr& comm, int num, vector<TC_Thread*>& join)
{
    srand(time(NULL));

    TestBasePrx tbp = comm->stringToProxy<TestBasePrx>(obj);

    for (int i = 0; i < num; ++i)
    {
        TestSyncThread<TestBaseCase>* tst = new TestSyncThread<TestBaseCase>(tbp);

        tst->addTestCase(new TestMapByte());
        tst->addTestCase(new TestMapInt());
        tst->addTestCase(new TestMapStr());
        tst->addTestCase(new TestMapIntFloat());
        tst->addTestCase(new TestMapIntDouble());
        tst->addTestCase(new TestMapIntStr());
        tst->addTestCase(new TestVectorByte());
        tst->addTestCase(new TestVectorInt());
        tst->addTestCase(new TestVectorStr());
        tst->addTestCase(new TestVectorShort());
        tst->addTestCase(new TestVectorFloat());
        tst->addTestCase(new TestVectorDouble());

        tst->start();

        join.push_back(tst);
    }
}

int main(int argc, char *argv[])
{
    try
	{
        if (argc < 4)
        {
            cout << "Usage:" << argv[0] << " <config file> <sync thread num> <async thread num>" << endl;

            return 0;
        }

        //////////////////////////////////////////////////////////

        TC_Config conf;

        conf.parseFile(argv[1]);

		for (;;)
		{
			cout << "begin---------------------------------------------" << endl;

			taf::CommunicatorPtr comm = new Communicator();

			comm->setProperty(conf);

			//////////////////////////////////////////////////////////

			vector<TC_Thread*> threads;

			threads.clear();

			testSync(comm, TC_Common::strto<uint32_t>(argv[2]), threads);

			testAsync(comm, TC_Common::strto<uint32_t>(argv[3]), threads);

			for (uint32_t i = 0; i < threads.size(); ++i)
			{
				threads[i]->getThreadControl().join();

				delete threads[i];

				threads[i] = NULL;
			}

			sleep(1);

			cout << "done----------------------------------------------" << endl;
		//////////////////////////////////////////////////////////
		}
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}
