#include <iterator>
#include <algorithm>
#include "DbHandle.h"
#include "RegistryServer.h"

//加载cache的间隔n次后备份
#define DUMP_CACHE_INTERVAL 10

TC_ThreadLock CDbHandle::_mutex;

ObjectsCache CDbHandle::_objectsCache[2];
int CDbHandle::_objectsFlag = 0;

std::map<int, CDbHandle::GroupPriorityEntry> CDbHandle::_mapGroupPriority[2];
int CDbHandle::_mapGroupPriorityFlag = 0;

std::map<std::string, int> CDbHandle::_mapServantStatus[2];
int CDbHandle::_mapServantStatusFlag = 0;

map<string, NodePrx> CDbHandle::_mapNodePrxCache;
TC_ThreadLock CDbHandle::_NodePrxLock;

//key-ip, value-组编号
map<string,int> CDbHandle::_mServerGroupCache;
//key-group_name, value-组编号
map<string,int> CDbHandle::_mGroupNameIDCache;
vector< map<string,string> >CDbHandle::_vServerGroupRule;

CDbHandle::SetDivisionCache CDbHandle::_setDivisionCache[2];
int CDbHandle::_setDivisionFlag;

FileHashMap CDbHandle::_fileCache;

extern RegistryServer g_app;


int CDbHandle::init(TC_Config * pconf)
{
    try
    {
        //TC_ThreadLock::Lock lock(_mutex); //避免并行wbl初始化mysql连接,否则会core

        TC_DBConf tcDBConf;
        tcDBConf.loadFromMap(pconf->getDomainMap("/taf/db"));
		string option = pconf->get("/taf/db<dbflag>","");
		if(!option.empty() && option == "CLIENT_MULTI_STATEMENTS")
		{
			tcDBConf._flag = CLIENT_MULTI_STATEMENTS;
			_bEnMultiSql = true;
			LOG->debug()<<__FUNCTION__<<" tcDBConf._flag: "<<option<<endl;
		}
        _mysqlReg.init(tcDBConf);
    }
    catch(TC_Config_Exception &ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
    }

    return 0;
}

int CDbHandle::registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li)
{
    try
    {
        string sSelectSql =
            "select present_state, node_obj, template_name "
            "from t_node_info "
            "where node_name='"+ _mysqlReg.escapeString(name) + "'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSelectSql);
        LOG->debug()<<__FUNCTION__<<" select node affected:"<<res.size()<<endl;

        string sTemplateName;
        if(res.size() != 0)
        {
            //合法性判断，是否存在名字相同已经存活但node obj不同的注册节点
            if(res[0]["present_state"] == "active" && res[0]["node_obj"] != ni.nodeObj)
            {
                LOG->error()<<"registery node :" << name <<" error, other node has registered."<<endl;
                return 1;
            }
            //传递已配置的模板名
            sTemplateName = res[0]["template_name"];
        }

        string sSql =
            "replace into t_node_info "
            "    (node_name, node_obj, endpoint_ip, endpoint_port, data_dir, load_avg1, load_avg5, load_avg15,"
            "     last_reg_time, last_heartbeat, setting_state, present_state, taf_version, template_name)"
            "values('" + _mysqlReg.escapeString(name) + "', '" + _mysqlReg.escapeString(ni.nodeObj) + "', "
            "    '" + _mysqlReg.escapeString(ni.endpointIp) + "',"
            "    '" + taf::TC_Common::tostr<int>(ni.endpointPort) + "',"
            "    '" + _mysqlReg.escapeString(ni.dataDir) + "','" + taf::TC_Common::tostr<float>(li.avg1) + "',"
            "    '" + taf::TC_Common::tostr<float>(li.avg5) + "',"
            "    '" + taf::TC_Common::tostr<float>(li.avg15) + "', now(), now(), 'active', 'active', " +
            "    '" + _mysqlReg.escapeString(ni.version) + "', '" + _mysqlReg.escapeString(sTemplateName) + "')";

        _mysqlReg.execute(sSql);
        LOG->debug()<<"registery node :" << name <<" affected:"<<_mysqlReg.getAffectedRows()<<endl;

        NodePrx nodePrx;
        g_app.getCommunicator()->stringToProxy(ni.nodeObj, nodePrx);

        TC_ThreadLock::Lock lock(_NodePrxLock);
        _mapNodePrxCache[name] = nodePrx;

    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<< " " << name << " exception: "<<ex.what()<<endl;
        return 2;
    }
    catch(taf::TafException & ex)
    {
        LOG->error()<<__FUNCTION__<< " "<< name << " exception: "<<ex.what()<<endl;
        return 3;
    }

    return 0;
}

int CDbHandle::destroyNode(const string & name)
{
    try
    {
        string sSql =
            "update t_node_info as node "
            "    left join t_server_conf as server using (node_name) "
            "set node.present_state = 'inactive', server.present_state='inactive' "
            "where node.node_name='" + _mysqlReg.escapeString(name) + "'";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__<<" "<< name << " affected:" <<_mysqlReg.getAffectedRows()<<endl;

        TC_ThreadLock::Lock lock(_NodePrxLock);
        _mapNodePrxCache.erase(name);
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" "<< name << " exception: "<<ex.what()<<endl;
    }

    return 0;
}


int CDbHandle::keepAlive(const string & name, const LoadInfo & li)
{
    try
    {
        int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        string sSql =
            "update t_node_info "
            "set last_heartbeat=now(), present_state='active',"
            "    load_avg1=" + taf::TC_Common::tostr<float>(li.avg1) + ","
            "    load_avg5=" + taf::TC_Common::tostr<float>(li.avg5) + ","
            "    load_avg15=" + taf::TC_Common::tostr<float>(li.avg15) + " "
            "where node_name='" + _mysqlReg.escapeString(name) + "'";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__<<" " << name << " affected:"<<_mysqlReg.getAffectedRows()
            <<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<<endl;

        if(_mysqlReg.getAffectedRows() == 0) {
            return 1;
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<< " " << name << " exception: "<<ex.what()<<endl;
        return 2;
    }

    return 0;
}

map<string, string> CDbHandle::getActiveNodeList(string & result)
{
    map<string, string> mapNodeList;
    try
    {
        string sSql =
            "select node_name, node_obj from t_node_info "
            "where present_state='active'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" (present_state='active') affected:"<<res.size()<<endl;
        for(unsigned i = 0; i < res.size(); i++)
        {
            mapNodeList[res[i]["node_name"]] = res[i]["node_obj"];
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        result = string(__FUNCTION__) + " exception: " + ex.what();
        LOG->error()<< result <<endl;
        return mapNodeList;
    }

    return  mapNodeList;
}

int CDbHandle::getNodeVersion(const string &nodeName, string &version, string & result)
{
    try
    {
        string sSql =
            "select taf_version from t_node_info "
            "where node_name='" + _mysqlReg.escapeString(nodeName) + "'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" (node_name='"<<nodeName<<"') affected:"<<res.size()<<endl;
        if(res.size() > 0)
        {
            version = res[0]["taf_version"];
            return 0;

        }
        result = "node_name(" + nodeName + ") int table t_node_info not exist";
    }
    catch(TC_Mysql_Exception & ex)
    {
        result = string(__FUNCTION__) + " exception: " + ex.what();
        LOG->error()<< result <<endl;
    }
    return  -1;
}

vector<ServerDescriptor> CDbHandle::getServers(const string & app, const string & serverName, const string & nodeName, bool withDnsServer)
{
    string sSql;
    vector<ServerDescriptor>  vServers;
	unsigned num = 0;
	int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();

    try
    {
        //server详细配置
        string sCondition;
        sCondition += "server.node_name='" + _mysqlReg.escapeString(nodeName) + "'";
        if(app != "")        sCondition += " and server.application='" + _mysqlReg.escapeString(app) + "' ";
        if(serverName != "") sCondition += " and server.server_name='" + _mysqlReg.escapeString(serverName) + "' ";
        if(withDnsServer == false) sCondition += " and server.server_type !='taf_dns' "; //不获取dns服务

        sSql =
            "select server.application, server.server_name, server.node_name, base_path, "
            "    exe_path, setting_state, present_state, adapter_name, thread_num, async_thread_num, endpoint,"
            "    profile,template_name, "
            "    allow_ip, max_connections, servant, queuecap, queuetimeout,protocol,handlegroup,shmkey,shmcap,"
            "    patch_version, patch_time, patch_user, "
            "    server_type, start_script_path, stop_script_path, monitor_script_path,config_center_port ,"
            "	 enable_set, set_name, set_area, set_group "
            "from t_server_conf as server "
            "    left join t_adapter_conf as adapter using(application, server_name, node_name) "
            "where " + sCondition ;

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
		num = res.size();
        //对应server在vector的下标
        map<string, int> mapAppServerTemp;

        //获取模版profile内容
        map<string, string> mapProfile;

        //分拆数据到server的信息结构里
        for(unsigned i = 0; i < res.size(); i++)
        {
            string sServerId = res[i]["application"] + "." + res[i]["server_name"]
                + "_" + res[i]["node_name"];

            if(mapAppServerTemp.find(sServerId) == mapAppServerTemp.end())
            {
                //server配置
                ServerDescriptor server;
                server.application  = res[i]["application"];
                server.serverName   = res[i]["server_name"];
                server.nodeName     = res[i]["node_name"];
                server.basePath     = res[i]["base_path"];
                server.exePath      = res[i]["exe_path"];
                server.settingState = res[i]["setting_state"];
                server.presentState = res[i]["present_state"];
                server.patchVersion = res[i]["patch_version"];
                server.patchTime    = res[i]["patch_time"];
                server.patchUser    = res[i]["patch_user"];
                server.profile      = res[i]["profile"];
                server.serverType   = res[i]["server_type"];
                server.startScript  = res[i]["start_script_path"];
                server.stopScript   = res[i]["stop_script_path"];
                server.monitorScript    = res[i]["monitor_script_path"];
                server.configCenterPort = TC_Common::strto<int>(res[i]["config_center_port"]);

				server.setId = "";
				if (TC_Common::lower(res[i]["enable_set"]) == "y")
				{
					server.setId = res[i]["set_name"] + "." +  res[i]["set_area"] + "." + res[i]["set_group"];
				}

                //获取父模版profile内容
                if(mapProfile.find(res[i]["template_name"]) == mapProfile.end())
                {
                    string sResult;
                    mapProfile[res[i]["template_name"]] = getProfileTemplate(res[i]["template_name"], sResult);
                }

                TC_Config tParent,tProfile;
                tParent.parseString(mapProfile[res[i]["template_name"]]);
                tProfile.parseString(server.profile);
				int iDefaultAsyncThreadNum = 3;
				int iConfigAsyncThreadNum = TC_Common::strto<int>(TC_Common::trim(res[i]["async_thread_num"]));
				iDefaultAsyncThreadNum = iConfigAsyncThreadNum > iDefaultAsyncThreadNum? iConfigAsyncThreadNum:iDefaultAsyncThreadNum;
				server.asyncThreadNum = TC_Common::strto<int>(tProfile.get("/taf/application/client<asyncthread>", TC_Common::tostr(iDefaultAsyncThreadNum)));
                tParent.joinConfig(tProfile,true);
                server.profile = tParent.tostr();

                mapAppServerTemp[sServerId] = vServers.size();
                vServers.push_back(server);
            }

            //adapter配置
            AdapterDescriptor adapter;
            adapter.adapterName = res[i]["adapter_name"];
            if(adapter.adapterName == "")
            {
                //adapter没配置，left join 后为 NULL,不放到adapters map
                continue;
            }

            adapter.threadNum       = res[i]["thread_num"];
            adapter.endpoint        = res[i]["endpoint"];
            adapter.maxConnections  = TC_Common::strto<int>(res[i]["max_connections"]);
            adapter.allowIp         = res[i]["allow_ip"];
            adapter.servant         = res[i]["servant"];
            adapter.queuecap        = TC_Common::strto<int>(res[i]["queuecap"]);
            adapter.queuetimeout    = TC_Common::strto<int>(res[i]["queuetimeout"]);
            adapter.protocol        = res[i]["protocol"];
            adapter.handlegroup     = res[i]["handlegroup"];
            adapter.shmkey          = TC_Common::strto<int>(res[i]["shmkey"]);
            adapter.shmcap          = TC_Common::strto<int>(res[i]["shmcap"]);

            vServers[mapAppServerTemp[sServerId]].adapters[adapter.adapterName] = adapter;
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" "<< app << "." << serverName << "_" << nodeName
            <<" exception: "<<ex.what()<<"|"<<sSql<<endl;
        return vServers;
    }
	catch(TC_Config_Exception & ex)
	{
		LOG->error()<<__FUNCTION__<<" "<< app << "." << serverName << "_" << nodeName
			<<" TC_Config_Exception exception: "<<ex.what()<<endl;
		throw TafException(string("TC_Config_Exception exception: ") + ex.what());
	}

	LOG->debug()<< app << "." << serverName << "_" << nodeName
		<<" getServers affected:"<<num
		<<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<<endl;

    return  vServers;

}

string CDbHandle::getProfileTemplate(const string & sTemplateName, string & sResultDesc)
{
    map<string, int> mapRecursion;
    return getProfileTemplate(sTemplateName, mapRecursion, sResultDesc);
}

string CDbHandle::getProfileTemplate(const string & sTemplateName, map<string, int> & mapRecursion, string & sResultDesc)
{
    try
    {
        string sSql = "select template_name, parents_name, profile from t_profile_template "
                "where template_name='" + _mysqlReg.escapeString(sTemplateName) + "'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        if(res.size() == 0)
        {
            sResultDesc += "(" + sTemplateName + ":template not found)";
            return "";
        }

        TC_Config confMyself, confParents;
        confMyself.parseString(res[0]["profile"]);
        //mapRecursion用于避免重复继承
        mapRecursion[res[0]["template_name"]] = 1;

        if(res[0]["parents_name"] != "" && mapRecursion.find(res[0]["parents_name"]) == mapRecursion.end())
        {
            confParents.parseString(getProfileTemplate(res[0]["parents_name"], mapRecursion, sResultDesc));
            confMyself.joinConfig(confParents, false);
        }
        sResultDesc += "("+sTemplateName+":OK)";

        LOG->debug()<<__FUNCTION__<< " " << sTemplateName << " " << sResultDesc <<endl;

        return confMyself.tostr();
    }
    catch(TC_Mysql_Exception & ex)
    {
        sResultDesc += "(" + sTemplateName + ":" + ex.what() + ")";
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
    }
    catch(TC_Config_Exception & ex)
    {
        sResultDesc += "(" + sTemplateName + ":" + ex.what() + ")";
        LOG->error()<<__FUNCTION__<<" TC_Config_Exception exception: "<<ex.what()<<endl;
    }

    return  "";
}

vector<string> CDbHandle::getAllApplicationNames(string & result)
{
    vector<string> vApps;
    try
    {
        string sSql = "select distinct application from t_server_conf";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" affected:"<<res.size()<<endl;

        for(unsigned i = 0; i < res.size(); i++)
        {
            vApps.push_back(res[i]["application"]);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return vApps;
    }

    return vApps;

}

vector<vector<string> > CDbHandle::getAllServerIds(string & result)
{
    vector<vector<string> > vServers;
    try
    {
        string sSql =
            "select application, server_name, node_name, setting_state, present_state,server_type from t_server_conf";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" affected:"<<res.size()<<endl;

        for(unsigned i = 0; i < res.size(); i++)
        {
            vector<string> server;
            server.push_back(res[i]["application"] + "." + res[i]["server_name"] +  "_" + res[i]["node_name"]);
            server.push_back(res[i]["setting_state"]);
            server.push_back(res[i]["present_state"]);
            server.push_back(res[i]["server_type"]);
            vServers.push_back(server);
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return vServers;
    }

    return vServers;

}


int CDbHandle::updateServerState(const string & app, const string & serverName, const string & nodeName,
        const string & stateFields, taf::ServerState state, int processId)
{
    try
    {
		int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        if(stateFields != "setting_state" && stateFields != "present_state")
        {
            LOG->debug()<< app << "." << serverName << "_" << nodeName
                <<" not supported fields:"<<stateFields<<endl;
            return -1;
        }

        string sProcessIdSql = (stateFields == "present_state" ?
            (", process_id = " + TC_Common::tostr<int>(processId) + " ") : "");

        string sSql =
            "update t_server_conf "
            "set " + stateFields + " = '" + etos(state) + "' " + sProcessIdSql +
            "where application='" + _mysqlReg.escapeString(app) + "' "
            "    and server_name='" + _mysqlReg.escapeString(serverName) + "' "
            "    and node_name='" + _mysqlReg.escapeString(nodeName) + "' ";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__<< " "<< app << "." << serverName << "_" << nodeName
            << " affected:"<<_mysqlReg.getAffectedRows()
            << "|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<<endl;

        std::map<std::string, int> & map_status = _mapServantStatus[_mapServantStatusFlag];
        map_status[app + serverName + nodeName] = static_cast<int>(state);
        return 0;

    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" "<< app << "." << serverName << "_" << nodeName
            <<" exception: "<<ex.what()<<endl;
        return -1;
    }
}

int CDbHandle::updateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo)
{
    const size_t sizeStep = 1000;

    for (std::vector<taf::ServerStateInfo>::size_type i = 0; i < vecStateInfo.size(); )
    {
        int iEnd = (i + sizeStep >= vecStateInfo.size())?vecStateInfo.size():(i + sizeStep);
        if (doUpdateServerStateBatch(vecStateInfo, i, iEnd) != 0) return -1;
        i = iEnd;
    }

    return 0;
}

int CDbHandle::doUpdateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, const size_t sizeBegin, const size_t sizeEnd)
{
    std::map<std::string, int> & map_status = _mapServantStatus[_mapServantStatusFlag];

    

    std::map<std::string, std::map<std::string, std::vector<int> > > map_sort;
    for (std::vector<taf::ServerStateInfo>::size_type i = sizeBegin; i < sizeEnd; i++)
    {
        std::map<std::string, int>::iterator it = map_status.find(vecStateInfo[i].application + vecStateInfo[i].serverName + vecStateInfo[i].nodeName);
    	
        
	if (it != map_status.end() && it->second == static_cast<int>(vecStateInfo[i].serverState))
	{
		LOG->debug() << __FUNCTION__ << " it->first" <<it->first<<"|"<<it->second<<"|"<< etos(vecStateInfo[i].serverState) << endl;
		 continue;
	}

        map_sort[vecStateInfo[i].application][vecStateInfo[i].serverName].push_back(i);
    }

    LOG->debug() << __FUNCTION__ << " map_status size(" << map_status.size()<<"|map_sort|"<<map_sort.size() << endl;

    if (map_sort.empty())
    {
        LOG->debug() << __FUNCTION__ << " vector size(" << vecStateInfo.size() << ") do nothing within the same state in cache" << endl;
        return 0;
    }

	int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
	std::string sCommand;
	if(_bEnMultiSql)
	{
		std::string sPrefix = "UPDATE t_server_conf SET present_state='";
		for (std::map<std::string, std::map<std::string, std::vector<int> > >::iterator it_app = map_sort.begin(); it_app != map_sort.end(); it_app++)
		{
			 for (std::map<std::string, std::vector<int> >::iterator it_svr = it_app->second.begin(); it_svr != it_app->second.end(); it_svr++)
			 {
				for (std::vector<int>::size_type i = 0; i < it_svr->second.size(); i++)
				{
					sCommand += (sCommand == ""?"":";") + sPrefix + etos(vecStateInfo[it_svr->second[i]].serverState)
						+ "', process_id= " + TC_Common::tostr<int>(vecStateInfo[it_svr->second[i]].processId)
						+ " WHERE application='" + _mysqlReg.escapeString(it_app->first)
						+ "' AND server_name='" + _mysqlReg.escapeString(it_svr->first)
						+ "' AND node_name='" + _mysqlReg.escapeString(vecStateInfo[it_svr->second[i]].nodeName) + "'";
				}
			 }
		}
	}
	else
	{
	    std::string sPidCommand, sPreCommand;
	    for (std::map<std::string, std::map<std::string, std::vector<int> > >::iterator it_app = map_sort.begin(); it_app != map_sort.end(); it_app++)
	    {
	        sPidCommand += " WHEN '" + it_app->first + "' THEN CASE server_name ";
	        sPreCommand += " WHEN '" + it_app->first + "' THEN CASE server_name ";

	        for (std::map<std::string, std::vector<int> >::iterator it_svr = it_app->second.begin(); it_svr != it_app->second.end(); it_svr++)
	        {
	            sPidCommand += " WHEN '" + it_svr->first + "' THEN CASE node_name ";
	            sPreCommand += " WHEN '" + it_svr->first + "' THEN CASE node_name ";

	            for (std::vector<int>::size_type i = 0; i < it_svr->second.size(); i++)
	            {
	                sPidCommand += " WHEN '" + _mysqlReg.escapeString(vecStateInfo[it_svr->second[i]].nodeName) + "' THEN " + TC_Common::tostr<int>(vecStateInfo[it_svr->second[i]].processId);
	                sPreCommand += " WHEN '" + _mysqlReg.escapeString(vecStateInfo[it_svr->second[i]].nodeName) + "' THEN '" + etos(vecStateInfo[it_svr->second[i]].serverState) + "'";
	            }

	            sPidCommand += " ELSE process_id END";
	            sPreCommand += " ELSE present_state END";
	        }

	        sPidCommand += " ELSE process_id END";
	        sPreCommand += " ELSE present_state END";
	    }
		sCommand = "UPDATE t_server_conf SET process_id= CASE application " + sPidCommand + " ELSE process_id END, present_state= CASE application " + sPreCommand + " ELSE present_state END";
	}

	if(!sCommand.empty())
	{
		try
	    {
	        _mysqlReg.execute(sCommand);
			int iRows = 0;
			if(_bEnMultiSql)
			{
				for (iRows = mysql_affected_rows(_mysqlReg.getMysql()); !mysql_next_result(_mysqlReg.getMysql());
							iRows += mysql_affected_rows(_mysqlReg.getMysql())) ;
			}
			else
			{
				iRows = mysql_affected_rows(_mysqlReg.getMysql());
			}

			if(iRows> 0)
			{
		        LOG->debug() << __FUNCTION__ << " sql: " << sCommand << " vector:" << vecStateInfo.size() << " affected:" << iRows
					<< "|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<< endl;
			}
	        return 0;
	    }
	    catch(TC_Mysql_Exception & ex)
	    {
	        LOG->error() << __FUNCTION__ << " exception: " << ex.what() << " sql:" << sCommand << endl;
	        return -1;
	    }
	}
	return 0;
}

int CDbHandle::setPatchInfo(const string & app, const string & serverName, const string & nodeName,
            const string & version, const string & user)
{
    try{
        string sSql =
            "update t_server_conf "
            "set patch_version = '" + _mysqlReg.escapeString(version) + "', "
            "   patch_user = '" + _mysqlReg.escapeString(user) + "', "
            "   patch_time = now() "
            "where application='" + _mysqlReg.escapeString(app) + "' "
            "    and server_name='" + _mysqlReg.escapeString(serverName) + "' "
            "    and node_name='" + _mysqlReg.escapeString(nodeName) + "' ";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__<< " "<< app << "." << serverName << "_" << nodeName
            <<" affected:"<<_mysqlReg.getAffectedRows()<<endl;

        return 0;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" "<< app << "." << serverName << "_" << nodeName
            <<" exception: "<<ex.what()<<endl;
        return -1;
    }
}

int CDbHandle::setServerTafVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version)
{
    try{

		int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        string sSql =
            "update t_server_conf "
            "set taf_version = '" + _mysqlReg.escapeString(version) + "' "
            "where application='" + _mysqlReg.escapeString(app) + "' "
            "    and server_name='" + _mysqlReg.escapeString(serverName) + "' "
            "    and node_name='" + _mysqlReg.escapeString(nodeName) + "' ";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__<< " "<< app << "." << serverName << "_" << nodeName
            <<" affected:"<<_mysqlReg.getAffectedRows()
            <<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<< endl;

        return 0;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" "<< app << "." << serverName << "_" << nodeName
            <<" exception: "<<ex.what()<<endl;
        return -1;
    }
}

NodePrx CDbHandle::getNodePrx(const string & nodeName)
{
    try
    {
        TC_ThreadLock::Lock lock(_NodePrxLock);

        if(_mapNodePrxCache.find(nodeName) != _mapNodePrxCache.end())
        {
            return _mapNodePrxCache[nodeName];
        }

        string sSql =
            "select node_obj "
            "from t_node_info "
            "where node_name='" + _mysqlReg.escapeString(nodeName) + "' and present_state='active'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" '"<< nodeName <<"' affected:"<<res.size()<<endl;

        if(res.size() == 0)
        {
            throw taf::TafException("node '"+ nodeName + "' not registered");
        }

        NodePrx nodePrx;
        g_app.getCommunicator()->stringToProxy(res[0]["node_obj"], nodePrx);

        _mapNodePrxCache[nodeName] = nodePrx;

        return nodePrx;

    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<< " "<< nodeName << " exception: "<<ex.what()<<endl;
        throw taf::TafException(string("get node record from db error:") + ex.what());
    }
    catch(taf::TafException & ex)
    {
        LOG->error()<<__FUNCTION__<< " "<< nodeName << " exception: "<<ex.what()<<endl;
        throw ex;
    }

}

int CDbHandle::checkNodeTimeout(unsigned uTimeout)
{
    try
    {
		//这里先检查下，记录下有哪些节点超时了，方便定位问题
		string sTmpSql = "select node_name from t_node_info where last_heartbeat < date_sub(now(), INTERVAL " + taf::TC_Common::tostr(uTimeout) + " SECOND)";
		taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sTmpSql);

		LOG->debug()<<__FUNCTION__<<" affected:"<< taf::TC_Common::tostr(res.data()) <<endl;

        string sSql =
            "update t_node_info as node "
            "    left join t_server_conf as server using (node_name) "
            "set node.present_state='inactive', server.present_state='inactive', server.process_id=0 "
            "where last_heartbeat < date_sub(now(), INTERVAL " + taf::TC_Common::tostr(uTimeout) + " SECOND)";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__ << " (" << uTimeout  <<"s) affected:"<<_mysqlReg.getAffectedRows()<<endl;


        return _mysqlReg.getAffectedRows();

    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }

}

int CDbHandle::checkRegistryTimeout(unsigned uTimeout)
{
    try
    {
        string sSql =
            "update t_registry_info "
            "set present_state='inactive' "
            "where last_heartbeat < date_sub(now(), INTERVAL " + taf::TC_Common::tostr(uTimeout) + " SECOND)";

        _mysqlReg.execute(sSql);
        LOG->debug()<<__FUNCTION__ << " (" << uTimeout  <<"s) affected:"<<_mysqlReg.getAffectedRows()<<endl;

        return _mysqlReg.getAffectedRows();

    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }

}

int CDbHandle::checkSettingState(const int iCheckLeastChangedTime)
{
    try
    {
		LOG->debug()<<__FUNCTION__<<"____________________________________"<<endl;
        string sSql =
            "select application, server_name, node_name, setting_state "
            "from t_server_conf "
            "where setting_state='active' "  //检查应当启动的
            "and server_type != 'taf_dns'"  //仅用来提供dns服务的除外
            "and registry_timestamp >='" + TC_Common::tm2str(TC_TimeProvider::getInstance()->getNow()- iCheckLeastChangedTime) + "'";

		int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        LOG->debug()<<__FUNCTION__<<" setting_state='active' affected:"<<res.size()
				  <<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<< endl;

        NodePrx nodePrx;
        for(unsigned i = 0; i < res.size(); i++)
        {
            string sResult;
            LOG->debug()<<"checking ["<< i <<"]: "<<res[i]["application"]<<"."<<res[i]["server_name"]<<"_"<<res[i]["node_name"]<<endl;
            try
            {
                nodePrx = getNodePrx(res[i]["node_name"]);
                if(nodePrx)
                {
                    if(nodePrx->getSettingState(res[i]["application"], res[i]["server_name"], sResult) != Active)
                    {
                        string sTempSql = "select application, server_name, node_name, setting_state "
                            "from t_server_conf "
                            "where setting_state='active' "
                            "and application = '" + res[i]["application"] + "' "
                            "and server_name = '" + res[i]["server_name"] + "' "
                            "and node_name = '"   + res[i]["node_name"]   + "'";
                        if(_mysqlReg.queryRecord(sTempSql).size()== 0)
                        {
                            LOG->debug()<<res[i]["application"]<<"."<<res[i]["server_name"]<<"_"<<res[i]["node_name"]
                                     <<" not setting active,and not need restart"<<endl;
                            continue;
                        }
                        LOG->debug()<<res[i]["application"]<<"."<<res[i]["server_name"]<<"_"<<res[i]["node_name"]
                                  <<" not setting active, start it"<<endl;
                        int iRet = nodePrx->startServer(res[i]["application"], res[i]["server_name"], sResult);
                        LOG->debug()<<"startServer ret="<<iRet<< ",result=" << sResult << endl;
                    }
                }
            }

            catch(taf::TafException & ex)
            {
                LOG->error()<<"checking "<<res[i]["application"]<<"."<<res[i]["server_name"]<<"_"<<res[i]["node_name"]
                    <<"' exception: "<<ex.what()<<endl;
            }
            catch(exception & ex)
            {
                LOG->error()<<"checking "<<res[i]["application"]<<"."<<res[i]["server_name"]<<"_"<<res[i]["node_name"]
                    <<"' exception: "<<ex.what()<<endl;
            }
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }
    LOG->debug()<<__FUNCTION__<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;

    return 0;
}

int CDbHandle::getGroupId(const string& ip)
{
    bool bFind      = false;
    int iGroupId    = -1;
    string sOrder;
    string sAllowIpRule;
    string sDennyIpRule;
    vector<map<string,string> > vServerGroupInfo;
    try
    {
        {
            TC_ThreadLock::Lock lock(_mutex);
            map<string,int>::iterator it = _mServerGroupCache.find(ip);
            if( it != _mServerGroupCache.end())
            {
                return it->second;
            }
            vServerGroupInfo = _vServerGroupRule;
        }

        for(unsigned i = 0; i < vServerGroupInfo.size(); i++)
        {
            iGroupId                    = TC_Common::strto<int>(vServerGroupInfo[i]["group_id"]);
            sOrder                      = vServerGroupInfo[i]["ip_order"];
            sAllowIpRule                = vServerGroupInfo[i]["allow_ip_rule"];
            sDennyIpRule                = vServerGroupInfo[i]["denny_ip_rule"];
            vector<string> vAllowIp     = TC_Common::sepstr<string>(sAllowIpRule,",|;");
            vector<string> vDennyIp     = TC_Common::sepstr<string>(sDennyIpRule,",|;");
            if(sOrder == "allow_denny")
            {
                if(TC_Common::matchPeriod(ip,vAllowIp))
                {
                    bFind = true;
                    break;
                }
                /*
                if(TC_Common::matchPeriod(ip,vDennyIp))
                {
                    continue;
                }
                */
            }
            else if(sOrder == "denny_allow")
            {
                if(TC_Common::matchPeriod(ip,vDennyIp))
                {
                    //在不允许的ip列表中则不属于本行所对应组  继续匹配查找
                    continue;
                }
                if(TC_Common::matchPeriod(ip,vAllowIp))
                {
                    bFind = true;
                    break;
                }
            }
        }

        if(bFind == true)
        {
            TC_ThreadLock::Lock lock(_mutex);
            _mServerGroupCache[ip] = iGroupId;

            LOGINFO("get groupId succ|ip|"<<ip
                <<"|group_id|"<<iGroupId<<"|ip_order|"<<sOrder
                <<"|allow_ip_rule|"<<sAllowIpRule
                <<"|denny_ip_rule|"<<sDennyIpRule
                <<"|ServerGroupCache|"<<TC_Common::tostr(_mServerGroupCache)<<endl);

            return iGroupId;
        }
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
    }
    catch (exception & ex)
    {
        LOG->error()<<__FUNCTION__ << " " << ex.what() << endl;
    }
    return -1;
}

int CDbHandle::getGroupIdByName(const string &sGroupName)
{
	bool bFind = false;
	int iGroupId = -1;
	 vector<map<string,string> > vServerGroupInfo;
	try
	{
		if (sGroupName.empty())
		{
			return iGroupId;
		}

		{
	            TC_ThreadLock::Lock lock(_mutex);
	            map<string,int>::iterator it = _mGroupNameIDCache.find(sGroupName);
	            if( it != _mGroupNameIDCache.end())
	            {
	            	LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << sGroupName << "|" << it->second << endl);
 	                return it->second;
	            }
	            vServerGroupInfo = _vServerGroupRule;
	        }

		for(vector<map<string,string> >::size_type i=0; i<vServerGroupInfo.size(); ++i)
		{
			if (sGroupName == vServerGroupInfo[i]["group_name"])
			{
				iGroupId = TC_Common::strto<int>(vServerGroupInfo[i]["group_id"]);
				LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << sGroupName << "|" << iGroupId << endl);
				bFind = true;
				break;
			}
		}

		if (bFind)
		{
			TC_ThreadLock::Lock lock(_mutex);
	                _mGroupNameIDCache[sGroupName] = iGroupId;

	                LOGINFO("get groupId succ|group_name|"<<sGroupName
	                <<"|group_id|"<<iGroupId
	                <<"|GroupNameCache|"<<TC_Common::tostr(_mGroupNameIDCache)<<endl);

			LOGINFO( __FUNCTION__ << ":" << __LINE__  << "|" << sGroupName << "|" << iGroupId << endl);
	                return iGroupId;
		}
	}
	catch(exception &ex)
	{
		 LOG->error()<<__FUNCTION__ << ":" <<__LINE__ << "|exception:" << ex.what() << endl;
	}
	catch(...)
	{
		 LOG->error()<<__FUNCTION__ << ":" <<__LINE__ <<"|unknown exception"<< endl;
	}

	LOGINFO(__FUNCTION__ << ":" << __LINE__  <<  "|" << sGroupName << "|" << endl);
	return -1;
}

int CDbHandle::updateGroupId2DB()
{
    try
    {
        int iCount = 0;
        string sSql;
        string sValues;
        static map<string,int> g_mServerGroupCache;
        map<string,int> mServerGroupCache;
        {
            TC_ThreadLock::Lock lock(_mutex);
            mServerGroupCache = _mServerGroupCache;
        }
        map<string,int>::iterator it = mServerGroupCache.begin();
        while(it != mServerGroupCache.end())
        {
            if(g_mServerGroupCache[it->first] == it->second)
            {
                it++;
                continue;
            }
            sValues = " when '" + it->first+ "' then " + TC_Common::tostr<int>(it->second) + " ";
            if(iCount == 0)
            {
                sSql = "update t_node_info set group_id =  case  endpoint_ip  ";
                sSql += sValues;
            }
            else
            {
                sSql = sSql + " " + sValues;
            }
            iCount++;
            if ( iCount >= 1000 )
            {
                sSql +=" end";
                _mysqlReg.execute(sSql);
                LOGINFO(__FUNCTION__ << " sql:"<<sSql<<endl);
                LOG->debug()<<__FUNCTION__ << " affected:"<<mysql_affected_rows(_mysqlReg.getMysql())<<endl;
                iCount = 0;
            }
            it++;
        }
        if (iCount>0)
        {
            sSql +=" end";
            _mysqlReg.execute(sSql);
            LOGINFO(__FUNCTION__ << " sql:"<<sSql<<endl);
            LOG->debug()<<__FUNCTION__ << " affected:"<<mysql_affected_rows(_mysqlReg.getMysql())<<endl;
        }
        {

            TC_ThreadLock::Lock lock(_mutex); //缓存上次已入库的数据
            g_mServerGroupCache.clear();
            g_mServerGroupCache = mServerGroupCache;
        }
        return 0;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }
}

int CDbHandle::loadIPPhysicalGroupInfo()
{
    try
    {
        string sSql = "select group_id,ip_order,allow_ip_rule,denny_ip_rule,group_name from t_server_group_rule "
                     "order by group_id";
        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);
        LOG->debug()<<__FUNCTION__<<" get server group from db, records affected:"<<res.size()<<endl;

        TC_ThreadLock::Lock lock(_mutex);
        _vServerGroupRule.clear();
        _vServerGroupRule = res.data();

        _mServerGroupCache.clear();  //规则改变 清除以前缓存
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
    }
    catch (exception & ex)
    {
        LOG->debug()<<__FUNCTION__ << " " << ex.what() << endl;

    }
    return -1;
}

int CDbHandle::loadGroupPriority()
{
	std::map<int, GroupPriorityEntry> mapPriority;

	try
	{
		std::string s_command("SELECT id,group_list,station FROM t_group_priority ORDER BY list_order ASC");

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(s_command);
        LOG->debug() << __FUNCTION__ << " load group priority from db, records affected:" << res.size() << endl;

		for (unsigned int i = 0; i < res.size(); i++)
		{
            mapPriority[i].sGroupID = res[i]["id"];
            mapPriority[i].sStation = res[i]["station"];

            std::vector<int> vecGroupID = TC_Common::sepstr<int>(res[i]["group_list"], "|,;", false);
            std::copy(vecGroupID.begin(), vecGroupID.end(), std::inserter(mapPriority[i].setGroupID, mapPriority[i].setGroupID.begin()));
            LOG->debug() << "loaded groups priority to cache [" << mapPriority[i].sStation << "] group size:" << mapPriority[i].setGroupID.size() << endl;
        }

        _mapGroupPriority[!_mapGroupPriorityFlag] = mapPriority;
		_mapGroupPriorityFlag = !_mapGroupPriorityFlag;
        LOG->debug()<<"loaded groups priority to cache [" << _mapGroupPriorityFlag << "] virtual group size:" << _mapGroupPriority[_mapGroupPriorityFlag].size() << endl;
	}
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error() << __FUNCTION__ << " exception: " << ex.what() << endl;
        return -1;
    }
    catch (exception & ex)
    {
        LOG->debug() << __FUNCTION__ << " " << ex.what() << endl;
        return -1;
    }

	return 0;
}


int CDbHandle::computeInactiveRate()
{
    try
    {
        std::string sCommand("SELECT SUM(CASE present_state WHEN 'active' THEN 1 ELSE 0 END) AS active, "
                             "SUM(CASE present_state WHEN 'inactive' THEN 1 ELSE 0 END) AS inactive FROM t_node_info;");

		int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sCommand);
		LOGINFO(__FUNCTION__<<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<<endl);
        if (res.size() != 1)
        {
             LOG->debug() << "loaded inactive rate failed, size(" << res.size() << ") ne 1" << endl;
             return -1;
        }

        int iActive     = TC_Common::strto<int>(res[0]["active"]);
        int iInactive   = TC_Common::strto<int>(res[0]["inactive"]);

        if (iActive < 0 || iInactive < 0)
        {
            LOG->debug()<<"loaded inactive rate failed, active(" << iActive << ") inactive(" << iInactive << ")" << endl;
            return -2;
        }

        return (iInactive + iActive) <= 0?0:static_cast<int>(iInactive/(iInactive + iActive) * 100);
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__ << " exception: " << ex.what() << endl;
        return -3;
    }
    catch (exception & ex)
    {
        LOG->error() << __FUNCTION__ << " " << ex.what() << endl;
        return -4;
    }
}

TC_Mysql::MysqlData CDbHandle::UnionRecord(TC_Mysql::MysqlData& data1,TC_Mysql::MysqlData& data2)
{
	TC_Mysql::MysqlData result;

	result.data() = data1.data();

	vector<map<string, string> >& vTmp = data2.data();
	vector<map<string, string> >::iterator it = vTmp.begin();
	for(;it != vTmp.end();it++)
	{
		result.data().push_back(*it);
	}

	return result;
}

int CDbHandle::loadObjectIdCache(const bool bRecoverProtect, const int iRecoverProtectRate,const int iLoadTimeInterval,const bool bLoadAll)
{
    vector<EndpointF>  vEndpoints;
    ObjectsCache objectsCache;
    SetDivisionCache setDivisionCache;
    std::map<std::string, int> mapStatus;

    try
    {
		//加载分组信息一定要在加载服务信息之前
		loadIPPhysicalGroupInfo();

        //加载存活server及registry列表信息
        string sSql1 =
            "select adapter.servant,adapter.endpoint,server.enable_group,server.setting_state,server.present_state,server.application,server.server_name,server.node_name,server.enable_set,server.set_name,server.set_area,server.set_group,server.ip_group_name "
            "from t_adapter_conf as adapter right join t_server_conf as server using (application, server_name, node_name)";

		//增量加载逻辑
		if(!bLoadAll)
		{
			string sInterval = TC_Common::tm2str(TC_TimeProvider::getInstance()->getNow()- iLoadTimeInterval);
			sSql1 += " ,(select distinct application,server_name from t_server_conf";
			sSql1 += " where registry_timestamp >='" + sInterval + "'";
			sSql1 += " union select distinct application,server_name from t_adapter_conf";
			sSql1 += " where registry_timestamp >='" + sInterval + "') as tmp";
			sSql1 += " where server.application=tmp.application and server.server_name=tmp.server_name";
		}

		string sSql2 = "select servant, endpoint, enable_group, present_state as setting_state, present_state, taf_version as application, taf_version as server_name, taf_version as node_name,'N' as enable_set,'' as set_name,'' as set_area,'' as set_group ,'' as ip_group_name from t_registry_info";
		
		LOG->debug()<<__FUNCTION__<<" sql1:"<<sSql1<<endl;
		LOG->debug()<<__FUNCTION__<<" sql1:"<<sSql2<<endl;	

		int64_t iStart = TC_TimeProvider::getInstance()->getNowMs();
        taf::TC_Mysql::MysqlData res1  = _mysqlReg.queryRecord(sSql1);

		taf::TC_Mysql::MysqlData res2  = _mysqlReg.queryRecord(sSql2);
        LOG->debug()<<__FUNCTION__<<" load Active objects from db, records affected:"<<(res1.size()+res2.size())
			        <<"|cost:"<<(TC_TimeProvider::getInstance()->getNowMs()-iStart)<<endl;

		taf::TC_Mysql::MysqlData res = UnionRecord(res1,res2);
        for(unsigned i = 0; i < res.size(); i++)
        {
            try
            {
                if (res[i]["servant"].empty() && res[i]["endpoint"].empty())
                {
                    LOG->debug() << res[i]["application"] << "-" << res[i]["server_name"] << "-" << res[i]["node_name"] << " NULL"<< endl;
                    mapStatus[res[i]["application"] + res[i]["server_name"] + res[i]["node_name"]] = (res[i]["setting_state"] == "active" && res[i]["present_state"] == "active")?taf::Active:taf::Inactive;
                    continue;
                }

                TC_Endpoint ep;
                ep.parse(res[i]["endpoint"]);

                EndpointF epf;
                epf.host        = ep.getHost();
                epf.port        = ep.getPort();
                epf.timeout     = ep.getTimeout();
                epf.istcp       = ep.isTcp();
                epf.grid        = ep.getGrid();
                //epf.grouprealid = getGroupId(epf.host);
				epf.grouprealid = res[i]["ip_group_name"].empty()?getGroupId(epf.host):getGroupIdByName(res[i]["ip_group_name"]);
                epf.groupworkid = TC_Common::lower(res[i]["enable_group"]) == "y"?epf.grouprealid:-1;
				epf.setId       = "";
                epf.qos         = ep.getQos();
				if (TC_Common::lower(res[i]["enable_set"]) == "y")
				{
					epf.setId = res[i]["set_name"] + "." + res[i]["set_area"] + "." + res[i]["set_group"];
				}
				LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << res[i]["servant"] << "." << epf.host << "|" << epf.grouprealid << "|" << epf.groupworkid << "|" << res[i]["setting_state"] << "|" << res[i]["present_state"] << endl);
				bool bActive = true;
                if (res[i]["setting_state"] == "active" && res[i]["present_state"] == "active")
                {
                    //存活列表
                    objectsCache[res[i]["servant"]].vActiveEndpoints.push_back(epf);
                    mapStatus[res[i]["application"] + res[i]["server_name"] + res[i]["node_name"]] = taf::Active;
				}
                else
                {
                    //非存活列表
                    objectsCache[res[i]["servant"]].vInactiveEndpoints.push_back(epf);
                    mapStatus[res[i]["application"] + res[i]["server_name"] + res[i]["node_name"]] = taf::Inactive;
		    		bActive = false;
				}

				//set划分信息
				if (TC_Common::lower(res[i]["enable_set"]) == "y")
				{
					if (res[i]["set_name"].empty() || res[i]["set_area"].empty() || res[i]["set_group"].empty()
						|| res[i]["set_name"]=="*" ||res[i]["set_area"]=="*")
					{
						LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|" << res[i]["servant"] << "." << epf.host << "|set division invalid[" << res[i]["set_name"] << "." << res[i]["set_area"] << "." << res[i]["set_group"] << "]"<< endl;
						continue;
				    }

					//用于判断是否启动set
					string sSetDiv  = res[i]["set_name"] + "." + res[i]["servant"];
					//set区域
					string sSetArea = res[i]["set_name"] + "." + res[i]["set_area"];
					//set全称
					string sSetId   = res[i]["set_name"] + "." + res[i]["set_area"] + "." + res[i]["set_group"];

					SetServerInfo setServerInfo;
					setServerInfo.bActive = bActive;
					setServerInfo.epf 	 = epf;
					setServerInfo.sServant = res[i]["servant"];
					setDivisionCache[sSetDiv][sSetArea][sSetId].push_back(setServerInfo);
					LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|" << res[i]["servant"] << "." << epf.host << "|" << sSetDiv << "|" << sSetArea << "|" <<sSetId << "|" << setServerInfo.bActive << endl);
				}
            }
            catch (TC_EndpointParse_Exception & ex)
            {
                LOG->debug()<<__FUNCTION__ << " " << ex.what() << endl;
            }
        }

        //替换到cache
        int iRate = bRecoverProtect == true?computeInactiveRate():0;
        if (bRecoverProtect == true && iRate > iRecoverProtectRate && _objectsCache[_objectsFlag].size() > 0)
        {
            LOG->debug() << __FUNCTION__ << " now database recover protect valid, rate(" << iRate << ")" << endl;
            return -1;
        }

		updateObjectsCache(objectsCache,bLoadAll);
		updateStatusCache(mapStatus,bLoadAll);
		updateDivisionCache(setDivisionCache,bLoadAll);

        LOG->debug()<<"loaded objects to cache [" << _objectsFlag << "] size:" <<objectsCache.size() << endl;
        LOG->debug()<<"loaded servant status to cache[" << _mapServantStatusFlag << "] size:" << mapStatus.size() << endl;
		LOG->debug()<<"loaded set server to cache[" << _setDivisionFlag << "] size:" << setDivisionCache.size() << endl;
		FDLOG("query")<<"loaded objects to cache [" << _objectsFlag << "] size:"<<objectsCache.size() << endl;
        FDLOG("query")<<"loaded set server to cache[" << _setDivisionFlag << "] size:" << setDivisionCache.size() << endl;

		static int iCount = DUMP_CACHE_INTERVAL;
        if(++iCount > DUMP_CACHE_INTERVAL)
        {
            iCount = 0;
            dumpCache2HashMap(objectsCache,bLoadAll);
        }

        //更新group信息到node数据表
        //updateGroupId2DB();
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;

        //数据库异常且缓存的列表为空，尝试从文件加载
        if(_objectsCache[_objectsFlag].size() == 0)
        {
            loadCacheFromHashMap(objectsCache);
            _objectsCache[!_objectsFlag] = objectsCache;
            _objectsFlag = !_objectsFlag;
            LOG->debug()<<"loaded objects from file to cache [" << _objectsFlag << "] size:" <<objectsCache.size() << endl;
            FDLOG("query")<<"loaded objects from file to cache [" << _objectsFlag << "] size:"<<objectsCache.size() << endl;
        }
        return -1;
    }
    catch (exception & ex)
    {
        LOG->debug()<<__FUNCTION__ << " " << ex.what() << endl;
        return -1;
    }

    return 0;
}

int CDbHandle::updateRegistryInfo2Db(bool bRegHeartbeatOff)
{
	if(bRegHeartbeatOff)
	{
		LOG->debug()<<"updateRegistryInfo2Db not need to update reigstry status !"<<endl;
		return 0;
	}

    map<string, string>::iterator iter;
    map<string, string> mapServantEndpoint = g_app.getServantEndpoint();
    if(mapServantEndpoint.size() == 0)
    {
        LOG->error()<<"fatal error, get registry servant failed!"<<endl;
        return -1;
    }

    try
    {
        string sSql =
            "replace into t_registry_info (locator_id, servant, endpoint, last_heartbeat, present_state, taf_version) "
            "values ";

        extern TC_Config * g_pconf;
        TC_Endpoint locator;
        locator.parse(mapServantEndpoint[(*g_pconf)["/taf/objname<QueryObjName>"]]);

        for(iter=mapServantEndpoint.begin(); iter!=mapServantEndpoint.end(); iter++)
        {
            sSql += (iter==mapServantEndpoint.begin() ? string("") : string(", ")) +
                "('"+ locator.getHost() + ":" + TC_Common::tostr<int>(locator.getPort()) + "', "
                "'"+ iter->first + "', '"+ iter->second +"', now(), 'active', " +
                "'"+ _mysqlReg.escapeString(TAF_VERSION) + "')";
        }
        _mysqlReg.execute(sSql);
		LOG->debug()<<__FUNCTION__<<" affected:"<<_mysqlReg.getAffectedRows()<<endl;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }
    catch(exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }

    return 0;
}

vector<EndpointF> CDbHandle::findObjectById(const string & id)
{
    ObjectsCache::iterator it;
    ObjectsCache & usingCache = _objectsCache[_objectsFlag];

    if((it = usingCache.find(id)) != usingCache.end())
    {
        return it->second.vActiveEndpoints;
    }
    else
    {
        vector<EndpointF> activeEp;
        return activeEp;
    }
}


int CDbHandle::findObjectById4All(const string & id, vector<EndpointF>& activeEp, vector<EndpointF>& inactiveEp)
{
    ObjectsCache::iterator it;
    ObjectsCache & usingCache = _objectsCache[_objectsFlag];

    if((it = usingCache.find(id)) != usingCache.end())
    {
        activeEp   = it->second.vActiveEndpoints;
        inactiveEp = it->second.vInactiveEndpoints;
    }
    else
    {
        activeEp.clear();
        inactiveEp.clear();
    }

    return  0;
}

vector<EndpointF> CDbHandle::getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, int iGroupId, ostringstream & os)
{
    os << "|";
    vector<EndpointF> vResult;

    for(unsigned i=0; i< vecEps.size(); i++)
    {
        os << vecEps[i].host << ":" << vecEps[i].port << "(" << vecEps[i].groupworkid << ");";
        if (GroupSelect == ENUM_USE_WORK_GROUPID && vecEps[i].groupworkid == iGroupId)
        {
            vResult.push_back(vecEps[i]);
        }
        if (GroupSelect == ENUM_USE_REAL_GROUPID && vecEps[i].grouprealid == iGroupId)
        {
            vResult.push_back(vecEps[i]);
        }
    }

    return vResult;
}

vector<EndpointF> CDbHandle::getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, const set<int> & setGroupID, ostringstream & os)
{
	os << "|";
	std::vector<EndpointF> vecResult;

	for (std::vector<EndpointF>::size_type i = 0; i < vecEps.size(); i++)
	{
		os << vecEps[i].host << ":" << vecEps[i].port << "(" << vecEps[i].groupworkid << ")";
		if (GroupSelect == ENUM_USE_WORK_GROUPID && setGroupID.count(vecEps[i].groupworkid) == 1)
		{
			vecResult.push_back(vecEps[i]);
		}
        if (GroupSelect == ENUM_USE_REAL_GROUPID && setGroupID.count(vecEps[i].grouprealid) == 1)
        {
            vecResult.push_back(vecEps[i]);
        }
	}

	return vecResult;
}

int CDbHandle::findObjectByIdInSameGroup(const string & id,
                                 const string & ip,
                                 vector<EndpointF>& activeEp,
                                 vector<EndpointF>& inactiveEp,
                                 ostringstream &os)
{


    activeEp.clear();
    inactiveEp.clear();

    int iClientGroupId  = getGroupId(ip);

    os<<"|("<<iClientGroupId<<")";

    if(iClientGroupId == -1)
    {
        return findObjectById4All(id,activeEp,inactiveEp);
    }

    ObjectsCache::iterator it;
    ObjectsCache & usingCache = _objectsCache[_objectsFlag];

    if((it = usingCache.find(id)) != usingCache.end())
    {
        activeEp    = getEpsByGroupId(it->second.vActiveEndpoints, ENUM_USE_WORK_GROUPID, iClientGroupId, os);
        inactiveEp  = getEpsByGroupId(it->second.vInactiveEndpoints, ENUM_USE_WORK_GROUPID, iClientGroupId, os);

        if (activeEp.size() == 0) //没有同组的endpoit,匹配未启用分组的服务
        {
           activeEp    = getEpsByGroupId(it->second.vActiveEndpoints, ENUM_USE_WORK_GROUPID, -1, os);
           inactiveEp  = getEpsByGroupId(it->second.vInactiveEndpoints, ENUM_USE_WORK_GROUPID, -1, os);
        }
        if (activeEp.size() == 0) //没有同组的endpoit
        {
            activeEp   = it->second.vActiveEndpoints;
            inactiveEp = it->second.vInactiveEndpoints;
        }
    }
    return  0;
}

int CDbHandle::findObjectByIdInGroupPriority(const std::string &sID, const std::string &sIP, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os)
{
	vecActive.clear();
	vecInactive.clear();

	int iClientGroupID = getGroupId(sIP);
	os << "|(" << iClientGroupID << ")";
	if (iClientGroupID == -1)
	{
		return findObjectById4All(sID, vecActive, vecInactive);
	}

	ObjectsCache & usingCache = _objectsCache[_objectsFlag];
	ObjectsCache::iterator itObject = usingCache.find(sID);
	if (itObject == usingCache.end()) return 0;

	//首先在同组中查找
	{
		vecActive 	= getEpsByGroupId(itObject->second.vActiveEndpoints, ENUM_USE_WORK_GROUPID, iClientGroupID, os);
		vecInactive	= getEpsByGroupId(itObject->second.vInactiveEndpoints, ENUM_USE_WORK_GROUPID, iClientGroupID, os);
        os << "|(In Same Group: " << iClientGroupID << " Active=" << vecActive.size() << " Inactive=" << vecInactive.size() << ")";
	}

	//启用分组，但同组中没有找到，在优先级序列中查找
	std::map<int, GroupPriorityEntry> & mapPriority = _mapGroupPriority[_mapGroupPriorityFlag];
	for (std::map<int, GroupPriorityEntry>::iterator it = mapPriority.begin(); it != mapPriority.end() && vecActive.empty(); it++)
	{
        if (it->second.setGroupID.count(iClientGroupID) == 0)
        {
            os << "|(Not In Priority " << it->second.sGroupID << ")";
            continue;
        }
		vecActive	= getEpsByGroupId(itObject->second.vActiveEndpoints, ENUM_USE_WORK_GROUPID, it->second.setGroupID, os);
		vecInactive	= getEpsByGroupId(itObject->second.vInactiveEndpoints, ENUM_USE_WORK_GROUPID, it->second.setGroupID, os);
        os << "|(In Priority: " << it->second.sGroupID << " Active=" << vecActive.size() << " Inactive=" << vecInactive.size() << ")";
	}

	//没有同组的endpoit,匹配未启用分组的服务
	if (vecActive.empty())
	{
		vecActive	= getEpsByGroupId(itObject->second.vActiveEndpoints, ENUM_USE_WORK_GROUPID, -1, os);
		vecInactive	= getEpsByGroupId(itObject->second.vInactiveEndpoints, ENUM_USE_WORK_GROUPID, -1, os);
        os << "|(In No Grouop: Active=" << vecActive.size() << " Inactive=" << vecInactive.size() << ")";
	}

	//在未分组的情况下也没有找到，返回全部地址(此时基本上所有的服务都已挂掉)
	if(vecActive.empty())
	{
		vecActive	= itObject->second.vActiveEndpoints;
		vecInactive	= itObject->second.vInactiveEndpoints;
        os << "|(In All: Active=" << vecActive.size() << " Inactive=" << vecInactive.size() << ")";
	}

	return 0;
}

int CDbHandle::findObjectByIdInSameStation(const std::string &sID, const std::string & sStation, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os)
{
	vecActive.clear();
	vecInactive.clear();

    //获得station所有组
    std::map<int, GroupPriorityEntry> & mapPriority         = _mapGroupPriority[_mapGroupPriorityFlag];
    std::map<int, GroupPriorityEntry>::iterator itGroup     = mapPriority.end();
    for (itGroup = mapPriority.begin(); itGroup != mapPriority.end(); itGroup++)
    {
        if (itGroup->second.sStation != sStation) continue;

        break;
    }

    if (itGroup == mapPriority.end())
    {
        os << "|not found station:" << sStation;
        return -1;
    }

	ObjectsCache & usingCache = _objectsCache[_objectsFlag];
	ObjectsCache::iterator itObject = usingCache.find(sID);
	if (itObject == usingCache.end()) return 0;

	//查找对应所有组下的IP地址
    vecActive	= getEpsByGroupId(itObject->second.vActiveEndpoints, ENUM_USE_REAL_GROUPID, itGroup->second.setGroupID, os);
	vecInactive	= getEpsByGroupId(itObject->second.vInactiveEndpoints, ENUM_USE_REAL_GROUPID, itGroup->second.setGroupID, os);

    return 0;
}

int CDbHandle::findObjectByIdInSameSet(const string &sID, const vector<string> &vtSetInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os)
{
	string sSetDiv    = vtSetInfo[0] + "." + sID ;
	string sSetArea = vtSetInfo[0] + "." + vtSetInfo[1];
	string sSetId     = vtSetInfo[0] + "." + vtSetInfo[1] + "." + vtSetInfo[2];
	SetDivisionCache::iterator setDivIt;

	SetDivisionCache &usingSetDivisionCache = _setDivisionCache[_setDivisionFlag];
	setDivIt = usingSetDivisionCache.find(sSetDiv);
	if (setDivIt == usingSetDivisionCache.end())
	{//此情况下没启动set
		LOGINFO( __FUNCTION__ << ":" << __LINE__ << "|haven't start set|" << sSetDiv << endl);
		return -1;
	}

	map<string, map<string,vector<SetServerInfo> > >::iterator setAreaIt = setDivIt->second.find(sSetArea);
	if (setAreaIt == (setDivIt->second).end())
	{
		LOGINFO(__FUNCTION__ << ":" << __LINE__ << "|no set area found|" << setDivIt->first << "|"<< sSetArea << endl);
		return -3;
	}

	if (vtSetInfo[2] == "*")
	{//检索通配组和set组中的所有服务
		map<string,vector<SetServerInfo> >::iterator setIdIt = setAreaIt->second.begin();
		for(;setIdIt!=setAreaIt->second.end();++setIdIt)
		{
			vector<SetServerInfo> &vtSetServerInfo = setIdIt->second;
			for(size_t i=0; i<vtSetServerInfo.size(); ++i)
			{
				if(vtSetServerInfo[i].sServant == sID)
				{
					if (vtSetServerInfo[i].bActive)
					{
						vecActive.push_back(vtSetServerInfo[i].epf);
					}
					else
					{
						vecInactive.push_back(vtSetServerInfo[i].epf);
					}
				}
			}
		}

		return (vecActive.empty()&&vecInactive.empty())? -2 : 0;
	}
	else
	{

		// 1.从指定set组中查找
		int iRet = findObjectByIdInSameSet(sSetId, setAreaIt->second, vecActive, vecInactive, os);
		if (iRet != 0 && vtSetInfo[2]!="*")
		{// 2. 1中没找到，在通配组里找
			string sWildSetId =  vtSetInfo[0] + "." + vtSetInfo[1] + ".*";
			iRet = findObjectByIdInSameSet(sWildSetId, setAreaIt->second, vecActive, vecInactive, os);
		}

		return iRet;
	}


}

int CDbHandle::findObjectByIdInSameSet(const string &sSetId, const map<string, vector<SetServerInfo> > &mpSetServerInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os)
{
	map<string, vector<SetServerInfo> >::const_iterator it = mpSetServerInfo.find(sSetId);
	if (it == mpSetServerInfo.end())
	{
		return -2;
	}

	const vector<SetServerInfo> &vtSetServerInfo =  it->second;
	for(size_t i=0; i<vtSetServerInfo.size(); ++i)
	{
		if (vtSetServerInfo[i].bActive)
		{
			vecActive.push_back(vtSetServerInfo[i].epf);
		}
		else
		{
			vecInactive.push_back(vtSetServerInfo[i].epf);
		}
	}

	int iRet = (vecActive.empty()&&vecInactive.empty())? -2 : 0;
	return iRet;
}
int CDbHandle::getNodeTemplateName(const string nodeName, string & sTemplateName)
{
    try
    {
        string sSql =
            "select template_name "
            "from t_node_info "
            "where node_name='" + _mysqlReg.escapeString(nodeName) + "'";

        taf::TC_Mysql::MysqlData res = _mysqlReg.queryRecord(sSql);

        if(res.size() != 0)
        {
            sTemplateName = res[0]["template_name"];
        }

        LOG->debug()<<__FUNCTION__<<" '"<< nodeName <<"' affected:"<<res.size()
            << " get template_name:'" << sTemplateName << "'" <<endl;

        return 0;
    }
    catch(TC_Mysql_Exception & ex)
    {
        LOG->error()<<__FUNCTION__<<" exception: "<<ex.what()<<endl;
        return -1;
    }

    return 0;
}

int CDbHandle::initFileCache()
{
    try
    {
        extern TC_Config * g_pconf;
        _fileCache.initDataBlockSize(TC_Common::strto<size_t>(g_pconf->get("/taf/objcache<min_block>", "50")),
                TC_Common::strto<size_t>(g_pconf->get("/taf/objcache<max_block>"), "200"),
                TC_Common::strto<float>(g_pconf->get("/taf/objcache<factor>", "1.2")));

        _fileCache.initStore(g_pconf->get("/taf/objcache<FilePath>", "objectCache.dat").c_str(),
                TC_Common::toSize(g_pconf->get("/taf/objcache<FileSize>", "1M"), 0));
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<" exception:" << ex.what() << endl;
        return -1;
    }

    return 0;
}

int CDbHandle::dumpCache2HashMap(ObjectsCache & cache,const bool bLoadAll)
{
    try
    {
        if(bLoadAll)
			_fileCache.clear();

        ObjectsCache::iterator it;
        ObjectName name;
        ObjectItem item;
        for(it=cache.begin(); it != cache.end(); it++)
        {
            name.ObjectId = it->first;
            item = it->second;
            int iRet = _fileCache.set(name, item);
            if(iRet != TC_HashMap::RT_OK)
            {
                LOG->error()<<"set to hash error:" << iRet << " " << it->first << endl;
            }
        }
        LOG->debug()<<"dump cache to file, size:" << cache.size() << endl;
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<" exception:" << ex.what() << endl;
        return -1;
    }

    return 0;
}


int CDbHandle::loadCacheFromHashMap(ObjectsCache & cache)
{
    try
    {
        //initFileCache();

        cache.clear();

        FileHashMap::hash_iterator it= _fileCache.hashBegin();
        for(it = _fileCache.hashBegin(); it != _fileCache.hashEnd(); it++)
        {
            vector<pair<ObjectName, ObjectItem> > vp;
            it->get(vp);
            for(size_t i=0; i < vp.size(); i++)
            {
                cache[vp[i].first.ObjectId] = vp[i].second;
            }
        }
    }
    catch(exception &ex)
    {
        LOG->error()<<__FUNCTION__<<" exception:" << ex.what() << endl;
        return -1;
    }

    return 0;
}

void CDbHandle::updateStatusCache(const std::map<std::string, int>& mStatus,bool updateAll)
{
	//全量更新
	 LOG->error()<<__FUNCTION__<<"mStatus"<<mStatus.size()<<endl;
	if(updateAll)
	{
		_mapServantStatus[!_mapServantStatusFlag] = mStatus;
		_mapServantStatusFlag = !_mapServantStatusFlag;
		//由于是双buffer机制，此处要两个buffer同时更新，保证数据的实时性
		_mapServantStatus[!_mapServantStatusFlag] = mStatus;
	}
	else
	{
		std::map<std::string, int>& tmpStatus = _mapServantStatus[!_mapServantStatusFlag];
		std::map<std::string, int>::const_iterator it = mStatus.begin();
		for(;it != mStatus.end();it++)
		{
			tmpStatus[it->first] = it->second;
		}

		_mapServantStatusFlag = !_mapServantStatusFlag;
	}
}

void CDbHandle::updateObjectsCache(const ObjectsCache& objCache,bool updateAll)
{
	//全量更新
	if(updateAll)
	{
		_objectsCache[!_objectsFlag] = objCache;
		_objectsFlag = !_objectsFlag;
		//由于是双buffer机制，此处要两个buffer同时更新，保证数据的实时性
		_objectsCache[!_objectsFlag] = objCache;
	}
	else
	{
		ObjectsCache& tmpObjCache = _objectsCache[!_objectsFlag];
		ObjectsCache::const_iterator it = objCache.begin();
		for(;it != objCache.end();it++)
		{
			//增量的时候加载的是服务的所有节点，因此这里直接替换
			tmpObjCache[it->first] = it->second;
		}

		_objectsFlag = !_objectsFlag;
	}
}

void CDbHandle::updateDivisionCache(const SetDivisionCache& setDivisionCache,bool updateAll)
{
	//全量更新
	if(updateAll)
	{
		_setDivisionCache[!_setDivisionFlag] = setDivisionCache;
		_setDivisionFlag = !_setDivisionFlag;
		//由于是双buffer机制，此处要两个buffer同时更新，保证数据的实时性
		_setDivisionCache[!_setDivisionFlag] = setDivisionCache;
	}
	else
	{
		SetDivisionCache& tmpsetCache = _setDivisionCache[!_setDivisionFlag];
		SetDivisionCache::const_iterator it = setDivisionCache.begin();
		for(;it != setDivisionCache.end();it++)
		{
			tmpsetCache[it->first] = it->second;
		}
		_setDivisionFlag = !_setDivisionFlag;
	}
}


