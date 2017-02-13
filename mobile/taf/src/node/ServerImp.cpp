#include "ServerImp.h"

int ServerImp::keepAlive( const taf::ServerInfo& serverInfo, taf::JceCurrentPtr current )
{
    try
    {   
        string sApp     = serverInfo.application;
        string sName    = serverInfo.serverName;

        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( sApp, sName );
        if ( pServerObjectPtr )
        {
            pServerObjectPtr->keepAlive(serverInfo.pid,serverInfo.adapter);
            return 0;
        }
        LOG->debug() << "server " << serverInfo.application << "." << serverInfo.serverName << " is not exist"<< endl;
    }
    catch ( exception& e )
    {
        LOG->debug() << "catch exception :" << e.what() << endl;
    }
    catch ( ... )
    {
        LOG->debug() << "unkown exception catched" << endl;
    }
    return -1;
}

int ServerImp::reportVersion( const string &app,const string &serverName,const string &version,taf::JceCurrentPtr current)
{
    try
    {
        LOG->debug() << "reportVersion|server|" << app << "." << serverName << "|version|" << version<< endl;
        ServerObjectPtr pServerObjectPtr = ServerFactory::getInstance()->getServer( app, serverName );
        if ( pServerObjectPtr )
        {
            pServerObjectPtr->setVersion( version );
            return 0;
        }
        LOG->debug() << "server " << app << "." << serverName << " is not exist"<< endl;
    }
    catch ( exception& e )
    {
        LOG->debug() << "catch exception :" << e.what() << endl;
    }
    catch ( ... )
    {
        LOG->debug() << "unkown exception catched" << endl;
    }
    return -1;
}


