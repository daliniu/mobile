#include <sys/un.h>
#include <iostream>
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "servant/Communicator.h"
#include "TransFunc.h"
#include "XmlProxyCallback.h"
#include "TransProxyManager.h"

TC_Atomic g_RequestID;

XmlProxyCallback::XmlProxyCallback(const ServantPtr & servant, const JceCurrentPtr & current):ServantCallback("XmlProxyCallback", servant, current),_sizeResponse(0),_bHasResXml(false)
{
	_sResponseEncode = "gbk";
	_mapTransFunc.insert(std::make_pair("utf8", TransFunc::ToUTF8));
	_mapTransFunc.insert(std::make_pair("utf-8", TransFunc::ToUTF8));
}

XmlProxyCallback::~XmlProxyCallback()
{
	if (_bHasResXml == true)
	{
		LOG->debug() << __FUNCTION__ << " " << __LINE__ << " SIZE:" << _sizeResponse<< endl;
		_xmlRes.OutOfElem();
		_sendXmlResponse(_xmlRes, _sResponseEncode);
	}
}

void XmlProxyCallback::doRequest(const std::string & sXmlText)
{
	XmlProxyCallbackPtr cb = this;

	if (!_xmlReq.SetDoc(sXmlText) || !_xmlReq.FindElem("request"))
	{
		LOG->error() << " parse xml fail" << endl;
		_sendXmlMessage("parse xml fail");
		return ;
	}
	_bHasResXml = true;
	_xmlRes.AddElem("response");
	_xmlRes.IntoElem();

	_sResponseEncode = _xmlReq.GetAttrib("charset");

	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		if (_xmlReq.GetTagName() != "req") continue;

		MCD_STR sRequestID = _xmlReq.GetAttrib("id");
		MCD_STR sSvtName;
		MCD_STR sFunName;
		taf::JceOutputStream<taf::BufferWriter> os;
		
		for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
		{
			MCD_STR sTagName = _xmlReq.GetTagName();
			MCD_STR sTagData = _xmlReq.GetData();
			if (sTagName == "svt")
			{
				sSvtName = sTagData;
			}
			else if (sTagName == "fun")
			{
				sFunName = sTagData;
			}
			else if (sTagName == "para")
			{
				_writeParameters(os);
			}
		}
		_xmlReq.OutOfElem();

		_invoke(sRequestID, sSvtName, sFunName, os.getBuffer(), os.getLength(), cb);
	}
}


void XmlProxyCallback::doResponse(const int iRequestID, const std::vector<char> & vecBuffer)
{
	if (_mapRequest.find(iRequestID) == _mapRequest.end())
	{
		LOG->debug() << "L:" << __LINE__ << " not found request-id " << iRequestID << " in map" << endl;
		return ;
	}

	try
	{
		taf::JceInputStream<taf::BufferReader> is;
		is.setBuffer(vecBuffer);

		_xmlRes.AddElem("res");
		_xmlRes.AddAttrib("id", _mapRequest[iRequestID]);
		_xmlRes.IntoElem();

		_xmlReq.ResetPos();
		_xmlReq.FindElem();
		_xmlReq.IntoElem();

		while (_xmlReq.FindElem("req"))
		{
			if (_xmlReq.GetAttrib("id") != _mapRequest[iRequestID]) continue;

			_xmlReq.IntoElem();
			while (_xmlReq.FindElem())
			{
				MCD_STR sTagName = _xmlReq.GetTagName();
				if (sTagName == "ret" || sTagName == "return")
				{
					_readReturn(is);
				}
				else if (sTagName == "para" || sTagName == "parameters")
				{
					_readParameters(is);
				}
			}
			_xmlReq.OutOfElem();
			break;
		}

		_xmlReq.OutOfElem();
		_xmlRes.OutOfElem();

		if (++_sizeResponse == _mapRequest.size())
		{
			_bHasResXml = false;
			_xmlRes.OutOfElem();
			_sendXmlResponse(_xmlRes, _sResponseEncode);
		}
	}
	catch (JceDecodeException & e)
	{
		LOG->debug() << "L:" << __LINE__ << " decode error: " << e.what() << endl;
	}
	catch (exception & e)
	{
		LOG->debug() << "L:" << __LINE__ << " error:" << e.what() << endl;
	}
	catch (...)
	{
		LOG->debug() << "L:" << __LINE__ << " unknown exception error" << endl;
	}
}


/************************************************************************************************** 
 **	内部使用函数
 */

void XmlProxyCallback::_invoke(const std::string & sRequestID, const std::string & sSvtName, const std::string & sFunName, const char * pBuffer, const size_t sizeBuffer, const XmlProxyCallbackPtr & cb)
{
	LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|SVT:" << sSvtName << "|FUN:" << sFunName << endl;
	//输入参数检查
	if (sRequestID.empty() || sSvtName.empty() || sFunName.empty())
	{
		LOG->error() << "L:" << __LINE__ << " request-id or servant or function empty!" << endl;
		_addError(sRequestID, sSvtName, sFunName, "req invalid, request-id or servant or function empty!");
		return ;
	}

	//获取代理对象
	ServantPrx proxy = TransProxyManager::getInstance()->getProxy(sSvtName);
	if (!proxy) { _addError(sRequestID, sSvtName, sFunName, "not found servantproxy"); return ; }

	//用自己的requestid, 回来的时候好匹配
	int iRequestID = g_RequestID.add(1);
	try
    {
		_mapRequest[iRequestID] = sRequestID;

		proxy->rpc_call_async(iRequestID, sFunName, pBuffer, sizeBuffer, cb);
	}
	catch (exception & e)
	{
		_mapRequest.erase(iRequestID);
		_addError(sRequestID, sSvtName, sFunName, e.what());
		LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|SVT:" << sSvtName << "|FUN:" << sFunName << "|ERROR:" << e.what() << endl;
	}
	catch (...)
	{
		_mapRequest.erase(iRequestID);
		_addError(sRequestID, sSvtName, sFunName, "unknown error");
		LOG->debug() << "F:" << __FUNCTION__ << "|L:" << __LINE__ << "|SVT:" << sSvtName << "|FUN:" << sFunName << "|ERROR: unknown exception" << endl;
	}
}


void XmlProxyCallback::_readReturn(taf::JceInputStream<taf::BufferReader> & is)
{
	_xmlRes.AddElem("ret");

	_xmlRes.IntoElem();
	_xmlReq.IntoElem();

	while (_xmlReq.FindElem())
	{
		int iTag = 0;
		_readField(iTag, true, _xmlReq.GetTagName(), is);
	}

	_xmlReq.OutOfElem();
	_xmlRes.OutOfElem();
}


void XmlProxyCallback::_readField(int &iTag, bool isRequire, const MCD_STR & sTagName, taf::JceInputStream<taf::BufferReader> & is)
{
	if (TC_Common::lower(sTagName) == "void")
	{
		//empty
	}
	else if (TC_Common::lower(sTagName) == "bool")
	{
		taf::Bool bValue;
		if (!isRequire)
		{
			bValue = TC_Common::lower(_xmlReq.GetData()) == "true"?true:false;
		}

		is.read(bValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), bValue?"true":"false");            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "byte")
	{
		taf::Char cValue;
		if (!isRequire)
		{
			cValue = TC_Common::x2c(_xmlReq.GetData());
		}

		is.read(cValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::bin2str(TC_Common::tostr(cValue)));
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "short")
	{
		taf::Short siValue = isRequire?0:TC_Common::strto<taf::Short>(_xmlReq.GetData());

		is.read(siValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName),TC_Common::tostr(siValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "int")
	{
		taf::Int32 iValue = isRequire?0:TC_Common::strto<taf::Int32>(_xmlReq.GetData());

		is.read(iValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::tostr(iValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "long")
	{
		taf::Int64 lValue = isRequire?0:TC_Common::strto<taf::Int64>(_xmlReq.GetData());

		is.read(lValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::tostr(lValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "float")
	{
		taf::Float fValue = isRequire?0:TC_Common::strto<taf::Float>(_xmlReq.GetData());

		is.read(fValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::tostr(fValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "double")
	{
		taf::Double dValue = isRequire?0:TC_Common::strto<taf::Double>(_xmlReq.GetData());

		is.read(dValue,iTag,isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::tostr(dValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "string")
	{
		string sValue = isRequire?"":_xmlReq.GetData();

		is.read(sValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), sValue);            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "enum")
	{
		taf::Int32 iValue = isRequire?0:TC_Common::strto<taf::Int32>(_xmlReq.GetData());

		is.read(iValue, iTag, isRequire);

		_xmlRes.AddElem(TC_Common::lower(sTagName), TC_Common::tostr(iValue));            
		_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
	}
	else if (TC_Common::lower(sTagName) == "struct")
	{
		if (is.skipToTag(iTag))
		{
			_xmlRes.AddElem(TC_Common::lower(sTagName));            
			_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));

			DataHead h;
			h.readFrom(is);
			if (h.getType() != DataHead::eStructBegin)
			{
				char s[64];
				snprintf(s, sizeof(s), "read 'struct' type mismatch, tag: %d, get type: %d.", iTag, h.getType());
				throw JceDecodeException(s);
			}

			_xmlRes.IntoElem();
			_xmlReq.IntoElem();
			while (_xmlReq.FindElem())
			{
				MCD_STR sSubTagName = _xmlReq.GetTagName();
				int iSubTag = TC_Common::strto<int>(_xmlReq.GetAttrib("tag"));

				MCD_STR sSubRequire = _xmlReq.GetAttrib("isRequire");
				if (sSubRequire == "")
				{
					sSubRequire = "true";
				}
				bool bSubRequire = TC_Common::lower(sSubRequire) == "true";

				_readField(iSubTag, bSubRequire, sSubTagName, is);
			}
			_xmlReq.OutOfElem();
			_xmlRes.OutOfElem();

			is.skipToStructEnd();
		}
		else if (isRequire)
		{
			char s[64];
			snprintf(s, sizeof(s), "require field not exist, tag: %d", iTag);
			throw JceDecodeException(s);
		}
	}
	else if (TC_Common::lower(sTagName) == "vector")
	{
		if (is.skipToTag(iTag))
		{
			DataHead h;
			h.readFrom(is);
			switch (h.getType())
			{
				case DataHead::eList:
				{
					_xmlRes.AddElem(TC_Common::lower(sTagName));            
					_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));

					Int32 size;
					is.read(size, 0);
					if (size < 0)
					{
						char s[128];
						snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, size: %d", iTag, h.getType(), size);
						throw JceDecodeException(s);
					}

					_xmlRes.IntoElem();
					_xmlReq.IntoElem();
					for (Int32 i = 0; i < size; ++i)
					{
						_xmlReq.ResetMainPos();
						if (_xmlReq.FindElem())
						{
							MCD_STR innerTagName = _xmlReq.GetTagName();            
							int innerTag = 0;
							MCD_STR strInnerRequire = _xmlReq.GetAttrib("isRequire");
							if (strInnerRequire == "")
							{
								strInnerRequire = "true";
							}
							bool InnerIsRequire = TC_Common::lower(strInnerRequire) == "true";
							_readField(innerTag, InnerIsRequire, innerTagName, is);
						}
					}
					_xmlReq.OutOfElem();
					_xmlRes.OutOfElem();
				}
				break;

				default:
				{
					char s[64];
					snprintf(s, sizeof(s), "read 'vector' type mismatch, tag: %d, get type: %d.", iTag, h.getType());
					throw JceDecodeException(s);
				}
			}
		}
		else if (isRequire)
		{
			char s[64];
			snprintf(s, sizeof(s), "require field not exist, tag: %d", iTag);
			throw JceDecodeException(s);
		}
	}
	else if (TC_Common::lower(sTagName) == "vector_char")
	{
		if (is.skipToTag(iTag))
		{
			DataHead h;
			h.readFrom(is);
			switch (h.getType())
			{
				case DataHead::eSimpleList:
				{

					DataHead hh;
					hh.readFrom(is);
					if (hh.getType() != DataHead::eChar)
					{
						char s[128];
						snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d, %d", iTag, h.getType(), hh.getType());
						throw JceDecodeException(s);
					}
					Int32 size;
					is.read(size, 0);
					if (size < 0)
					{
						char s[128];
						snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, %d, size: %d", iTag, h.getType(), hh.getType(), size);
						throw JceDecodeException(s);
					}

					std::vector<Char> v;
					v.resize(size);
					is.readBuf(&v[0], size);

					string strHex = TC_Common::bin2str((const void *)&v[0],v.size(),"",0);

					_xmlRes.AddElem("vector_char", strHex);            
					_xmlRes.AddAttrib("tag", TC_Common::tostr(iTag));
				}
				break;
			default:
				{
					char s[64];
					snprintf(s, sizeof(s), "read 'vector' type mismatch, tag: %d, get type: %d.", iTag, h.getType());
					throw JceDecodeException(s);
				}
			}
		}
		else if (isRequire)
		{
			char s[64];
			snprintf(s, sizeof(s), "require field not exist, tag: %d", iTag);
			throw JceDecodeException(s);
		}
	}
	else if (TC_Common::lower(sTagName) == "map")
	{
		if (is.skipToTag(iTag))
		{
			_xmlRes.AddElem(TC_Common::lower(sTagName));            
			_xmlRes.AddAttrib("tag",TC_Common::tostr(iTag));

			DataHead h;
			h.readFrom(is);
			switch (h.getType())
			{
			case DataHead::eMap:
				{
					Int32 size;
					is.read(size, 0);
					if (size < 0)
					{
						char s[128];
						snprintf(s, sizeof(s), "invalid map, tag: %d, size: %d", iTag, size);
						throw JceDecodeException(s);
					}

					_xmlReq.IntoElem();//into map
					_xmlRes.IntoElem();//into map

					for (Int32 i = 0; i < size; ++i)
					{
						_xmlRes.AddElem("entry");
						_xmlRes.IntoElem();//into entry

						_xmlReq.ResetMainPos();
						if (_xmlReq.FindElem())//find entry
						{
							_xmlReq.IntoElem();//into entry
							for (Int32 j = 0 ; j < 2; j++)
							{
								if (_xmlReq.FindElem())
								{
									MCD_STR innerTagName = _xmlReq.GetTagName();            
									int innerTag = j;

									MCD_STR strRequire = _xmlReq.GetAttrib("isRequire");
									if (strRequire == "")
									{
										strRequire = "true";
									}
									bool isRequire = TC_Common::lower(strRequire) == "true";

									//TODO test
									_readField(innerTag,isRequire,innerTagName,is);
								}
							}
							_xmlReq.OutOfElem();//out entry                            
						}
						_xmlRes.OutOfElem();//out entry
					}

					_xmlRes.OutOfElem();//out map
					_xmlReq.OutOfElem();//out map
				}
				break;
			default:
				{
					char s[64];
					snprintf(s, sizeof(s), "read 'map' type mismatch, tag: %d, get type: %d.", iTag, h.getType());
					throw JceDecodeException(s);
				}
			}
		}
		else if (isRequire)
		{
			char s[64];
			snprintf(s, sizeof(s), "require field not exist, tag: %d", iTag);
			throw JceDecodeException(s);
		}
	}
}


void XmlProxyCallback::_readParameters(taf::JceInputStream<taf::BufferReader> &is)
{
	_xmlRes.AddElem("para");

	_xmlRes.IntoElem();
	_xmlReq.IntoElem();
	int iTag = 0 ;
	while (_xmlReq.FindElem())
	{
		iTag++;
		if (TC_Common::lower(_xmlReq.GetAttrib("out")) == "true")
		{
			_readField(iTag, true, _xmlReq.GetTagName(), is);
		}
	}
	_xmlReq.OutOfElem();
	_xmlRes.OutOfElem();
}


void XmlProxyCallback::_writeField(const int &iTag, const MCD_STR &sTagName, const MCD_STR &sData, taf::JceOutputStream<taf::BufferWriter> & os)
{
	if (TC_Common::lower(sTagName) == "bool")
	{
		taf::Bool bValue = TC_Common::upper(sData) == "TRUE";
		os.write(bValue, iTag);
	}
	else if (TC_Common::lower(sTagName) == "byte")
	{
		taf::Char cValue;
		if (sData.size() > 1)
		{
			cValue = TC_Common::x2c(sData);
		}
		os.write(cValue,iTag);
	}
	else if (TC_Common::lower(sTagName) == "short")
	{
		os.write(TC_Common::strto<taf::Short>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "int")
	{
		os.write(TC_Common::strto<taf::Int32>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "long")
	{
		os.write(TC_Common::strto<taf::Int64>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "float")
	{
		os.write(TC_Common::strto<taf::Float>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "double")
	{
		os.write(TC_Common::strto<taf::Double>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "string")
	{
		os.write(sData, iTag);
	}
	else if (TC_Common::lower(sTagName) == "enum")
	{
		os.write(TC_Common::strto<taf::Int32>(sData), iTag);
	}
	else if (TC_Common::lower(sTagName) == "struct")
	{
		DataHead h(DataHead::eStructBegin, iTag);
		h.writeTo(os);

		_xmlReq.IntoElem();
		while (_xmlReq.FindElem())
		{
			int iSubTag = TC_Common::strto<int>(_xmlReq.GetAttrib("tag"));
			_writeField(iSubTag, _xmlReq.GetTagName(), _xmlReq.GetData(), os);
		}
		_xmlReq.OutOfElem();

		h.setType(DataHead::eStructEnd);
		h.setTag(0);
		h.writeTo(os);
	}
	else if (TC_Common::lower(sTagName) == "vector")
	{
		DataHead h(DataHead::eList, iTag);
		h.writeTo(os);

		_xmlReq.IntoElem();
		int n = 0;
		taf::JceOutputStream<taf::BufferWriter> vectorCt;        
		while (_xmlReq.FindElem())
		{
			++ n;
			int iSubTag = 0;
			_writeField(iSubTag, _xmlReq.GetTagName(), _xmlReq.GetData(), vectorCt);
		}
		_xmlReq.OutOfElem();

		os.write(n, 0);
		os.writeBuf(vectorCt.getBuffer(), vectorCt.getLength());
	}
	else if (TC_Common::lower(sTagName) == "vector_char")
	{
		DataHead h(DataHead::eSimpleList, iTag);
		h.writeTo(os);

		DataHead hh(DataHead::eChar, 0);
		hh.writeTo(os);

		string sBin = TC_Common::str2bin(_xmlReq.GetData());        
		os.write(sBin.length(), 0);                
		os.writeBuf(sBin.c_str(), sBin.length());
	}
	else if (TC_Common::lower(sTagName) == "map")
	{
		DataHead h(DataHead::eMap, iTag);
		h.writeTo(os);

		_xmlReq.IntoElem();//into map
		int n = 0 ;
		taf::JceOutputStream<taf::BufferWriter> mapCt;        
		while (_xmlReq.FindElem())//find entry
		{
			++ n;
			_xmlReq.IntoElem();//into entry
			for (int i = 0 ; i < 2; i++)
			{
				if (_xmlReq.FindElem())//find key or value
				{
					_writeField(i, _xmlReq.GetTagName(), _xmlReq.GetData(), mapCt);
				}
			}
			_xmlReq.OutOfElem();//out entry
		}
		_xmlReq.OutOfElem();//out map

		os.write(n,0);        
		os.writeBuf(mapCt.getBuffer(), mapCt.getLength());
	}
}

/**
 * 
 * 将xml的parameters参数写入到jceOutputStream中
 * 
 * @param inXml 
 * @param _os 
 */
void XmlProxyCallback::_writeParameters(taf::JceOutputStream<taf::BufferWriter> &os)
{
	_xmlReq.IntoElem();
	int iTag = 0;
	while (_xmlReq.FindElem())
	{
		iTag ++;

		//忽略out参数
		if (TC_Common::lower(_xmlReq.GetAttrib("in")) == "true")
		{
			_writeField(iTag, _xmlReq.GetTagName(), _xmlReq.GetData(), os);
		}
	}

	_xmlReq.OutOfElem();
}

void XmlProxyCallback::_sendXmlResponse(const CMarkup &outXml, const std::string & sEncode)
{
	std::map<std::string, TTransFunc>::iterator it = _mapTransFunc.find(sEncode);

	TC_HttpResponse response;
	if (it != _mapTransFunc.end())
	{
		std::string s;
		bool bTrans = (it->second)(outXml.GetDoc(), s);
		response.setResponse(200, "OK", bTrans?s:outXml.GetDoc());
	}
	else
	{
		response.setResponse(200, "OK", outXml.GetDoc());
	}
	response.setConnection("close");

	std::string buffer = response.encode();
	getCurrent()->sendResponse(buffer.data(), buffer.size());
}

void XmlProxyCallback::_sendXmlMessage(const std::string & sMessage)
{
	CMarkup xmlMessage;

    xmlMessage.AddElem("response");
    xmlMessage.IntoElem();
    xmlMessage.AddElem("message", sMessage);
	xmlMessage.OutOfElem();

	_sendXmlResponse(xmlMessage, _sResponseEncode);
}

void XmlProxyCallback::_addError(const std::string & sRequestID, const std::string & sSvtName, const std::string & sFunName, const std::string & sError)
{
	_xmlRes.AddElem("res");
	_xmlRes.AddAttrib("id", sRequestID);

    _xmlRes.IntoElem();

	_xmlRes.AddElem("svt", sSvtName);
	_xmlRes.AddElem("fun", sFunName);
	_xmlRes.AddElem("err", sError);

	_xmlRes.OutOfElem();
}

