#include "servant/AppCache.h"

namespace taf
{

//////////////////////////////////////////////////////////////////////
// 缓存
void AppCache::setCacheInfo(const string &sFile,int32_t iSynInterval)
{   
    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);            

        string sPath = TC_File::extractFilePath(sFile);    

        TC_File::makeDirRecursive(sPath);        

        _sFile = sFile;

        _iSynInterval = iSynInterval;
        
        if (TC_File::isFileExistEx(_sFile))
        {
            _tFileCache.parseFile(_sFile);                 
        } 

        //如果是旧版本数据（无版本号)直接清理
        if (_tFileCache.get(string(APPCACHE_ROOT_PATH)+"<tafversion>","") == "")
        {
            TC_Config tFileCache;  

            _tFileCache = tFileCache;
        }
    } 
    catch(exception &e)
    {
        LOG->error() <<"[TAF][AppCache setCacheInfo "<<e.what()<<"]"<<endl;
    }
}

string AppCache::get(const string & sName, const string sDomain)
{    
    if(_sFile.empty())
    {
        return "";
    }
      
    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        string sValue = _tFileCache.get(string(APPCACHE_ROOT_PATH) + "/" + sDomain + "<" + sName + ">"); 
        return sValue;
    } 
    catch(exception &e)
    {
        LOG->error() <<"[TAF][AppCache get"<<sName<<","<<e.what()<<"]"<<endl;
    }
    return "";
}

map<string, string> AppCache::getDomainMap(const string &path)
{
	map<string, string> m;

	if(_sFile.empty())
    {
        return m;
    }
      
    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);
        m = _tFileCache.getDomainMap(string(APPCACHE_ROOT_PATH) + "/" + path); 
    } 
    catch(exception &e)
    {
        LOG->error() <<"[TAF][AppCache getDomainMap"<<path<<","<<e.what()<<"]"<<endl;
    }
    return m;
}

int AppCache::set(const string &sName,const string &sValue,const string sDomain)
{   
    if(_sFile.empty())
    {
        return -1;
    }

    try
    {
        TC_LockT<TC_ThreadMutex> lock(*this);                   
        map<string, string> m;  
        m[sName] = sValue;
        TC_Config tConf;
        tConf.insertDomainParam(string(APPCACHE_ROOT_PATH)+"/"+sDomain,m,true);   
        if(_tLastSynTime == 0)   //第一次写数据 打印tafversion
        {   
            m.clear(); 
            m["tafversion"] = string(TAF_VERSION);
            tConf.insertDomainParam(string(APPCACHE_ROOT_PATH),m,true); 
        }           
        _tFileCache.joinConfig(tConf,true);
                 
        time_t now = TC_TimeProvider::getInstance()->getNow();
        if(_tLastSynTime + _iSynInterval/1000 > now)
        {
            return 0;
        }
        _tLastSynTime = now;      
        TC_File::save2file(_sFile,_tFileCache.tostr());
        return 0;
    } 
    catch(exception &e)
    {
        LOG->error() <<"[TAF][AppCache set name "<<sName<<",value"<<sValue<<","<<e.what()<<"]"<<endl;
    }

    return -1;
}

//////////////////////////////////////////////////////////////////////
}

