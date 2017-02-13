#include "xmltrans.h"
#include <sstream>

static std::string trans_tolower(const std::string &s)
{
	std::string sString = s;
	for (std::string::iterator iter = sString.begin(); iter != sString.end(); ++iter)
	{
		*iter = tolower(*iter);
	}

	return sString;
}

template<typename T>
std::string trans_tostr(const T &t)
{
	std::ostringstream sBuffer;
	sBuffer << t;
	return sBuffer.str();
}

static char trans_x2c(const std::string &sWhat)
{
	register char digit;

	if (sWhat.length() < 2)
	{
		return '\0';
	}

	digit = (sWhat[0] >= 'A' ? ((sWhat[0] & 0xdf) - 'A')+10 : (sWhat[0] - '0'));
	digit *= 16;
	digit += (sWhat[1] >= 'A' ? ((sWhat[1] & 0xdf) - 'A')+10 : (sWhat[1] - '0'));

	return(digit);
}

template<typename T>
T trans_strto(const std::string &sStr)
{
	std::istringstream sBuffer(sStr);

	T t;
	sBuffer >> t;

	return t;
}

//参照phorix的优化
static char c_b2s[256][4]={"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f","10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f","20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f","30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f","40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f","50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f","60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f","70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f","80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f","90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f","a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf","c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df","e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef","f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"};

static std::string trans_bin2str(const void *buf, size_t len, const std::string &sSep, size_t lines)
{
    if(buf == NULL || len <=0 )
    {
        return "";
    }

    std::string sOut;
    const unsigned char *p = (const unsigned char *) buf;

    for (size_t i = 0; i < len; ++i, ++p)
    {
        sOut += c_b2s[*p][0];
        sOut += c_b2s[*p][1];
        sOut += sSep;

        //换行
        if ((lines != 0) && ((i+1) % lines == 0))
        {
            sOut += "\n";
        }
    }

    return sOut;
}

static std::string trans_bin2str(const std::string &sBinData, const std::string &sSep = "", size_t lines = 0)
{
    return trans_bin2str((const void *)sBinData.data(), sBinData.length(), sSep, lines);
}

/************************************************************************************************** 
 ** 对外转换函数 
 **  
 */
XmlTrans::XmlTrans()
{
	_init();
}

void XmlTrans::_init()
{
	iVersion 		= 2; 
	cPacketType 	= 0; 
	iMessageType	= 0; 
	iRequestId 		= 0; 
	sServantName 	= ""; 
	sFuncName 		= ""; 
	iTimeout 		= 0; 
	sBuffer.clear(); 
	context.clear(); 
	status.clear();
}


bool XmlTrans::parse(const std::string & sXmlText)
{
	if (!_xmlReq.SetDoc(sXmlText))
	{
		return false;
	}

	return true;
}

void XmlTrans::xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode)
{
	_xml2wup(mapRequest, sResEncode, true, false);
}

void XmlTrans::xml2res(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode)
{
	_xml2wup(mapRequest, sResEncode, true, true);
}

void XmlTrans::_xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode, bool bWithTaf, bool bWithReturn)
{
	_xmlReq.ResetPos();
	if (!_xmlReq.FindElem("request"))
	{
		throw std::runtime_error("not found xml root <request></request>");
	}
	sResEncode = _xmlReq.GetAttrib("charset");

	taf::JceOutputStream<taf::BufferWriter> os;
	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		if (_xmlReq.GetTagName() != "req") continue;

		MCD_STR sRequestID = _xmlReq.GetAttrib("id");
		MCD_STR sSvtName;
		MCD_STR sFunName;
		

		MAP_DATA mapData;
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
			else if (sTagName == "ret" && bWithReturn)
			{
				_xml2wupReturn(mapData);
			}
			else if (sTagName == "para")
			{
				_xml2wup(mapData);
			}
		}
		_xmlReq.OutOfElem();


		//转换map
		os.reset();
		os.write(mapData, 0);

		XmlRequest & request = mapRequest[sRequestID];
		request.sSvtName = sSvtName;
		request.sFunName = sFunName;
		if (bWithTaf == false)
		{
			request.vecBuff.assign(os.getBuffer(), os.getBuffer() + os.getLength());
		}
		else
		{
			_init();
			iRequestId		= trans_strto<int>(sRequestID);
			sServantName 	= sSvtName; 
			sFuncName 		= sFunName; 
			sBuffer.assign(os.getBuffer(), os.getBuffer() + os.getLength());

			os.reset();
			writeTo(os);

			taf::Int32 iHeaderLen = htonl(sizeof(taf::Int32) + os.getLength());

			request.vecBuff.resize(sizeof(taf::Int32) + os.getLength());
			memcpy(&request.vecBuff[0], &iHeaderLen, sizeof(taf::Int32));
			memcpy(&request.vecBuff[sizeof(taf::Int32)], os.getBuffer(), os.getLength());
		}
	}
}

void XmlTrans::_xml2wup(MAP_DATA & mapData)
{
	taf::JceOutputStream<taf::BufferWriter> os;

	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		//如果是类型描述，则直接跳过
		MCD_STR sDesc = _xmlReq.GetAttrib("desc");
		if ((_xmlReq.GetAttrib("out") == "true") && (sDesc.empty() || sDesc == "true")) continue;

		MCD_STR sVName;
		MCD_STR sCName;

		os.reset();
		_writeField(0, sVName, sCName, os);

		MAP_DATA_ITEM_BUFF & v = mapData[sVName][sCName];
		v.assign(os.getBuffer(), os.getBuffer() + os.getLength());
	}
	_xmlReq.OutOfElem();
}

void XmlTrans::_xml2wupReturn(MAP_DATA & mapData)
{
	taf::JceOutputStream<taf::BufferWriter> os;
	
	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		MCD_STR sVName;
		MCD_STR sCName;

		_writeField(0, sVName, sCName, os);

		MAP_DATA_ITEM_BUFF & v = mapData[""][sCName];
		v.assign(os.getBuffer(), os.getBuffer() + os.getLength());
	}

	_xmlReq.OutOfElem();
}

void XmlTrans::xml2jce(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode)
{
	_xmlReq.ResetPos();
	if (!_xmlReq.FindElem("request"))
	{
		throw std::runtime_error("not found xml root <request></request>");
	}
	sResEncode = _xmlReq.GetAttrib("charset");
	sResEncode = _xmlReq.GetAttrib("charset");

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
				_xml2jce(os);
			}
		}
		_xmlReq.OutOfElem();

		XmlRequest & request = mapRequest[sRequestID];
		request.sSvtName = sSvtName;
		request.sFunName = sFunName;
		request.vecBuff.assign(os.getBuffer(), os.getBuffer() + os.getLength());
	}
}


void XmlTrans::_xml2jce(taf::JceOutputStream<taf::BufferWriter> & osjce)
{
	int iTag = 0;
	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		iTag ++;

		_writeField(iTag, osjce);
	}
	_xmlReq.OutOfElem();
}

void XmlTrans::wup2xml(const std::string & sResponse, std::string & sXmlText)
{
	_wup2xml(sResponse.c_str(), sResponse.size(), sXmlText, true);
}

void XmlTrans::wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText)
{
	_wup2xml(szResponse, sizeLength, sXmlText, true);
}
void XmlTrans::_wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText, bool bWithTaf)
{
	_xmlRes.RemoveElem();

	_xmlRes.AddElem("response");
	_xmlRes.IntoElem(); 

	_wup2xml(szResponse, sizeLength, bWithTaf);

	_xmlRes.OutOfElem();
	sXmlText = _xmlRes.GetDoc();
}

void XmlTrans::_wup2xml(const char * szResponse, size_t sizeLength, bool bWithTaf)
{
	if(sizeLength < sizeof(taf::Int32)) throw runtime_error("packet length too short");
	
	for (size_t size_pos = 0; size_pos < sizeLength; )
	{
		_init();

		int i_length = ::ntohl((int) *((int*)(szResponse + size_pos)));
        if (i_length <= 0 || size_pos + i_length > sizeLength)
        {
            throw std::runtime_error("length error");
        }
		size_pos += i_length;

		taf::JceInputStream<taf::BufferReader> is;
		is.setBuffer(szResponse + size_pos - i_length + sizeof(taf::Int32), i_length - sizeof(taf::Int32));
		readFrom(is);

		is.reset();
		is.setBuffer(sBuffer);
		is.read(_mapRes, 0, true);
	
		std::string sRequestID = trans_tostr(iRequestId);

		_xmlRes.AddElem("res");
		_xmlRes.AddAttrib("id", sRequestID);
		_xmlRes.IntoElem();

		_xmlReq.ResetPos();
		_xmlReq.FindElem();

		for (_xmlReq.IntoElem(); _xmlReq.FindElem("req"); )
		{
			if (_xmlReq.GetAttrib("id") != sRequestID) continue;

			for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
			{
				MCD_STR sTagName = _xmlReq.GetTagName();
				if (sTagName == "ret")
				{
					_wup2xmlReturn();
				}
				else if (sTagName == "para")
				{
					_wup2xmlPara();
				}
			}
			_xmlReq.OutOfElem();

			break;
		}
		_xmlRes.OutOfElem();
	}
}

void XmlTrans::_wup2xmlReturn()
{
	MAP_DATA::iterator it = _mapRes.find("");
	if (it == _mapRes.end() || it->second.size() < 1)
	{
		return ;
	}

	_xmlRes.AddElem("ret");

	_xmlRes.IntoElem();
	_xmlReq.IntoElem();

	taf::JceInputStream<taf::BufferReader> is;
	is.setBuffer(it->second.begin()->second);
	while (_xmlReq.FindElem())
	{
		_readField(0, true, _xmlReq.GetTagName(), is);
	}

	_xmlReq.OutOfElem();
	_xmlRes.OutOfElem();
}

void XmlTrans::_wup2xmlPara()
{
	_xmlRes.AddElem("para");
	_xmlRes.IntoElem();

	for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
	{
		if (trans_tolower(_xmlReq.GetAttrib("out")) == "true")
		{
			MAP_DATA::iterator it = _mapRes.find(_xmlReq.GetAttrib("vname"));
			if (it == _mapRes.end()) continue;
			if (it->second.size() < 1) continue;

			taf::JceInputStream<taf::BufferReader> is;
			is.setBuffer(it->second.begin()->second);

			_readField(0, true, _xmlReq.GetTagName(), is);
		}
	}
	_xmlReq.OutOfElem();

	_xmlRes.OutOfElem();
}

void XmlTrans::jce2xml(const std::map<std::string, std::vector<char> > & mapResponse, std::string & sXmlText)
{
	_xmlRes.RemoveElem();

	_xmlRes.AddElem("response");
	_xmlRes.IntoElem(); 

	for (std::map<std::string, std::vector<char> >::const_iterator it = mapResponse.begin(); it != mapResponse.end(); it++)
	{
		_jce2xml(it->first, it->second);
	}

	_xmlRes.OutOfElem();
	sXmlText = _xmlRes.GetDoc();
}

void XmlTrans::_jce2xml(const std::string & sRequestID, const std::vector<char> & vecBuff)
{
	_xmlRes.AddElem("res");
	_xmlRes.AddAttrib("id", sRequestID);
	_xmlRes.IntoElem();

	_xmlReq.ResetPos();
	_xmlReq.FindElem();

	for (_xmlReq.IntoElem(); _xmlReq.FindElem("req"); )
	{
		if (_xmlReq.GetAttrib("id") != sRequestID) continue;

		taf::JceInputStream<taf::BufferReader> is;
		is.setBuffer(vecBuff);

		for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
		{
			MCD_STR sTagName = _xmlReq.GetTagName();
			if (sTagName == "ret")
			{
				_jce2xmlReturn(is);
			}
			else if (sTagName == "para")
			{
				_jce2xmlPara(is);
			}
		}
		_xmlReq.OutOfElem();

		break;
	}
	_xmlRes.OutOfElem();
}


void XmlTrans::_jce2xmlReturn(taf::JceInputStream<taf::BufferReader> & is)
{
	_xmlRes.AddElem("ret");

	_xmlRes.IntoElem();
	_xmlReq.IntoElem();

	while (_xmlReq.FindElem())
	{
		_readField(0, true, _xmlReq.GetTagName(), is);
	}

	_xmlReq.OutOfElem();
	_xmlRes.OutOfElem();
}


void XmlTrans::_jce2xmlPara(taf::JceInputStream<taf::BufferReader> & is)
{
	_xmlRes.AddElem("para");
	_xmlRes.IntoElem();

	_xmlReq.IntoElem();
	for (int iTag = 1; _xmlReq.FindElem(); iTag++)
	{
		if (trans_tolower(_xmlReq.GetAttrib("out")) == "true")
		{
			_readField(iTag, true, _xmlReq.GetTagName(), is);
		}
	}
	_xmlReq.OutOfElem();

	_xmlRes.OutOfElem();
}


void XmlTrans::_readField(const int iTag, bool isRequire, const std::string & sTagName, taf::JceInputStream<taf::BufferReader> & is)
{
	if (trans_tolower(sTagName) == "void")
	{
		//empty
	}
	else if (trans_tolower(sTagName) == "bool")
	{
		taf::Bool bValue;
		if (!isRequire)
		{
			bValue = trans_tolower(_xmlReq.GetData()) == "true"?true:false;
		}

		is.read(bValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), bValue?"true":"false");            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "byte")
	{
		taf::Char cValue;
		if (!isRequire)
		{
			cValue = trans_x2c(_xmlReq.GetData());
		}

		is.read(cValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_bin2str(trans_tostr(cValue)));
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "short")
	{
		taf::Short siValue = isRequire?0:trans_strto<taf::Short>(_xmlReq.GetData());

		is.read(siValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName),trans_tostr(siValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "int")
	{
		taf::Int32 iValue = isRequire?0:trans_strto<taf::Int32>(_xmlReq.GetData());

		is.read(iValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_tostr(iValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "long")
	{
		taf::Int64 lValue = isRequire?0:trans_strto<taf::Int64>(_xmlReq.GetData());

		is.read(lValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_tostr(lValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "float")
	{
		taf::Float fValue = isRequire?0:trans_strto<taf::Float>(_xmlReq.GetData());

		is.read(fValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_tostr(fValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "double")
	{
		taf::Double dValue = isRequire?0:trans_strto<taf::Double>(_xmlReq.GetData());

		is.read(dValue,iTag,isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_tostr(dValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "string")
	{
		std::string sValue = isRequire?"":_xmlReq.GetData();

		is.read(sValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), sValue);            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "enum")
	{
		taf::Int32 iValue = isRequire?0:trans_strto<taf::Int32>(_xmlReq.GetData());

		is.read(iValue, iTag, isRequire);

		_xmlRes.AddElem(trans_tolower(sTagName), trans_tostr(iValue));            
		_xmlRes.AddAttrib("tag", trans_tostr(iTag));
	}
	else if (trans_tolower(sTagName) == "struct")
	{
		if (is.skipToTag(iTag))
		{
			_xmlRes.AddElem(trans_tolower(sTagName));            
			_xmlRes.AddAttrib("tag", trans_tostr(iTag));

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
				int iSubTag = trans_strto<int>(_xmlReq.GetAttrib("tag"));

				MCD_STR sSubRequire = _xmlReq.GetAttrib("isRequire");
				if (sSubRequire == "")
				{
					sSubRequire = "true";
				}
				bool bSubRequire = trans_tolower(sSubRequire) == "true";

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
	else if (trans_tolower(sTagName) == "vector")
	{
		if (is.skipToTag(iTag))
		{
			DataHead h;
			h.readFrom(is);
			switch (h.getType())
			{
			case DataHead::eList:
				{
					_xmlRes.AddElem(trans_tolower(sTagName));            
					_xmlRes.AddAttrib("tag", trans_tostr(iTag));

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
							bool InnerIsRequire = trans_tolower(strInnerRequire) == "true";
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
	else if (trans_tolower(sTagName) == "vector_char")
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

					_xmlRes.AddElem("vector_char", std::string(&v[0], size));
					_xmlRes.AddAttrib("tag", trans_tostr(iTag));
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
	else if (trans_tolower(sTagName) == "map")
	{
		if (is.skipToTag(iTag))
		{
			_xmlRes.AddElem(trans_tolower(sTagName));            
			_xmlRes.AddAttrib("tag",trans_tostr(iTag));

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
									bool isRequire = trans_tolower(strRequire) == "true";

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

/************************************************************************************************** 
 **	XML操作函数 
 ** 
 */
void XmlTrans::_writeField(int iTag, taf::JceOutputStream<taf::BufferWriter> & os)
{
	MCD_STR sVName;
	MCD_STR sCName;
	_writeField(iTag, sVName, sCName, os);
}

void XmlTrans::_writeField(int iTag, std::string & sVName, std::string & sCName, taf::JceOutputStream<taf::BufferWriter> & os)
{
	MCD_STR sTagName = _xmlReq.GetTagName();
	MCD_STR sTagData = _xmlReq.GetData();
	sVName = _xmlReq.GetAttrib("vname");
	sCName = sTagName;

	if (sTagName == "bool")
	{
		taf::Bool bValue = trans_tolower(sTagData) == "true";
		os.write(bValue, iTag);
	}
	else if (sTagName == "byte")
	{
		taf::Char cValue = sTagData.size() > 1?trans_x2c(sTagData):'\0';
		os.write(cValue, iTag);
	}
	else if (sTagName == "short")
	{
		os.write(trans_strto<taf::Short>(sTagData), iTag);
	}
	else if (sTagName == "int")
	{
		sCName = "int32";
		os.write(trans_strto<taf::Int32>(sTagData), iTag);
	}
	else if (trans_tolower(sTagName) == "long")
	{
		sCName = "int64";
		os.write(trans_strto<taf::Int64>(sTagData), iTag);
	}
	else if (trans_tolower(sTagName) == "float")
	{
		os.write(trans_strto<taf::Float>(sTagData), iTag);
	}
	else if (trans_tolower(sTagName) == "double")
	{
		os.write(trans_strto<taf::Double>(sTagData), iTag);
	}
	else if (trans_tolower(sTagName) == "string")
	{
		os.write(sTagData, iTag);
	}
	else if (trans_tolower(sTagName) == "enum")
	{
		os.write(trans_strto<taf::Int32>(sTagData), iTag);
	}
	else if (trans_tolower(sTagName) == "struct" )
	{
		sCName = _xmlReq.GetAttrib("cname");

		DataHead h(DataHead::eStructBegin, iTag);
		h.writeTo(os);

		_xmlReq.IntoElem();
		while (_xmlReq.FindElem())
		{
			if (_xmlReq.GetAttrib("desc") == "true") continue;

			int iSubTag = trans_strto<int>(_xmlReq.GetAttrib("tag"));
			_writeField(iSubTag, os);
		}
		_xmlReq.OutOfElem();

		h.setType(DataHead::eStructEnd);
		h.setTag(0);
		h.writeTo(os);
	}
	else if (trans_tolower(sTagName) == "vector_char")
	{
		sCName = "list<char>";

		DataHead h(DataHead::eSimpleList, iTag);
		h.writeTo(os);

		DataHead hh(DataHead::eChar, 0);
		hh.writeTo(os);

		std::string sBin = _xmlReq.GetData();
		os.write(sBin.size(), 0);
		os.writeBuf(sBin.c_str(), sBin.size());
	}
	else if (trans_tolower(sTagName) == "vector")
	{
		std::string sSubVName;
		std::string sSubCName;

		int n = 0;
		taf::JceOutputStream<taf::BufferWriter> osvec;
		for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
		{
			bool bDesc = (_xmlReq.GetAttrib("desc") == "true");
			if (bDesc == true && sSubCName.empty())
			{
				taf::JceOutputStream<taf::BufferWriter> ostemp;
				_writeField(0, sSubVName, sSubCName, ostemp);
			}
			else if (bDesc == false)
			{
				++ n;
				_writeField(0, sSubVName, sSubCName, osvec);
			}
		}
		_xmlReq.OutOfElem();

		sCName = "list<" + sSubCName + ">";

		DataHead h(DataHead::eList, iTag);
		h.writeTo(os);
		os.write(n, 0);
		os.writeBuf(osvec.getBuffer(), osvec.getLength());
	}
	else if (trans_tolower(sTagName) == "map")
	{
		std::string sSubVName;
		std::string sSubCNameF;
		std::string sSubCNameS;

		int n = 0 ;
		taf::JceOutputStream<taf::BufferWriter> osmap;        
		for (_xmlReq.IntoElem(); _xmlReq.FindElem(); )
		{
			if (_xmlReq.GetAttrib("desc") == "true") continue;

			++ n;
			_xmlReq.IntoElem();//into entry
			for (int i = 0 ; i < 2; i++)
			{
				if (_xmlReq.FindElem())//find key or value
				{
					_writeField(i, sSubVName, i == 0?sSubCNameF:sSubCNameS, osmap);
				}
			}
			_xmlReq.OutOfElem();//out entry
		}
		_xmlReq.OutOfElem();//out map

		sCName = "map<" + sSubCNameF + "," + sSubCNameS + ">";

		DataHead h(DataHead::eMap, iTag);
		h.writeTo(os);
		os.write(n,0);        
		os.writeBuf(osmap.getBuffer(), osmap.getLength());
	}
}


void XmlTrans::getServant(const char * szBuff, size_t sizeLength, std::vector<XmlRequest> & vecServnat)
{
	if(sizeLength < sizeof(taf::Int32)) throw runtime_error("packet length too short");
	
	for (size_t size_pos = 0; size_pos < sizeLength; )
	{
		_init();

		int i_length = ::ntohl((int) *((int*)(szBuff + size_pos)));
        if (i_length < 0 || size_pos + i_length > sizeLength)
        {
            throw std::runtime_error("length error");
        }
		size_pos += i_length;

		taf::JceInputStream<taf::BufferReader> is;
		is.setBuffer(szBuff + size_pos - i_length + sizeof(taf::Int32), i_length - sizeof(taf::Int32));
		readFrom(is);
		
		XmlRequest res;
		res.sSvtName	= sServantName;
		res.sFunName	= sFuncName;

		vecServnat.push_back(res);
	}
}

