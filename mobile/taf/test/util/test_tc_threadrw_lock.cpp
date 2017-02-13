#include "util/tc_thread_rwlock.h"
#include <iostream>

using namespace taf;

TC_ThreadRWLocker  g_rwl;
unsigned int g_testnum = 0;

static void testwrlockimp(void *arg)
{
	int i = *(int*)arg;
	while(true)
	{
		if(i==1) //|| i==5)//write
		{
			{
				TC_ThreadWLock wlock(g_rwl);
				g_testnum++;
			}
			cout<<"Thread_"<<i<<" is writing:"<<g_testnum<<endl;
			sleep(6);
		}
		else//read
		{
			{
				TC_ThreadRLock rlock(g_rwl);
				cout<<"Thread_"<<i<<" is reading:"<<g_testnum<<endl;
			}
			sleep(1);
		}
		cout<<"testwrlockimp_"<<i<<" is testing"<<endl;
	}
}

static void testwrlock()
{
	pthread_t		_tid;
	for(int i=1;i<=4;i++)
	{
		int ret = pthread_create(&_tid,
					   0,
					   (void *(*)(void *))&testwrlockimp,
					   (void*)&i);
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	while(1)
	{
		static bool init = false;
		if(!init)
		{
			try{
					testwrlock();
					init = true;
			}
			catch(exception e)
			{
				cout<<__FUNCTION__<<":"<<__LINE__<<" exception: "<<e.what()<<endl;
			}
		}
		sleep(2);
	}
    return 0;
}


