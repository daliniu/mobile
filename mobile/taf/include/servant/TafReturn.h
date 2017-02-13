#ifndef __TAFRETURN_H__
#define __TAFRETURN_H__

#include "AutoTimeLog.h"
#include "AutoRef.h"
#include "FutureCall.h"
#include "servant/Application.h"
#include "RollLogHelper.h"
#include "LockedSingleton.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    struct AutoFDLog
    {
        void operator()(const std::string &str)
        {
            FDLOG()<<str<<endl;
        }
    };

    struct __nouseStorageType
    {
    };

    //回包类,用于自动处理回报和按天日志 模板参数为本函数的参数(返回值因为处理需要,限制为taf::Int32)
    template<typename LogObj,typename storageType=__nouseStorageType>
        class TafReturnBase:public taf::TC_HandleBase,public LogObj,public RollLogHelper,public FutureCall<JceCurrentPtr,taf::Int32,storageType &>
    {
        protected:
            typedef FutureCall<JceCurrentPtr,taf::Int32,storageType&> CallType;
            TafReturnBase(JceCurrentPtr Current,const std::string& str)
                :LogObj(!str.empty()?str+"|":""),RollLogHelper(!str.empty()?str+"|":""),mCurrent(Current)
            {
                Current->setResponse(false);
                mRet=-1;
                mIsReturned=false;
            } 
        public:
            virtual ~TafReturnBase()
            {
                instantReturn(mRet);
                (taf::TC_Common::tostr(mRet)+"|")<<*this;
                if(!CallType::isDone())
                {
                    *this<<"   Warning : Missing Return Packet Function !!!";
                }
            }
            //立即回包 *注意*LOG还是在析构的时候输出
            void instantReturn(taf::Int32 ret)
            {
                if(!mIsReturned)
                {
                    mIsReturned=true;
                    mRet=ret;
                    doCalls(mCurrent,ret,mStorage);
                }
            }
            int setDefaultRetValue(int iRet)
            {
                return mRet=iRet;
            }
            static AutoRef<TafReturnBase> create(JceCurrentPtr Current,const std::string &str="")
            {
                return new TafReturnBase(Current,str);
            }
        public:
            storageType& getStorage()
            {
                return mStorage;
            }
            LockedGuard<storageType,TC_ThreadRecLock> getSharedStorage()
            {
                TC_ThreadRecLock &pLock = CallType::mlock;
                return LockedGuard<storageType,TC_ThreadRecLock>(&mStorage,&pLock);
            }
        protected:
            void doCalls(JceCurrentPtr current,taf::Int32 ret,storageType &storage)
            {
                CallType::doCalls(current,ret,storage);
            }
            JceCurrentPtr mCurrent;
            int mRet;
            bool mIsReturned;
            storageType mStorage;
        private:
            
            TafReturnBase(const TafReturnBase&);
            void operator=(const TafReturnBase&);
    };

    template<typename storageType=__nouseStorageType>       //C++11 模板别名gcc 4.6不支持
        class TafLogReturn:public TafReturnBase<AutoLogObj<AutoFDLog>,storageType>
    {
        protected:
            typedef TafReturnBase<AutoLogObj<AutoFDLog>,storageType> baseType;
            TafLogReturn(JceCurrentPtr Current,const std::string &str="")
                :baseType(Current,str)
            {
            }
    };

    template<typename storageType=__nouseStorageType>       //C++11 模板别名gcc 4.6不支持
        class TafTimeReturn:public TafReturnBase<AutoTimeLogObj<AutoFDLog>,storageType>
    {
        protected:
            typedef TafReturnBase<AutoTimeLogObj<AutoFDLog>,storageType> baseType;
            TafTimeReturn(JceCurrentPtr Current,const std::string &str="")
                :baseType(Current,str)
            {
            }
    };

    template<typename storageType=__nouseStorageType>       //C++11 模板别名gcc 4.6不支持
        class TafReturn:public TafReturnBase<FakeLogObj,storageType>
    {
        protected:
            typedef TafReturnBase<AutoLogObj<FakeLogObj>,storageType> baseType;
            TafReturn(JceCurrentPtr Current,const std::string &str="")
                :baseType(Current,str)
            {
            }
    };

}





#endif
