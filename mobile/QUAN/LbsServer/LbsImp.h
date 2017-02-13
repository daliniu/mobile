#ifndef _LbsImp_H_
#define _LbsImp_H_

#include "servant/Application.h"
#include "Lbs.h"
#include "Project.h"
#include "jmem/jmem_hashmap.h"
#include "Base.h"


using  namespace QUAN;

typedef JceHashMap<CacheKey,CacheValue, ThreadLockPolicy, FileStorePolicy> LbsHashMap;



//lbs条件
struct LbsCond
{
    int x;              //x坐标
    int y;              //y坐标
    int hour;           //超时时间
    int farDist;        //最远
    int nearDist;       //最近
};

//lbscell
struct LbsCell
{
    UInt32 row;
    UInt16 col;
};



inline bool operator<(const LbsCell  &c1,const LbsCell &c2)
{
    if(c1.row == c2.row)
    {
        return c1.col <c2.col;
    }
    else 
    {
        return c1.row < c2.row;
    }
};


/**
 *
 *
 */
class LbsImp : public QUAN::Lbs
{
public:
    /**
     *
     */
    virtual ~LbsImp() {}

    /**
     *
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroy();

    virtual taf::Int32 getNearbyUser(const LbsReq &tReq, LbsRsp &tRsp,taf::JceCurrentPtr current);

    virtual taf::Int32 updateLbsMap(const LbsReq &tReq,taf::JceCurrentPtr current);
    /**
     *
     */
    virtual int test(taf::JceCurrentPtr current) { return 0;};

private:
    int updateLbsMap(const LbsReq &tReq,const LbsCond &tLbsCond);

    int getLbsCond(const LbsReq &tReq,LbsCond & tLbsCond);

    void getLbsCell(const LbsCond & tLbsCond, set<LbsCell> &vLbsCell);

    void getLbsUser(const LbsCond & tLbsCond,const LbsCell &tLbsCell,vector<LbsUser> &vLbsUser);

    
    static void xy2grid(const Int32 x, const Int32 y, UInt32& row, UInt16& col, UInt16 & idx);

    static void grid2xy(const UInt32 row, const UInt16 col, const UInt16 idx,  Int32& x, Int32& y);


private:

    CProject        _tCProject;

    int _iFarDist;                  //最远
    int _iNearDist;                 //最近
    int _iMaxUserNum;               //最多返回用户数
    int _iHour;                     //失效时间

};
/////////////////////////////////////////////////////
#endif
