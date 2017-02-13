#ifndef __PROPERTY_IMP_H_
#define __PROPERTY_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "log/taf_logger.h"
#include "servant/PropertyF.h"
#include "PropertyDimenHashMap.h"

using namespace taf;

class PropertyDimenImp : public ReportPropertyF,public TC_ThreadLock
{
public:
 

    /**
     *
     */
    PropertyDimenImp()   { }

    /**
     * 析够函数
     */
    ~PropertyDimenImp()  { }

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize() { _tTime = 0 ;}

    /**
     * 退出
     */
    virtual void destroy() { }

    /**
    * 上报性属信息
    * @param statmsg, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int reportPropMsg(const map<ReportPropMsgHead,ReportPropMsgBody>& statmsg, taf::JceCurrentPtr current );

private:
    void dump2file();
       
private:
     time_t     _tTime; 
     string     _sDate;
     string     _sFlag; 
};

#endif


