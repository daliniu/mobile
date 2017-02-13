#ifndef __TAFRETURN_H__
#define __TAFRETURN_H__

#include "AutoTimeLog.h"
#include "AutoRef.h"
#include "FutureCall.h"
#include "servant/Application.h"
#include "RollLogHelper.h"

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
                :LogObj(!str.empty()?str+"|":"|"),RollLogHelper(!str.empty()?str+"|":""),mCurrent(Current)
            {
                Current->setResponse(false);
                mRet=-1;
                mIsReturned=false;
            } 
        public:
            virtual ~TafReturnBase()
            {
                instantReturn(mRet);
            }
            //立即回包 *注意*LOG还是在析构的时候输出
            void instantReturn(taf::Int32 ret)
            {
                if(!mIsReturned)
                {
                    mIsReturned=true;
                    (taf::TC_Common::tostr(ret)+"]")<<*this;
                    if(CallType::isCallbackEmtpy())
                    {
                        *this<<"   Warning : Missing Return Packet Function !!!";
                    }
                    else
                    {
                        doCalls(mCurrent,ret,mStorage);
                    }
                }
            }
            //记录回调返回值 如果返回值小于0,会自动当做最终返回值处理
            //计划废弃该函数
            const int setStepRet(int iRet)
            {
                if(iRet<0)
                {
                    mRet=iRet;
                }
                else
                {
                    ","<<*this;
                    iRet<<*this;
                }
                return iRet;
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
