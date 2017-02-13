#if !BOOST_PP_IS_ITERATING

#include <util/tc_loki.h>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/function.hpp>

#define MEMBER_FUNCTION_SIZE               BOOST_PP_CAT(CLASS_NAME, _MEMBER_FUNCTION_SIZE)
#define CLASS_SEQ                          BOOST_PP_CAT(CLASS_NAME, _SEQ)
#define MEMBER_FUNCTION_COLUMNS            BOOST_PP_CAT(CLASS_NAME, _MEMBER_FUNCTION_COLUMNS) 
#define RETURN_COLUMNS(n)                  BOOST_PP_TUPLE_ELEM(3, 0, BOOST_PP_TUPLE_ELEM(MEMBER_FUNCTION_SIZE, n, MEMBER_FUNCTION_COLUMNS))
#define FUNCTION_COLUMNS(n)                BOOST_PP_TUPLE_ELEM(3, 1, BOOST_PP_TUPLE_ELEM(MEMBER_FUNCTION_SIZE, n, MEMBER_FUNCTION_COLUMNS))
#define ARG_COLUMNS(n)                     BOOST_PP_TUPLE_ELEM(3, 2, BOOST_PP_TUPLE_ELEM(MEMBER_FUNCTION_SIZE, n, MEMBER_FUNCTION_COLUMNS))

#define PRED(r, state) \
	BOOST_PP_NOT_EQUAL(\
	BOOST_PP_TUPLE_ELEM(2, 0, state), \
	BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)) \
	) \
	/**/

#define OP(r, state) \
	(\
	BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 0, state)), \
	BOOST_PP_TUPLE_ELEM(2, 1, state) \
	) \
	/**/

#define MACRO(r, state) BOOST_PP_COMMA_IF(BOOST_PP_TUPLE_ELEM(2, 0, state)) BOOST_PP_CAT(class T, BOOST_PP_TUPLE_ELEM(2, 0, state) = boost::function<int(taf::JceCurrentPtr, ARG_COLUMNS(BOOST_PP_TUPLE_ELEM(2, 0, state)))>) 

#define MACRO_II(r, state) BOOST_PP_COMMA_IF(BOOST_PP_TUPLE_ELEM(2, 0, state)) BOOST_PP_CAT(class T, BOOST_PP_TUPLE_ELEM(2, 0, state))

#define MACRO_I(r, state) BOOST_PP_COMMA_IF(BOOST_PP_TUPLE_ELEM(2, 0, state)) boost::function<int(taf::JceCurrentPtr, ARG_COLUMNS(BOOST_PP_TUPLE_ELEM(2, 0, state)))> 

template<class C>
class SharedCallback;

template<>
class SharedCallback<BOOST_PP_CAT(CLASS_NAME, PrxCallback)>  : public BOOST_PP_CAT(CLASS_NAME, PrxCallback)
{
private:
	string                      _sLog;
	taf::JceCurrentPtr          _tCurrent;

public:

	SharedCallback(taf::JceCurrentPtr current)
	{
		_tCurrent = current;
	}

	void setCallerLog(string const& sLog)
	{
		_sLog = sLog;
	}

#include <boost/preprocessor/iteration/iterate.hpp>

#define BOOST_PP_ITERATION_PARAMS_1 (4, (0, BOOST_PP_DEC(MEMBER_FUNCTION_SIZE), "SharedCallback.h", 1))

#include BOOST_PP_ITERATE()

#else
#define N BOOST_PP_ITERATION()

	boost::function<int(taf::JceCurrentPtr, ARG_COLUMNS(N))> BOOST_PP_CAT(_tBind_, FUNCTION_COLUMNS(N));

	void BOOST_PP_CAT(setBind, FUNCTION_COLUMNS(N))(boost::function<int(taf::JceCurrentPtr, ARG_COLUMNS(N))> tBind)
	{
		BOOST_PP_CAT(_tBind_, FUNCTION_COLUMNS(N)) = tBind;
	}

	virtual void BOOST_PP_CAT(callback_, FUNCTION_COLUMNS(N))(RETURN_COLUMNS(N) ret, ARG_COLUMNS(N) tRsp)
	{
		LOG->debug() << ret << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << JCETOSTR(tRsp) << endl;

		// µ÷ÓÃbindº¯Êý
		BOOST_PP_CAT(_tBind_, FUNCTION_COLUMNS(N))(_tCurrent, tRsp);

		// Int32 iRet = ret;
		// CLASS_NAME::BOOST_PP_CAT(async_response_, FUNCTION_COLUMNS(N))(_tCurrent, iRet, tRsp);


		FDLOG() << ret << "|" << _sLog << "|" << JCETOSTR(tRsp) << endl;
	}

	virtual void BOOST_PP_CAT(BOOST_PP_CAT(callback_, FUNCTION_COLUMNS(N)), _exception)(RETURN_COLUMNS(N) ret)
	{
		Int32 iRet = FAILURE;
		taf::TL::TypeTraits<ARG_COLUMNS(N)>::ReferencedType  tRsp;

		LOG->debug() << ret << "|" << string(__FUNCTION__) << "|" << _sLog << "|" << JCETOSTR(tRsp) << endl;

		CLASS_NAME::BOOST_PP_CAT(async_response_, FUNCTION_COLUMNS(N))(_tCurrent, iRet, tRsp);

		FDLOG() << ret << "|" << _sLog << "|" << JCETOSTR(tRsp) << endl;
	}
#if N == BOOST_PP_DEC(MEMBER_FUNCTION_SIZE)
};
#undef MEMBER_FUNCTION_SIZE 
#undef MEMBER_FUNCTION_COLUMNS 
#undef CLASS_NAME 
#undef CLASS_SEQ 
#endif
#endif
