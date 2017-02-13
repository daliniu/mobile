#include "LbsImp.h"
#include "servant/Application.h"
#include "CommHashMap.h"
#include "util/tc_encoder.h"
#include <math.h>

using namespace std;


LbsHashMap g_lbs_hashmap;


bool LbsUserLess(const LbsUser  &t1,const LbsUser &t2)
{
    if(t1.iDistance < t2.iDistance)
    {    
        return true;
    } 
    if(t1.iDistance == t2.iDistance)
    { 
        return t1.iTime > t2.iTime;  //时间近的拍前
    }
    return false;
};


//////////////////////////////////////////////////////
void LbsImp::initialize()
{
    //initialize servant here:
    //...

    __TRY__

    TC_Config tConf;
    __TRY__

    tConf.parseFile(ServerConfig::BasePath + "LbsServer.conf");
    __CATCH__

    _iFarDist   = TC_Common::strto<int>(tConf.get("/main/lbs<far>","6000"));       //最远  6公里
    _iNearDist  = TC_Common::strto<int>(tConf.get("/main/lbs<near>","0"));         //最近
    _iHour      = TC_Common::strto<int>(tConf.get("/main/lbs<hour>","4"));         //失效小时

    _iMaxUserNum    = TC_Common::strto<int>(tConf.get("/main/lbs<max_user_num>","50"));         //最低返回用户数

    __CATCH__
}

//////////////////////////////////////////////////////
void LbsImp::destroy()
{
    //destroy servant here:
    //...
}



taf::Int32 LbsImp::getNearbyUser(const LbsReq &tReq, LbsRsp &tRsp,taf::JceCurrentPtr current)
{
    int iRet = -1;

    string sLog = string(__FUNCTION__) +"|"+JCETOSTR(tReq);

    LOG->debug()<<sLog<< "|---------------------------------access"<< endl;

    __TRY__

    LbsCond tLbsCond;

    PROC_BEGIN

    //获取lbs限制条件
    iRet = getLbsCond(tReq,tLbsCond);

    if (iRet != 0)
    {
        break;
    }

    set<LbsCell> vLbsCell;
    vector<LbsUser> vLbsUser;

    int iInterval   = 10000; 

    int iMaxUserNum = _iMaxUserNum*1.2;

    //根据lbs限制条件获取用户cell,最多1000公里
    for (unsigned i=0; i< 1000; i++)
    {
        vLbsCell.clear();

        tLbsCond.nearDist   = iInterval*i;
        tLbsCond.farDist    = iInterval*(i+1);

        getLbsCell(tLbsCond, vLbsCell);

        LOG->debug()<<__FUNCTION__<<" get LbsCell size :"<<vLbsCell.size()<<"|"<<tReq.lId <<endl;

        //从用户cell获取用户
        for ( set<LbsCell>::iterator it = vLbsCell.begin() ; it != vLbsCell.end(); it++ )
        {

            getLbsUser(tLbsCond,*it,vLbsUser);
        }

        if (vLbsUser.size()> (unsigned)iMaxUserNum )
        {
            LOG->debug()<<__FUNCTION__<<" get user size :"<<vLbsUser.size()<<">"<< iMaxUserNum <<endl;
            break;
        }
        if (tLbsCond.farDist > _iFarDist)
        {
            LOG->debug()<<__FUNCTION__<<" LbsCond too far :"<<tLbsCond.farDist<<">"<< _iFarDist <<endl;
            break;
        }
    }

    LOG->debug()<<__FUNCTION__<<" get LbsUser size :"<<vLbsUser.size()<<"|"<<tReq.lId<<endl;

    sort(vLbsUser.begin(),vLbsUser.end(),LbsUserLess);

    vector<long>   vTempId;
    map<long,int>  mTempId;

    for (unsigned i=0; i< vLbsUser.size();i++)
    {
        LOG->debug()<<__FUNCTION__<<"LbsUser | "<<i<<"| "<<vLbsUser[i].lId <<"|"<<vLbsUser[i].iDistance <<"|"<<vLbsUser[i].iTime<<endl;

        if (mTempId[vLbsUser[i].lId] == 0) //借助map扣除重复元素，并保持vector中序不变
        {
            if (vTempId.size() <= (unsigned)iMaxUserNum)  //不要超过最大返回数
            {
                mTempId[vLbsUser[i].lId] = 1;
                tRsp.vId.push_back(vLbsUser[i]);
            }
        }
    }


    iRet = 0;


    PROC_END   


    __CATCH__

    ostringstream os;
    os<< iRet<<"|" <<sLog<<"|"<<JCETOSTR(tRsp)<< endl;
    if (iRet !=  0)
    {
        LOG->error()<<os.str(); 
    } else
    {
        LOG->debug()<<os.str();
    }
    FDLOG() <<os.str();


    return  iRet;

}



taf::Int32 LbsImp::updateLbsMap(const LbsReq &tReq,taf::JceCurrentPtr current)
{
    int iRet = -1;

    string sLog = string(__FUNCTION__) +"|"+JCETOSTR(tReq);

    LOG->debug()<<sLog<< "|---------------------------------access"<< endl;

    __TRY__


    LbsCond tLbsCond;

    PROC_BEGIN

    //非法数据
    if (tReq.lId <=0  || (tReq.dLat== 0 && tReq.dLng == 0))
    {
        break;
    }

    //获取lbs限制条件

    iRet = getLbsCond(tReq,tLbsCond);

    if (iRet != 0)
    {
        break;
    }
    updateLbsMap(tReq,tLbsCond);

    PROC_END   

    __CATCH__

    ostringstream os;
    os<< iRet<<"|" <<sLog<< endl;
    if (iRet < 0)
    {
        LOG->error()<<os.str(); 
    } else
    {
        LOG->debug()<<os.str(); 
    }
    FDLOG() <<os.str();
    return iRet;
}


int LbsImp::getLbsCond(const LbsReq &tReq,LbsCond & tLbsCond)
{
    if (tReq.dLng ==0  &&  tReq.dLat == 0)
    {
        LOG->error()<<__FUNCTION__<<" get getLbsCond lbs info  invalid  (Lng  :"<<tReq.dLng <<" lat:"<< tReq.dLat<<")" <<endl;
        return  -1;
    }

    tLbsCond.farDist        = _iFarDist;
    tLbsCond.nearDist       = _iNearDist;
    tLbsCond.hour           = _iHour;

    _tCProject.Gauss_BL2xy(tReq.dLat,tReq.dLng,tLbsCond.x,tLbsCond.y);

    return  0;
}


//取出离用户x,y距离[nearDist,farDist]范围内Cell

void LbsImp::getLbsCell(const LbsCond & tLbsCond, set<LbsCell> &vLbsCell)
{
    int x           = tLbsCond.x; 
    int y           = tLbsCond.y; 

    int iFarDist    = tLbsCond.farDist;
    int iNearDist   = tLbsCond.nearDist;

    int iInterval   = 2000; //格子步长为2公里

    for (int tmpx = x - iFarDist; tmpx <= x + iFarDist; tmpx += iInterval )
    {
        for (int tmpy = y - iFarDist; tmpy <= y + iFarDist; tmpy += iInterval )
        {

            if (tmpx > x- iNearDist && tmpx < x+ iNearDist &&  tmpy > y- iNearDist && tmpy <  y + iNearDist)
            {
                continue;
            }
            UInt32 row;
            UInt16 idx;
            UInt16 col;
            xy2grid(tmpx, tmpy, row, col, idx);
            LbsCell tLbsCell;

            tLbsCell.row    = row;
            tLbsCell.col    = col;
            vLbsCell.insert(tLbsCell);          
        }
    }


}

//根据限制条件tLbsCond，获取cell中的用户
void LbsImp::getLbsUser(const LbsCond & tLbsCond,const LbsCell &tLbsCell,vector<LbsUser> &vLbsUser)
{
    taf::Int64 x        = tLbsCond.x; 
    taf::Int64 y        = tLbsCond.y; 
    int iHour           = tLbsCond.hour;

    CacheKey      tKey;
    CacheValue    tValue;
    tKey.row    = tLbsCell.row;
    tKey.col    = tLbsCell.col;

    if (CommHashMap::getInstance()->getHashMap(g_lbs_hashmap,tKey,tValue) != 0)
    {
        return;
    }

    int iNow = TC_TimeProvider::getInstance()->getNow();
    for (map<Int64,LbsInfo>::const_iterator it= tValue.mLbsUser.begin();it != tValue.mLbsUser.end();it++)
    {
        LbsUser  tLbsUser;

        //非法用户
        if (it->first <= 0  || (it->second.x == 0 && it->second.y == 0))
        {
            continue;
        }
        //是否已超时
        if ( iNow - it->second.time > 3600*iHour  )
        {
            continue;
        }

        tLbsUser.lId        = it->first;
        tLbsUser.iTime     = it->second.time;
        tLbsUser.iDistance =  (x- it->second.x) > abs(y- it->second.y)?abs(x- it->second.x) : abs(y- it->second.y);           
        tLbsUser.iDistance  = (int(tLbsUser.iDistance*100/1000))*1.0/100;
        vLbsUser.push_back(tLbsUser);
    }  
}

//void LbsImp::updateLbsMap(taf::Int64 yyuid,const LbsCond &tLbsCond)
int LbsImp::updateLbsMap(const LbsReq &tReq,const LbsCond &tLbsCond)
{
    int  iRet =  -1;

    //非法数据
    if (tReq.lId <=0)
    {
        return -1;
    }

    int x       = tLbsCond.x; 
    int y       = tLbsCond.y; 

    CacheKey      tKey;
    CacheValue    tValue;

    UInt32 row;
    UInt16 idx;
    UInt16 col;
    xy2grid(x, y, row, col, idx);

    tKey.row    = row;
    tKey.col    = col;

    LbsInfo     tLbsInfo;
    tLbsInfo.idx        = idx;
    tLbsInfo.x          = x;
    tLbsInfo.y          = y;

    time_t tTimeNow  = TC_TimeProvider::getInstance()->getNow();
    tLbsInfo.time   = tTimeNow;


    {
        static  TC_ThreadLock g_mutex;
        TC_ThreadLock::Lock  lock( g_mutex );

        iRet = CommHashMap::getInstance()->getHashMap(g_lbs_hashmap,tKey,tValue);
        if (iRet == 0)
        {
            tValue.mLbsUser[tReq.lId] = tLbsInfo; //增加用户lbs信息


            if (tTimeNow -  tValue.iLastCleanTime >  600)
            {
                map<Int64,LbsInfo>::iterator it= tValue.mLbsUser.begin();
                while ( it != tValue.mLbsUser.end() )
                {
                    //是否已超时
                    if ( tTimeNow - it->second.time > 3600*tLbsCond.hour  || it->first <= 0)
                    {
                        tValue.mLbsUser.erase(it++);
                        LOG->debug()<<__FUNCTION__<<"| erase time out lbs yyuid|"<<it->first
                        <<"|time|"<<TC_Common::strto<long long>(TC_Common::tm2str(it->second.time))
                        <<"|x|"<< it->second.x 
                        <<"|y|"<< it->second.y<<endl; 

                        continue;
                    } else
                    {
                        it++;
                    }
                }
                tValue.iLastCleanTime = tTimeNow;
            }

            iRet = CommHashMap::getInstance()->addHashMap(g_lbs_hashmap,tKey,tValue);
        }
    }



    LOG->debug()<<__FUNCTION__<<"|ret|"<<iRet<<"|yyuid|"<< tReq.lId <<"|x|"<<tLbsInfo.x
    <<"|y|"<<tLbsInfo.y <<"|time|"<< tLbsInfo.time<<endl;

    return iRet;

}



void LbsImp::xy2grid(const Int32 x, const Int32 y, UInt32& row, UInt16& col, UInt16 & idx)
{
    //以10X10公里为一行.每行以key为代表
    row = x/10000;
    row = (row << 16) | (y/10000);

    //每列2x2公里范围，所以每行有25列
    col = ((x%10000)/2000)*5 + (y%10000)/2000;

    //每个uin增加一个附加信息下标。精确到100米
    idx = ( ( ((x%10000)%2000)/100 )<< 8) + ((y%10000)%2000/100);
}

void LbsImp::grid2xy(const UInt32 row, const UInt16 col, const UInt16 idx,  Int32& x, Int32& y)
{
    x = (row >> 16)*10000  + (col/5)*2000 + (idx>>8)*100;
    y = (row & 0xFFFF)*10000 + (col%5)*2000 + (idx&0xFF)*100;
    //cout << "row = " << row << " x = " << x << " y= " << y << endl;
}
