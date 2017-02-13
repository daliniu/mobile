#ifndef _XML_PROXY_CALL_BACK_H_
#define _XML_PROXY_CALL_BACK_H_

#include <iostream>
#include "servant/Application.h"
#include "Markup.h"
#include "util/tc_http.h"

using namespace taf;
using namespace std;

namespace taf
{
	class XmlProxyCallback;
	typedef TC_AutoPtr<XmlProxyCallback> XmlProxyCallbackPtr;

	class XmlProxyCallback: public taf::ServantCallback
	{
	public:
		XmlProxyCallback(const ServantPtr & servant, const JceCurrentPtr & current);
		~XmlProxyCallback();

	public:
		void doRequest(const std::string & sXmlText);
		void doResponse(const int iRequestID, const std::vector<char> & vecBuffer);

	private:
		void _invoke(const std::string & sRequestID, const std::string & sSvtName, const std::string & sFunName, const char * pBuffer, const size_t sizeBuffer, const XmlProxyCallbackPtr & cb);

		void _readReturn(taf::JceInputStream<taf::BufferReader> & is);

		void _readField(int &iTag, bool isRequire, const MCD_STR &sTagName, taf::JceInputStream<taf::BufferReader> & is);
		void _readParameters(taf::JceInputStream<taf::BufferReader> & is);

		void _writeField(const int &iTag, const MCD_STR &sTagName, const MCD_STR &sData, taf::JceOutputStream<taf::BufferWriter> & os);
		void _writeParameters(taf::JceOutputStream<taf::BufferWriter> & os);

		void _sendXmlResponse(const CMarkup & outXml, const std::string & sEncode);
		void _sendXmlMessage(const std::string & sMessage);
		void _addError(const std::string & sRequestID, const std::string & sSvtName, const std::string & sFunName, const std::string & sError);

	private:
		typedef TC_Functor<bool, TL::TLMaker<const string&, string&>::Result> TTransFunc;
		std::map<std::string, TTransFunc> _mapTransFunc;
		std::string _sResponseEncode;	//返回编码字符集，目前支持GBK(默认),UTF-8

		std::map<int, std::string> _mapRequest;	//发送到对应SVR请求与XML请求文件中request的映射关系
		std::map<int, std::string>::size_type _sizeResponse; //目前已经处理完毕的请求数目

		bool _bHasResXml;	//是否已经回应请求包了

		CMarkup _xmlReq;	//请求XML文件
		CMarkup _xmlRes;	//结果XML文件
	};
}
////////////////////////////////////////////
#endif
