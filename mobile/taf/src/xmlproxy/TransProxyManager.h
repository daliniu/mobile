#ifndef __TRANSPROXYMANAGER_H__
#define __TRANSPROXYMANAGER_H__

#include "util/tc_singleton.h"
#include "servant/Application.h"

class TransProxyManager : public TC_Singleton<TransProxyManager>, public TC_ThreadLock
{
public:
	TransProxyManager(){}

	/**
	 * 加载配置文件 
	 *  
	 * @param sFile 
	 * @return string 
	 */

	std::string loadproxy(const std::string & sFile);

	/**
	 * 根据配置名，获得代理类 
	 *  
	 *  @return ServantPrx;
	 */

	ServantPrx getProxy(const std::string & sMidName);

protected:
	std::map<std::string, ServantPrx> _mapProxy;	//配置sMidName到ServantProxy的转换
	std::map<std::string, std::string> _mapName;	//配置sMidName到ServantName的转换
};

#endif

