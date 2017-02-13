#ifndef __LOCKEDSINGLETON_H__
#define __LOCKEDSINGLETON_H__

#include "util/tc_singleton.h"

template <typename Derived,typename Base>
struct add_base
{
    typedef class :public Derived,public Base{}DerivedWithBase;
    typedef typename std::conditional<std::is_base_of<Base,Derived>::value,Derived,DerivedWithBase>::type type;
};

template <typename Derived,typename Base>
struct add_base_direct
{
    typedef class :public Derived,public Base{}type;
};

template <typename T,typename L>
class LockedGuard
{
    public:
        LockedGuard(T* t,L* l)
            :t(t),l(l)
        {
#ifdef LOCKEDGUARD_DEBUG
            cout<<"lock"<<endl;
#endif
            l->lock();
        }
        ~LockedGuard()
        {
            if(l)
            {
#ifdef LOCKEDGUARD_DEBUG
                cout << "unlock" << endl;
#endif
                l->unlock();
            }
        }
        T* operator->() const
        {
            return t;
        }
        T& operator*() const
        {
            return *t;
        }
        LockedGuard(const LockedGuard& rhs);
        void operator=(const LockedGuard& rhs);
    private:
        T* t;
        L* l;
};

template <typename T,
          template <class> class CreatePolicy = taf::CreateUsingNew,
          template <class> class LifetimePolicy = taf::DefaultLifetime>
class LockedSingleton :public taf::TC_ThreadLock, protected taf::TC_Singleton<T, CreatePolicy, LifetimePolicy>
{
public:
    static LockedGuard<T,T> getLockedInstance()
    {
        auto instance = taf::TC_Singleton<T, CreatePolicy, LifetimePolicy>::getInstance();
        return LockedGuard<T, T>( instance, instance );
    }
protected:
    static LockedGuard<T,T> getInstance()
    {
        auto instance = taf::TC_Singleton<T, CreatePolicy, LifetimePolicy>::getInstance();
        return LockedGuard<T, T>( instance, instance );
    }
};

template <typename T>
class LockedValue
{
private:
    mutable taf::TC_ThreadLock lock;
    T t;

public:
    template <typename... Args>
    LockedValue( Args... param )
        : t( param... )
    {
    }
    LockedGuard<const T,taf::TC_ThreadLock> operator->() const
    {
        return LockedGuard<const T, taf::TC_ThreadLock>( &t, &lock );
    }
    LockedGuard<T,taf::TC_ThreadLock> operator->() 
    {
        return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
    }
    LockedGuard<T,taf::TC_ThreadLock> operator*()
    {
        return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
    }
    LockedGuard<const T,taf::TC_ThreadLock> getLocked() const
    {
        return LockedGuard<const T, taf::TC_ThreadLock>( &t, &lock );
    }
    LockedGuard<T,taf::TC_ThreadLock> getLocked()
    {
        return LockedGuard<T, taf::TC_ThreadLock>( &t, &lock );
    }
};

#endif
