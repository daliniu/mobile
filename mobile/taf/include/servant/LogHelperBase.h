#ifndef __LOGHELPERBASE_H__
#define __LOGHELPERBASE_H__

#include <iostream>
#include <type_traits>
#include <vector>
#include <map>
#include <set>
#include <deque>
#include <sstream>
#include <list>
#include "jce/Jce.h"

namespace LogHelperBaseInner
{
    template <typename T,bool protect=false>
    inline typename std::enable_if<std::is_base_of<taf::JceStructBase, T>::value, std::ostream&>::type writeToStream( std::ostream& ls, const T& t )
    {
        ls << "{";
        t.displaySimple( ls );
        ls << "}";
        return ls;
    }


    template <class K, class V>
    inline std::ostream& writeToStream( std::ostream& ls, const std::map<K, V>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << t->first << "=>" << t->second;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "," << t->first << "=>" << t->second;
            }
        }
        return ls;
    }

    template <class K, class V>
    inline std::ostream& writeToStream( std::ostream& ls, const std::multimap<K, V>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << t->first << "=>" << t->second;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "," << t->first << "=>" << t->second;
            }
        }
        return ls;
    }

    template <typename T>
    inline std::ostream& writeToStream( std::ostream& ls, const std::vector<T>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << *t;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "|" << *t;
            }
        }
        return ls;
    }

    template <typename T>
    inline std::ostream& writeToStream( std::ostream& ls, const std::deque<T>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << *t;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "|" << *t;
            }
        }
        return ls;
    }

    template <typename T>
    inline std::ostream& writeToStream( std::ostream& ls, const std::list<T>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << *t;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "|" << *t;
            }
        }
        return ls;
    }

    template <typename T>
    inline std::ostream& writeToStream( std::ostream& ls, const std::set<T>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << *t;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "|" << *t;
            }
        }
        return ls;
    }

    template <typename T>
    inline std::ostream& writeToStream( std::ostream& ls, const std::multiset<T>& v )
    {
        if ( !v.empty() )
        {
            auto t = v.begin();
            ls << *t;
            for ( ++t; t != v.end(); ++t )
            {
                ls << "|" << *t;
            }
        }
        return ls;
    }

    template <typename T,bool protect=true>
    inline typename std::enable_if<std::is_base_of<taf::JceStructBase, T>::value == false, std::ostream&>::type writeToStream( std::ostream& ls, const T& t )
    {
        static_assert(!protect,"Target Type is not supported");
        return ls<<t;
    }
}


#endif
