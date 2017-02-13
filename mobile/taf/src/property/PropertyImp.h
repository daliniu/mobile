#ifndef __PROPERTY_IMP_H_
#define __PROPERTY_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "log/taf_logger.h"
#include "servant/PropertyF.h"
#include "PropertyHashMap.h"

using namespace taf;

class PropertyImp : public PropertyF,public TC_ThreadLock
{
public:


    /**
     *
     */
    PropertyImp();

    /**
     * 析够函数
     */
    ~PropertyImp()  { }

    /**
     * 初始化
     *
     * @return int
     */
    virtual void initialize();

    /**
     * 退出
     */
    virtual void destroy() { }

    /**
    * 上报性属信息
    * @param statmsg, 上报信息
    * @return int, 返回0表示成功
    */
    virtual int reportPropMsg(const map<StatPropMsgHead,StatPropMsgBody>& statmsg, taf::JceCurrentPtr current );

private:
    void dump2file();

};

#endif


