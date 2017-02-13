#include "KevinTestServant.h"
#include "util/tc_http.h"
#include "util/tc_file.h"
#include "util/tc_option.h"
#include "log/taf_logger.h"
#include "servant/Application.h"
//#include "servant/AppCache.h"

using namespace taf;

int testsetuser(TC_Option & op, CommunicatorPtr & pcomm)
{
    try
    {
		int iLoop = atoi(op.getValue("loop").c_str());
		iLoop     = iLoop > 10000 || iLoop <= 0?1:iLoop;
		std::cout << "LOOP:" << iLoop << std::endl;

		std::string sUin = op.getValue("uin");
		if (sUin.empty()) sUin = "155069599";
		std::cout << "UIN:" << sUin << std::endl;
		
		TafDyeingSwitch dye;
		if (op.hasParam("dyeing"))
		{
			dye.enableDyeing();
		}
				
		timeval begin = {0, 0};
		gettimeofday(&begin, NULL);
		
		for (int i = 0; i < iLoop; i++)
		{
			LOG->debug() << "iclient dfdfd" << endl;
		
			Test::KevinTestServantPrx proxy = pcomm->stringToProxy<Test::KevinTestServantPrx>("Test.KevinTestServer.KevinTestServantObj");

			Test::stUserInfo_t inUser;
			inUser.sUserID 		= sUin;
			inUser.sUserName	= "kevin";
			inUser.sUserDesc	= "SZ.TENCENT";

			Test::stUserInfo_t out;
			std::string sRet = proxy->setUser(sUin, inUser, out);
			std::cout << "11ret:" << sRet << std::endl;

			int iTime = atoi(op.getValue("sleep").c_str());
			iTime = iTime <= 0 ?0:iTime;
			if (iTime > 0) 
			{
				std::cout << "To sleep " << iTime << "ms" << std::endl;
				usleep(iTime * 1000);
			}
		}

		timeval end = {0, 0};
		gettimeofday(&end, NULL);
	
		printf("time:%lu\n", (end.tv_sec - begin.tv_sec)* 1000 + (end.tv_usec - begin.tv_usec)/1000);
	}
    catch (std::exception & e)
    {
        std::cout << "sss:" << e.what() << std::endl;
		
		return -1;
    }

	return 0;
}



/************************************************************************************************
 **	异步调用接口
 **
 **/
class SelfPrxCallback: public Test::KevinTestServantPrxCallback
{
public:
	virtual ~SelfPrxCallback(){}
	
	virtual void callback_test(taf::Int32 ret)
	{ 
		LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << endl;
		std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << endl;
	}
	virtual void callback_test_exception(taf::Int32 ret)
	{ 
		throw std::runtime_error("callback_test_exception() overloading incorrect."); 
	}

	virtual void callback_setUser(const std::string& ret,  const Test::stUserInfo_t& outUser)
	{ 
		std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << "|" << ServantProxy::taf_invoke_endpoint().toString() << endl;
		
		LOG->debug() << "CLIENT|" << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << "|" << ServantProxy::taf_invoke_endpoint().toString() << endl;
	}
	virtual void callback_setUser_exception(taf::Int32 ret)
	{ throw std::runtime_error("callback_setUser_exception() overloading incorrect."); }

	virtual void callback_pay(const std::string& ret,  const std::string& sret)
	{ 
		LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << endl;
		std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|RET:" << ret << endl;
	}
	virtual void callback_pay_exception(taf::Int32 ret)
	{ throw std::runtime_error("callback_pay_exception() overloading incorrect."); }
};

int testsetuser_async(TC_Option & op, CommunicatorPtr & pcomm)
{
    try
    {
		int iLoop = atoi(op.getValue("loop").c_str());
		iLoop     = iLoop > 10000 || iLoop <= 0?1:iLoop;
		std::cout << "LOOP:" << iLoop << std::endl;

		std::string sUin = op.getValue("uin");
		if (sUin.empty()) sUin = "155069599";
		std::cout << "UIN:" << sUin << std::endl;
		
		TafDyeingSwitch dye;
		if (op.hasParam("dyeing"))
		{
			dye.enableDyeing();
		}
				
		timeval begin = {0, 0};
		gettimeofday(&begin, NULL);
		
		std::string sServant = "Test.KevinTestServer.KevinTestServantObj";
		
		if (op.hasParam("host") && op.hasParam("port"))
		{
			sServant += "@tcp -h " + op.getValue("host") + " -p " + op.getValue("port") + " -t 60000";
		}
		LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|" << sServant << endl;
		std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|" << sServant << endl;
		
		Test::KevinTestServantPrx proxy = pcomm->stringToProxy<Test::KevinTestServantPrx>(sServant);
		
		for (int i = 0; i < iLoop; i++)
		{
			LOG->debug() << "iclient dfdfd" << endl;
		
			Test::stUserInfo_t inUser;
			inUser.sUserID 		= sUin;
			inUser.sUserName	= "kevin";
//			inUser.sUserDesc	= "SZ.TENCENT";

			Test::KevinTestServantPrxCallbackPtr cb = new SelfPrxCallback();
			proxy->async_setUser(cb, sUin, inUser);
			std::cout << "sended call for async_setUser" << std::endl;
			
			int iTime = atoi(op.getValue("sleep").c_str());
			iTime = iTime <= 0 ?0:iTime;
			if (iTime > 0) 
			{
				std::cout << "To sleep " << iTime << "ms" << std::endl;
				usleep(iTime * 1000);
			}
			LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "--------------------------------" <<std::endl;
		}

		timeval end = {0, 0};
		gettimeofday(&end, NULL);
	
		printf("time:%lu\n", (end.tv_sec - begin.tv_sec)* 1000 + (end.tv_usec - begin.tv_usec)/1000);
	}
    catch (std::exception & e)
    {
        std::cout << "sss:" << e.what() << std::endl;
		
		return -1;
    }

	
	std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "--------------------------------" <<std::endl;
	return 0;
}


/**************************************************************************************************
 **	pay接口
 **
 **/
int testpay(TC_Option & op, CommunicatorPtr & pcomm)
{
	std::string sServant = "Test.KevinTestServer.KevinTestServantObj";
	
	if (op.hasParam("host") && op.hasParam("port"))
	{
		sServant += "@tcp -h " + op.getValue("host") + " -p " + op.getValue("port") + " -t 60000";
	}
	
	std::cout << __FUNCTION__  << "|" << __LINE__ << "|SERVANT:" << sServant << std::endl;
	
	std::string sUin = op.getValue("uin");
	if (sUin.empty()) sUin = "kevin";
		
    try
    {
		TafDyeingSwitch dye;
		if (op.hasParam("dyeing"))
		{
			dye.enableDyeing();
		}

		Test::KevinTestServantPrx proxy = pcomm->stringToProxy<Test::KevinTestServantPrx>(sServant);

		int iLoop = atoi(op.getValue("loop").c_str());
		iLoop = (iLoop <= 0 || iLoop >= 100)?1:iLoop;

		
		const int ISIZE = 5;
		int uin[] = {1, 2, 12, 22, 3};

		for (int i = 0; i < iLoop; i++)
		{
			std::string spay ;
			//std::string sRet = proxy->pay(sUin, spay);
			
			std::string sRet = proxy->taf_hash(uin[i%ISIZE])->pay(TC_Common::tostr(uin[i%ISIZE]), spay);
			
			//std::cout << "uin:" << uin[i%ISIZE] << "ret:" << sRet << std::endl;
			printf("uin:%2d IP:%s\n", uin[i%ISIZE], ServantProxy::taf_invoke_endpoint().toString().c_str());
			//std::cout << "endpoint:" << ServantProxy::taf_invoke_endpoint().toString() << std::endl;
			
			//sleep(1);
		}
	}
    catch (std::exception & e)
    {
        std::cout << "sss:" << e.what() << std::endl;
		
		return -1;
    }

	return 0;
}


void report_property(TC_Option & op, CommunicatorPtr & pcomm)
{
	//初始化分布数据范围
	vector<int> v;
	v.push_back(10);
	v.push_back(30);
	v.push_back(50);
	v.push_back(80);
	v.push_back(100);

	//创建test1属性，该属性用到了上诉所有的集中统计方式，注意distrv的初始化
	taf::PropertyReportPtr srp = pcomm->getStatReport()->createPropertyReport("MyKevinProperty", 
		taf::PropertyReport::sum(),	//求和 
		taf::PropertyReport::avg(), 
		taf::PropertyReport::count(),
		taf::PropertyReport::max(), 
		taf::PropertyReport::min(),
		taf::PropertyReport::distr(v));

	//上报数据，property只支持int类型的数据上报
	for ( int i = 0; i < 10; i++ )
	{
		sleep(1);
		srp->report(rand() % 10);
	}
}


void report_notify(TC_Option & op, CommunicatorPtr & pcomm)
{
	TAF_NOTIFY_NORMAL("123456");
	TAF_NOTIFY_WARN("123456");
	TAF_NOTIFY_ERROR("123456");
	
	TafRemoteNotify::getInstance()->report("REPORT:123456");
}



void test_stat(TC_Option & op, CommunicatorPtr & pcomm)
{
	taf::StatReport * stat = pcomm->getStatReport();
	
	stat->report("Test.KevinTestServer_Client", "mypay", "172.25.38.67", 28000, taf::StatReport::STAT_SUCC, 1000, 0, true);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	测试第三方协议
//

static void streamRequest(const RequestPacket & request, string & buff)
{
	unsigned int iLen = ::htonl(sizeof(int) + sizeof(unsigned int) + request.sBuffer.size());
	unsigned int iID  = ::htonl(request.iRequestId);
        
	buff.append((const char *)&iLen, sizeof(int));
	buff.append((const char *)&iID, sizeof(int));
	buff.append((const char *)(&request.sBuffer[0]), request.sBuffer.size());
	
	std::cout << "Encode Len:" << buff.size() << "|" << sizeof(int) + request.sBuffer.size() << std::endl;
}

void test_thrid(TC_Option & op, CommunicatorPtr & pcomm)
{
	std::cout << __FUNCTION__ << "|" << __LINE__ << std::endl;
	std::string sServant = "Test.KevinTestServer.ThridProtoServantObj@tcp -h " + op.getValue("host") + " -p " + op.getValue("port"); 

	std::cout << "Servant:" << sServant << std::endl;
	
	
	taf::ServantPrx proxy = pcomm->stringToProxy<taf::ServantPrx>(sServant);

	ProxyProtocol proto;
	
	proto.requestFunc	= streamRequest;
	proto.responseFunc	= ProxyProtocol::streamResponse<0, unsigned int, true, 4, unsigned int, true>;
	
	proxy->taf_set_protocol(proto);
	
	taf::ResponsePacket rsp;
	proxy->rpc_call(proxy->taf_gen_requestid(), "Test.KevinTestServer.ThridProtoServantObj", sServant.c_str(), sServant.size(), rsp);


	std::cout << "ret:" << rsp.iRequestId << std::endl;
}


/**
	./KevinTestClient --test-pay --host=172.27.205.110 --port=28000 --mainsleep=100

	./KevinTestClient --test-report --host=172.27.205.110 --port=28000 --mainsleep=100

	./KevinTestClient --test-notify --host=172.27.205.110 --port=28000

	./KevinTestClient --test-stat --host=172.27.205.110 --port=28000 --mainsleep=200

	./KevinTestClient --test-pay --dyeing --host=172.27.205.110 --port=28000 --mainsleep=200
	./KevinTestClient --test-pay --dyeing --host=172.27.205.110 --port=28000 --mainsleep=200

	./KevinTestClient --test-pay --host=172.27.205.110 --port=28000  --uin=155069599 --mainsleep=200  --dyeing 

	./KevinTestClient --test-setuser --loop=40 --sleep=1000 --async --mainsleep=100
	
	./KevinTestClient --test-setuser --mainsleep=100
	
	
	./KevinTestClient --test-thrid --host=172.27.205.110 --port=29000 --mainsleep=20
 ***/
//./KevinTestClient --test-thrid --host=172.27.205.110 --port=28100 --mainsleep=20
//./KevinTestClient --test-thrid --host=172.27.39.21 --port=28100 --mainsleep=20
//./KevinTestClient --test-setuser --async --dyeing --host=172.27.205.110 --port=28000 --mainsleep=20
//./KevinTestClient --test-setuser --loop=100 --sleep=3000 --async --mainsleep=100



void usage(int argc, char ** argv)
{
	std::cout << "--test-pay --dyeing" << std::endl;
	std::cout << "--test-setuser --dyeing --loop<> --uin<> --sleep<ms> --async" << std::endl;
	
	
	std::cout << "\n\ncommon options:" << std::endl;
	std::cout << "--mainsleep<s>\n" << std::endl;

	std::cout << argv[0] << " --test-pay --host=172.25.38.67 --port=28000 --uin=155069599 --mainsleep=200" << std::endl;
	
}


int main(int argc , char ** argv)
{
	try
	{
		TC_Option op;
		op.decode(argc, argv);

		CommunicatorPtr pcomm = new Communicator();
		pcomm->setProperty("locator", "taf.tafregistry.QueryObj@tcp -h 172.27.34.213 -p 17890");
		pcomm->setProperty("property", "taf.tafproperty.PropertyObj");
		pcomm->setProperty("stat", "taf.tafstat.StatObj");
		pcomm->setProperty("report-interval", "1000");
		pcomm->setProperty("notify", "taf.tafnotify.NotifyObj");
		pcomm->setProperty("modulename", "Test.KevinTestServerCC");

		TafRemoteNotify::getInstance()->setNotifyInfo(pcomm, "taf.tafnotify.NotifyObj", "Test", "KevinTestServer1111111");
		
		std::cout << "ModuleName:" << ClientConfig::ModuleName << std::endl;	
			/*
		for (std::set<std::string>::iterator it = ClientConfig::SetLocalIp.begin(); it != ClientConfig::SetLocalIp.end(); it++)
		{
			std::cout << "IP:" << *it << std::endl;
		}
		
	
		AppCache::getInstance()->setCacheInfo("./data/client.tafdat", 0);
		*/
	
		TafRollLogger::getInstance()->setLogInfo("Test", "KevinTestServer", "./log", 100000, 10, pcomm, "taf.taflog.LogObj");
		TafRollLogger::getInstance()->sync(false);
		TafTimeLogger::getInstance()->setLogInfo(pcomm, "taf.taflog.LogObj", "Test", "KevinTestServer", "./log");
		TafTimeLogger::getInstance()->initFormat("test", "%Y%m%d");
		TafTimeLogger::getInstance()->enableRemote("test", false);
		
		if (op.hasParam("dyeing"))
		{
			std::cout << "Enable dyeing" << endl;
			std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << endl;
			TafRollLogger::getInstance()->setLogInfo("Test", "KevinTestServer", "./log", 100000, 10, pcomm, "taf.taflog.LogObj");
			std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << endl;
			TafRollLogger::getInstance()->sync(false);
			std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << endl;
			TafTimeLogger::getInstance()->setLogInfo(pcomm, "taf.taflog.LogObj", "Test", "KevinTestServer", "./log");
			std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << endl;
		}
		
		std::cout << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << endl;
		
		if (op.hasParam("test-pay"))
		{
			testpay(op, pcomm);
		}
		else if (op.hasParam("test-setuser") && !op.hasParam("async"))
		{
			testsetuser(op, pcomm);
		}
		else if (op.hasParam("test-setuser") && op.hasParam("async"))
		{
			testsetuser_async(op, pcomm);
		}
		else if (op.hasParam("test-report"))
		{
			report_property(op, pcomm);
		}
		else if (op.hasParam("test-notify"))
		{
			report_notify(op, pcomm);
		}
		else if (op.hasParam("test-stat"))
		{
			test_stat(op, pcomm);
		}
		else if (op.hasParam("test-thrid"))
		{
			test_thrid(op, pcomm);
		}
		else
		{
			usage(argc, argv);
		}
	
		if (op.hasParam("mainsleep"))
		{
			int time = atoi(op.getValue("mainsleep").c_str());
			if (time > 0) sleep(time);
		}
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
