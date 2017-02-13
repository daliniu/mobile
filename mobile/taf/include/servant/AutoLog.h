#ifndef __AUTO_LOG_H_
#define __AUTO_LOG_H_

#include <string>
#include <sstream>
#include <stdexcept>
#include "LogHelperBase.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    class FakeLogObj
    {
    public:
        FakeLogObj()
        {
        }

    protected:
        FakeLogObj( const std::string& str )
        {
        }

    protected:
        template <typename T>
        FakeLogObj& operator<<( const T& t )
        {
            return *this;
        }
        template <typename T>
        FakeLogObj& prefix( const T& t )
        {
            return *this;
        }
        template <typename T>
        friend FakeLogObj& operator<<( const T& t, FakeLogObj& obj );
    };

    template <typename T>
    inline FakeLogObj& operator<<( const T& t, FakeLogObj& obj )
    {
        return obj.prefix( t );
    }

    template <typename LogFuncObj>
    class AutoLogObj
    {
    public:
        AutoLogObj()
        {
            bIsVaild = true;
        }
        AutoLogObj( const AutoLogObj& obj )
        {
            AutoLogObj& tempObj = const_cast<AutoLogObj&>( obj );
            bIsVaild = true;
            innerLogStr.swap( tempObj.innerLogStr );
            tempObj.bIsVaild = false;
        }
        AutoLogObj( const std::string& str )
        {
            bIsVaild = true;
            innerLogStr = str;
        }
        ~AutoLogObj()
        {
            if ( bIsVaild )
            {
                LogFuncObj obj;
                obj( innerLogStr );
            }
        }
        template <typename T>
        AutoLogObj& operator<<(const T& t )
        {
            return append(t);
        }
        template <typename T>
        AutoLogObj& prefix( const T& t )
        {
            CheckSelf();
            std::stringstream ss;
            LogHelperBaseInner::writeToStream<T,false>(ss,t);
            innerLogStr = ss.str() + innerLogStr;
            return *this;
        }
        template <typename T>
        AutoLogObj& append(const T& t )
        {
            CheckSelf();
            std::stringstream ss;
            LogHelperBaseInner::writeToStream<T,false>(ss,t);
            innerLogStr.append( ss.str() );
            return *this;
        }
        template <typename T>
        AutoLogObj& operator()(const T& t )
        {
            CheckSelf();
            std::stringstream ss;
            LogHelperBaseInner::writeToStream<T,false>(ss,t);
            if(!innerLogStr.empty())
            {
                innerLogStr.append("|");
            }
            innerLogStr.append( ss.str() );
            return *this;
        }

        std::string& operator()()
        {
            return innerLogStr;
        }
        const std::string& operator()() const
        {
            return innerLogStr;
        }

        void clear()
        {
            innerLogStr.clear();
            bIsVaild = true;
        }
        bool isVaild()
        {
            return bIsVaild;
        }

    private:
        void CheckSelf() const
        {
            if ( !bIsVaild )
            {
                throw std::runtime_error( "Access Invaild LogObj" );
            }
        }


        bool bIsVaild;
        std::string innerLogStr;
    };

    template <typename T, typename LogFuncObj>
    inline AutoLogObj<LogFuncObj>& operator<<( const T& t, AutoLogObj<LogFuncObj>& obj )
    {
        return obj.prefix( t );
    }
}
#endif
