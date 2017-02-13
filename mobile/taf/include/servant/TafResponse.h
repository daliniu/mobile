#ifndef __TAFRESPONSE_H__
#define __TAFRESPONSE_H__

#include "AutoTimeLog.h"
#include "AutoRef.h"
#include "FutureCall.h"
#include "servant/Application.h"
#include "RollLogHelper.h"
#include "TupleAssign.h"
#include "TupleExpand.h"
#include "LockedSingleton.h"

// auto resp=TafResponse::create(JceCurrent,async_response_func);
// auto resp=TafLogResponse::create(JceCurrent,async_response_func);
// auto resp=TafTimeResponse::create(JceCurrent,async_response_func);
// 
// auto resp=TafResponse::create<storage>(JceCurrent,async_response_func);
// auto resp=TafLogResponse::create<storage>(JceCurrent,async_response_func);
// auto resp=TafTimeResponse::create<storage>(JceCurrent,async_response_func);
// resp->getStorage().value;
// resp->getSharedStorage()->value;
//
// resp->instantReturn(ret, param...)  用指定返回值和参数回包,可用 _ 占位
//
// resp->bind(ret,param...)            绑定变量，在回包时取值，可用 _ 占位,注意被绑定对象的生存期、线程安全性
//
// resp->set(ret,param...)             设置自动返回值和参数,可用 _ 占位
//
// resp->get(ret,param...)             获取自动返回值和参数,可用 _ 占位
//
// resp->get<0>()  /  get<1>()  ...    获取第几个参数的引用,get<0>()就是取得返回值引用
//
// resp->getTuple()                    获取包含返回值和参数的引用的Tuple包,
// resp->getTuple<0,1...>()            通过索引值，获取包含部分返回值和参数的引用的Tuple包,注意输出的tuple的索引号是重新从0开始的
// resp->getTuple<1,0...>()            索引值不一定要按原始顺序，
// resp->getTuple<_,0...>()            索引值可以用 _ 占位，
//
// 值优先级,但不推荐对同一参数多次赋值、绑定，会产生多次复制,用占位符不会产生复制
// instantReturn > bind > set/引用直接赋值

namespace tafAsync
{
    namespace TafResponse
    {
        template<typename T,typename...Args>
        class TafResponsePrefixAdapter
        {
        protected:
            T& t;
        public:
            TafResponsePrefixAdapter( T& t )
                : t( t )
            {
            }
            TafResponsePrefixAdapter& operator+=( const std::function<void( taf::Int32, Args... )>& callback )
            {
                taf::TC_ThreadRecLock::Lock lock( t.mlock );
                t.callbacks.push_front( callback );
                return *this;
            }
        };
        template<typename T,typename...Args>
        class TafResponseSuffixAdapter
        {
        protected:
            T& t;
        public:
            TafResponseSuffixAdapter( T& t )
                : t( t )
            {
            }
            TafResponseSuffixAdapter& operator+=( const std::function<void( taf::Int32, Args... )>& callback )
            {
                taf::TC_ThreadRecLock::Lock lock( t.mlock );
                t.callbacks.push_back( callback );
                return *this;
            }
        };
        template<typename T,typename C>
        class TafResponseCallbackAdapter
        {
        protected:
            T& t;
            C& c;
        public:
            TafResponseCallbackAdapter( T& t, C& c )
                : t( t )
                , c( c )
            {
            }
            TafResponseCallbackAdapter& operator+=(const typename C::element_type::FutureCallType::callType& callback)
            {
            }
        };

        template <typename... Args>
        class TafResponseImp : public taf::TC_HandleBase, public FutureCall<taf::Int32, Args...>
        {
        public:
            friend class TafResponsePrefixAdapter<TafResponseImp,Args...>;
            friend class TafResponseSuffixAdapter<TafResponseImp,Args...>;
            typedef void FutureCallFuncType( JceCurrentPtr, taf::Int32, Args... );
            typedef FutureCall<taf::Int32, Args...> FutureCallType;

        protected:
            TafResponseImp( JceCurrentPtr current, FutureCallFuncType* responseFunc )
                : mIsReturned( false )
            {
                current->setResponse( false );
                *this += [responseFunc, current]( taf::Int32 ret, Args... param )
                { responseFunc( current, ret, param... ); };
                this->set( -1 );
            }
            FutureCall<> mBindCall;
            bool mIsReturned;
        public:
            TafResponsePrefixAdapter<TafResponseImp,Args...> prefixHandler()
            {
                return TafResponsePrefixAdapter<TafResponseImp,Args...>(*this);
            }
            TafResponseSuffixAdapter<TafResponseImp,Args...> suffixHandler()
            {
                return TafResponseSuffixAdapter<TafResponseImp,Args...>(*this);
            }
        protected:
            TafResponseImp& operator+=( const std::function<void( taf::Int32, Args... )>& callback )
            {
                FutureCallType::operator+=(callback);
                return *this;
            }
            void doCalls( taf::Int32 ret, Args... param )
            {
                FutureCallType::doCalls(ret,param...);
            }
            void doFakeCalls( taf::Int32 ret, Args... param )
            {
                FutureCallType::doFakeCalls(ret,param...);
            }
        public : ~TafResponseImp()
            {
                taf::TC_ThreadRecLock::Lock lock( FutureCallType::mlock );
                if ( !mIsReturned )
                {
                    mBindCall.doCalls();
                    auto fn = [this]( taf::Int32 ret, Args... param )
                    { this->doCallsImp( ret, param... ); };
                    TupleExpand( fn, FutureCallType::futureTuple);
                }
            }
            template <typename... ParamArgs>
            void instantReturn( ParamArgs... param )
            {
                static_assert( sizeof...( param ) == 1 + sizeof...( Args ), "instantReturn Params Incorrect!" );
                taf::TC_ThreadRecLock::Lock lock( FutureCallType::mlock );
                if ( !mIsReturned )
                {
                    mIsReturned = true;
                    mBindCall.doCalls();
                    tuple_assign( FutureCallType::futureTuple, make_tuple( param... ) );
                    auto fn = [this]( taf::Int32 ret, Args... param )
                    { this->doCallsImp( ret, param... ); };
                    TupleExpand( fn, FutureCallType::futureTuple );
                }
            }
            template <typename... ParamArgs>
            void instantReturnWithDefault( ParamArgs... param )
            {
                taf::TC_ThreadRecLock::Lock lock( FutureCallType::mlock );
                if ( !mIsReturned )
                {
                    mIsReturned = true;
                    mBindCall.doCalls();
                    tuple_assign( FutureCallType::futureTuple, make_tuple( param... ) );
                    auto fn = [this]( taf::Int32 ret, Args... param )
                    { this->doCallsImp( ret, param... ); };
                    TupleExpand( fn, FutureCallType::futureTuple );
                }
            }
            template <typename... ParamArgs>
            void set( const ParamArgs&... param )
            {
                taf::TC_ThreadRecLock::Lock lock( FutureCallType::mlock );
                tuple_assign( FutureCallType::futureTuple, make_tuple( param... ) );
            }
            template <typename... ParamArgs>
            void bind( ParamArgs&... param )
            {
                taf::TC_ThreadRecLock::Lock lock( FutureCallType::mlock );
                mBindCall += std::bind( &TafResponseImp::set<ParamArgs...>, this, std::ref( param )... );
            }
        public:
            template <typename... ParamArgs>
            static inline AutoRef<TafResponseImp<ParamArgs...>> create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, ParamArgs... ) )
            {
                return new TafResponseImp<ParamArgs...>( current, responseFunc );
            }
        };

        template <typename Storage, typename... Args>
        class TafStorageResponseImp : public TafResponseImp<Args...>
        {
        public:
            typedef TafResponseImp<Args...> base;
            template <typename CreateStorage, typename... ParamArgs>
            static inline AutoRef<TafStorageResponseImp<CreateStorage, ParamArgs...>> create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, ParamArgs... ) )
            {
                return new TafStorageResponseImp<CreateStorage, ParamArgs...>( current, responseFunc );
            }

        public:
            Storage& getStorage()
            {
                return mStorage;
            }
            LockedGuard<Storage,TC_ThreadRecLock> getSharedStorage()
            {
                TC_ThreadRecLock& pLock=base::FutureCallType::mlock;
                return LockedGuard<Storage,TC_ThreadRecLock>(&mStorage,&pLock);
            }

        protected:
            TafStorageResponseImp( JceCurrentPtr current, typename base::FutureCallFuncType* responseFunc )
                : base( current, responseFunc )
            {
            }

        protected:
            Storage mStorage;
        };

        template <typename... Args>
        inline AutoRef<TafResponseImp<Args...>> create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ) )
        {
            return TafResponseImp<Args...>::create( current, responseFunc );
        }
        template <typename Storage, typename... Args>
        inline AutoRef<TafStorageResponseImp<Storage, Args...>> create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ) )
        {
            return TafStorageResponseImp<Storage, Args...>::template create<Storage, Args...>( current, responseFunc );
        }
    }
    namespace TafLogResponse
    {
        using namespace TafResponse;
        struct TafResponseAutoFDLog
        {
            void operator()( const std::string& str )
            {
                FDLOG() << str << endl;
            }
        };
        template <typename LogPolicy, typename... Args>
        class TafLogResponseImp : public TafResponseImp<Args...>, public LogPolicy, public RollLogHelper
        {
        public:
            static inline AutoRef<TafLogResponseImp<LogPolicy, Args...>>
            create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
            {
                return new TafLogResponseImp<LogPolicy, Args...>( current, responseFunc, funcName.empty()?current->getFuncName():funcName );
            }
            ~TafLogResponseImp()
            {
                ( taf::TC_Common::tostr( TafResponseImp<Args...>::template get<0>() ) + "|" ) << *this;
            }

        protected:
            typedef TafResponseImp<Args...> base;
            TafLogResponseImp( JceCurrentPtr current, typename base::FutureCallFuncType* responseFunc, const string& funcName )
                : base( current, responseFunc )
                , LogPolicy( funcName + "|" )
                , RollLogHelper( funcName + "|" )
            {
            }
        };

        template <typename LogPolicy, typename Storage, typename... Args>
        class TafLogStorageResponseImp : public TafStorageResponseImp<Storage, Args...>, public LogPolicy, public RollLogHelper
        {
        public:
            static inline AutoRef<TafLogStorageResponseImp<LogPolicy, Storage, Args...>>
            create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
            {
                return new TafLogStorageResponseImp<LogPolicy, Storage, Args...>( current, responseFunc, funcName.empty()?current->getFuncName():funcName );
            }
            ~TafLogStorageResponseImp()
            {
                ( taf::TC_Common::tostr( TafResponseImp<Args...>::template get<0>() ) + "|" ) << *this;
            }

        protected:
            typedef TafStorageResponseImp<Storage, Args...> base;
            TafLogStorageResponseImp( JceCurrentPtr current, typename base::FutureCallFuncType* responseFunc, const string& funcName )
                : base( current, responseFunc )
                , LogPolicy( funcName + "|" )
                , RollLogHelper( funcName + "|" )
            {
            }
        };

        template <typename... Args>
        inline AutoRef<TafLogResponseImp<AutoLogObj<TafResponseAutoFDLog>, Args...>>
        create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
        {
            return TafLogResponseImp<AutoLogObj<TafResponseAutoFDLog>, Args...>::create( current, responseFunc, funcName.empty() ? current->getFuncName() : funcName );
        }
        template <typename Storage, typename... Args>
        inline AutoRef<TafLogStorageResponseImp<AutoLogObj<TafResponseAutoFDLog>, Storage, Args...>>
        create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
        {
            return TafLogStorageResponseImp<AutoLogObj<TafResponseAutoFDLog>, Storage, Args...>::create( current, responseFunc, funcName.empty() ? current->getFuncName() : funcName );
        }
    }
    namespace TafTimeResponse
    {
        using namespace TafLogResponse;
        template <typename... Args>
        inline AutoRef<TafLogResponseImp<AutoTimeLogObj<TafResponseAutoFDLog>, Args...>>
        create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
        {
            return TafLogResponseImp<AutoTimeLogObj<TafResponseAutoFDLog>, Args...>::create( current, responseFunc, funcName.empty() ? current->getFuncName() : funcName );
        }
        template <typename Storage, typename... Args>
        inline AutoRef<TafLogStorageResponseImp<AutoTimeLogObj<TafResponseAutoFDLog>, Storage, Args...>>
        create( JceCurrentPtr current, void ( *responseFunc )( JceCurrentPtr current, taf::Int32, Args... ), const string& funcName="" )
        {
            return TafLogStorageResponseImp<AutoTimeLogObj<TafResponseAutoFDLog>, Storage, Args...>::create( current, responseFunc, funcName.empty() ? current->getFuncName() : funcName );
        }
    }
}

#endif
