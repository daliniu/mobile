#ifndef __AUTOREF_H__
#define __AUTOREF_H__

#include "util/tc_autoptr.h"

#ifdef TAF_ASYNC_DEBUG
namespace tafAsyncDebug
#else
namespace tafAsync
#endif
{
    template <typename T>
    class AutoRef
    {
    public:
        typedef T RefType;
        typedef T element_type;
        AutoRef( T *ptr=NULL )
            : ptr( ptr )
        {
        }
        AutoRef( taf::TC_AutoPtr<T> ptr )
            : ptr( ptr )
        {
        }
        AutoRef( const AutoRef &ref )
        {
            ptr = ref.ptr;
        }
        T *operator->() const
        {
            return ptr.get();
        }
        T &operator*() const
        {
            return *ptr;
        }
        operator taf::TC_AutoPtr<T> &() const
        {
            return ptr;
        }
        taf::TC_AutoPtr<T> getAutoPtr() const
        {
            return ptr;
        }
        operator T &() const
        {
            return *ptr;
        }

    private:
        taf::TC_AutoPtr<T> ptr;

    public:
        template <typename OP>
        auto operator>=( OP &&op ) const -> decltype( *ptr >= std::forward<OP>( op ) )
        {
            return ( *ptr >= std::forward<OP>( op ) );
        }
        template <typename OP>
        auto operator>( OP &&op ) const -> decltype( *ptr > std::forward<OP>( op ) )
        {
            return ( *ptr > std::forward<OP>( op ) );
        }
        template <typename OP>
        auto operator<=( OP &&op ) const -> decltype( *ptr <= std::forward<OP>( op ) )
        {
            return ( *ptr <= std::forward<OP>( op ) );
        }
        template <typename OP>
        auto operator<( OP &&op ) const -> decltype( *ptr < std::forward<OP>( op ) )
        {
            return ( *ptr < std::forward<OP>( op ) );
        }
        template <typename OP>
        auto operator==( OP &&op ) const -> decltype( *ptr == std::forward<OP>( op ) )
        {
            return ( *ptr == std::forward<OP>( op ) );
        }
        template <typename OP>
        auto operator!=( OP &&op ) const -> decltype( *ptr != std::forward<OP>( op ) )
        {
            return ( *ptr != std::forward<OP>( op ) );
        }

    public:
        template <typename OP>
        auto operator+=( OP &&op ) const -> decltype( ptr -> operator+=( std::forward<OP>( op ) ) )
        {
            return *ptr += std::forward<OP>( op );
        }
        template <typename OP>
        auto operator<<( OP &&op ) const -> decltype( *ptr << std::forward<OP>( op ) )
        {
            return *ptr << std::forward<OP>( op );
        }
        template <typename OP>
        auto operator|( OP &&op ) const -> decltype( *ptr | std::forward<OP>( op ) )
        {
            return *ptr | std::forward<OP>( op );
        }
    };

    template <typename OP, typename T>
    auto operator<<( OP &&op, const AutoRef<T> &ref ) -> decltype( std::forward<OP>( op ) << (T &)ref )
    {
        return std::forward<OP>( op ) << (T &)ref;
    }

    template <typename OP, typename T>
    auto operator|( OP &&op, const AutoRef<T> &ref ) -> decltype( std::forward<OP>( op ) | (T &)ref )
    {
        return std::forward<OP>( op ) | (T &)ref;
    }
}

#endif
