#include "KevinTestServantImp.h"
#include "servant/Application.h"

using namespace std;

//////////////////////////////////////////////////////
void KevinTestServantImp::initialize()
{
	//initialize servant here:
	//...

	vector<int> v;
	v.push_back(10);
	v.push_back(30);
	v.push_back(50);
	v.push_back(80);
	v.push_back(100);

			
	_rpproxy = Application::getCommunicator()->getStatReport()->createPropertyReport("test1", 
					PropertyReport::sum(), 
					PropertyReport::avg(), 
					PropertyReport::count(),
					PropertyReport::max(), 
					PropertyReport::min(),
					PropertyReport::distr(v));

}

//////////////////////////////////////////////////////
void KevinTestServantImp::destroy()
{
	//destroy servant here:
	//...
}


std::string KevinTestServantImp::setUser(const std::string & suin, const Test::stUserInfo_t & inUser,Test::stUserInfo_t &outUser,taf::JceCurrentPtr current)
{
	struct timeval recv = current->getRecvTime();
	LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|UIN:" << suin << "|" << recv.tv_sec << ":" << recv.tv_usec << endl;
	DLOG << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|UIN:" << suin << "|" << recv.tv_sec << ":" << recv.tv_usec << endl;
	std::string sHeader = taf::TC_Common::tostr(current->getContext());
		
	for (int i = 0; i < 10; i++)
	{
		std::string s = "kevin" + TC_Common::tostr<int>(i);
		char * p = getenv(s.c_str());
		std::string senv = p == NULL?"":p;
		LOG->debug() << "ENV:(" << s << " = " << senv  << ")" << std::endl;
		DLOG << "ENV:(" << s << " = " << senv  << ")" << std::endl;
	}

	outUser.sUserID 	= inUser.sUserID + "_return";
    outUser.sUserName	= inUser.sUserName + "_return";
    outUser.sUserDesc	= inUser.sUserDesc + "_return";

	return __FUNCTION__ + ServerConfig::LocalIp + taf::TC_Common::now2str();
}

std::string KevinTestServantImp::pay(const std::string & suin,std::string &sret,taf::JceCurrentPtr current)
{
	
	LOG->debug() << __FILE__ << "|" << __FUNCTION__ << "|" << __LINE__ << "|UIN:" << suin << "|TID:" << pthread_self() << endl;
	DLOG << __FILE__ << "|" << __LINE__ << "|UIN:" << suin << endl;
	//FDLOG("myddd") << __FILE__ << "|" << __LINE__ << "|UIN:" << suin << endl;

	//TafRemoteNotify::getInstance()->report("REPROT:" + suin);
	
	//上报普通信息
	//TAF_NOTIFY_NORMAL("NORMAL:" + suin);

	//上报警告信息
	//TAF_NOTIFY_WARN("WARN:" + suin);

	//上报错误信息
	//TAF_NOTIFY_ERROR("ERROR:" + suin);
	
	return __FILE__ + ServerConfig::LocalIp + taf::TC_Common::now2str();
}
