#include "AServantImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void AServantImp::initialize()
{
	//initialize servant here:
	//...
}

//////////////////////////////////////////////////////
void AServantImp::destroy()
{
	//destroy servant here:
	//...
}


int AServantImp::test(taf::JceCurrentPtr current) { return 0;};

string AServantImp::saysomething(const std::string& s, taf::JceCurrentPtr current)
{
    return s;
}
