#include "PropertyDimenImp.h"
#include "PropertyDimenServer.h"

///////////////////////////////////////////////////////////
//

int PropertyDimenImp::reportPropMsg(const map<ReportPropMsgHead,ReportPropMsgBody>& statmsg, taf::JceCurrentPtr current )
{
    LOG->info() << "report---------------------------------access size:" << statmsg.size() << endl;
    ostringstream os;
    for ( map<ReportPropMsgHead,ReportPropMsgBody>::const_iterator it = statmsg.begin(); it != statmsg.end(); it++ )
    {
        
        const ReportPropMsgHead &head = it->first;
        const ReportPropMsgBody &body = it->second;      
        dump2file(); 

        float rate =  (g_hashmap.getMapHead()._iUsedChunk) * 1.0/g_hashmap.allBlockChunkCount();

        if(rate >0.9)
        {
            g_hashmap.expand(g_hashmap.getMapHead()._iMemSize * 2);
            LOG->debug() << "PropertyDimenImp::reportPropMsg hashmap expand to "<<g_hashmap.getMapHead()._iMemSize<<endl;
        }

        PropDimenHead tHead;
        //tHead.time          = TC_TimeProvider::getInstance()->getNow(); 
        tHead.moduleName    = head.moduleName;
        tHead.propertyName  = head.propertyName;
        tHead.ip            = current->getIp();  
		
        tHead.dimension1    = head.dimension1;
        tHead.dimension2    = head.dimension2;
        tHead.dimension3    = head.dimension3;
        tHead.dimension4    = head.dimension4;
        tHead.dimension5    = head.dimension5;
        tHead.dimension6    = head.dimension6;
        tHead.dimension7    = head.dimension7;
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

void PropertyDimenImp::dump2file()
{
    static time_t g_tTime = 0;
    static string g_sDate;
    static string g_sFlag; 
    time_t tTimeNow     = TC_TimeProvider::getInstance()->getNow();         
    time_t tTimeInterv  = PropertyDimenServer::g_iInsertInterval*60;
    if(g_tTime == 0)
    {
        PropertyDimenServer::getTimeInfo(g_tTime,g_sDate,g_sFlag);
    }
    if(tTimeNow  - g_tTime > tTimeInterv )  //以分钟为单位
    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );
        if(tTimeNow  - g_tTime > tTimeInterv )
        {
            PropertyDimenServer::getTimeInfo(g_tTime,g_sDate,g_sFlag);   
            string sFile = PropertyDimenServer::g_sClonePatch+ "/"+g_sDate+g_sFlag+".txt";
            int iRet = g_hashmap.dump2file(sFile,true);
            if(iRet != 0)
            {
                TC_File::removeFile(sFile,false);
                LOG->error() << "PropertyDimenImp::dump2file |" << sFile <<"|ret|"<<iRet<< endl;
                return;
            }
            LOG->debug() << "PropertyDimenImp::dump2file |" << sFile << endl;
        }
    }  
}





