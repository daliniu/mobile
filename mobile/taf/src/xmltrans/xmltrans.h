#ifndef __XMLTRANS__H__
#define __XMLTRANS__H__

#include <string>
#include <vector>
#include <map>
#include "Markup.h"
#include "jce/Jce.h"
#include "jce/RequestF.h"

using namespace taf;

class XmlTrans : public taf::RequestPacket
{
public:
	struct XmlRequest
	{
		std::string sSvtName;
		std::string sFunName;

		std::vector<char> vecBuff;
	};
public:
	XmlTrans();

public:
	/**
	 * 传入XML内容，进行分析
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param sXml  XML文档内容
	 * 
	 * @return bool 成功返回true，否则false
	 */
	bool parse(const std::string & sXml);

	/**
	 * 将XML转换成WUP协议编码的字符串
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapRequest 轻轻XML文档分析出的所有请求
	 * @param sResEncode 请求要求的结果XML文档的编码方式
	 */
	void xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * 将XML转换成JCE协议编码的字符串
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapRequest 
	 * @param sResEncode 
	 */
	void xml2jce(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * 读取模拟结果XML，转换成WUP
	 * 
	 * @author kevintian (2010-8-27)
	 * 
	 * @param mapRequest 
	 * @param sResEncode 
	 */
	void xml2res(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * 将WUP协议编码的字符串转换成XML
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapResponse 
	 *  				  调用请求之后获得的返回map<请求ID，请求的返回数据>
	 * @param sXmlText 
	 */
	void wup2xml(const std::string & sResponse, std::string & sXmlText);

	/**
	 * 将WUP协议编码的字符串转换成XML
	 * 
	 * @author kevintian (2010-8-18)
	 * 
	 * @param szResponse 
	 * @param sizeLength 
	 * @param sXmlText 
	 */
	void wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText);

	/**
	 * 获取返回结果中WUP请求的信息
	 * 
	 * @author kevintian (2010-8-23)
	 * 
	 * @param szBuff 
	 * @param sizeLength 
	 * @param vecServnat 
	 */
	void getServant(const char * szBuff, size_t sizeLength, std::vector<XmlRequest> & vecServnat);

	/**
	 * 将JCE协议编码的字符串转换成XML
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapResponse 
	 *  				  调用请求之后获得的返回map<请求ID，请求的返回数据>
	 * @param sXmlText 
	 */
	void jce2xml(const std::map<std::string, std::vector<char> > & mapResponse, std::string & sXmlText);

private:
	typedef std::vector<char> MAP_DATA_ITEM_BUFF;
    typedef std::map<std::string, MAP_DATA_ITEM_BUFF> MAP_DATA_ITEM;
	typedef std::map<std::string, MAP_DATA_ITEM> MAP_DATA;

	void _xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode, bool bWithTaf, bool bWithReturn);
	void _xml2wup(MAP_DATA & mapData);
	void _xml2wupReturn(MAP_DATA & mapData);
	void _xml2jce(taf::JceOutputStream<taf::BufferWriter> & osjce);

	void _writeField(int iTag, taf::JceOutputStream<taf::BufferWriter> & os);
	void _writeField(int iTag, std::string & sVName, std::string & sCName, taf::JceOutputStream<taf::BufferWriter> & os);
	void _readField(const int iTag, bool isRequire, const std::string & sTagName, taf::JceInputStream<taf::BufferReader> & is);

	void _jce2xml(const std::string & sRequestID, const std::vector<char> & vecBuff);	
	void _jce2xmlReturn(taf::JceInputStream<taf::BufferReader> & is);
	void _jce2xmlPara(taf::JceInputStream<taf::BufferReader> &is);

	void _wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText, bool bWithTaf);
	void _wup2xml(const char * szResponse, size_t sizeLength, bool bWithTaf);
	void _wup2xmlReturn();
	void _wup2xmlPara();

	void _init();
private:
	CMarkup _xmlReq;	//请求XML文件
	CMarkup _xmlRes;	//结果XML文件

	MAP_DATA _mapRes;	//进行WUP解码的数据结构
};

#endif

