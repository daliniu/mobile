#ifndef __AUTOTIMELOG_H__
#define __AUTOTIMELOG_H__

#include "util/tc_timeprovider.h"
#include "util/tc_common.h"
#include "AutoLog.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    template<typename T> class AutoTimeLogObj:public AutoLogObj<T>
    {
        public:
            AutoTimeLogObj()
            {
                startTick=taf::TC_TimeProvider::getInstance()->getNowMs();
            }
            AutoTimeLogObj(const std::string &str)
                :AutoLogObj<T>(str)
            {
                startTick=taf::TC_TimeProvider::getInstance()->getNowMs();
            }
            AutoTimeLogObj(const AutoTimeLogObj& obj)
                :AutoLogObj<T>(obj)
            {
                startTick=obj.startTick;
                AutoTimeLogObj& tempObj=const_cast<AutoTimeLogObj&>(obj);
                costStr.swap(tempObj.costStr);
            }
            void addWatchPoint()
            {
                long now=taf::TC_TimeProvider::getInstance()->getNowMs();
                costStr=taf::TC_Common::tostr(now-startTick)+",";
            }
            ~AutoTimeLogObj()
            {
                if(AutoLogObj<T>::isVaild())
                {
                    long now=taf::TC_TimeProvider::getInstance()->getNowMs();
                    costStr="["+costStr+taf::TC_Common::tostr(now-startTick)+"ms]";

                    AutoLogObj<T>::prefix(costStr);
                }
            }

        private:
            long startTick;
            string costStr;
    };
}

#endif
