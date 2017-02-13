#include "TafCaseServant.h"
#include "util/tc_option.h"

using namespace taf;

int testgetuser(TC_Option & op, CommunicatorPtr & pcomm)
{
	Test::TafCaseServantPrx proxy = pcomm->stringToProxy<Test::TafCaseServantPrx>("Test.TafCaseServer.TafCaseServantObj");

	Test::UserInfo inUser;
	inUser.ubQQ = 155;
	inUser.usQQ = 65500;
	inUser.uiQQ = 2200000000LL;

	Test::UserInfo outUser;
	proxy->getUser(inUser, outUser);
	
	std::cout << "ubQQ:" << (unsigned int)outUser.ubQQ << std::endl;
	
	outUser.display(std::cout);

	return 0;
}

int main(int argc , char ** argv)
{
	try
	{
		TC_Option op;
		op.decode(argc, argv);

		CommunicatorPtr pcomm = new Communicator();
	
		pcomm->setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.34.213 -p 17890");
		pcomm->setProperty("modulename", "Test.KevinTestServerCC");
	
		testgetuser(op, pcomm);

		return 0;
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "exception...." << std::endl;
	}

	return 0;
}
