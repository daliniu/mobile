#include "util/tc_option.h"
#include "util/tc_common.h"
#include "hessian/HessianProxy.h"
#include "hessian.h"

hessian::TestHessianProxy hProxy;

int testInt(int i)
{
    return hProxy.testInt(i);
}

string testChar(const string &i)
{
    return hProxy.testChar(i);
}

unsigned char testByte(char i)
{
    return hProxy.testByte(i);
}

bool testBool(bool i)
{
    return hProxy.testBool(i);
}

short testShort(short i)
{
    return hProxy.testShort(i);
}

float testFloat(float i)
{
    return hProxy.testFloat(i);
}

double testDouble(double i)
{
    return hProxy.testDouble(i);
}

long long testLong(long long i)
{
    return hProxy.testLong(i);
}

string testString(string i)
{
    return hProxy.testString(i);
}

vector<string> testList()
{
    vector<string> vs;
	vs.push_back("abc1");
	vs.push_back("abc2");
	vs.push_back("abc3");
    return hProxy.testList(vs);
}

map<string, string> testMap()
{
    map<string, string> vs;
	vs["abc1"] = "abc1";
    vs["abc2"] = "abc2";
    vs["abc3"] = "abc3";
	
    return hProxy.testMap(vs);
}

map<string, vector<string> > testComplexMap()
{
    map<string, vector<string> > vs;
	vs["abc1"].push_back("abc1");
    vs["abc2"].push_back("abc1");
    vs["abc3"].push_back("abc1");
	
    return hProxy.testComplexMap(vs);
}

vector<bool> testBoolList()
{
    vector<bool> vs;
	vs.push_back(true);
	vs.push_back(true);
	vs.push_back(false);
    return hProxy.testBooleanList(vs);
}

vector<bool> testBoolArray()
{
    vector<bool> vs;
	vs.push_back(true);
	vs.push_back(true);
	vs.push_back(false);
    return hProxy.testBooleanArray(vs);
}

vector<short> testShortList()
{
    vector<short> vs;
	vs.push_back(1);
	vs.push_back(2);
	vs.push_back(3);
    return hProxy.testShortList(vs);
}

vector<short> testShortArray()
{
    vector<short> vs;
	vs.push_back(1);
	vs.push_back(2);
	vs.push_back(3);
    return hProxy.testShortArray(vs);
}

vector<int> testIntList()
{
    vector<int> vs;
	vs.push_back(1);
	vs.push_back(2);
	vs.push_back(3);
    return hProxy.testIntList(vs);
}

vector<int> testIntArray()
{
    vector<int> vs;
	vs.push_back(1);
	vs.push_back(2);
	vs.push_back(3);
    return hProxy.testIntArray(vs);
}

vector<char> testByteList()
{
    vector<char> vs;
	vs.push_back(1);
	vs.push_back(2);
	vs.push_back(3);
    return hProxy.testByteList(vs);
}

string testByteArray()
{
    string s = "中文";
    taf::BinaryWrapper bw = hProxy.testByteArray(taf::BinaryWrapper(s));
    return bw.value();
}

vector<float> testFloatArray()
{
    vector<float> vs;
	vs.push_back(1.1);
	vs.push_back(2.2);
	vs.push_back(3.3);
    return hProxy.testFloatArray(vs);
}

vector<float> testFloatList()
{
    vector<float> vs;
	vs.push_back(1.1);
	vs.push_back(2.2);
	vs.push_back(3.3);
    return hProxy.testFloatList(vs);
}

vector<double> testDoubleArray()
{
    vector<double> vs;
	vs.push_back(1.1);
	vs.push_back(2.2);
	vs.push_back(3.3);
    return hProxy.testDoubleArray(vs);
}

vector<double> testDoubleList()
{
    vector<double> vs;
	vs.push_back(1.1);
	vs.push_back(2.2);
	vs.push_back(3.3);
    return hProxy.testDoubleList(vs);
}

vector<string> testStringList()
{
    vector<string> vs;
	vs.push_back("abc1");
	vs.push_back("abc2");
	vs.push_back("abc3");
    return hProxy.testStringList(vs);
}

vector<string> testStringArray()
{
    vector<string> vs;
	vs.push_back("abc1");
	vs.push_back("abc2");
	vs.push_back("abc3");
    return hProxy.testStringArray(vs);
}

hessian::tagStruct get()
{
	hessian::tagStruct ts;
	ts.s = "hessian::tagStruct";
    ts.c = 'a';
    ts.by = 33;
    ts.b = true;
    ts.si = 343;
    ts.f = 453.43;
    ts.d = 45453.234234;
	ts.i = 1000;
    ts.l = 2323235234234LL;

    ts.sa.push_back(ts.s);
    ts.sa.push_back(ts.s);
    ts.ca = "abc";

    ts.bya = taf::BinaryWrapper("abc");

    ts.ba.push_back(ts.b);
    ts.ba.push_back(ts.b);

    ts.sia.push_back(ts.si);
    ts.sia.push_back(ts.si);

    ts.fa.push_back(ts.f);
    ts.fa.push_back(ts.f);

    ts.da.push_back(ts.d);
    ts.da.push_back(ts.d);

    ts.ia.push_back(ts.i);
    ts.ia.push_back(ts.i);

    ts.la.push_back(ts.l);
    ts.la.push_back(ts.l);
    return ts;
}

hessian::tagStruct testStruct()
{
    hessian::tagStruct ts = get();
	ts = hProxy.testStruct(ts);
    return ts;
}

vector<hessian::tagStruct> testListStruct()
{
    vector<hessian::tagStruct> vts;
    vts.push_back(get());
    vts.push_back(get());
    vts.push_back(get());
    
	vts = hProxy.testListStruct(vts);
    return vts;
}

map<string, vector<hessian::tagStruct> > testMapListStruct()
{
    map<string, vector<hessian::tagStruct> > v;

    vector<hessian::tagStruct> vts;
    vts.push_back(get());
    vts.push_back(get());
    vts.push_back(get());

    v["test1"] = vts;
    v["test2"] = vts;
    
	v = hProxy.testMapListStruct(v);
    return v;
}

hessian::tagStruct1 testStruct1()
{
	hessian::tagStruct1 ts;
	ts.s = "hessian::tagStruct1";
    ts.c = 'a';
	ts.i = 1000;
    ts.f = 453.43;
    ts.d = 45453.234234;
    ts.ls.push_back("abc1");
    ts.ls.push_back("abc2");
	ts.ms["abc1"] = "abc1";
    ts.ms["abc2"] = "abc2";
    ts.mi[1] = "abc1";
    ts.mi[2] = "abc2";

	ts = hProxy.testStruct1(ts);
    return ts;
}

hessian::tagStruct3 testStruct3()
{
	hessian::tagStruct3 ts;
/*
    ts.ii.push_back(1);
    ts.ii.push_back(2);
    ts.ii.push_back(3);

    ts.ii1.push_back(1);
    ts.ii1.push_back(2);
    ts.ii1.push_back(3);
    ts.byby.push_back(2);
    ts.byby.push_back(2);
    ts.ff.push_back(2.3);
    ts.ff.push_back(4.3);
*/    
    ts.bb.push_back(true);
    ts.bb.push_back(false);
	ts = hProxy.testStruct3(ts);

    return ts;
}


int main(int argc, char* argv[])
{
	try
	{

		hProxy.registryUrl("http://10.1.36.39:8081/test");
		hProxy.registryUrl("http://10.1.36.39:8085/test");
		hProxy.registryUrl("http://10.1.36.39:8089/test");
//		hProxy.registryUrl("http://127.0.0.1/test");
        while(true)
        {
            try
            {
                cout << "testChar:" << testChar("中文") << endl;
                usleep(10);
            }
            catch(exception &ex)
            {
                cout << ex.what() << endl;
            }
        }

        cout << "testByte:" << (int)testByte(34) << endl;
        cout << "testBool:" << testBool(true) << endl;
        cout << "testShort:" << testShort(34) << endl;
        cout << "testInt:"<< testInt(343434) << endl;
        cout << "testLong:"<< testLong(12456789012345LL) << endl;
        cout << "testFloat:"<< testFloat(3.14) << endl;
        cout << "testDouble:"<< testDouble(4546.2344f) << endl;

        cout << "testStringEmpty:" << hProxy.testStringEmpty() << endl;

        cout << "testString:"<< testString("中文") << endl;

        /////////////////////////////////////////////////////////////
        vector<char> vb = testByteList();
        cout << "testByteList::" << taf::TC_Common::tostr(vb.begin(), vb.end()) << endl;
        string s = testByteArray();
        cout << "testByteArray::" << s << endl;

        /////////////////////////////////////////////////////////////
        vector<bool> vbl = testBoolList();
        cout << "testBoolList::" << taf::TC_Common::tostr(vbl.begin(), vbl.end()) << endl;
        vector<bool> vba = testBoolArray();
        cout << "testBoolArray::" << taf::TC_Common::tostr(vba.begin(), vba.end()) << endl;

        /////////////////////////////////////////////////////////////
        vector<short> vsa = testShortArray();
        cout << "testShortArray::" << taf::TC_Common::tostr(vsa.begin(), vsa.end()) << endl;
        vector<short> vsl = testShortList();
        cout << "testShortHList::" << taf::TC_Common::tostr(vsl.begin(), vsl.end()) << endl;

        /////////////////////////////////////////////////////////////
        vector<int> via = testIntArray();
        cout << "testIntArray::" << taf::TC_Common::tostr(via.begin(), via.end()) << endl;
        vector<int> vil = testIntList();
        cout << "testIntList::" << taf::TC_Common::tostr(vil.begin(), vil.end()) << endl;

        /////////////////////////////////////////////////////////////
        vector<float> vfa = testFloatArray();
        cout << "testFloatArray::" << taf::TC_Common::tostr(vfa.begin(), vfa.end()) << endl;
        vector<float> vfl = testFloatList();
        cout << "testFloatList::" << taf::TC_Common::tostr(vfl.begin(), vfl.end()) << endl;

        /////////////////////////////////////////////////////////////
        vector<double> vda = testDoubleArray();
        cout << "testDoubleArray::" << taf::TC_Common::tostr(vda.begin(), vda.end()) << endl;
        vector<double> vdl = testDoubleList();
        cout << "testDoubleList::" << taf::TC_Common::tostr(vdl.begin(), vdl.end()) << endl;

        /////////////////////////////////////////////////////////////
        vector<string> vssa = testStringArray();
        cout << "testStringArray::" << taf::TC_Common::tostr(vssa.begin(), vssa.end()) << endl;
        vector<string> vssl = testStringList();
        cout << "testStringList::" << taf::TC_Common::tostr(vssl.begin(), vssl.end()) << endl;

        /////////////////////////////////////////////////////////////
        map<string, string> ms = testMap();
        cout << "testMap::" << taf::TC_Common::tostr(ms.begin(), ms.end()) << endl;

        testComplexMap();

        /////////////////////////////////////////////////////////////
        hessian::tagStruct ts = testStruct();
        cout << "testStruct::" << ts.s << ":" << ts.i << ":" << ts.l << endl;
        cout << "testStruct::" << ts.ca << ":" << ts.bya.value() << endl;
        cout << "testStruct::" << taf::TC_Common::tostr(ts.sa.begin(), ts.sa.end()) << endl;
        cout << "testStruct::" << taf::TC_Common::tostr(ts.ca.begin(), ts.ca.end()) << endl;
        cout << "testStruct::" << taf::TC_Common::tostr(ts.ba.begin(), ts.ba.end()) << endl;
        cout << "testStruct::" << taf::TC_Common::tostr(ts.fa.begin(), ts.fa.end()) << endl;
        cout << "testStruct::" << taf::TC_Common::tostr(ts.ia.begin(), ts.ia.end()) << endl;

        vector<hessian::tagStruct> vts = testListStruct();
        cout << "vector<hessian::tagStruct> size::" << vts.size() << endl;

        map<string, vector<hessian::tagStruct> > mts = testMapListStruct();
        cout << "map<string, vector<hessian::tagStruct> > size::" << mts.size() << endl;

        return 0;
        /////////////////////////////////////////////////////////////
        // 
        hessian::tagStruct3 ts3 = testStruct3();
//        cout << "testStruct3::" << taf::TC_Common::tostr(ts3.ii.begin(), ts3.ii.end()) << endl;
//        cout << "testStruct3::" << taf::TC_Common::tostr(ts3.ii1.begin(), ts3.ii1.end()) << endl;
//        cout << "testStruct3::" << taf::TC_Common::tostr(ts3.byby.begin(), ts3.byby.end()) << endl;
        cout << "testStruct3::" << taf::TC_Common::tostr(ts3.bb.begin(), ts3.bb.end()) << endl;
  //      cout << "testStruct3::" << taf::TC_Common::tostr(ts3.ff.begin(), ts3.ff.end()) << endl;

        /////////////////////////////////////////////////////////////
        hessian::tagStruct1 ts1 = testStruct1();
        
        cout << "testStruct1::" << taf::TC_Common::tostr(ts1.ms.begin(), ts1.ms.end()) << endl;
        cout << "testStruct1::" << taf::TC_Common::tostr(ts1.mi.begin(), ts1.mi.end()) << endl;
        cout << "testStruct1::" << taf::TC_Common::tostr(ts1.ls.begin(), ts1.ls.end()) << endl;

        return 0;
        /////////////////////////////////////////////////////////////
        /*
		hessian::tagStruct2 ts2;
		ts2.i.i = 1000;
		ts2.i.ls.push_back("abc1");
		ts2.i.ls.push_back("abc2");
		ts2.i.ls.push_back("abc3");
		ts2.i.ms["abc1"] = "abc2";
		ts2 = hProxy.testStruct2(ts2);
		cout << ts2.i.i << endl;
		cout << "testStruct2:" << taf::TC_Common::tostr(ts2.i.ls.begin(), ts2.i.ls.end()) << endl;
		cout << "testStruct2:" << taf::TC_Common::tostr(ts2.i.ms.begin(), ts2.i.ms.end()) << endl;

        /////////////////////////////////////////////////////////////
		hessian::tagComplexStruct tss;
		tss.s = "hessian::tagComplexStruct";
		tss = hProxy.testComplexStruct(tss);
		cout << "testComplexStruct:" << tss.s << endl;
        */
	}
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


