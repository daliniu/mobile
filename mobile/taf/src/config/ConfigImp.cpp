#include "ConfigImp.h"

using namespace taf;

void ConfigImp::loadconf()
{
    extern TC_Config * g_pconf;

    TC_DBConf tcDBConf;
    tcDBConf.loadFromMap(g_pconf->getDomainMap("/taf/db"));
    _mysqlConfig.init(tcDBConf);

}

void ConfigImp::initialize()
{
    loadconf();
}
int ConfigImp::ListConfig(const string &app,const string &server,  vector<string> &vf,taf::JceCurrentPtr current)
{
    try
    {
        string host =  current->getIp();
        LOG->debug() << "ListConfig|" << app << "." << server << "|" << host << endl;
       //查ip对应配置
        string sNULL;
        string sSql =
            "select filename from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(app+"."+server) + "' "
            "and host='"+_mysqlConfig.escapeString(host)+"' "
            "and level=" + TC_Common::tostr<int>(eLevelIpServer);
			" and set_name ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_area ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_group ='" + _mysqlConfig.escapeString(sNULL) +"' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql <<"|"<<res.size()<< endl;
        for(unsigned i=0; i<res.size();i++)
        {
            vf.push_back(res[i]["filename"]);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<"exception: "<<ex.what()<<endl;
        return -1;
    }

    return 0;
}
bool ConfigImp::getSetInfo(string& sSetName,string& sSetArea,string& sSetGroup,const string& sSetDivision)
{
	vector<string> vtSetDivisions = TC_Common::sepstr<string>(sSetDivision,".");
	if(vtSetDivisions.size() != 3)
	{
		return false;
	}
	else
	{
		sSetName = vtSetDivisions[0];
		sSetArea = vtSetDivisions[1];
		sSetGroup = vtSetDivisions[2];
		return true;
	}
}
int ConfigImp::ListConfigByInfo(const ConfigInfo& configInfo,  vector<string> &vf,taf::JceCurrentPtr current)
{
	try
	{
		string sHost =  configInfo.host.empty()?current->getIp():configInfo.host;
		LOG->debug() << "ListConfigByInfo|" << configInfo.appname << "." << configInfo.servername <<"|"<<configInfo.setdivision << "|" << sHost << endl;

		string sCondition;
		if(!configInfo.setdivision.empty())
		{
			string sSetName,sSetArea,sSetGroup;
			if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
			{
				sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
				sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
				sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";
			}
			else
			{
				LOG->error()<<"setdivision is invalid: "<<configInfo.setdivision<<endl;
				throw TC_Mysql_Exception("setdivision is invalid:"+configInfo.setdivision);
			}
		}
		else//兼容没有set信息的业务
		{
			string sNULL;
			sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) +"' ";
			sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) +"' ";
			sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";
		}

		//查ip对应配置
		string sSql =
			"select filename from t_config_files "
			"where server_name = '" + _mysqlConfig.escapeString(configInfo.appname+"."+configInfo.servername) + "' "
			"and host='"+_mysqlConfig.escapeString(sHost)+"' "
			"and level=" + TC_Common::tostr<int>(eLevelIpServer) + sCondition;

		TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
		for(unsigned i=0; i<res.size();i++)
		{
			vf.push_back(res[i]["filename"]);
		}
	}
	catch(TC_Mysql_Exception & ex)
	{
		LOG->error()<<"exception: "<<ex.what()<<endl;
		return -1;
	}
	return 0;
}

int ConfigImp::loadConfigByInfo(const ConfigInfo & configInfo,string &config,taf::JceCurrentPtr current)
{
	LOG->debug()<<"loadConfigByInfo|"<<configInfo.appname<<"|"<<configInfo.servername<<"|"<<configInfo.filename<<"|"<<configInfo.setdivision<<endl;

	int iRet = 0 ;
	if(configInfo.bAppOnly||configInfo.servername.empty())//应用级配置或者set级配置
	{
		iRet = loadAppConfigByInfo(configInfo,config,current);
	}
	else
	{
		iRet = loadConfigByHost(configInfo,config,current);
	}
	return iRet;
}

int ConfigImp::checkConfigByInfo(const ConfigInfo & configInfo, string &result,taf::JceCurrentPtr current)
{
	LOG->debug()<<"checkConfigByInfo|"<<configInfo.appname<<"|"<<configInfo.servername<<"|"<<configInfo.filename<<"|"<<configInfo.setdivision<<endl;

	int ret = loadConfigByHost(configInfo,result,current);
	if (ret != 0)
	{
		return -1;
	}

	try
	{
		TC_Config conf;
		conf.parseString(result);
	}
	catch(exception &ex)
	{
		result = ex.what();
		return -1;
	}
	return 0;
}

int ConfigImp::loadConfig(const std::string& app, const std::string& server, const std::string& fileName, string &config, taf::JceCurrentPtr current)
{
    if(!server.empty())
    {
        return loadConfigByHost(app + "." + server, fileName, current->getIp(), config, current);
    }
    else
    {
        return loadAppConfig(app, fileName, config, current);
    }
}

int ConfigImp::loadConfigByHost(const std::string& appServerName, const std::string& fileName, const string &host, string &config, taf::JceCurrentPtr current)
{
    LOG->debug() << "loadConfig|" << appServerName << "|" << fileName << "|" << host << endl;

    int iRet = 0 ;
    config = "";

    //多配置文件的分割符
    string sSep = "\r\n\r\n";
    try
    {
        string sAllServerConfig = "";
        int iAllConfigId = 0;
        //查公有配置
        string sNULL;
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appServerName) + "' "
            " and filename='" + _mysqlConfig.escapeString(fileName) +"' "
            " and level=" + TC_Common::tostr<int>(eLevelAllServer) +
            " and set_name ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_area ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_group ='" + _mysqlConfig.escapeString(sNULL) +"' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		 LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
        if(res.size() == 1)
        {
            sAllServerConfig = res[0]["config"];
            iAllConfigId = TC_Common::strto<int>(res[0]["id"]);
        }

        vector<int> referenceIds;
        iRet = getReferenceIds(iAllConfigId,referenceIds);
        LOG->debug() << "referenceIds.size()|" << referenceIds.size() << endl;
        if(iRet == 0)
        {
            for(size_t i = 0; i < referenceIds.size();i++)
            {
                int iRefId = referenceIds[i];
                string refConfig;
                iRet = loadConfigByPK(iRefId,refConfig);
                if(iRet == 0 && (!refConfig.empty()))
                {
                    if(config.empty())
                    {
                        config += refConfig;
                    }
                    else
                    {
                        config += sSep + refConfig;
                    }
                }
            }
        }

        //添加配置本身
        if(!sAllServerConfig.empty())
        {
            if(config.empty())
            {
                config += sAllServerConfig;
            }
            else
            {
                config += sSep + sAllServerConfig;
            }
        }


        string sIpServerConfig = "";
        int iIpConfigId = 0;

        sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appServerName) + "' "
            " and filename='" + _mysqlConfig.escapeString(fileName) +"' "
            " and host='"+_mysqlConfig.escapeString(host)+"' and level=" + TC_Common::tostr<int>(eLevelIpServer) +
			" and set_name ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_area ='" + _mysqlConfig.escapeString(sNULL) +"' "
			" and set_group ='" + _mysqlConfig.escapeString(sNULL) +"' ";

        res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
        if(res.size() == 1)
        {
            sIpServerConfig = res[0]["config"];
            iIpConfigId = TC_Common::strto<int>(res[0]["id"]);
        }

        iRet = getReferenceIds(iIpConfigId,referenceIds);
        LOG->debug() << "referenceIds.size()|" << referenceIds.size() << endl;
        if(iRet == 0)
        {
            for(size_t i = 0; i < referenceIds.size();i++)
            {
                int iRefId = referenceIds[i];
                string refConfig;
                iRet = loadConfigByPK(iRefId,refConfig);
                if(iRet == 0 && (!refConfig.empty()) )
                {
                    if(config.empty())
                    {
                        config += refConfig;
                    }
                    else
                    {
                        config += sSep + refConfig;
                    }
                }
            }
        }

        //添加配置本身
        if(!sIpServerConfig.empty())
        {
            if(config.empty())
            {
                config += sIpServerConfig;
            }
            else
            {
                config += sSep + sIpServerConfig;
            }
        }

    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        LOG->error()<<"exception: "<<ex.what()<<endl;
        iRet = -1;
    }

    return iRet;
}

int ConfigImp::checkConfig(const std::string& appServerName, const std::string& fileName, const string &host, string &result, taf::JceCurrentPtr current)
{
    int ret = loadConfigByHost(appServerName, fileName, host, result, current);
    if (ret != 0)
    {
        return -1;
    }

    try
    {
        TC_Config conf;
        conf.parseString(result);
    }
    catch(exception &ex)
    {
        result = ex.what();
        return -1;
    }

    return 0;
}

///////////////////////////////////////////////private
int ConfigImp::loadConfigByPK(int iConfigId, string &sConfig)
{
    int iRet = 0 ;
    //按照建立引用的先后返回
    try
    {
		string sSql =
			"select config from t_config_files where id=" + TC_Common::tostr<int>(iConfigId) +" order by id";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
        if(res.size() == 1)
        {
            sConfig = res[0]["config"];
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        sConfig = ex.what();
        LOG->error()<<"exception: "<<ex.what()<<endl;
        return -1;
    }

    return iRet;
}

int ConfigImp::loadRefConfigByPK(int iConfigId, const string& setdivision,string &sConfig)
{
	int iRet = 0 ;
	//按照建立引用的先后返回
	try
	{
		string sSql =
			"select server_name,filename,config from t_config_files where id=" + TC_Common::tostr<int>(iConfigId) +" order by id";

		TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
		if(res.size() == 1)
		{
			string sAppConfig = res[0]["config"];
			string sFileName = res[0]["filename"];
			string sServerName = res[0]["server_name"];

			string sSetName,sSetArea,sSetGroup;
			string sSetConfig;
			//查询该配置是否有set配置信息
			if(getSetInfo(sSetName,sSetArea,sSetGroup,setdivision))
			{
				string sCondition;
				sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
				sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
				sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";

				string sSql =
					"select config from t_config_files "
					"where server_name = '" + _mysqlConfig.escapeString(sServerName) + "' "
					" and filename='" + _mysqlConfig.escapeString(sFileName) +"' "
					" and level=" + TC_Common::tostr<int>(eLevelApp) + sCondition;

				TC_Mysql::MysqlData resSet = _mysqlConfig.queryRecord(sSql);
				LOG->debug() << "sql|" << sSql<<"|"<<resSet.size() << endl;
				if(resSet.size() == 1)
				{
					sSetConfig = resSet[0]["config"];
				}
			}
			sConfig = mergeConfig(sAppConfig,sSetConfig);
		}
	}
	catch(TC_Mysql_Exception & ex)
	{
		sConfig = ex.what();
		LOG->error()<<"exception: "<<ex.what()<<endl;
		return -1;
	}
	return iRet;
}
int ConfigImp::getReferenceIds(int iConfigId,vector<int> &referenceIds)
{
    int iRet = 0 ;
    referenceIds.clear();
    string sSql =
        "select reference_id from t_config_references where config_id=" + TC_Common::tostr<int>(iConfigId);
    LOG->debug() << "sql|" << sSql << endl;

    try
    {
        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
        LOG->debug() << "res.size()|" << res.size() << endl;
        for(size_t i = 0 ; i < res.size(); i++)
        {
            int iRefId = TC_Common::strto<int>(res[i]["reference_id"]);
            referenceIds.push_back(iRefId);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<"exception: "<<ex.what()<<endl;
        return -1;
    }
    return iRet;
}


int ConfigImp::loadAppConfig(const std::string& appName, const std::string& fileName, string &config, taf::JceCurrentPtr current)
{
    LOG->debug() << "loadAppConfig|" << appName << "|" << fileName << "|" << endl;

    int iRet = 0 ;
    config = "";

    try
    {
		string sNULL;
        //查公有配置
        string sSql =
            "select id,config from t_config_files "
            "where server_name = '" + _mysqlConfig.escapeString(appName) + "' "
            " and filename='" + _mysqlConfig.escapeString(fileName) +"' "
            " and level=" + TC_Common::tostr<int>(eLevelApp) +
            " and set_name ='" + _mysqlConfig.escapeString(sNULL) +"' "
            " and set_area ='" + _mysqlConfig.escapeString(sNULL) +"' "
            " and set_group ='" + _mysqlConfig.escapeString(sNULL) +"' ";

        TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;

        if(res.size() == 1)
        {
            config = res[0]["config"];
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        config = ex.what();
        LOG->error()<<"exception: "<<ex.what()<<endl;
        iRet = -1;
    }

    return iRet;

}

int ConfigImp::loadConfigByHost(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current)
{
	string sHost =  configInfo.host.empty()?current->getIp():configInfo.host;
	LOG->debug()<<"loadConfigByHost with ConfigInfo|"
				<<configInfo.appname<<"|"
				<<configInfo.servername<<"|"
				<<configInfo.filename<<"|"
				<<sHost<<"|"
				<<configInfo.setdivision<<endl;

	int iRet = 0 ;
	config = "";
	string sTemp = "";

	//多配置文件的分割符
	string sSep = "\r\n\r\n";
	try
	{
		string sCondition;
		if(!configInfo.setdivision.empty())
		{
			string sSetName,sSetArea,sSetGroup;
			if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
			{
				sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
				sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
				sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";
			}
			else
			{
				LOG->error()<<"setdivision is invalid: "<<configInfo.setdivision<<endl;
				throw TC_Mysql_Exception("setdivision is invalid:"+configInfo.setdivision);
			}
		}
		else//兼容没有set信息的业务
		{
			string sNULL;
			sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) +"' ";
			sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) +"' ";
			sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";
		}

		string sAllServerConfig = "";
		int iAllConfigId = 0;
		//查服务配置
		string sSql =
			"select id,config from t_config_files "
			"where server_name = '" + _mysqlConfig.escapeString(configInfo.appname+"."+configInfo.servername) + "' "
			" and filename='" + _mysqlConfig.escapeString(configInfo.filename) +"' "
			" and level=" + TC_Common::tostr<int>(eLevelAllServer) + sCondition;

		TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql <<"|"<<res.size()<< endl;
		if(res.size() == 1)
		{
			sAllServerConfig = res[0]["config"];
			iAllConfigId = TC_Common::strto<int>(res[0]["id"]);
		}

		//查服务配置的引用配置
		vector<int> referenceIds;
		iRet = getReferenceIds(iAllConfigId,referenceIds);
		LOG->debug() << "referenceIds.size()|" << referenceIds.size() << endl;
		if(iRet == 0)
		{
			for(size_t i = 0; i < referenceIds.size();i++)
			{
				int iRefId = referenceIds[i];
				string refConfig;

				//此处获取的服务配置的引用配置信息
				iRet = loadRefConfigByPK(iRefId,configInfo.setdivision,refConfig);
				if(iRet == 0 && (!refConfig.empty()))
				{
					sTemp = mergeConfig(sTemp,refConfig);
				}
			}
		}

		//添加服务配置本身
		config = mergeConfig(sTemp,sAllServerConfig);

		sTemp.clear();

		string sIpServerConfig = "";
		int iIpConfigId = 0;
		//查看节点级配置
		sSql =
			"select id,config from t_config_files "
			"where server_name = '" + _mysqlConfig.escapeString(configInfo.appname+"."+configInfo.servername) + "' "
			" and filename='" + _mysqlConfig.escapeString(configInfo.filename) +"' "
			" and host='"+_mysqlConfig.escapeString(sHost)+"' "
			" and level=" + TC_Common::tostr<int>(eLevelIpServer);

		res = _mysqlConfig.queryRecord(sSql);
		LOG->debug() << "sql|" << sSql<<"|"<<res.size() << endl;
		if(res.size() == 1)
		{
			sIpServerConfig = res[0]["config"];
			iIpConfigId = TC_Common::strto<int>(res[0]["id"]);
		}

		iRet = getReferenceIds(iIpConfigId,referenceIds);
		LOG->debug() << "referenceIds.size()|" << referenceIds.size() << endl;
		if(iRet == 0)
		{
			string refAppConfig;
			string refSetConfig;
			for(size_t i = 0; i < referenceIds.size();i++)
			{
				int iRefId = referenceIds[i];
				string refConfig;

				//此处获取的节点配置的引用配置信息
				iRet = loadRefConfigByPK(iRefId,configInfo.setdivision,refConfig);
				if(iRet == 0 && (!refConfig.empty()))
				{
					sTemp = mergeConfig(sTemp,refConfig);
				}
			}
		}

		config = mergeConfig(config,mergeConfig(sTemp,sIpServerConfig));

	}
	catch(TC_Mysql_Exception & ex)
	{
		config = ex.what();
		LOG->error()<<"exception: "<<ex.what()<<endl;
		iRet = -1;
	}
	return iRet;
}

int ConfigImp::loadAppConfigByInfo(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current)
{
	int iRet = 0;
	config="";
	try
	{
		string sSql =
			"select id,config from t_config_files "
			"where server_name = '" + _mysqlConfig.escapeString(configInfo.appname) + "' "
			" and filename='" + _mysqlConfig.escapeString(configInfo.filename) +"' "
			" and level=" + TC_Common::tostr<int>(eLevelApp);

		string sNULL;
		string sCondition;
		sCondition += " and set_name='" + _mysqlConfig.escapeString(sNULL) +"' ";
		sCondition += " and set_area='" + _mysqlConfig.escapeString(sNULL) +"' ";
		sCondition += " and set_group='" + _mysqlConfig.escapeString(sNULL) +"' ";

		//先获取app配置
		string sSqlAppNoSet = sSql + sCondition;

		TC_Mysql::MysqlData res = _mysqlConfig.queryRecord(sSqlAppNoSet);
		LOG->debug() << "sSqlAppNoSet|" << sSqlAppNoSet <<"|"<<res.size()<< endl;
		string sAppConfig;

		if(res.size() == 1)
		{
			sAppConfig = res[0]["config"];
		}

		//再获取app下有set信息的配置
		string sSetConfig;
		if(!configInfo.setdivision.empty())//存在set信息
		{
			string sSetName,sSetArea,sSetGroup;

			if(getSetInfo(sSetName,sSetArea,sSetGroup,configInfo.setdivision))
			{
				string sCondition;
				sCondition += " and set_name='" +_mysqlConfig.escapeString(sSetName)+"' ";
				sCondition += " and set_area='" +_mysqlConfig.escapeString(sSetArea)+"' ";
				sCondition += " and set_group='" +_mysqlConfig.escapeString(sSetGroup)+"' ";

				string sSqlSet = sSql + sCondition;
				res = _mysqlConfig.queryRecord(sSqlSet);

				LOG->debug() << "sSqlSet|" << sSqlSet <<"|"<<res.size()<< endl;
				if(res.size() == 1)
				{
					sSetConfig = res[0]["config"];
				}
			}
			else
			{
				LOG->error()<<"setdivision is invalid: "<<configInfo.setdivision<<endl;
				throw TC_Mysql_Exception("setdivision is invalid:"+configInfo.setdivision);
			}
		}

		config = mergeConfig(sAppConfig,sSetConfig);

	}
	catch(TC_Mysql_Exception & ex)
	{
		config = ex.what();
		LOG->error()<<"exception: "<<ex.what()<<endl;
		iRet = -1;
	}
	return iRet;
}

string ConfigImp::mergeConfig(const string& sLowConf,const string& sHighConf)
{

	if(sLowConf.empty())
	{
		return  sHighConf;
	}
	else if(sHighConf.empty())
	{
		return  sLowConf;
	}
	else
	{
		return sLowConf + "\r\n\r\n" + sHighConf;
	}
}


