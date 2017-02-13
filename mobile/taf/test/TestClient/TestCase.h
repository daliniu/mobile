#ifndef _TEST_CASE_H_
#define _TEST_CASE_H_

#include "Random.h"
#include "Test.h"
#include "ToStr.h"
#include "Collector.h"
#include "util/tc_common.h"
#include "TestBasePrxCallbackImp.h"

using namespace Random;
using namespace taf;
using namespace Test;

////////////////////////////////////////////////////////////////////////
struct TestBaseCase
{
    typedef TestBasePrx proxy_type;

    typedef TestBasePrxCallbackPtr callback_proxy_type;

    TestBaseCase(const string& name): _name(name) {}

    virtual ~TestBaseCase() {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb) = 0;

    const string& getName() const { return _name; }

    string _name;
};
////////////////////////////////////////////////////////////////////////
struct TestStr : public TestBaseCase
{
    TestStr() : TestBaseCase("TestStr") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        string si = Random::random<string>::gen();

		string so;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(si);

            proxy->async_testStr(cb, i, si);
        }
        else
        {
            id = proxy->testStr(i, si, so);

			if (id == i && ToStr::toString(si) == ToStr::toString(so))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(si), id, ToStr::toString(so), _name);
			}
        }

        return id;
    }
};
struct TestMapByte : public TestBaseCase
{
    TestMapByte() : TestBaseCase("TestMapByte") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<char, string> mi = Random::randomMap<char, string>::gen();

        map<char, string> mo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapByte(cb, i, mi);
        }
        else
        {
            id = proxy->testMapByte(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestMapInt : public TestBaseCase
{
    TestMapInt() : TestBaseCase("TestMapInt") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<int, int> mi = Random::randomMap<int, int>::gen();

        map<int, int> mo;

		int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapInt(cb, i, mi);
        }
        else
        {
        	id = proxy->testMapInt(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestMapStr : public TestBaseCase
{
    TestMapStr() : TestBaseCase("TestMapStr") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<string, string> mi = Random::randomMap<string, string>::gen();

        map<string, string> mo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapStr(cb, i, mi);
        }
        else
        {
            id = proxy->testMapStr(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestMapIntFloat : public TestBaseCase
{
    TestMapIntFloat() : TestBaseCase("TestMapIntFloat") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<int, float> mi = Random::randomMap<int, float>::gen();

        map<int, float> mo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapIntFloat(cb, i, mi);
        }
        else
        {
            id = proxy->testMapIntFloat(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestMapIntDouble : public TestBaseCase
{
    TestMapIntDouble() : TestBaseCase("TestMapIntDouble") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<int, double> mi = Random::randomMap<int, double>::gen();

        map<int, double> mo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapIntDouble(cb, i, mi);
        }
        else
        {
            id = proxy->testMapIntDouble(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestMapIntStr : public TestBaseCase
{
    TestMapIntStr() : TestBaseCase("TestMapIntStr") {}

    int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        map<int, string> mi = Random::randomMap<int, string>::gen();

        map<int, string> mo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(mi);

            proxy->async_testMapIntStr(cb, i, mi);
        }
        else
        {
            id = proxy->testMapIntStr(i, mi, mo);

			if (id == i && ToStr::toString(mi) == ToStr::toString(mo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(mi), id, ToStr::toString(mo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorByte : public TestBaseCase
{
    TestVectorByte() : TestBaseCase("TestVectorByte") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<char> vi = Random::randomVector<char>::gen();

        vector<char> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorByte(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorByte(i, vi, vo);

			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorInt : public TestBaseCase
{
    TestVectorInt() : TestBaseCase("TestVectorInt") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<int> vi = Random::randomVector<int>::gen();

        vector<int> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorInt(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorInt(i, vi, vo);

			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorStr : public TestBaseCase
{
    TestVectorStr() : TestBaseCase("TestVectorStr") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<string> vi = Random::randomVector<string>::gen();

        vector<string> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorStr(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorStr(i, vi, vo);
			
			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorShort : public TestBaseCase
{
    TestVectorShort() : TestBaseCase("TestVectorShort") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<short> vi = Random::randomVector<short>::gen();

        vector<short> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorShort(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorShort(i, vi, vo);

			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorFloat : public TestBaseCase
{
    TestVectorFloat() : TestBaseCase("TestVectorFloat") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<float> vi = Random::randomVector<float>::gen();

        vector<float> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorFloat(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorFloat(i, vi, vo);

			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
struct TestVectorDouble : public TestBaseCase
{
    TestVectorDouble() : TestBaseCase("TestVectorDouble") {}

    virtual int doTest(TestBasePrx proxy, int i, TestBasePrxCallbackPtr cb)
    {
        vector<double> vi = Random::randomVector<double>::gen();

        vector<double> vo;

        int id = -1;

        if (cb)
        {
            ((TestBasePrxCallbackImp*)cb.get())->_id = i;

            ((TestBasePrxCallbackImp*)cb.get())->_name = _name;

            ((TestBasePrxCallbackImp*)cb.get())->_request = ToStr::toString(vi);

            proxy->async_testVectorDouble(cb, i, vi);
        }
        else
        {
            id = proxy->testVectorDouble(i, vi, vo);

			if (id == i && ToStr::toString(vi) == ToStr::toString(vo))
			{
        		Collector::getInstance()->collect(i, "S", id, "S", _name);
			}
			else
			{
        		Collector::getInstance()->collect(i, ToStr::toString(vi), id, ToStr::toString(vo), _name);
			}
        }

        return id;
    }
};
////////////////////////////////////////////////////////////////////////
#endif
