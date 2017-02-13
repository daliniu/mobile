#include "PlatformInfo.h"
#include "util/tc_clientsocket.h"
#include "NodeServer.h"

NodeInfo PlatformInfo::getNodeInfo() const
{
    NodeInfo tNodeInfo;
    tNodeInfo.nodeName      = getNodeName();
    tNodeInfo.dataDir       = getDataDir();
    TC_Endpoint tEndPoint   = g_app.getAdapterEndpoint("NodeAdapter");
    tNodeInfo.nodeObj       = ServerConfig::Application + "." + ServerConfig::ServerName + ".NodeObj@" + tEndPoint.toString();
    tNodeInfo.endpointIp    = tEndPoint.getHost();
    tNodeInfo.endpointPort  = tEndPoint.getPort();
    tNodeInfo.timeOut       = tEndPoint.getTimeout();
    tNodeInfo.version       = TAF_VERSION;
    return tNodeInfo;
}

LoadInfo PlatformInfo::getLoadInfo() const
{
    LoadInfo info;
    info.avg1   = -1.0f;
    info.avg5   = -1.0f;
    info.avg15  = -1.0f;
    double loadAvg[3];
    if ( getloadavg( loadAvg, 3 ) != -1 )
    {
        info.avg1   = static_cast<float>( loadAvg[0] );
        info.avg5   = static_cast<float>( loadAvg[1] );
        info.avg15  = static_cast<float>( loadAvg[2] );
    }
    return  info;
}

string PlatformInfo::getNodeName() const
{
    string name = "";
    try
    {
        name = (g_app.getConfig())["/taf/node<nodeid>"];
        if ( name == "" )
        {
            char host[1024 + 1];
            if ( gethostname( host, 1024 ) != -1 )
            {
                name = host;
            }
            if ( name.empty() )
            {
                LOG->debug() << "property `taf/node<nodeid>' is not set and hostname is empty" << endl;
            }
        }
    }
    catch ( exception & e )
    {
        LOG->error() << e.what() << endl;
        exit(-1);
    }
    return name;
}

string PlatformInfo::getDataDir() const
{
    string sDataDir;
    sDataDir = ServerConfig::DataPath;
    if ( TC_File::isAbsolute(sDataDir) == false)
    {
        char cwd[PATH_MAX];
        if ( getcwd( cwd, PATH_MAX ) == NULL )
        {
            LOG->error() << "cannot get the current directory:\n" << endl;
            exit( 0 );
        }
        sDataDir = string(cwd) + '/' + sDataDir;
    }
    sDataDir = TC_File::simplifyDirectory(sDataDir);
    if ( sDataDir[sDataDir.length() - 1] == '/' )
    {
        sDataDir = sDataDir.substr( 0, sDataDir.length() - 1 );
    }    
    return sDataDir;
}

string PlatformInfo::getDownLoadDir() const
{
    string sDownLoadDir = "";
    try
    {
        sDownLoadDir = g_app.getConfig().get("/taf/node<downloadpath>","");
        if(sDownLoadDir == "")
        {
            string sDataDir = getDataDir();   
            string::size_type pos =  sDataDir.find_last_of("/");
            if(pos != string::npos)
            {
                sDownLoadDir    = sDataDir.substr(0,pos)+"/tmp/download/";
            }
        }
        sDownLoadDir = TC_File::simplifyDirectory(sDownLoadDir);
        if(!TC_File::makeDirRecursive( sDownLoadDir ))
        {
            string sResult =  "property `taf/node<downloadpath>' is not set and cannot create dir: " + sDownLoadDir;
            LOG->error()<<"PlatformInfo::getDownLoadDir "<<sResult << endl;
            exit(-1);
        }
    }
    catch ( exception & e )
    {
        LOG->error() <<"PlatformInfo::getDownLoadDir "<< e.what() << endl;
        exit(-1);
    }
    return sDownLoadDir;
}


