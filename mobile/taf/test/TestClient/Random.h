#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <string>
#include <sstream>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////
namespace Random
{
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class T> struct random;
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<char>
    { 
        static char gen() 
        {
            char c = char(rand()%80 + 1 + int(' '));

            if (c == '|') c = '@';

            return c;
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<short> 
    { 
        static short gen() 
        {
            return (unsigned short)rand(); 
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<int> 
    { 
        static int gen()
        {
            return (int)rand(); 
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<float> 
    { 
        static float gen()
        {
            return (float)(rand()*(float)(1.0)/rand()); 
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<double> 
    { 
        static double gen()
        {
            return (double)(rand()*(double)(1.0)/rand()); 
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<> struct random<string>
    {
        static string gen()
        {
            uint32_t len = rand()%100; 
            string s = "";
            for (uint32_t i = 0; i < len; ++i)
            {
                s.append(1, random<char>::gen());
            }
            return s;
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class T> struct randomVector
    {
        static vector<T> gen()
        {
            uint32_t len = rand()%100; 
            vector<T> v;
            for (uint32_t i = 0; i < len; ++i)
            {
                v.push_back(random<T>::gen());
            }
            return v;
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
    template<class K, class T> struct randomMap
    {
        static map<K, T> gen()
        {
            uint32_t len = rand()%100; 
            map<K, T> m;
            for (uint32_t i = 0; i < len; ++i)
            {
                m[random<K>::gen()] = random<T>::gen();
            }
            return m;
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////
};
////////////////////////////////////////////////////////////////////////////////////////////////
#endif
