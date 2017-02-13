#include "TestImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void TestImp::initialize()
{

}

//////////////////////////////////////////////////////
void TestImp::destroy()
{
}

taf::Int32 TestImp::test(taf::Int32 a,taf::Int32 b,taf::JceCurrentPtr current)
{
	ROLLLOG<<"a = "<<a<<",b = "<<b<<".result = "<<a+b<<endl;
}