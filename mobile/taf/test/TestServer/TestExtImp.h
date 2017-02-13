#ifndef _TEST_EXT_IMP_H_
#define _TEST_EXT_IMP_H_

#include "Test.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

class TestExtImp : public Test::TestExt
{
public:
    /**
     *
     * @param id
     * @param mi
     * @param mo
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testMyInfo(taf::Int32 id,
                                  const Test::MyInfo &mi,
                                  Test::MyInfo &mo,
                                  taf::JceCurrentPtr current)
    {
        mo = mi;

        return id;
    }
    /**
     *
     * @param id
     * @param ti
     * @param to
     * @param current
     *
     * @return taf::Int32
     */
    virtual taf::Int32 testTestInfo(taf::Int32 id,
                                    const Test::TestInfo &ti,
                                    Test::TestInfo &to,
                                    taf::JceCurrentPtr current)
    {
        to = ti;

        return id;
    }
	/**
	 *
	 *
	 */
    virtual taf::Int32 testMapStruct(taf::Int32 id,
					                 const map<taf::Int32, Test::MyInfo> & mi,
									 map<taf::Int32, Test::MyInfo> &mo,
									 taf::JceCurrentPtr current)
	{
        mo = mi;

        return id;
	}
	/**
	 *
	 *
	 */
    virtual taf::Int32 testVectorStruct(taf::Int32 id,
					                    const vector<Test::TestInfo> & vi,
										vector<Test::TestInfo> &vo,
										taf::JceCurrentPtr current)
	{
        vo = vi;

        return id;
	}
    /**
     * ÍË³ö
     */
    virtual void destroy()
    {
    }
    /**
     * ³õÊ¼»¯
     *
     * @return int
     */
    virtual void initialize()
    {
    }
};
#endif
