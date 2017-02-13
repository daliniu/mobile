#include "AdminRegImp.h"

extern TC_Config * g_pconf;

void AdminRegImp::initialize()
{
    LOG->debug()<<"begin AdminRegImp init"<<endl;

    //初始化配置db连接
    _db.init(g_pconf);

    LOG->debug()<<"AdminRegImp init ok."<<endl;
}

vector<string> AdminRegImp::getAllApplicationNames(string & result, taf::JceCurrentPtr current)
{
    LOG->debug() << "into " << __FUNCTION__ <<endl;
    return _db.getAllApplicationNames(result);
}

vector<string> AdminRegImp::getAllNodeNames(string & result, taf::JceCurrentPtr current)
{
    map<string, string> mNodes = _db.getActiveNodeList(result);
    map<string, string>::iterator it;
    vector<string> vNodes;

    LOG->debug() << "into " << __FUNCTION__ <<endl;
    for(it = mNodes.begin(); it != mNodes.end(); it++)
    {
        vNodes.push_back(it->first);
    }

    return vNodes;
}

int AdminRegImp::getNodeVesion(const string &nodeName, string &version, string & result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        return _db.getNodeVersion(nodeName,version,result);
    }
    catch(TafException & ex)
    {
        result = string(string(__FUNCTION__)) + " '" + nodeName + "' exception:" + ex.what();
        LOG->error()<< result << endl;
    }
    return -1;
}

bool AdminRegImp::pingNode(const string & name, string & result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        NodePrx nodePrx = _db.getNodePrx(name);
        nodePrx->taf_ping();
        result = "succ";
        return true;
    }
    catch(TafException & ex)
    {
        result = string(string(__FUNCTION__)) + " '" + name + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return false;
    }

    return false;
}

int AdminRegImp::shutdownNode(const string & name, string & result, taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << name << "|" << current->getIp() << ":" << current->getPort() <<endl;

    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        NodePrx nodePrx = _db.getNodePrx(name);
        return nodePrx->shutdown(result);
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + name + "' exception:" + ex.what();
        LOG->error()<< result <<endl;
        return -1;
    }
}


///////////////////////////////////
vector<vector<string> > AdminRegImp::getAllServerIds(string & result, taf::JceCurrentPtr current)
{
    LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << current->getIp() << ":" << current->getPort() <<endl;

    return _db.getAllServerIds(result);
}

int AdminRegImp::getServerState(const string & application, const string & serverName, const string & nodeName,
            ServerStateDesc &state, string &result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;

        vector<ServerDescriptor> server;
        server = _db.getServers(application, serverName, nodeName,true);
        if(server.size() == 0)
        {
            result = " '" + application  + "." + serverName + "_" + nodeName + "' no config";
            return -1;
        }
        state.settingStateInReg = server[0].settingState;
        state.presentStateInReg = server[0].presentState;
        state.patchVersion = server[0].patchVersion;
        state.patchTime = server[0].patchTime;
        state.patchUser = server[0].patchUser;

        //判断是否为dns 非dns才需要到node调用
        if(server[0].serverType == "taf_dns")
        {
            LOG->debug() <<" '" + application  + "." + serverName + "_" + nodeName + "' is taf_dns server"<<endl;
            state.presentStateInNode = server[0].presentState;
        }
        else
        {
            NodePrx nodePrx = _db.getNodePrx(nodeName);
            ServerState stateInNode = nodePrx->getState(application, serverName, result);
            state.presentStateInNode = etos(stateInNode);
            state.processId = nodePrx->getServerPid(application, serverName, result);
        }

        return 0;
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }
}

int AdminRegImp::getGroupId(const string & ip, int &groupId, string &result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<"ip:"<<ip<<endl;

        return _db.getGroupId(ip);
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + ip + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }
}

int AdminRegImp::startServer(const string & application, const string & serverName, const string & nodeName,
        string & result, taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;
    int iRet = -1;

    try
    {

        //更新数据库server的设置状态
        _db.updateServerState(application, serverName, nodeName, "setting_state", taf::Active);

        vector<ServerDescriptor> server;
        server = _db.getServers(application, serverName, nodeName, true);

        //判断是否为dns 非dns才需要到node启动服务
        if(server.size() != 0 && server[0].serverType == "taf_dns")
        {
            LOG->debug() <<" '" + application  + "." + serverName + "_" + nodeName + "' is taf_dns server"<<endl;
            iRet =  _db.updateServerState(application, serverName, nodeName, "present_state", taf::Active);
        }
        else
        {
            NodePrx nodePrx = _db.getNodePrx(nodeName);
	  		LOG->debug()  << "call node startServer: "<< __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;
            iRet =  nodePrx->startServer(application, serverName, result);
        }
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }

    LOG->debug()  << "success startServer: "<< __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() << "|" << iRet <<endl;

    return iRet;

}

int AdminRegImp::stopServer(const string & application, const string & serverName, const string & nodeName,
        string & result, taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;
    int iRet = -1;
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        //更新数据库server的设置状态
        _db.updateServerState(application, serverName, nodeName, "setting_state", taf::Inactive);

        vector<ServerDescriptor> server;
        server = _db.getServers(application, serverName, nodeName, true);

        //判断是否为dns 非dns才需要到node启动服务
        if(server.size() != 0 && server[0].serverType == "taf_dns")
        {
            LOG->debug() <<" '" + application  + "." + serverName + "_" + nodeName + "' is taf_dns server"<<endl;
            iRet =  _db.updateServerState(application, serverName, nodeName, "present_state", taf::Inactive);
        }
        else
        {
            NodePrx nodePrx = _db.getNodePrx(nodeName);
	 		LOG->debug() << "call stopServer: "<< __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;
            iRet =  nodePrx->stopServer(application, serverName, result);
        }
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }

    LOG->debug() << "succes stopServer: "<< __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort()  << "|" << iRet <<endl;

    return iRet;

}

int AdminRegImp::restartServer(const string & application, const string & serverName, const string & nodeName,
        string & result, taf::JceCurrentPtr current)
{
	LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;

    bool isDnsServer = false;

    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;


        vector<ServerDescriptor> server;
        server = _db.getServers(application, serverName, nodeName, true);

        //判断是否为dns 非dns才需要到node停止、启动服务
        if(server.size() != 0 && server[0].serverType == "taf_dns")
        {
            isDnsServer =  true;
        }
        else
        {
            NodePrx nodePrx = _db.getNodePrx(nodeName);
            nodePrx->stopServer(application, serverName, result);
        }
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
    }

    try
    {
        //从停止状态发起的restart需重设状态
        _db.updateServerState(application, serverName, nodeName, "setting_state", taf::Active);


        //判断是否为dns 非dns才需要到node启动服务
        if(isDnsServer == true)
        {
            LOG->debug() <<" '" + application  + "." + serverName + "_" + nodeName + "' is taf_dns server"<<endl;
            return _db.updateServerState(application, serverName, nodeName, "present_state", taf::Active);
        }
        else
        {
            NodePrx nodePrx = _db.getNodePrx(nodeName);
            return nodePrx->startServer(application, serverName, result);
        }
    }
    catch(TafException & ex)
    {
        result += string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;

        return -1;
    }

}

int AdminRegImp::notifyServer(const string & application, const string & serverName, const string & nodeName,
            const string &command, string &result, taf::JceCurrentPtr current)
{
    LOG->debug() << __FILE__ << "|" << __LINE__ << "|into " << __FUNCTION__ << "|" << application << "." << serverName << "_" << nodeName << "|" << current->getIp() << ":" << current->getPort() <<endl;

    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        NodePrx nodePrx = _db.getNodePrx(nodeName);
        return nodePrx->notifyServer(application, serverName, command, result);
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }
    return 0;
}

int AdminRegImp::patchServer(const string & application, const string & serverName, const string & nodeName,
        bool shutdown, const string & version, const string & user, string & result, taf::JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << application + "." + serverName + "_" + nodeName << "|V:" << version << "|U:" << user << endl;

    try
    {
        //获取Node版本号
        int iNodeVersion = 0;
        std::string sVersion, sResult;
        if (_db.getNodeVersion(nodeName, sVersion, sResult) == 0)
        {
            int iMajor = 0, iMinor = 0, iPatch = 0;
            sscanf(sVersion.c_str(), "%d.%d.%d", &iMajor, &iMinor, &iPatch);
            iNodeVersion = iMajor * 100 + iMinor * 10 + iPatch;
        }
        LOG->debug() << __FUNCTION__ << "|" << application + "." + serverName + "_" + nodeName << "|TAF:" << iNodeVersion << endl;


        //发起发布请求
        NodePrx nodePrx = _db.getNodePrx(nodeName);

        int iTafTimeout = nodePrx->taf_timeout();
        int iRet = 0;
        try
        {
            //result 传递version信息，
            string s = version;
            if (iNodeVersion >= 165) s += "," + user;
            iRet = nodePrx->patch(application, serverName, shutdown, (*g_pconf)["/taf/objname<patchServerObj>"], s);
            result = s;
        }
        catch(TafException & ex)
        {
            result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
            LOG->error()<< result << endl;
        }
        nodePrx->taf_timeout(iTafTimeout);

        if (iRet == 0 && iNodeVersion < 165)
        {
            _db.setPatchInfo(application, serverName, nodeName, version, user);
        }

	 LOG->debug() << __FUNCTION__ << "|" << application + "." + serverName + "_" + nodeName << "|TAF:" << iNodeVersion << "|ret:" << iRet << "|result:" << result << endl;

        return iRet;
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }

}

int AdminRegImp::batchPatch(const taf::PatchRequest & req, string & result, taf::JceCurrentPtr current)
{
    taf::PatchRequest reqPro = req;
    reqPro.patchobj = (*g_pconf)["/taf/objname<patchServerObj>"];

    LOG->debug() << __FUNCTION__        << "|"
                 << reqPro.appname + "." + reqPro.servername + "_" + reqPro.nodename << "|"
                 << reqPro.binname      << "|"
                 << reqPro.version      << "|"
                 << reqPro.user         << "|"
                 << reqPro.servertype   << "|"
                 << reqPro.patchobj     << "|"
                 << reqPro.md5          << endl;

    try
    {
        NodePrx proxy = _db.getNodePrx(reqPro.nodename);
        if (!proxy)
        {
            LOG->error() << __FUNCTION__ << "|" << reqPro.appname + "." + reqPro.servername + "_" + reqPro.nodename << "|get proxy error" << endl;

            result = "get proxy error for " + reqPro.nodename;
            return -1;
        }

	int iRet = 0;
        iRet = proxy->patchPro(reqPro, result);
	LOG->debug() << __FUNCTION__   <<  ":" << __LINE__   << "|success patchPro|"
                 << reqPro.appname + "." + reqPro.servername + "_" + reqPro.nodename << "|"
                 << reqPro.binname      << "|"
                 << reqPro.version      << "|"
                 << reqPro.user         << "|"
                 << reqPro.servertype   << endl;
	return iRet;
    }
    catch (std::exception & ex)
    {
        LOG->error() << __FUNCTION__ << "|" << reqPro.appname + "." + reqPro.servername + "_" + reqPro.nodename << "|Exception:" << ex.what() << endl;
        result = ex.what();
        return -1;
    }
    catch (...)
    {
        LOG->error() << __FUNCTION__ << "|" << reqPro.appname + "." + reqPro.servername + "_" + reqPro.nodename << "|Unknown Exception" << endl;
        result = "Unknown Exception";
        return -1;
    }

    return 0;
}

int AdminRegImp::patchSubborn(const string & application, const string & serverName, const string & nodeName, const string & sSrcFile, const string & sDstFile, string &result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;

        NodePrx nodePrx = _db.getNodePrx(nodeName);

        int iTafTimeout = nodePrx->taf_timeout();
        int iRet = 0;
        try
        {
            //result 传递version信息
            iRet = nodePrx->patchSubborn((*g_pconf)["/taf/objname<patchServerObj>"], application, serverName, sSrcFile, sDstFile, result);
        }
        catch(TafException & ex)
        {
            result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
            LOG->error()<< result << endl;
        }
        nodePrx->taf_timeout(iTafTimeout);

        return iRet;
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }
}

int AdminRegImp::getPatchPercent( const string& application, const string& serverName,  const string & nodeName,
        PatchInfo &tPatchInfo, JceCurrentPtr current)
{
    int iRet = -1;
    string &result = tPatchInfo.sResult;
    try
    {
        LOG->debug() << "into " <<  string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
				<< "|caller: " << current->getIp()  << ":" << current->getPort() <<endl;

        NodePrx nodePrx = _db.getNodePrx(nodeName);

	    LOG->debug() << "get nodeprx| " <<  string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
				<< "|caller: " << current->getIp()  << ":" << current->getPort() <<endl;
        iRet = nodePrx->getPatchPercent(application, serverName, tPatchInfo);
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName + " Caller:" + current->getIp() + ":" + TC_Common::tostr(current->getPort())
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
    }

    LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "caller: " +  application  + "." + serverName + "_" + nodeName << "|"<< current->getIp()  << ":" << current->getPort()  << "|end|" <<  " ret:" << iRet  << endl;
    return iRet;

}

int AdminRegImp::loadServer(const string & application, const string & serverName, const string & nodeName,
        string & result, taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "into " << __FUNCTION__ <<endl;
        NodePrx nodePrx = _db.getNodePrx(nodeName);
        return nodePrx->loadServer(application, serverName, result);
    }
    catch(TafException & ex)
    {
        result = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< result << endl;
        return -1;
    }
}


int AdminRegImp::getProfileTemplate(const std::string & profileName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current)
{
    profileTemplate = _db.getProfileTemplate(profileName, resultDesc);

    LOG->debug() << __FUNCTION__ << " get " << profileName  << " return length:"<< profileTemplate.size() << endl;

    return 0;
}

int AdminRegImp::getServerProfileTemplate(const string & application, const string & serverName, const string & nodeName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << " get " << application<<"."<<serverName<<"_"<<nodeName<< endl;

    int iRet =  -1;
    try
    {
        if(application != "" && serverName != "" && nodeName != "")
        {
            vector<ServerDescriptor> server;
            server = _db.getServers(application, serverName, nodeName, true);
            if(server.size() > 0)
            {
                profileTemplate = server[0].profile;
                iRet = 0;
            }
        }
    }
    catch(exception & ex)
    {
        resultDesc = string(__FUNCTION__) + " '" + application  + "." + serverName + "_" + nodeName
                + "' exception:" + ex.what();
        LOG->error()<< resultDesc << endl;
    }

    LOG->debug() << __FUNCTION__ << " get " << application<<"."<<serverName<<"_"<<nodeName << " ret:"<<iRet<<" return length:"<< profileTemplate.size() << endl;

    return iRet;
}




