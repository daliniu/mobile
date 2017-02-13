#include "PatchServer.h"
#include "PatchImp.h"
#include "PatchCache.h"

extern TC_Config *g_conf;

PatchCache g_PatchCache;

void PatchServer::initialize()
{
    //增加对象
    addServant<PatchImp>(ServerConfig::Application + "." + ServerConfig::ServerName +".PatchObj");
    
    size_t MemMax   = TC_Common::toSize(g_conf->get("/taf<MemMax>", "40M"), 1024*1024);
    size_t MemMin   = TC_Common::toSize(g_conf->get("/taf<MemMin>", "5M"), 1024*1024);
    size_t MemNum   = TC_Common::strto<size_t>(g_conf->get("/taf<MemNum>", "10"));
    g_PatchCache.setMemOption(MemMax, MemMin, MemNum);
    LOG->debug() << __FUNCTION__ << "|MAX:" << MemMax << "|MIN:" << MemMin << "|NUM:" << MemNum << endl;
}

void PatchServer::destroyApp()
{
    cout << "PatchServer::destroyApp ok" << endl;
}

