#ifndef __SEQUENCECALL_H__
#define __SEQUENCECALL_H__

#include <functional>
#include <list>
#include "util/tc_thread.h"
#include "jce/JceType.h"
#include "TupleExpand.h"
#include "TupleAssign.h"
//批量回调类

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    template <typename... Args>
    class FutureCall
    {
    public:
        typedef std::function<void(Args... param)> callType;
        FutureCall()
            : bIsDone( false )
            , bIsReady( false )
        {
        }
        FutureCall& operator+=(const std::function<void( Args... param )>& callback )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            if ( !bIsReady )
            {
                callbacks.push_back( callback );
            }
            else
            {
                TupleExpand( callback, futureTuple );
            }
            return *this;
        }
        void doCalls( Args... param )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            futureTuple = std::make_tuple( param... );
            doCallsImp( param... );
            bIsReady = bIsDone = true;
            mlock.notifyAll();
        }
        void doFakeCalls( Args... param )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            futureTuple = std::make_tuple( param... );
            bIsReady = true;
        }
        bool isCallbackEmtpy()
        {
            return callbacks.empty();
        }
        bool isReady()
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            return bIsReady;
        }
        bool isDone()
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            return bIsDone;
        }
        void cancel()
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            bIsReady = bIsDone = false;
            callbacks.clear();
            mlock.notifyAll();
        }
        bool waitForDone(taf::Int32 timeout_ms=-1)
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            if ( !bIsDone)
            {
                if(timeout_ms>0)
                {
                    mlock.timedWait( timeout_ms );
                }
                else if(timeout_ms<0)
                {
                    mlock.wait();
                }
            }
            return bIsDone;
        }
        template <typename... ParamArgs>
        void get( ParamArgs&... param )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            auto tempParam = std::tie( param... );
            tuple_assign( tempParam, futureTuple );
        }
        template <size_t i>
        typename std::enable_if<i != ( size_t ) - 1, typename tuple_element<i + 1, tuple<tuple_assign_inner::TUPLE_DEFAULT_VALUE, typename decay<Args>::type...>>::type&>::type get()
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            return std::get<i>( futureTuple );
        }
        template <size_t i>
        typename std::enable_if<i == ( size_t ) - 1, const tuple_assign_inner::TUPLE_DEFAULT_VALUE&>::type get()
        {
            return _;
        }
    protected:
        template <size_t... I>
        auto getTupleImp( integer_sequence<size_t, I...> ) -> decltype( forward_as_tuple( ( (FutureCall*)NULL ) -> get<I>()... ) )
        {
            return forward_as_tuple( this->get<I>()... );
        }
    public:
        auto getTuple() -> decltype( ( (FutureCall*)NULL ) -> getTupleImp( typename integer_sequence_detail::make_integer_sequence<size_t, sizeof...( Args )>::type() ) )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            return getTupleImp( typename integer_sequence_detail::make_integer_sequence<size_t, sizeof...( Args )>::type() );
        }
        template <size_t... I>
        auto getTuple() -> decltype( forward_as_tuple( ( (FutureCall*)NULL ) -> get<I>()... ) )
        {
            taf::TC_ThreadRecLock::Lock lock( mlock );
            return forward_as_tuple( this->get<I>()... );
        }
    protected:
        void doCallsImp( Args... param )
        {
            try
            {
                for ( auto fn : callbacks )
                {
                    fn( param... );
                }
            }
            catch ( ... )
            {
                callbacks.clear();
                throw;
            }
            callbacks.clear();
        }
        bool bIsDone;
        bool bIsReady;
        std::tuple<typename std::decay<Args>::type...> futureTuple;
        std::list<std::function<void( Args... param )>> callbacks;
        taf::TC_ThreadRecLock mlock;
    };
}

#endif
