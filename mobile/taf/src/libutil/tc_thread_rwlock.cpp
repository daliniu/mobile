#include "util/tc_thread_rwlock.h"
#include <string.h>
#include <iostream>
#include <cassert>

namespace taf
{

TC_ThreadRWLocker::TC_ThreadRWLocker()
{
	int ret = ::pthread_rwlock_init(&m_sect, NULL);
	assert(ret == 0);

	if(ret != 0)
	{
		throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::TC_ThreadRWLocker] pthread_rwlock_init error", ret);
	}
}

TC_ThreadRWLocker::~TC_ThreadRWLocker()
{
	int ret = ::pthread_rwlock_destroy(&m_sect);
	if(ret != 0)
	{
		cerr<<"[TC_ThreadRWLocker::~TC_ThreadRWLocker] pthread_rwlock_destroy error:"<<string(strerror(ret))<<endl;;
	}

}

void TC_ThreadRWLocker::ReadLock() const
{
	int ret = ::pthread_rwlock_rdlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::ReadLock] pthread_rwlock_rdlock dead lock error", ret);
		}
		else
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::ReadLock] pthread_rwlock_rdlock error", ret);
		}
	}
}


void TC_ThreadRWLocker::WriteLock() const
{
	int ret = ::pthread_rwlock_wrlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::WriteLock] pthread_rwlock_wrlock dead lock error", ret);
		}
		else
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::WriteLock] pthread_rwlock_wrlock error", ret);
		}
	}

}

void TC_ThreadRWLocker::TryReadLock() const
{
	int ret = ::pthread_rwlock_tryrdlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::TryReadLock] pthread_rwlock_tryrdlock dead lock error", ret);
		}
		else
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::TryReadLock] pthread_rwlock_tryrdlock error", ret);
		}
	}

}

void TC_ThreadRWLocker::TryWriteLock() const
{
	int ret = ::pthread_rwlock_trywrlock(&m_sect);
	if(ret != 0)
	{
		if(ret == EDEADLK)
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::TryWriteLock] pthread_rwlock_trywrlock dead lock error", ret);
		}
		else
		{
			throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::TryWriteLock] pthread_rwlock_trywrlock error", ret);
		}
	}

}

void TC_ThreadRWLocker::Unlock() const
{
	int ret = ::pthread_rwlock_unlock(&m_sect);
	if(ret != 0)
	{
		throw TC_ThreadRW_Exception("[TC_ThreadRWLocker::Unlock] pthread_rwlock_unlock error", ret);
	}

}

}


