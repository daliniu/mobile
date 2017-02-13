#include "PropertyImp.h"
#include "PropertyServer.h"

///////////////////////////////////////////////////////////
//

PropertyImp::PropertyImp()
{
}

void PropertyImp::initialize()
{

}

int PropertyImp::reportPropMsg(const map<StatPropMsgHead,StatPropMsgBody>& statmsg, taf::JceCurrentPtr current )
{
    LOG->info() << "report---------------------------------access size:" << statmsg.size() << endl;
    ostringstream os;
    for ( map<StatPropMsgHead,StatPropMsgBody>::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
    {

        const StatPropMsgHead &head = it->first;
        const StatPropMsgBody &body = it->second;
        dump2file();

        float rate =  (g_hashmap.getMapHead()._iUsedChunk) * 1.0/g_hashmap.allBlockChunkCount();

        if(rate >0.9)
        {
            g_hashmap.expand(g_hashmap.getMapHead()._iMemSize * 2);
            LOG->debug() << "PropertyImp::reportPropMsg hashmap expand to "<<g_hashmap.getMapHead()._iMemSize<<endl;
        }

        PropHead tHead;
        //tHead.time          = TC_TimeProvider::getInstance()->getNow();
        tHead.moduleName    = head.moduleName;
        tHead.propertyName  = head.propertyName;
        tHead.setName       = head.setName;
        tHead.setArea       = head.setArea;
        tHead.setID         = head.setID;
        tHead.ip            = current->getIp();

        os.str("");
        head.displaySimple(os);
        body.displaySimple(os);

        int iRet = g_hashmap.add(tHead,body);
        if(iRet != TC_HashMap::RT_OK )
        {
            LOG->error()<< "add hashmap erro iRet:"<<iRet<< endl;
        }
        LOG->info()<<"ret|"<<iRet<<"|"<<os.str()<< endl;
    }
    return 0;
}

void PropertyImp::dump2file()
{
    static time_t g_tTime = 0;
    static string g_sDate;
    static string g_sFlag;
    time_t tTimeNow     = TC_TimeProvider::getInstance()->getNow();
    time_t tTimeInterv  = PropertyServer::g_iInsertInterval*60;
    if(g_tTime == 0)
    {
        PropertyServer::getTimeInfo(g_tTime,g_sDate,g_sFlag);
    }
    if(tTimeNow  - g_tTime > tTimeInterv )  //以分钟为单位
    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );
        if(tTimeNow  - g_tTime > tTimeInterv )
        {
            PropertyServer::getTimeInfo(g_tTime,g_sDate,g_sFlag);
            string sFile = PropertyServer::g_sClonePatch+ "/"+g_sDate+g_sFlag+".txt";
            int iRet = g_hashmap.dump2file(sFile,true);
            if(iRet != 0)
            {
                TC_File::removeFile(sFile,false);
                LOG->error() << "PropertyImp::dump2file |" << sFile <<"|ret|"<<iRet<< endl;
                return;
            }
            LOG->debug() << "PropertyImp::dump2file |" << sFile << endl;
        }
    }
}





