#include "TafCaseServantImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void TafCaseServantImp::initialize()
{
	//initialize servant here:
	//...
}

//////////////////////////////////////////////////////
void TafCaseServantImp::destroy()
{
	//destroy servant here:
	//...
}


taf::Int32 TafCaseServantImp::getUser(const Test::UserInfo & inUser,Test::UserInfo &outUser,taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __FUNCTION__ << endl;
	inUser.display(LOG->debug());
	
	outUser = inUser;
	
	return 0;
}

taf::Int32 TafCaseServantImp::testUnsigned(taf::UInt8 ubInUin,taf::UInt16 usInUin,taf::UInt32 uiInUin,taf::UInt8 &ubOutUin,taf::UInt16 &usOutUin,taf::UInt32 &uiOutUin,taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __FUNCTION__ << endl;

	LOG->debug() << "ubInUin:" << ubInUin << endl;
	LOG->debug() << "usInUin:" << usInUin << endl;
	LOG->debug() << "uiInUin:" << uiInUin << endl;

	ubOutUin = ubInUin;
	usOutUin = usInUin;
	uiOutUin = uiInUin;

	return 0;
}
