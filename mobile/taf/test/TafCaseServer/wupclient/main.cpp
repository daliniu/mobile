#include <iostream>
#include "util/tc_socket.h"
#include "util/tc_file.h"
#include "util/tc_option.h"
#include "jce/wup.h"

#include "TafCaseServant.h"


struct option_t
{
	std::string 	sHost;
	unsigned short	iPort;
	
	std::string		sServant;
	std::string		sFunc;
	
	unsigned short	iWupVer;
};


int testunsigned(const struct option_t & option)
{
	try
	{
		//设置WUP数据
		wup::TafUniPacket<> up;
		
		//up.setServantName("Test.TafCaseServer.TafCaseServantObj");
		up.setServantName("Java.TestCase.TafCaseServant");
		up.setFuncName("testUnsigned");
		up.put("ubInUin", (UInt8)155);
		up.put("usInUin", (UInt16)65500);
		up.put("uiInUin", (UInt32)2200000000LL);
		
		//转换成数据流
		char szSendBuff[2048];
		uint32_t iSendBuffLen = sizeof(szSendBuff);
		up.encode(szSendBuff, iSendBuffLen);
		std::cout << "Encode Len:" << iSendBuffLen << "|" << std::endl;
		
		//初始化
		taf::TC_Socket conn;
		conn.createSocket();
		//conn.connect("172.27.205.110", 30000);
		conn.connect("10.130.64.220", 12121);
		
		int iSend = conn.send(szSendBuff, iSendBuffLen);
		std::cout << "Send Buffer:" << iSend << std::endl;
		
		char szRecvBuff[2048];
		memset(szRecvBuff, 0, sizeof(szRecvBuff));

		int iRecvLen = conn.recv(szRecvBuff, sizeof(szRecvBuff));
		std::cout << "Recv Len:" << iRecvLen << std::endl;

		//解码
		wup::TafUniPacket<> de;
		de.decode(szRecvBuff, iRecvLen);

		unsigned char 	ubUin = 0;
		unsigned short	usUin = 0;
		unsigned int	uiUin = 0;
		
		de.get("ubOutUin", ubUin);
		de.get("usOutUin", usUin);
		de.get("uiOutUin", uiUin);
		
		std::cout << "ubOutUin:" << ubUin << std::endl;
		std::cout << "usOutUin:" << usUin << std::endl;
		std::cout << "uiOutUin:" << uiUin << std::endl;
	}
	catch (std::exception & ex)
	{
		std::cout << "Exception:" << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception." << std::endl;
	}
	

	return 0;
}


int testgetuser(const struct option_t & option)
{
	try
	{
		//设置WUP数据
		wup::TafUniPacket<> up;
		up.setTafVersion(option.iWupVer);
		
		up.setServantName(option.sServant);
		up.setFuncName(option.sFunc);
		
		Test::UserInfo user;
		user.usQQ = 65500;
		user.uiQQ = 2200000000LL;
		up.put("inUser", user);
		
		//转换成数据流
		char szSendBuff[2048];
		uint32_t iSendBuffLen = sizeof(szSendBuff);
		up.encode(szSendBuff, iSendBuffLen);
		std::cout << "Encode Len:" << iSendBuffLen << "|" << std::endl;
		
		//初始化
		taf::TC_Socket conn;
		conn.createSocket();
		conn.connect(option.sHost, option.iPort);
		
		int iSend = conn.send(szSendBuff, iSendBuffLen);
		std::cout << "Send Buffer:" << iSend << std::endl;
		
		char szRecvBuff[2048];
		memset(szRecvBuff, 0, sizeof(szRecvBuff));

		int iRecvLen = conn.recv(szRecvBuff, sizeof(szRecvBuff));
		std::cout << "Recv Len:" << iRecvLen << std::endl;

		//解码
		wup::TafUniPacket<> de;
		de.setTafVersion(option.iWupVer);
		de.decode(szRecvBuff, iRecvLen);

		Test::UserInfo deuser;
		de.get("outUser", deuser);
		
		std::cout << "ubOutUin:" << deuser.usQQ << std::endl;
	}
	catch (std::exception & ex)
	{
		std::cout << "Exception:" << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception." << std::endl;
	}
	

	return 0;
}

//./wupclient --s=Java.TestCase.TafCaseServant --h=10.130.64.220  --p=12121 --w=2 --f=getUser
//./wupclient --s=Test.TafCaseServer.TafCaseServantObj --h=172.27.205.110 --p=30000 --w=2 --f=getUser

int main(int argc, char ** argv)
{
	taf::TC_Option op;
	op.decode(argc, argv);

	struct option_t option;
	option.sHost	= op.getValue("h");
	option.iPort	= atoi(op.getValue("p").c_str());
	option.sServant	= op.getValue("s");
	option.iWupVer	= atoi(op.getValue("w").c_str());
	option.sFunc	= op.getValue("f");
		
	std::cout << "Servant:" << option.sServant << std::endl;
	std::cout << "Func   :" << option.sFunc << std::endl;
	std::cout << "Host   :" << option.sHost << std::endl;
	std::cout << "Port   :" << option.iPort << std::endl;
	std::cout << "Wup    :" << option.iWupVer << std::endl;
	
	if (option.sFunc == "getUser")
	{
		testgetuser(option);
	}
	else if (option.sFunc == "testUnsigned")
	{
		testunsigned(option);
	}
	else
	{
		std::cout << "no func map, exit" << std::endl; 
	}
	
	return 0;
}
