#ifndef _COLLECTOR_H_
#define _COLLECTOR_H_
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "util/tc_thread_mutex.h"
#include "util/tc_logger.h"

using namespace std;
using namespace taf;

/////////////////////////////////////////////////////////////////////////////////////////
class Collector : public TC_ThreadMutex
{
public:
    static Collector* getInstance()
    {
        if (_instance == NULL)
        {
            _instance = new Collector();
        }
        return _instance;
    }

    void collect(int reqId, const string& ti, int rspId, const string& to, const string& desc = "");

protected:
    Collector();

    static Collector* _instance;

	TC_DayLogger _dayLog;
};
/////////////////////////////////////////////////////////////////////////////////////////
#endif
