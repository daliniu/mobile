#ifndef __TAFASYNCEXPERIMENT_H__
#define __TAFASYNCEXPERIMENT_H__

#include "AutoTimeLog.h"
#include "AutoRef.h"
#include "FutureCall.h"
#include "servant/Application.h"
#include "RollLogHelper.h"
#include "TupleExpand.h"
#include "TupleAssign.h"
/////////////////////////////////////////////////////////
#include "TafResponse.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    namespace tafAsyncExperiment
    {
#if 0
        struct __nouseStorageType
        {
        };
        template <typename LogObj, typename storageType = __nouseStorageType>
        class TafProxyReturnBase : public taf::TC_HandleBase, public FutureCall<JceCurrentPtr, taf::Int32, storageType &>
        {
        protected:
            typedef FutureCall<JceCurrentPtr, taf::Int32, storageType &> CallType;
            TafProxyReturnBase( JceCurrentPtr Current )
                : mCurrent( Current )
            {
                Current->setResponse( false );
                mRet = -1;
                mIsReturned = false;
            }

        public:
            virtual ~TafProxyReturnBase()
            {
                instantReturn( mRet );
            }
            //立即回包 *注意*LOG还是在析构的时候输出
            void instantReturn( taf::Int32 ret )
            {
                if ( !mIsReturned )
                {
                    mIsReturned = true;
                    mRet = ret;
                    doCalls( mCurrent, ret, mStorage );
                }
            }
            static AutoRef<TafProxyReturnBase> create( JceCurrentPtr Current, const std::string &str = "" )
            {
                return new TafProxyReturnBase( Current, str );
            }

        public:
            storageType &getStorage()
            {
                return mStorage;
            }

        protected:
            void doCalls( JceCurrentPtr current, taf::Int32 ret, storageType &storage )
            {
                CallType::doCalls( current, ret, storage );
            }
            JceCurrentPtr mCurrent;
            int mRet;
            bool mIsReturned;
            storageType mStorage;

        private:
            TafProxyReturnBase( const TafProxyReturnBase & );
            void operator=( const TafProxyReturnBase & );
        };
#endif

        template <typename Callback, typename... ProxyArgs>
        void TafProxy( JceCurrentPtr current, Callback cb,std::function<void( JceCurrentPtr, ProxyArgs... )> responseFunc)
        {
            current->setResponse(false);
            cb += [current, responseFunc]( ProxyArgs... param )
            {
                responseFunc( current, param... ); 
            };
            cb->errorHandler() += [current, responseFunc]( taf::Int32 ret )
            {
                std::tuple<ProxyArgs...> paramPack;
                TupleExpand( responseFunc, paramPack, current, -1 );
            };
        }
    }
}

#endif
