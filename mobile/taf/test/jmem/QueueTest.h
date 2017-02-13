#include "util/tc_shm.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_sem_mutex.h"
#include "jmem/jmem_queue.h"
#include "test.h"
#include <iostream>

using namespace taf;

extern JceQueue<Test::QueueElement1, SemLockPolicy, FileStorePolicy> g_queue;

int startQueueTest();
