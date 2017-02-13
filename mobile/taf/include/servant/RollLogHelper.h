#ifndef __ROLLLOGHELPER_H__
#define __ROLLLOGHELPER_H__

#include <string>
#include "util/tc_common.h"
#include "util/tc_logger.h"
#include "servant/Application.h"
#include "LogHelperBase.h"


#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    class RollLogStream
    {
        public:
            RollLogStream(int level,const std::string &str)
                :os(LOG->log(level))
            {
                os<<str;
            }
            template<typename T>
            RollLogStream& operator()(const T& value)
            {
                *this<<"|"<<value;
                return *this;
            }
            template <typename T>
            RollLogStream& operator<<( const T& t )
            {
                LogHelperBaseInner::writeToStream<T, false>( os, t );
                return *this;
            }
            typedef ostream& (*F)(ostream& os);
            RollLogStream& operator << (F f)
            {
                f(os);return *this;
            }
            operator ostream&()
            {
                return os;
            }
            RollLogStream(const RollLogStream&);
            RollLogStream& operator=( const RollLogStream& lt );
        protected:
            ostream& os;
    };
    class RollLogHelper
    {
        public:
            RollLogHelper(const std::string &str)
                :logPrefix(str)
            {}
            RollLogHelper(const char* str)
                :logPrefix(str)
            {}

            void setLogPrefix(const std::string &str)
            {
                logPrefix=str;
            }

            void setLogPrefix(const char *str)
            {
                logPrefix=str;
            }

            RollLogStream debug()
            {
                return RollLogStream(taf::TC_DayLogger::DEBUG_LOG,logPrefix);
            }
            RollLogStream error()
            {
                return RollLogStream(taf::TC_DayLogger::ERROR_LOG,logPrefix);
            }
            RollLogStream info()
            {
                return RollLogStream(taf::TC_DayLogger::INFO_LOG,logPrefix);
            }
        private:
            std::string logPrefix;
    };
}

#endif
