#include <iostream>

#include "RegistryImp.h"
#include "RegistryProcThread.h"
#include "RegistryServer.h"

void RegistryImp::initialize()
{
    LOG->debug()<<"begin RegistryImp init"<<endl;

    //初始化配置db连接
    extern TC_Config * g_pconf;
    _db.init(g_pconf);

    LOG->debug()<<"RegistryImp init ok."<<endl;

}

int RegistryImp::registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li, taf::JceCurrentPtr current)
{
    return _db.registerNode(name, ni, li);
}

int RegistryImp::keepAlive(const string & name, const LoadInfo & ni, taf::JceCurrentPtr current)
{
	RegistryProcInfo procInfo;
	procInfo.nodeName = name;
	procInfo.loadinfo = ni;
	procInfo.cmd = EM_NODE_KEEPALIVE;

	//放入异步处理线程中
	extern RegistryServer g_app;
	g_app.getRegProcThread()->put(procInfo);

	//return _db.keepAlive(name, ni);
    return 0;
}

vector<ServerDescriptor> RegistryImp::getServers(const string & nodeName, const string & app,
        const string & serverName, taf::JceCurrentPtr current)
{
    return  _db.getServers(nodeName, app, serverName);
}

int RegistryImp::updateServer(const string & nodeName, const string & app, const string & serverName,
        const taf::ServerStateInfo & stateInfo, taf::JceCurrentPtr current)
{
    return _db.updateServerState(app, serverName, nodeName, "present_state", stateInfo.serverState, stateInfo.processId);
}

int RegistryImp::updateServerBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, taf::JceCurrentPtr current)
{
	return _db.updateServerStateBatch(vecStateInfo);
}


int RegistryImp::destroy(const string & name, taf::JceCurrentPtr current)
{
    return _db.destroyNode(name);
}


int RegistryImp::reportVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version, taf::JceCurrentPtr current)
{
	RegistryProcInfo procInfo;
	procInfo.appName = app;
	procInfo.serverName = serverName;
	procInfo.nodeName = nodeName;
	procInfo.tafVersion = version;
	procInfo.cmd = EM_REPORTVERSION;

	//放入异步处理线程中
	extern RegistryServer g_app;
	g_app.getRegProcThread()->put(procInfo);

	return 0;
    //return _db.setServerTafVersion(app, serverName, nodeName, version);
}

int RegistryImp::getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,taf::JceCurrentPtr current)
{
    string sTemplateName;
    int iRet = _db.getNodeTemplateName(nodeName, sTemplateName);
    if(iRet != 0 || sTemplateName == "")
    {
        //默认模板配置
        extern TC_Config * g_pconf;
        sTemplateName = (*g_pconf)["/taf/nodeinfo<defaultTemplate>"];
    }

    string sDesc;
    profileTemplate = _db.getProfileTemplate(sTemplateName, sDesc);

    LOG->debug()<< nodeName << " get sTemplateName:" << sTemplateName << " result:" << sDesc << endl;

    return 0;
}

int RegistryImp::getClientIp(std::string &sClientIp,taf::JceCurrentPtr current)
{
    sClientIp = current->getIp();
    return 0;
}

int RegistryImp::updatePatchResult(const PatchResult & result, taf::JceCurrentPtr current)
{
    LOG->debug() << __FUNCTION__ << "|" << result.sApplication + "." + result.sServerName + "_" + result.sNodeName << "|V:" << result.sVersion << "|U:" <<  result.sUserName << endl;

	RegistryProcInfo procInfo;
	procInfo.appName = result.sApplication;
	procInfo.serverName = result.sServerName;
	procInfo.nodeName = result.sNodeName;
	procInfo.patchVersion = result.sVersion;
	procInfo.patchUserName = result.sUserName;

	procInfo.cmd = EM_UPDATEPATCHRESULT;

	//放入异步处理线程中
	extern RegistryServer g_app;
	g_app.getRegProcThread()->put(procInfo);

	return 0;
    //return _db.setPatchInfo(result.sApplication, result.sServerName, result.sNodeName, result.sVersion, result.sUserName);
}

