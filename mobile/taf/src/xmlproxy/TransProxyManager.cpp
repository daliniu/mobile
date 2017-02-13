#include "TransProxyManager.h"


std::string TransProxyManager::loadproxy(const std::string & sFile)
{
	try
	{
		TC_Config conf;
		conf.parseFile(sFile);
	
		TC_ThreadLock::Lock lock(*this);

		//查找根域中的转换关系
		std::map<std::string, std::string> mapName;
		conf.getDomainMap("/proxymap", mapName);

		//查找子域中的转换关系
		std::vector<std::string> vecDomain;
		if (conf.getDomainVector("/proxymap", vecDomain))
		{
			for (std::vector<std::string>::size_type i = 0; i < vecDomain.size(); i++)
			{
				std::map<std::string, std::string> mapTemp;
				if (conf.getDomainMap("/proxymap/" + vecDomain[i], mapTemp))
				{
					for (std::map<std::string, std::string>::iterator it = mapTemp.begin(); it != mapTemp.end(); it++)
						mapName[vecDomain[i] + "." + it->first] = it->second;
				}
			}
		}

		//新的转换关系和缓存中的进行比对
		for (std::map<std::string, std::string>::iterator it = _mapName.begin(); it != _mapName.end(); it++)
		{
			std::map<std::string, std::string>::iterator itTemp = mapName.find(it->first);
			if (itTemp == mapName.end())
			{
				_mapProxy.erase(it->first);
				continue;
			}

			if (itTemp->second != it->second)
			{
				_mapProxy.erase(it->first);
			}
		}

		_mapName.swap(mapName);


		for (std::map<std::string, std::string>::iterator it = _mapName.begin(); it != _mapName.end(); it++)
			LOG->debug() << "P:" << it->first << "   " << it->second << endl;

		return "loadproxy success";
	}
	catch (std::exception & ex)
	{
		LOG->error() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|ERROR:" << ex.what() << endl;
		return std::string("loadproxy error:") + ex.what();
	}

	return "loadproxy error";
}


ServantPrx TransProxyManager::getProxy(const std::string & sMidName)
{
	std::map<std::string, ServantPrx>::iterator itProxy = _mapProxy.find(sMidName);
	if (itProxy != _mapProxy.end())
	{
		LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|GET SERVANT IN BUFF:" << sMidName << "," << itProxy->second->taf_name() << endl;
		return itProxy->second;
	}

	std::map<std::string, std::string>::iterator itName = _mapName.find(sMidName);
	if (itName == _mapName.end())
	{
		LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|NOT FOUND (" << sMidName << ") IN PROXYMAP" << endl;
		return NULL;
	}

	{
		LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|CREATE SERVANTPRX (" << itName->second << ") FOR (" << sMidName << ")" << endl;
		_mapProxy[sMidName] = Application::getCommunicator()->stringToProxy<ServantPrx>(itName->second);
	}

	return _mapProxy[sMidName];
}

