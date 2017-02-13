#ifndef __TRANS_FUNC_H__
#define __TRANS_FUNC_H__

#include <string>

struct TransFunc
{
	static bool ToUTF8(const std::string & sFrom, std::string & sTo);
	static bool UTF8To(const std::string & sFrom, std::string & sTo);
};

#endif

