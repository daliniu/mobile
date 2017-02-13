#ifndef __TUPLEEXPAND_H__
#define __TUPLEEXPAND_H__

#include <tuple>

#include "Integer_sequence.h"
using namespace std;


namespace TupleExpandInner
{
    template <typename F, typename T, typename... Args, size_t... I>
    auto TupleExpandImp( F&& f, T&& t, integer_sequence<size_t, I...>, Args&&... params ) -> decltype( std::forward<F>(f)( std::forward<Args...>(params)..., get<I>( std::forward<T>(t) )... ) )
    {
        return std::forward<F>(f)( std::forward<Args...>(params)..., get<I>( std::forward<T>(t) )... );
    }
}

template<typename F,typename T,typename ...Args>
auto TupleExpand(F&& f,T&& t,Args&&... params)->decltype(TupleExpandInner::TupleExpandImp(std::forward<F>(f),std::forward<T>(t),typename integer_sequence_detail::make_integer_sequence<size_t,tuple_size<typename decay<T>::type>::value>::type(),std::forward<Args...>(params)...))
{
    return TupleExpandInner::TupleExpandImp(std::forward<F>(f),std::forward<T>(t),typename integer_sequence_detail::make_integer_sequence<size_t,tuple_size<typename decay<T>::type>::value>::type(),std::forward<Args...>(params)...);
}



#endif
