#include <iostream>
#include "util/tc_socket.h"
#include "util/tc_file.h"
#include "util/tc_option.h"

extern "C"{
#include "wup_c.h"
#include "TafCaseServant.h"
}


struct option_t
{
	std::string 	sHost;
	unsigned short	iPort;
	
	std::string		sServant;
	std::string		sFunc;
	
	unsigned short	iWupVer;
};

int testuser(const struct option_t & option)
{
	try
	{
		//设置WUP数据
		UniPacket * up = UniPacket_new();
		UniPacket_setVersion(up, option.iWupVer);

		JString_assign(up->sServantName, option.sServant.c_str(), option.sServant.size());
		JString_assign(up->sFuncName,    option.sFunc.c_str(),    option.sFunc.size());
		
		Test_UserInfo * user 	= Test_UserInfo_new();
		user->ubQQ				= 155;
		user->usQQ				= 65500;
		user->uiQQ				= 2200000000LL;
		user->uiTest			= 111;
	
		WUP_putStruct(up, "inUser", user);
		
		//转换成数据流
		char * szBuff;
		uint32_t iBuffLen = 0;
		int iRet = UniPacket_encode(up, &szBuff, &iBuffLen);
		
		std::cout << "Len:" << iBuffLen << "|" << iRet << std::endl;
		
		//初始化
		taf::TC_Socket conn;
		conn.createSocket();
		conn.connect(option.sHost, option.iPort);
		int iSend = conn.send(szBuff, iBuffLen);
		std::cout << "Send:" << iSend << std::endl;
		
		char szRecvBuff[2048];
		memset(szRecvBuff, 0, sizeof(szRecvBuff));
		
		int iRecvLen = conn.recv(szRecvBuff, sizeof(szRecvBuff));
		
		std::cout << "Recv:" << iRecvLen << std::endl;
	
		//解码
		UniPacket * de = UniPacket_new();
		UniPacket_setVersion(de, option.iWupVer);
		
		iRet = UniPacket_decode(de, szRecvBuff, iRecvLen);
		std::cout << "Decode Result:" << iRet << std::endl;

		Test_UserInfo * deuser 	= Test_UserInfo_new();
		iRet = WUP_getStruct(de, "outUser", deuser);
		std::cout << "Get Struct Result:" << iRet << "|"  << std::endl;
	
		std::cout << "ubQQ   :" << deuser->ubQQ << std::endl;
		std::cout << "usQQ   :" << deuser->usQQ << std::endl;
		std::cout << "uiQQ   :" << deuser->uiQQ << std::endl;
		std::cout << "uiTest :" << deuser->uiTest << std::endl;
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
	#if 0


	///////////////////////////////////////////////////////////////////////////////////////////////
	char *sz_encode = NULL;
	uint32_t encode_len = 0;
	int i = 0;
	int ret;

	//编码
	{
		//S1:生成一个UniAttribute结构体
		UniAttribute * attr = UniAttribute_new();
		if (attr == NULL)
		{
			printf("UniAttribute_new Error\n");
			return -1;
		}
		WUP_setVersion(attr, iVersion);
		
		//S2:基本类型的使用方法
		{
			WUP_putChar(attr, 	"mychar", 	123);
			WUP_putShort(attr, 	"myshort", 	30000);
			WUP_putFloat(attr, 	"myfloat", 	123.123);
			WUP_putDouble(attr, "mydouble", 12345.12345);
			WUP_putInt32(attr, 	"myi32", 	1234567890);
			WUP_putInt64(attr, 	"myi64", 	12345678900000ll);
		}
		//S3:结构体的使用方法
		{
			Test_QQUser * user = Test_QQUser_new();
			//user->uin = 10010;
			//user->num = 10000;
			//JString_assign(user->name, "Test_ABC", strlen("Test_ABC"));
	
			WUP_putStruct(attr, "my_struct", user);
		}
		//S4:编码一个VECTOR
		{
			JArray * array = JArray_new("Test_ABC");
			JceOutputStream * os = JceOutputStream_new();
			
			Test_QQUser * user = Test_QQUser_new();
			for (i = 0; i < 10; i++)
			{
				Test_QQUser_init(user);
				
				user->uin = i + 100;
				user->num = i + 100;
				
				char sTmp[1024];
				memset(sTmp, 0, sizeof(sTmp));
				snprintf(sTmp, sizeof(sTmp), "Test_ABC%d", i + 100);
				JString_assign(user->name, sTmp, strlen(sTmp));

				JceOutputStream_reset(os);
				ret = JceOutputStream_writeStruct(os, user, 0);
				
				printf("ret:%d|length:%d\n", ret, JceOutputStream_getLength(os));

				JArray_pushBack(array, JceOutputStream_getBuffer(os), JceOutputStream_getLength(os));
			}
		
			printf("dec array size:%d\n", JArray_size(array));
		
			WUP_putVector(attr, "my_vector", array);
		}

		//编码一个MAP
		{
			JMapWrapper * my_map 		= JMapWrapper_new("int32", "string");
			JceOutputStream * os_first 	= JceOutputStream_new();
			JceOutputStream * os_second	= JceOutputStream_new();

			for (i = 0; i < 10; i++)
			{
				JceOutputStream_reset(os_first);
				JceOutputStream_writeInt32(os_first, i + 100, 0);

				char sTmp[1024];				
				memset(sTmp, 0, sizeof(sTmp));
				snprintf(sTmp, sizeof(sTmp), "testmap%d", i + 100);
				JceOutputStream_reset(os_second);
				JceOutputStream_writeStringBuffer(os_second, sTmp, strlen(sTmp), 1);

				JMapWrapper_put(my_map, JceOutputStream_getBuffer(os_first), JceOutputStream_getLength(os_first), JceOutputStream_getBuffer(os_second),JceOutputStream_getLength(os_second));
			}

			WUP_putMap(attr, "my_map", my_map);
		}

		UniAttribute_encode(attr, &sz_encode, &encode_len);
	}

	printf("\n\n\n\n\nEncode-Length:%d\n", encode_len);
		
	{
		//解码
		UniAttribute * attr = UniAttribute_new();
		if (NULL == attr)
		{
			printf("%s|%d|\n", __FILE__, __LINE__);
			exit(0);
		}
		WUP_setVersion(attr, iVersion);

		ret = UniAttribute_decode(attr, sz_encode, encode_len);
		if (JCE_SUCCESS != ret)
		{
			printf("%s|%d|ERROR:%d|\n", __FILE__, __LINE__, ret);
			exit(0);
		}
		
	
		char c		= 0;
		short sh	= 0;
		float f		= 0;
		double d	= 0;
		Int32 i32	= 0;
		Int64 i64	= 0;

	

		ret = WUP_getChar(attr, "mychar", &c);
		printf("ret:%d, char:%d\n", ret, c);
		ret = WUP_getShort(attr, "myshort", &sh);
		printf("ret:%d, short:%d\n", ret, sh);
		ret = WUP_getFloat(attr, "myfloat", &f);
		printf("ret:%d, float:%f\n", ret, f);
		ret = WUP_getDouble(attr, "mydouble", &d);
		printf("ret:%d, double:%f\n", ret, d);
		ret = WUP_getInt32(attr, "myi32", &i32);
		printf("ret:%d, int32:%d\n", ret, i32);
		ret = WUP_getInt64(attr, "myi64", &i64);
		printf("ret:%d, int64:%lld\n", ret, i64);
		
		
		{
			printf("decoding my_struct-----------------------------------------------------------------\n");
			Test_QQUser * user = Test_QQUser_new();
			ret = WUP_getStruct(attr, "my_struct", user);
			printf("ret:%d\n", ret);

			printf("uin:%d\n", user->uin);
			printf("num:%d\n", user->num);
			printf("name:%s\n", JString_data(user->name));
		}
	
		//解码一个VECTOR
		{
			printf("decode my_vector--------------------------------------------------------------------\n");
			JArray * array 		= JArray_new("Test_ABC");
			JceInputStream * is = JceInputStream_new();

			ret = WUP_getVector(attr, "my_vector", array);
			printf("ret:%d, vector size:%d\n", ret, JArray_size(array));

			Test_QQUser * user = Test_QQUser_new();
			for (i = 0; i < JArray_size(array); i++)
			{
				printf("LENGTH:%d\n", JArray_getLength(array, i));
			
				Test_QQUser_init(user);
				
				JceInputStream_reset(is);
				JceInputStream_setBuffer(is, JArray_getPtr(array, i), JArray_getLength(array, i));
				JceInputStream_readStruct(is, user, 0, true);
				
				printf("uin:%d\n", user->uin);
				printf("num:%d\n", user->num);
				printf("name:%s\n\n", JString_data(user->name));
			}
		}
		
		
		//解码一个MAP
		{
			printf("decode my_map.....\n");
			JMapWrapper * my_map	= JMapWrapper_new("int32", "string");
			JString * string_tmp	= JString_new();
			JceInputStream * is 	= JceInputStream_new();

			ret = WUP_getMap(attr, "my_map", my_map);
			printf("ret:%d, map size:%d\n", ret, JMapWrapper_size(my_map));
			
			for(i = 0; i < JMapWrapper_size(my_map); ++i)
			{   
				JceInputStream_setBuffer(is, JArray_getPtr(my_map->first, i), JArray_getLength(my_map->first, i));
				Int32 iFirst = 0;
				JceInputStream_readInt32(is, &iFirst, 0, true);
				
				JceInputStream_setBuffer(is, JArray_getPtr(my_map->second, i), JArray_getLength(my_map->second, i));
				JceInputStream_readString(is, string_tmp, 1, true);
				printf("map index:%d, first:%d, second:%s\n", i, iFirst, JString_data(string_tmp));
			}
		}

	}
	
		#endif
		
	return 0;
}





int testunsigned(const struct option_t & option)
{
	try
	{
		//设置WUP数据
		UniPacket * up = UniPacket_new();
		UniPacket_setVersion(up, option.iWupVer);
	
		JString_assign(up->sServantName, option.sServant.c_str(), option.sServant.size());
		JString_assign(up->sFuncName,    option.sFunc.c_str(),    option.sFunc.size());
		
		WUP_putUInt8	(up, "ubInUin", 155);
		WUP_putUInt16	(up, "usInUin", 65500);
		WUP_putUInt32	(up, "uiInUin", 2200000000LL);

		//转换成数据流
		char * szBuff;
		uint32_t iBuffLen = 0;
		int iRet = UniPacket_encode(up, &szBuff, &iBuffLen);
		
		std::cout << "Encode Len:" << iBuffLen << "|" << iRet << std::endl;
		
		//taf::TC_File::save2file("my.data", szBuff, iBuffLen);				
		
		//初始化
		taf::TC_Socket conn;
		conn.createSocket();
		conn.connect(option.sHost, option.iPort);
		int iSend = conn.send(szBuff, iBuffLen);
		std::cout << "Send   Len:" << iSend << std::endl;
		
		char szRecvBuff[2048];
		memset(szRecvBuff, 0, sizeof(szRecvBuff));

		int iRecvLen = conn.recv(szRecvBuff, sizeof(szRecvBuff));
		std::cout << "Recv   Len:" << iRecvLen << std::endl;

		//解码
		UniPacket * de = UniPacket_new();
		//UniPacket_setVersion(de, option.iWupVer);
		iRet = UniPacket_decode(de, szRecvBuff, iRecvLen);
		
		std::cout << "Decode     :" << iRet << std::endl;
		std::cout << "Wup Version:" << UniPacket_getVersion(de) << "|IN:" << de->attr._iVer << std::endl;
		unsigned char 	ubUin = 0;
		unsigned short	usUin = 0;
		unsigned int	uiUin = 0;
		
		WUP_getUInt8	(de, "ubOutUin", &ubUin);
		WUP_getUInt16	(de, "usOutUin", &usUin);
		WUP_getUInt32	(de, "uiOutUin", &uiUin);
		
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

//./main java/cpp
//./wupcclient --s=Test.TafCaseServer.TafCaseServantObj --f=testUnsigned --h=172.27.206.150 --p=31000 --w=2 
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
	
	if (option.sFunc == "testUnsigned")
	{
		testunsigned(option);
	}
	else
	{
		testuser(option);
	}
	
	
	return 0;
}
