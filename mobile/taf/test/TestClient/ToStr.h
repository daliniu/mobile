#ifndef _TO_STR_H_
#define _TO_STR_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

struct ToStr
{
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class T> 
    static string toString(const T& t)
    {
        ostringstream os; os << t; return os.str();
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class T> 
    static string toString(const vector<T>& t)
    {
        ostringstream os; os << "(";
        typename vector<T>::const_iterator it;
        for (it = t.begin(); it != t.end(); ++it)
        {
           if (it != t.begin()) os << "|";
           os << toString<T>(*it);
        }
        os << ")";
        return os.str();
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class T, class V> 
    static string toString(const map<T, V>& t)
    {
        ostringstream os; os << "[";
        typename map<T, V>::const_iterator it;
        for (it = t.begin(); it != t.end(); ++it)
        {
           if (it != t.begin()) os << "|";
           os << toString<T>(it->first) << "=" << toString<V>(it->second);
        }
        os << "]";
        return os.str();
    }
    /////////////////////////////////////////////////////////////////////////////////////////
};
#endif
