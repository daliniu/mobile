#ifndef _TEST_BASE_IMP_H_
#define _TEST_BASE_IMP_H_

#include "Test.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

class TestBaseImp : public Test::TestBase
{
public:
    /**
     *
     * @param T
     * @param v
     *
     * @return string
     */
    template<class T> string toString(const vector<T>& v)
    {
        return "";

        ostringstream os;

        os << "vsize:" << v.size() << "|";

        for (uint32_t i = 0; i < v.size(); ++i)
        {
            os << TC_Common::tostr(v[i]) << "|";
        }
        return os.str();
    }
    /**
     *
     * @param K
     * @param T
     * @param m
     *
     * @return string
     */
    template<class K, class T> string toString(const map<K, T>& m)
    {
        return "";

        ostringstream os;

        os << "msize:" << m.size() << "|";

        typename map<K, T>::const_iterator it;
        for (it = m.begin(); it != m.end(); ++it)
        {
            os << TC_Common::tostr(it->first) << "=" << TC_Common::tostr(it->second) << "|";
        }
        return os.str();
    }
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapIntDouble(taf::Int32 id,
                                        const map<taf::Int32, taf::Double> &mi,
                                        map<taf::Int32, taf::Double> &mo,
                                        taf::JceCurrentPtr current)
    {

        cout << "testMapIntDouble" << endl;

        LOG->debug() << "testMapIntDouble:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapIntFloat(taf::Int32 id,
                                       const map<taf::Int32, taf::Float> &mi,
                                       map<taf::Int32, taf::Float> &mo,
                                       taf::JceCurrentPtr current)
    {
        LOG->debug() << "testMapIntFloat:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapByte(taf::Int32 id,
                                   const map<taf::Char, std::string> &mi,
                                   map<taf::Char, std::string> &mo, taf::JceCurrentPtr current)
    {
        LOG->debug() << "testMapByte:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }

    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapInt(taf::Int32 id,
                                  const map<taf::Int32, taf::Int32> &mi,
                                  map<taf::Int32, taf::Int32> &mo, taf::JceCurrentPtr current)
    {
        LOG->debug() << "testMapInt:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapIntStr(taf::Int32 id,
                                     const map<taf::Int32, std::string> &mi,
                                     map<taf::Int32, std::string> &mo,
                                     taf::JceCurrentPtr current)
    {
        LOG->debug() << "testMapIntStr:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMapStr(taf::Int32 id,
                                  const map<std::string, std::string> &mi,
                                  map<std::string, std::string> &mo, taf::JceCurrentPtr current)
    {
        LOG->debug() << "testMapStr:" << id << ":" << toString(mi) << endl;

        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param si
     * @param so
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testStr(taf::Int32 id,
                               const std::string &si,
                               std::string &so,
                               taf::JceCurrentPtr current)
    {
        LOG->debug() << "testStr:" << id << ":" << "" << endl; ///si << endl;

        so = si;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorByte(taf::Int32 id,
                                      const vector<taf::Char> &vi,
                                      vector<taf::Char> &vo,
                                      taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorByte:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorDouble(taf::Int32 id,
                                        const vector<taf::Double> &vi,
                                        vector<taf::Double> &vo,
                                        taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorDouble:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorInt(taf::Int32 id,
                                     const vector<taf::Int32> &vi,
                                     vector<taf::Int32> &vo,
                                     taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorInt:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorStr(taf::Int32 id,
                                     const vector<std::string> &vi,
                                     vector<std::string> &vo,
                                     taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorStr:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorFloat(taf::Int32 id,
                                       const vector<taf::Float> &vi,
                                       vector<taf::Float> &vo,
                                       taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorFloat:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     *
     * @param id
     * @param vi
     * @param vo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testVectorShort(taf::Int32 id,
                                       const vector<taf::Short> &vi,
                                       vector<taf::Short> &vo,
                                       taf::JceCurrentPtr current)
    {
        LOG->debug() << "testVectorShort:" << id << ":" << toString(vi) << endl;

        vo = vi;

        return id;
    }
    /**
     * ³õÊ¼»¯
     *
     * @return int
     */
    virtual void initialize()
    {
		cout << "BaseImp initialize" << endl;
    }
    /**
     * ÍË³ö
     */
    virtual void destroy()
    {
		cout << "BaseImp destroy" << endl;
    }
};

#endif



