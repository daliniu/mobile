#include "Collector.h"

Collector* Collector::_instance = NULL;

Collector::Collector() 
{
	string pid = TC_Common::tostr<uint32_t>(getpid());

	_dayLog.init(string("./log/day_") + pid);
}

void Collector::collect(int reqId, const string& ti, int rspId, const string& to, const string& desc/* = ""*/)
{
	TC_LockT<TC_ThreadMutex> lock(*this);

    _dayLog.any() << pthread_self() << "|Q|" << desc << "|" << reqId << "|" << ti << "|R|" << rspId << "|" << to << "|" << endl;
}
