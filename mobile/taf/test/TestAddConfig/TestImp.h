#ifndef _TestImp_H_
#define _TestImp_H_

#include "util/tc_config.h"
#include "servant/Application.h"
#include "Test.h"
#include "wbl.h"
#include "wbl_comm.h"
#include "wbl_config_file.h"
#include "wbl_log.h"

#define ROLLLOG LOG->debug()

using namespace taf;
using namespace wbl;
/**
 *
 *
 */
class TestImp : public Test::TestABC
{
public:
	/**
	 *
	 */
	virtual ~TestImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	/**
	 *
	 */
    virtual taf::Int32 test(taf::Int32 a,taf::Int32 b,taf::JceCurrentPtr current);
};
/////////////////////////////////////////////////////
#endif
