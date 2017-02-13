#ifndef __TUPLEASSIGN_H__
#define __TUPLEASSIGN_H__

#include <stdlib.h>
#include <tuple>

namespace tafAsync
{
    namespace tuple_assign_inner
    {
        struct TUPLE_DEFAULT_VALUE
        {
            template <typename T>
            const TUPLE_DEFAULT_VALUE& operator=( const T& ) const
            {
                return *this;
            }
            constexpr operator size_t() const
            {
                return -1;
            }
        };


        template <size_t Idx1, size_t Idx2>
        std::_Tuple_impl<Idx1> tuple_assign( std::_Tuple_impl<Idx1>& lhs, std::_Tuple_impl<Idx2>&& rhs )
        {
            return lhs;
        }

        template <size_t Idx1, size_t Idx2, typename Head1, typename Head2, typename... Tails1, typename... Tails2>
        std::_Tuple_impl<Idx1, Head1, Tails1...> tuple_assign( std::_Tuple_impl<Idx1, Head1, Tails1...>& lhs, std::_Tuple_impl<Idx2, Head2, Tails2...>&& rhs )
        {
            tuple_element_assign( lhs._M_head(), std::move( rhs._M_head() ) );
            tuple_assign( lhs._M_tail(), std::move( rhs._M_tail() ) );
            return lhs;
        }

        template <size_t Idx1, size_t Idx2>
        std::_Tuple_impl<Idx1> tuple_assign( std::_Tuple_impl<Idx1>& lhs, std::_Tuple_impl<Idx2>& rhs )
        {
            return lhs;
        }

        template <size_t Idx1, size_t Idx2, typename Head1, typename Head2, typename... Tails1, typename... Tails2>
        std::_Tuple_impl<Idx1, Head1, Tails1...> tuple_assign( std::_Tuple_impl<Idx1, Head1, Tails1...>& lhs, std::_Tuple_impl<Idx2, Head2, Tails2...>& rhs )
        {
            tuple_element_assign( lhs._M_head(), rhs._M_head() );
            tuple_assign( lhs._M_tail(), rhs._M_tail() );
            return lhs;
        }
    }
    template <typename LT, typename RT>
    void tuple_element_assign( LT& lhs, const RT&& rhs )
    {
        lhs = rhs;
    }
    template <typename LT>
    void tuple_element_assign( LT& lhs, const tuple_assign_inner::TUPLE_DEFAULT_VALUE&& rhs )
    {
    }
    template <typename... Tails1, typename... Tails2>
    std::tuple<Tails1...> tuple_assign( std::tuple<Tails1...>& lhs, std::tuple<Tails2...>&& rhs )
    {
        return lhs;
    }
    template <typename Head1, typename Head2, typename... Tails1, typename... Tails2>
    std::tuple<Head1, Tails1...> tuple_assign( std::tuple<Head1, Tails1...>& lhs, std::tuple<Head2, Tails2...>&& rhs )
    {
        tuple_element_assign( lhs._M_head(), std::forward<Head2>( rhs._M_head() ) );
        tuple_assign_inner::tuple_assign( lhs._M_tail(), std::move( rhs._M_tail() ) );
        return lhs;
    }
    template <typename LT, typename RT>
    void tuple_element_assign( LT& lhs, const RT& rhs )
    {
        lhs = rhs;
    }
    template <typename LT>
    void tuple_element_assign( LT& lhs, const tuple_assign_inner::TUPLE_DEFAULT_VALUE& rhs )
    {
    }
    template <typename... Tails1, typename... Tails2>
    std::tuple<Tails1...> tuple_assign( std::tuple<Tails1...>& lhs, std::tuple<Tails2...>& rhs )
    {
        return lhs;
    }
    template <typename Head1, typename Head2, typename... Tails1, typename... Tails2>
    std::tuple<Head1, Tails1...> tuple_assign( std::tuple<Head1, Tails1...>& lhs, std::tuple<Head2, Tails2...>& rhs )
    {
        tuple_element_assign( lhs._M_head(), std::forward<Head2>( rhs._M_head() ) );
        tuple_assign_inner::tuple_assign( lhs._M_tail(), rhs._M_tail() );
        return lhs;
    }
#ifndef DO_NOT_USING_TUPLE_ASSIGN
    constexpr tuple_assign_inner::TUPLE_DEFAULT_VALUE _{};
#endif
}



#endif
