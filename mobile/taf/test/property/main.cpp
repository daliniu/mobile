#include <iostream>
#include "servant/Communicator.h"
#include "servant/Application.h"
#include <cmath>

#include "servant/PropertyReport.h"
#include "servant/StatF.h"
#include "servant/PropertyF.h"

using namespace std;
using namespace taf;

taf::Communicator _comm;
typedef  map<StatPropMsgHead, StatPropMsgBody>		MapStatPropMsg;

void testProperty()
{
    vector<int> v;
    v.push_back(10);
    v.push_back(30);
    v.push_back(50);
    v.push_back(80);
    v.push_back(100);

    //PropertyReportPtr srp1 = _comm.getStatReport()->createPropertyReport("test1", PropertyReport::sum(),PropertyReport::avg(), PropertyReport::count(), PropertyReport::max(), PropertyReport::min());
    PropertyReportPtr srp2 = _comm.getStatReport()->createPropertyReport("testjohnson", PropertyReport::sum(),PropertyReport::avg() ,PropertyReport::distr(v));
	int propValue = 0x47777777;
	srand((int)time(0)); 
	
	MapStatPropMsg mTemp;
	StatPropMsgHead head;
	StatPropMsgBody body;
	
	head.moduleName 	 = "test.property";
	head.ip 			 = "172.27.34.213";
	head.propertyName	 = "testjohnson";
	
	StatPropInfo sp;
	sp.policy = "Sum";
	sp.value  = "5555"; //0x1777 7777
	body.vInfo.push_back(sp);

	
	sp.policy = "Avg";
	sp.value  = "100000"; //0x1777 7777
	body.vInfo.push_back(sp);
	
	sp.policy = "Max";
	sp.value  = "9990000"; //0x1777 7777
	body.vInfo.push_back(sp);
	mTemp[head] = body;


	PropertyFPrx propertyPrx = NULL;
 	propertyPrx = _comm.stringToProxy<PropertyFPrx>("taf.tafproperty.PropertyObj@udp -h 172.27.34.213 -p 14337");
	LOG->debug() << "[TAF][StatReport::reportPropMsg send size:[" << mTemp.size()<< "]"<< endl;

	for ( int i = 0; i <  6; i++ )
	{
		propertyPrx->async_reportPropMsg(NULL,mTemp);
		sleep(10);
	}

/*
    for ( int i = 0; i < 160; i++ )
    {
        usleep(1000000);
		propValue = 0x17777777 + rand() % 100;
        LOG->debug()<<"i:"<<i << "propValue:" << propValue <<endl;
       // srp1->report(propValue );
        srp2->report(propValue);
    }
    */
    LOG->error() << "finish" << endl;
}

int main( int argc, char* argv[] )
{
    try
    {
        _comm.setProperty("modulename", "test.property");
        _comm.setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.34.213 -p 17890");
        _comm.setProperty("property", "taf.tafproperty.PropertyObj");
        _comm.setProperty("report-interval", "1000");

        testProperty();
    }
    catch ( exception& ex )
    {
        cout << ex.what() << endl;
    }
    return 0;
}


