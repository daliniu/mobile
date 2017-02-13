#ifndef __QUANBASE_H_
#define __QUANBASE_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace QUAN
{

#define PROC_BEGIN do{
#define PROC_END   }while(0);


#define __TRY__ try\
    {

#define __CATCH__ }\
    catch (std::exception const& e)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__ << string(" catch std exception: ") + e.what() << endl;\
    }\
    catch (...)\
    {\
        LOG->error() <<__FILE__<<"::"<< __FUNCTION__<< " catch unknown exception" << endl;\
    }

template <typename T> string JCETOSTR(T &t) 
{   
    ostringstream os; 
    t.displaySimple(os);
    return os.str();
}

template<typename T>  string JCETOSTR(const vector<T> &t) 
{ 
    ostringstream os; 
    for(unsigned i=0;i<t.size();i++) 
    {   
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template<typename T>  string JCETOSTR(vector<T> &t)
{
    ostringstream os;
    for(unsigned i=0;i<t.size();i++)
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template <typename T> string SIMPLEJCETOSTR(T &t)
{
    ostringstream os;
    t.displaySimple(os);
    return os.str();
}

template<typename T>  string SIMPLEJCETOSTR(const vector<T> &t)
{
    ostringstream os;
    for(unsigned i=0;i<t.size();i++)
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template<typename T>  string SIMPLEJCETOSTR(vector<T> &t)
{
    ostringstream os;
    for(unsigned i=0;i<t.size();i++)
    {
        os<<JCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}


template <typename T> string DETAILJCETOSTR(T &t) 
{   
    ostringstream os; 
    t.displaySimple(os);
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(const vector<T> &t) 
{ 
    ostringstream os; 
    for(unsigned i=0;i<t.size();i++) 
    {   
        os<<DETAILJCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(vector<T> &t)
{
    ostringstream os;
    for(unsigned i=0;i<t.size();i++)
    {
        os<<DETAILJCETOSTR(t[i]);
        os<<"|";
    }
    return os.str();
}



/*

template <typename T> string DETAILJCETOSTR(T &t) 
{   
    ostringstream os; 
    t.display(os); 
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(const vector<T> &t) 
{ 
    ostringstream os; 
    for(unsigned i=0;i<t.size();i++) 
    {   
        os<<DETAILJCETOSTR(t[i]);
        os<<endl;
    }
    return os.str();
}

template<typename T>  string DETAILJCETOSTR(vector<T> &t)
{
    ostringstream os;
    for(unsigned i=0;i<t.size();i++)
    {
        os<<DETAILJCETOSTR(t[i]);
        os<<endl;
    }
    return os.str();
}

*/


inline int getId(long lUId, string& sId)
{
    time_t now = TC_TimeProvider::getInstance()->getNow();
    
    char cBuf[64];
    snprintf(cBuf, sizeof cBuf, "%ld%ld", now, lUId);

    sId = cBuf;
   
    return 0;
}
const long gUIdXorNum = 5253;
const long gUserVersion = 101;

inline int decodeUId(long lId)
{
    if (gUserVersion == 101)
        return ((lId & 0x0000FFFFFFFF0000) >> 16) ^ gUIdXorNum;
    else
        return lId;
}

}
#endif
