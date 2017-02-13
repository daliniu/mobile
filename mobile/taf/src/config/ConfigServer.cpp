#include "ConfigServer.h"
#include "ConfigImp.h"

void ConfigServer::initialize()
{
    //增加对象
    addServant<ConfigImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".ConfigObj");
}

void ConfigServer::destroyApp()
{
}

