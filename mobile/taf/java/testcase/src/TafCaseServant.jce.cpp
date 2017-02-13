module Test
{
	struct UserInfo
	{
		0 require 	unsigned byte		ubQQ;
		1 require 	unsigned short		usQQ;
		2 require 	unsigned int		uiQQ;
		3 optional	unsigned int		uiTest = 11;
	};

interface TafCaseServant
{
    int test();
	
	int getUser(UserInfo inUser, out UserInfo outUser);
	int testUnsigned(unsigned byte ubInUin, unsigned short usInUin, unsigned int uiInUin, out unsigned byte ubOutUin, out unsigned short usOutUin, out unsigned int uiOutUin);
};

}; 
