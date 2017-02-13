#ifndef __TAFCALLBACKS_H__
#define __TAFCALLBACKS_H__

#include <exception>
#include <tuple>

#include "util/tc_common.h"
#include "util/tc_logger.h"
#include "servant/Application.h"
#include "util/tc_autoptr.h"
#include "jce/JceType.h"

#include "AutoTimeLog.h"
#include "AutoRef.h"
#include "FutureCall.h"
#include "TupleExpand.h"
#include "TupleAssign.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
#ifndef DISABLE_CONDITION_CALLBACK
    template <typename Callback>
    class ConditionExpr
    {
    public:
        typedef typename Callback::RetType RetType;
        typedef Callback SelfCallback;
        typedef typename Callback::ConditionCallback SelfConditionCallback;
        typedef std::function<bool( RetType )> Condition;
        ConditionExpr( Callback& cb, Condition cond )
            : cb( cb )
            , cond( cond )
        {
        }

    public:
        Callback& cb;
        Condition cond;
    };

    template <typename... Args> // avoid GCC BUG
    class TafCallback;

    template <typename... Args> // avoid GCC BUG
    class TafConditionCallback;

    template <typename retType, typename... Args>
    class TafConditionCallback<retType, Args...>
    {
    protected:
        typedef TafCallback<retType, Args...> RawCallback;

    public:
        friend class TafCallback<retType, Args...>;
        typedef retType RetType;

        TafConditionCallback( const ConditionExpr<RawCallback>& condexpr )
            : condexpr( condexpr )
        {
        }
        TafConditionCallback* operator->()
        {
            return this;
        }
        TafConditionCallback& operator+=( const std::function<void( retType, Args... param )>& callback )
        {
            auto& condFn = condexpr.cond;
            condexpr.cb += [condFn, callback]( retType ret, Args... param )
            {
                if ( condFn( ret ) )
                {
                    callback( ret, param... );
                }
            };
            return *this;
        }
        template <typename... HoldArgs>
        TafConditionCallback& hold( HoldArgs... HoldParams )
        {
            auto paramPack = make_tuple( HoldParams... );
            condexpr.cb += [paramPack]( retType, Args... param )
            {};
            return *this;
        }
        template <typename... BindArgs>
        TafConditionCallback& bind( BindArgs&... BindParams )
        {
            auto& condFn = condexpr.cond;
            auto paramPack = std::tie( BindParams... );
            condexpr.cb += [condFn, paramPack]( retType ret, Args... param ) mutable
            {
                if ( condFn( ret ) )
                {
                    tuple_assign(paramPack,make_tuple( ret, param... ));
                }
            };
            return *this;
        }
        template <typename... BindArgs>
        TafConditionCallback& bind( std::tuple<BindArgs&...>&& BindParams )
        {
            auto fn = [this]( BindArgs&... param )
            { this->bind(  param... ); };
            TupleExpand( fn, BindParams );
            return *this;
        }

    protected:
        ConditionExpr<RawCallback> condexpr;
    };
#endif

    //回调类,模板参数为外部调用的返回值+参数,
    template <typename retType, typename... Args>
    class TafCallback<retType, Args...> : public FutureCall<retType, Args...>
    {
    public:
        typedef FutureCall<retType, Args...> FutureCallType;
        FutureCall<taf::Int32>& errorHandler()
        {
            return mErrorHandler;
        }
        TafCallback& operator+=( const std::function<void( retType, Args... )>& callback )
        {
            FutureCallType::operator+=( callback );
            return *this;
        }
        template<typename Policy>
        void onExceptionProtect(Args... param)
        {
            //##todo
        }
#ifndef DISABLE_CONDITION_CALLBACK
    public:
        typedef TafConditionCallback<retType, Args...> ConditionCallback;
        typedef retType RetType;
        typedef ConditionExpr<TafCallback> SelfConditionExpr;
        SelfConditionExpr operator<=( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret <= cond; } ) );
        }
        SelfConditionExpr operator<( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret < cond; } ) );
        }
        SelfConditionExpr operator>=( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret >= cond; } ) );
        }
        SelfConditionExpr operator>( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret > cond; } ) );
        }
        SelfConditionExpr operator==( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret == cond; } ) );
        }
        SelfConditionExpr operator!=( retType cond )
        {
            return SelfConditionExpr( ( *this ), std::move( [cond]( retType ret )
            { return ret != cond; } ) );
        }
#endif
    protected:
        void doCalls( retType ret, Args... param )
        {
            mErrorHandler.cancel();
            FutureCallType::doCalls( ret, param... );
        }
        template <typename... noUseArgs>
        void doCalls( noUseArgs... ) // avoid other arglist compile error
        {
            throw std::runtime_error( "Unexpected Call !!!" );
        }
        void handleException( taf::Int32 ret )
        {
            FutureCallType::cancel();
            if ( mErrorHandler.isCallbackEmtpy() )
            {
                LOG->error() << "[taf] exception in async call, ret:" + taf::TC_Common::tostr( ret ) << endl;
            }
            else
            {
                mErrorHandler.doCalls( ret );
            }
        }

    private:
        FutureCall<taf::Int32> mErrorHandler;
    };

#ifndef DISABLE_CONDITION_CALLBACK
    template <typename T>
    inline typename T::SelfConditionCallback when( T&& t )
    {
        return typename T::SelfConditionCallback( t );
    }
    template <typename Callback, typename Condition>
    inline typename std::remove_reference<Callback>::type::RefType::ConditionCallback when( Callback&& cb, Condition&& cond )
    {
        typedef typename std::remove_reference<Callback>::type::RefType RealCallback;
        return typename RealCallback::ConditionCallback( typename RealCallback::SelfConditionExpr( std::forward<Callback>( cb ), std::forward<Condition>( cond ) ) );
    }
#endif

    template <typename T>
    class TafStorage : public taf::TC_HandleBase
    {
    public:
        typedef T storageType;
        storageType& getStorage()
        {
            return mStorage;
        }
        static taf::TC_AutoPtr<TafStorage> create()
        {
            return new TafStorage;
        }

    private:
        storageType mStorage;
    };
}



#endif
