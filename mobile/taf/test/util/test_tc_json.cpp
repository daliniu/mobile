#include "util/tc_json.h"
#include "util/tc_common.h"

using namespace taf;

#define KEY  "dRa93"
#define KEY3 "dRa938066"

#define TESTKEY "12345678"

void test()
{
    string s = "{ \"b\": true, \"by\": 1, \"d\": 4.13, \"f\": 3.14, \"ii\": 68901, \"li\": 12345, \"m\": { \"test1\": \"test1\", \"test2\": \"test2\", \"test3\": \"test3\" }, \"mi\": {  }, \"s\": \"test\", \"si\": 345, \"t\": 1, \"vm\": [ { \"test1\": \"test1\", \"test2\": \"test2\", \"test3\": \"test3\" }, { \"test1\": \"test1\", \"test2\": \"test2\", \"test3\": \"test3\" }, { \"test1\": \"test1\", \"test2\": \"test2\", \"test3\": \"test3\" } ], \"vs\": [ \"test1\", \"test2\", \"test3\" ] }";
    cout << "src:" << s << ":" << s.length()<< endl;
    try
    {
        cout << "change:"<< TC_Json::writeValue(TC_Json::getValue(s))<<endl;
    }
    catch(TC_Json_Exception & e)
    {
        cout<<"excepton:"<<e.what()<<endl;
    }

    JsonValuePtr p=TC_Json::getValue(s);
    if(NULL == p)
    {
        cout<<"json error"<<endl;
        return;
    }
    JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(p);
    if(NULL == pObj)
    {
        cout<<"json error"<<endl;
        return;
    }

    p=pObj->value["b"];
    if(NULL == p)
    {
        cerr<<"json b error"<<endl;
        return ;
    }

    JsonValueBooleanPtr pBoolen=JsonValueBooleanPtr::dynamicCast(p);
    if(NULL == pBoolen)
    {
        cerr<<"json b error"<<endl;
        return ;
    }
    cout<<"b:"<<pBoolen->getVaule()<<endl;


}

int main(int argc, char *argv[])
{
    test();
    return 0;
}


