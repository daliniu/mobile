#include "util/tc_option.h"
#include "util/tc_common.h"
#include "hessian/HessianProxy.h"

taf::HessianProxy hProxy;

//java:public int testInt(int i);
void testHessian1()
{
	vector<taf::HObjectPtr> vParam;

	taf::HIntegerPtr iPtr(new taf::HInteger(1000));
	vParam.push_back(iPtr);

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testInt", vParam);

	taf::HIntegerPtr tPtr = taf::HIntegerPtr::dynamicCast(vResult);
	cout << "testInt:" << tPtr->value() << endl;
}

//	public int testIntString(int i, HString s);
void testHessian2()
{
	vector<taf::HObjectPtr> vParam;

	vParam.push_back(new taf::HInteger(1000));
	vParam.push_back(new taf::HString("1000"));

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testIntString", vParam);

	taf::HIntegerPtr tPtr = taf::HIntegerPtr::dynamicCast(vResult);
	cout << "testIntString:" << tPtr->value() << endl;
}

//////////////////////////////////////////////////////////
//	public class tagSimpleStruct implements java.io.Serializable
//	{
//		HString 	s = "abc";
//		int		i;
//	}
//	public tagStruct testStruct(tagStruct ts);

void testHessian3()
{
	taf::HMapPtr lm = new taf::HMap();
	lm->value()[new taf::HString("s")] = new taf::HString("abc1");
	lm->value()[new taf::HString("i")] = new taf::HInteger(1000);

	vector<taf::HObjectPtr> vParam;
	vParam.push_back(lm);

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testSimpleStruct", vParam);

	taf::HMapPtr     tPtr    = taf::HMapPtr::dynamicCast(vResult);
	taf::HStringPtr  sPtr    = taf::HStringPtr::dynamicCast(tPtr->find(new taf::HString("s")));
	taf::HIntegerPtr iPtr    = taf::HIntegerPtr::dynamicCast(tPtr->find(new taf::HString("i")));
	cout << "testSimpleStruct:" << sPtr->value() << ":" << iPtr->value() << endl;
}

//////////////////////////////////////////////////////////
//	public class tagStruct implements java.io.Serializable
//	{
//		HString 	s = "abc";
//		byte   	b[] = new byte[20];
//		double 	d;
//		int		i;
//		HList<HString> ls = new ArrayHList<HString>();
//		long	l;
//		HashHMap<HString, HString> ms = new HashHMap<HString, HString>();
//		HashHMap<HInteger, HString> mi = new HashHMap<HInteger, HString>();
//	}
//	public tagStruct testStruct(tagStruct ts);
void testHessian4()
{
	taf::HMapPtr lm  = new taf::HMap();
	lm->value()[new taf::HString("s")]    = new taf::HString("abc1");
	lm->value()[new taf::HString("b")]    = new taf::HBinary("abc2");
	lm->value()[new taf::HString("d")]    = new taf::HDouble(3.14);
	lm->value()[new taf::HString("i")]    = new taf::HInteger(5000);

	taf::HListPtr lp = new taf::HList();
	lp->value().push_back(new taf::HString("list1"));
	lp->value().push_back(new taf::HString("list2"));
	lp->value().push_back(new taf::HString("list3"));
	lm->value()[new taf::HString("ls")]   = lp;

	lm->value()[new taf::HString("l")]    = new taf::HLong(123456789);

	taf::HMapPtr lmm = new taf::HMap();
	lmm->value()[new taf::HString("abc1")] = new taf::HString("abc1");
	lmm->value()[new taf::HString("abc2")] = new taf::HString("abc2");
	lm->value()[new taf::HString("ms")]   = lmm;

	taf::HMapPtr lmi = new taf::HMap();
	lmi->value()[new taf::HInteger(1)] = new taf::HString("1");
	lmi->value()[new taf::HInteger(2)] = new taf::HString("2");
	lm->value()[new taf::HString("mi")]   = lmi;

	vector<taf::HObjectPtr> vParam;
	vParam.push_back(lm);

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testStruct", vParam);

	taf::HMapPtr tPtr = taf::HMapPtr::dynamicCast(vResult);
	taf::HStringPtr sPtr     = taf::HStringPtr::dynamicCast(tPtr->find(new taf::HString("s")));
	taf::HBinaryPtr bPtr     = taf::HBinaryPtr::dynamicCast(tPtr->find(new taf::HString("b")));
	taf::HDoublePtr dPtr     = taf::HDoublePtr::dynamicCast(tPtr->find(new taf::HString("d")));
	taf::HIntegerPtr iPtr    = taf::HIntegerPtr::dynamicCast(tPtr->find(new taf::HString("i")));
	taf::HLongPtr lPtr       = taf::HLongPtr::dynamicCast(tPtr->find(new taf::HString("l")));

	taf::HListPtr lsPtr      = taf::HListPtr::dynamicCast(tPtr->find(new taf::HString("ls")));
	taf::HMapPtr  msPtr       = taf::HMapPtr::dynamicCast(tPtr->find(new taf::HString("ms")));
	taf::HMapPtr  miPtr       = taf::HMapPtr::dynamicCast(tPtr->find(new taf::HString("mi")));

	cout << "testStruct HString:" << sPtr->value() << endl;
	cout << "testStruct HBinary:" << bPtr->value() << endl;
	cout << "testStruct HDouble:" << dPtr->value() << endl;
	cout << "testStruct HInteger:" << iPtr->value() << endl;
	cout << "testStruct HLong:" << lPtr->value() << endl;
	std::list<taf::HObjectPtr>::iterator it = lsPtr->value().begin();
	while(it != lsPtr->value().end())
	{
		taf::HStringPtr sPtr     = taf::HStringPtr::dynamicCast(*it);
		cout << "testStruct HString list:" << sPtr->value() << endl;
		++it;
	}

	std::map<taf::HObjectPtr, taf::HObjectPtr>::iterator itm = msPtr->value().begin();
	while(itm != msPtr->value().end())
	{
		taf::HStringPtr s1Ptr     = taf::HStringPtr::dynamicCast(itm->first);
		taf::HStringPtr s2Ptr     = taf::HStringPtr::dynamicCast(itm->second);
		cout << "testStruct HString map:" << s1Ptr->value() << "=" << s2Ptr->value() << endl;
		++itm;
	}

	std::map<taf::HObjectPtr, taf::HObjectPtr>::iterator iti = miPtr->value().begin();
	while(iti != miPtr->value().end())
	{
		taf::HIntegerPtr s1Ptr     = taf::HIntegerPtr::dynamicCast(iti->first);
		taf::HStringPtr s2Ptr     = taf::HStringPtr::dynamicCast(iti->second);
		cout << "testStruct HInteger map:" << s1Ptr->value() << "=" << s2Ptr->value() << endl;
		++iti;
	}
}

void testHessian5()
{
	vector<taf::HObjectPtr> vParam;

	vParam.push_back(new taf::HInteger(1000));
	vParam.push_back(new taf::HString("1000"));

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testIntString", vParam);

	taf::HIntegerPtr tPtr = taf::HIntegerPtr::dynamicCast(vResult);
	cout << "testIntString:" << tPtr->value() << endl;
}

void testHessian6()
{
	vector<taf::HObjectPtr> vParam;

	taf::HMapPtr lm  = new taf::HMap();
	lm->value()[new taf::HString("i")]    = new taf::HInteger(5000);

	taf::HMapPtr lmm  = new taf::HMap();
	lmm->value()[new taf::HString("i")]    = lm;

	vParam.push_back(lmm);

	taf::HObjectPtr vResult;
	vResult = hProxy.call("testStruct2", vParam);

	taf::HMapPtr tPtr = taf::HMapPtr::dynamicCast(vResult);
	taf::HMapPtr  msPtr      = taf::HMapPtr::dynamicCast(tPtr->find(new taf::HString("i")));
	taf::HIntegerPtr iPtr    = taf::HIntegerPtr::dynamicCast(msPtr->find(new taf::HString("i")));

	cout << "testStruct2:" << iPtr->value() << endl;
}

int main(int argc, char* argv[])
{
	try
	{
		hProxy.registryUrl("http://127.0.0.1/test");

		testHessian1();
		testHessian2();
		testHessian3();
		testHessian4();
		testHessian5();
		testHessian6();
	}
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}

