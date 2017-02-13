#include "TransFunc.h"
#include "util/tc_encoder.h"

bool TransFunc::ToUTF8(const std::string & sFrom, std::string & sTo)
{
	sTo = taf::TC_Encoder::gbk2utf8(sFrom);

	return true;
}

