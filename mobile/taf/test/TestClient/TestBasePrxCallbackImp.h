#ifndef _TEST_BASE_PRX_CALLBACK_IMP_H_
#define _TEST_BASE_PRX_CALLBACK_IMP_H_

#include "Test.h"
#include "Collector.h"

using namespace Test;

class TestBasePrxCallbackImp : public TestBasePrxCallback
{
public:
    int _id;

    string _name;

	string _request;

public:
    virtual void callback_testMapByte(taf::Int32 _ret, map<taf::Char, std::string> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testMapInt(taf::Int32 _ret, map<taf::Int32, taf::Int32> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testMapIntDouble(taf::Int32 _ret, map<taf::Int32, taf::Double> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testMapIntFloat(taf::Int32 _ret, map<taf::Int32, taf::Float> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testMapIntStr(taf::Int32 _ret, map<taf::Int32, std::string> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testMapStr(taf::Int32 _ret, map<std::string, std::string> &mo)
    {
		string resp = ToStr::toString(mo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(mo), _name);
    }

    virtual void callback_testStr(taf::Int32 _ret, std::string &so)
    {
		string resp = ToStr::toString(so);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(so), _name);
    }

    virtual void callback_testVectorByte(taf::Int32 _ret, vector<taf::Char> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }

    virtual void callback_testVectorDouble(taf::Int32 _ret, vector<taf::Double> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }

    virtual void callback_testVectorFloat(taf::Int32 _ret, vector<taf::Float> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }

    virtual void callback_testVectorInt(taf::Int32 _ret, vector<taf::Int32> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }

    virtual void callback_testVectorShort(taf::Int32 _ret, vector<taf::Short> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }

    virtual void callback_testVectorStr(taf::Int32 _ret, vector<std::string> &vo)
    {
		string resp = ToStr::toString(vo);

		if (_ret == _id && _request == resp) 
		{
        	Collector::getInstance()->collect(_id, "Y", _ret, "Y", _name);

			return;
		}
        Collector::getInstance()->collect(_id, _request, _ret, ToStr::toString(vo), _name);
    }
};
#endif
