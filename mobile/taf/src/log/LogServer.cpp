#include "LogServer.h"
#include "util/tc_logger.h"
#include "LogImp.h"

void LogServer::initialize()
{
    string s;
    loadLogFormat("","",s);

    //日志路径
    g_globe._log_path = _conf["/taf/log<logpath>"];

    //启动写线程
    g_globe._group.start(TC_Common::strto<size_t>(_conf["/taf/log<logthread>"]));

    //增加对象
    addServant<LogImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".LogObj");

        //增加对象
    addServant<LogImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".LogPubObj");

    TAF_ADD_ADMIN_CMD_NORMAL("reloadLogFormat", LogServer::loadLogFormat);
}
bool LogServer::loadLogFormat(const string& command, const string& params, string& result)
{
    try
    {
         TC_Config conf;
         vector<string> vHourlist;
         conf.parseFile(ServerConfig::ConfigFile);
         
         map<string,string> mAppIdcRelation;
         conf.getDomainMap("/taf/log/appIdc",mAppIdcRelation);
         g_globe._mAppIdcRelation = mAppIdcRelation;    



         string sHour = conf["/taf/log/format<hour>"];
         vHourlist = TC_Common::sepstr<string>(sHour,"|;,");
         sort(vHourlist.begin(),vHourlist.end());
         unique(vHourlist.begin(),vHourlist.end());
         g_globe._vHourlist = vHourlist;
         

         result = "loadLogFormat succ hour:" +sHour +":idc" + TC_Common::tostr(mAppIdcRelation);

         LOG->debug() << "reloadLogFormat|" << command << "|" << params << "|" << result << endl;
         return true;
    }
    catch(exception &e )
    {
        result =  e.what();
        LOG->error() << "reloadLogFormat|" << command << "|" << params << "|" << result << endl;
    }

    return false;
}

void LogServer::destroyApp()
{
    cout << "LogServer::destroyApp ok" << endl;
}

