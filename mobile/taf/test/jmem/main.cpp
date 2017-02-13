#include "QueueTest.h"
#include "HashMapTest.h"
#include "HashMapCompactTest.h"
#include "RBTreeTest.h"
#include "MultiHashMapTest.h"
#include "util/tc_option.h"

#include <iostream>

using namespace taf;

int main(int argc, char* argv[])
{
	try
	{
        TC_Option option;
        option.decode(argc, argv);

        if(option.hasParam("queue"))
        {
            startQueueTest();
        }
        else if(option.hasParam("hashmap"))
        {
            startHashMapTest(argc, argv);
        }
        else if(option.hasParam("rbtree"))
        {
            startRBTreeTest(argc, argv);
        }
		else if(option.hasParam("mhashmap"))
		{
			startMultiHashMapTest(argc, argv);
		}
        else if(option.hasParam("hashmap-compact"))
        {
            startHashMapCompactTest(argc, argv);
        }
        else
        {
            cout << argv[0] << " --queue --hashmap --hashmap-compact --rbtree --mhashmap" << endl;
        }
	}
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


