#include "NotifyImp.h"
#include "jmem/jmem_hashmap.h"

extern JceHashMap<NotifyKey, NotifyInfo, ThreadLockPolicy, FileStorePolicy> * g_notifyHash;

void NotifyImp::loadconf()
{
	extern TC_Config * g_pconf;

	TC_DBConf tcDBConf;
	tcDBConf.loadFromMap(g_pconf->getDomainMap("/taf/db"));
	_mysqlConfig.init(tcDBConf);

    uMaxPageSize = TC_Common::strto<size_t>((*g_pconf)["/taf/hash<max_page_size>"]);
    uMaxPageNum  = TC_Common::strto<size_t>((*g_pconf)["/taf/hash<max_page_num>"]);
}

void NotifyImp::initialize()
{
	loadconf();
}

void NotifyImp::reportServer(const string& sServerName,
									 const string& sThreadId,
									 const string& sResult,
									 taf::JceCurrentPtr current)
{
	LOG->debug() << "reportServer|" << sServerName << "|" << current->getIp() << "|" << sThreadId << "|" << sResult << endl;
	DLOG << "reportServer|" << sServerName << "|" << current->getIp() << "|" << sThreadId << "|" << sResult << endl;

    string sql;
    TC_Mysql::RECORD_DATA rd;
    rd["server_name"]   = make_pair(TC_Mysql::DB_STR, sServerName);
    rd["server_id"]     = make_pair(TC_Mysql::DB_STR, sServerName + "_" + current->getIp());
    rd["thread_id"]     = make_pair(TC_Mysql::DB_STR, sThreadId);
    rd["result"]        = make_pair(TC_Mysql::DB_STR, sResult);
    rd["notifytime"]    = make_pair(TC_Mysql::DB_INT, "now()");

    _mysqlConfig.insertRecord("t_server_notifys", rd);
}

void NotifyImp::notifyServer(const string& sServerName,
									 NOTIFYLEVEL level,
									 const string& sMessage,
									 taf::JceCurrentPtr current)
{
	LOG->debug() << "notifyServer|" << sServerName << "|" << current->getIp() << "|" << taf::etos(level) << "|" << sMessage << endl;
	DLOG << "notifyServer|" << sServerName << "|" << current->getIp() << "|" << taf::etos(level) << "|" << sMessage << endl;

    string sServerId = sServerName + "_" + current->getIp();

    NotifyKey stKey0;
    stKey0.name = sServerName;
    stKey0.ip   = current->getIp();
    stKey0.page = 0;

    NotifyInfo stInfo0;

    NotifyItem stItem;
    stItem.sTimeStamp   = TC_Common::now2str("%Y-%m-%d %H:%M:%S");
    stItem.sServerId    = sServerId;
    stItem.iLevel       = level;
    stItem.sMessage     = sMessage;

    int iRet;
    iRet = g_notifyHash->get(stKey0, stInfo0);
    LOG->debug() << "get " << sServerId << " page "<< stKey0.page << " info return :" << iRet << endl;
    if(iRet == TC_HashMap::RT_LOAD_DATA_ERR)
    {
        return;
    }

    if(stInfo0.notifyItems.size() < uMaxPageSize)
    {
        stInfo0.notifyItems.push_back(stItem);
        iRet = g_notifyHash->set(stKey0, stInfo0);
        LOG->debug() << "set " << sServerId << " page "<< stKey0.page << " info return :" << iRet << endl;
        return;
    }

    //0Ò³ÖÃ»»³öÈ¥
    NotifyKey stKeyReplPage = stKey0;
    stKeyReplPage.page = (stInfo0.nextpage + 1) % uMaxPageNum;
    if(stKeyReplPage.page == 0)
    {
        stKeyReplPage.page = 1;
    }
    iRet = g_notifyHash->set(stKeyReplPage, stInfo0);

    //ÐÞ¸Ä0Ò³
    stInfo0.nextpage = stKeyReplPage.page;
    stInfo0.notifyItems.clear();
    stInfo0.notifyItems.push_back(stItem);
    iRet = g_notifyHash->set(stKey0, stInfo0);

    return;
}


int NotifyImp::getNotifyInfo(const taf::NotifyKey & stKey,taf::NotifyInfo &stInfo,taf::JceCurrentPtr current)
{
    int iRet = g_notifyHash->get(stKey, stInfo);

    ostringstream os;
	stKey.displaySimple(os);
    os << "|";
	stInfo.displaySimple(os);

    LOG->debug()<<iRet<< "|" << os.str() << endl;

    return iRet;
}


