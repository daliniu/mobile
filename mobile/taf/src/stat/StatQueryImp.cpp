#include "StatQueryImp.h"
StatQueryHashMap g_slave_hashmap;
StatQueryHashMap g_master_hashmap;

int StatQueryImp::query(const StatQueryKey& tKey,bool bMaster,StatQueryValue &tValue, taf::JceCurrentPtr current )
{
    int iRet        = -1;
    time_t tBegin   = TC_Common::now2us();
    LOG->debug()<< "query---------------------------------access"<< endl;
    ostringstream os;
    tKey.display(os);
    LOG->debug()<<os.str()<< endl;
    if(bMaster == true)
    {
        time_t t = 0;
        iRet = g_master_hashmap.get(tKey,tValue,t);       
    }
    else
    {
        time_t t = 0;
        iRet = g_slave_hashmap.get(tKey,tValue,t);    
    }
    os.str("");
    tValue.display(os);
    time_t tEnd = TC_Common::now2us();
    LOG->debug() << "|" << tEnd - tBegin <<"|"<<os.str()<< endl;
    return iRet;
}

int StatQueryImp::queryDbInf(const string &sModule,Db &tDb, taf::JceCurrentPtr current )
{
    try
    {
       return StatDbManager::getInstance()->getDbRouter(sModule,tDb); 
    } 
    catch(exception &e)
    {
        LOG->debug() << "StatQueryImp::queryDbInf exception:"<< e.what() << endl;
    }
    return -1;
}

