#include "NotifyServer.h"
#include "NotifyImp.h"
#include "jmem/jmem_hashmap.h"

JceHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, FileStorePolicy> * g_notifyHash;

void NotifyServer::initialize()
{
    //增加对象
    addServant<NotifyImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".NotifyObj");

    //初始化hash
	extern TC_Config * g_pconf;
    g_notifyHash = new JceHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, FileStorePolicy>();

    g_notifyHash->initDataBlockSize(TC_Common::strto<size_t>((*g_pconf)["/taf/hash<min_block>"]),
            TC_Common::strto<size_t>((*g_pconf)["/taf/hash<max_block>"]),
            TC_Common::strto<float>((*g_pconf)["/taf/hash<factor>"]));

    string sHashFile = ServerConfig::DataPath + "/" + (*g_pconf)["/taf/hash/<file_path>"];
    size_t iSize     = TC_Common::toSize((*g_pconf)["/taf/hash<file_size>"], 1024*1024*10);

    g_notifyHash->initStore(sHashFile.c_str(), iSize);
}

void NotifyServer::destroyApp()
{
    cout << "NotifyServer::destroyApp ok" << endl;
}

