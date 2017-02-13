#include "Jce_mcare.h"

/**************************************************************************************************
 * MCare平台JString 类实现
 */
JString::JString()
{
	_data.append('\0');
}

JString::JString(const JString& r)
{
    _data = r._data;
}

JString::JString(const signed char * s, size_type len)
{
    assign(s, len);
}

JString::JString(const signed char * s)
{
    assign(s, strlen((char*)s));
}

signed char & JString::operator[](size_type n)  
{ 
    return _data[n]; 
}

const signed char& JString::operator[](size_type n) const
{ 
    return _data[n]; 
}

JString& JString::operator=(const JString& r)
{
    _data = r._data;
    return *this;
}

JString& JString::operator=(const signed char * r)
{
    assign(r, strlen((char*)r));
    return *this;
}

bool JString::operator==(const JString& r) const
{
    return _data == r._data;
}

bool JString::operator<(const JString& r) const
{
    return _data < r._data;
}

void JString::assign(const signed char * r, size_type len)
{
    _data.assign(r, len);
    _data.append('\0');
}

void JString::assign(const signed char * f, const signed char * l)    
{ 
    assign(f, l - f); 
}

void JString::append(const signed char * r, size_type len)
{
    _data.resize(_data.size() - 1);
    _data.append(r, len);
    _data.append('\0'); 
}

void JString::append(const JString &s)
{
    append(s.data(), s.size());
}

JString::size_type JString::length() const  
{ 
    return size(); 
}

JString::size_type JString::size() const        
{ 
    return _data.size() - 1; 
}

const signed char* JString::data() const   
{ 
    return c_str(); 
}

const signed char* JString::c_str() const  
{ 
    return _data.data(); 
}

void JString::AppendData(unsigned char * r, size_type len) 
{ 
    append((const signed char*)r, len);
}

void JString::AppendData(const JString &s)
{ 
    append(s);
}

void JString::ToString( int nNum )
{
    int nBitCount = 0 ;
    bool bPositive = (bool)((nNum >= 0) ? true : false) ;
    int nNumber = (bPositive==true) ? nNum : nNum * (-1) ;
    
    //计算位数
    do 
	{
        nNumber = nNumber/10 ;
        nBitCount ++ ;
    } while( nNumber > 0 );
    
    if (bPositive == false)
        nBitCount ++ ;

    _data.resize(nBitCount);
    
    nNumber = (bPositive==true) ? nNum : (nNum * (-1)) ;
    int nUnit = 0 ;
    do
    {
        nUnit = nNumber % 10 ;
        nNumber = nNumber/10 ;
        
        nBitCount -- ;
        switch(nUnit)
        {
			case 0 : _data[nBitCount] = L'0' ;    break;
			case 1 : _data[nBitCount] = L'1' ;    break;
			case 2 : _data[nBitCount] = L'2' ;    break;
			case 3 : _data[nBitCount] = L'3' ;    break;
			case 4 : _data[nBitCount] = L'4' ;    break;
			case 5 : _data[nBitCount] = L'5' ;    break;
			case 6 : _data[nBitCount] = L'6' ;    break;
			case 7 : _data[nBitCount] = L'7' ;    break;
			case 8 : _data[nBitCount] = L'8' ;    break;
			case 9 : _data[nBitCount] = L'9' ;    break;
			default: break ;
        }
    } while( nNumber > 0 ) ;
    
    if( bPositive == false )
        _data[0] = L'-' ;
    
    _data.append('\0');
}

bool JString::IsEmpty() 
{
    return _data.size() == 0;
}

void JString::Empty()   
{
    _data.clear();
}


/************************************************************************
 * BufferReader 类实现
 */
BufferReader::BufferReader() : _cur(0)
{

}

BufferReader::~BufferReader()
{

}

BufferReader* BufferReader::NewLC()
{
    BufferReader* self = new BufferReader();
    if(self == NULL) return NULL;

    return self;
}

BufferReader* BufferReader::NewL()
{
    BufferReader* self=BufferReader::NewLC();
    return self;
}

void BufferReader::reset()
{
    _cur = 0;
}

Int32 BufferReader::readBuf(void * buf, unsigned int len)
{
    Int32 ret = 0;

    if ((ret = peekBuf(buf, len)) != JCE_SUCCESS) return ret;
    _cur += len;

    return JCE_SUCCESS;
}

Int32 BufferReader::peekBuf(void * buf, unsigned int len, unsigned int offset)
{
    if(_cur + offset + len > _buf.size())
    {
        char s[64];
        String si;
        si.ToString(_buf.size());
        strncpy(s, "buffer overflow when peekBuf, over ", sizeof(s)-1);
        strncat(s, (const char*)si.data(), sizeof(s)-strlen(s)-1 );
        _sLastError = (signed char*)s;
		return JCE_DECODE_ERROR;
    }

    memcpy(buf, &_buf[_cur + offset], len);
    return JCE_SUCCESS;
}

Int32 BufferReader::skip(unsigned int len)
{
    if (_cur + len > _buf.size())
    {
        String si;
        si.ToString(_buf.size());
        
        char s[64];
        strncpy(s, "buffer overflow when peekBuf, over ", sizeof(s)-1);
        strncat(s, (const char*)si.data(), sizeof(s)-strlen(s)-1 );
        _sLastError = (signed char*)s;
		return JCE_DECODE_ERROR;
    }
    
    _cur += len;
    return JCE_SUCCESS;
}

Int32 BufferReader::setBuffer(const signed char * buf, unsigned int len)
{
	_buf.assign(buf, len);
	_cur = 0;
	return JCE_SUCCESS;
}

String BufferReader::getLastError() 
{
    return _sLastError; 
}

/************************************************************************/
/* BufferWriter 类实现                                                  */
/************************************************************************/
BufferWriter::BufferWriter()
{

}

BufferWriter::~BufferWriter()
{

}

BufferWriter* BufferWriter::NewLC()
{
	BufferWriter* self = new BufferWriter();
	if(self == NULL)
	{
		return NULL;
	}

	return self;
}

BufferWriter* BufferWriter::NewL()
{
    BufferWriter* self=BufferWriter::NewLC();
    return self;
}

void BufferWriter::reset() 
{ 
    _buf.clear();
}

void BufferWriter::writeBuf(const void * buf, unsigned int len)
{
    signed char * p = (signed char *) buf;
    _buf.append(p, len);
}

const signed char * BufferWriter::getBuffer() const    
{ 
    return _buf.data(); 
}

unsigned int BufferWriter::getLength() const        
{ 
    return _buf.size(); 
}

String BufferWriter::getLastError()
{ 
    return _sLastError; 
}


/**************************************************************************************************
 * DataHead 类实现
 */
DataHead::DataHead(UInt8 type, UInt8 tag) : _type(type), _tag(tag)
{

}

DataHead::~DataHead()
{

}

DataHead* DataHead::NewLC(UInt8 type, UInt8 tag)
{
    DataHead * self = new DataHead(type, tag);
    if (self == NULL) return NULL;

    return self;
}

DataHead* DataHead::NewL(UInt8 type, UInt8 tag)
{
    DataHead * self = DataHead::NewLC(type, tag);
    return self;
}

UInt8 DataHead::getTag() const        
{ 
    return _tag; 
}

void DataHead::setTag(UInt8 t)    
{ 
    _tag = t; 
}

UInt8 DataHead::getType() const       
{ 
    return _type; 
}

void DataHead::setType(UInt8 t)       
{ 
    _type = t; 
}

Int32 DataHead::readFrom(JceInputStream & is)
{
	Int32 ret = 0;
	UInt32 n = 0;

	ret = peekFrom(is, &n);
	if (JCE_SUCCESS != ret)	return ret;

	ret = is.skip(n);
	return ret;
}

Int32 DataHead::peekFrom(JceInputStream & is, UInt32 *n)
{
	helper h;
	Int32 ret = 0;

	*n = 1;
	if ((ret = is.peekBuf(&h, 1)) != JCE_SUCCESS)
	{	
		return ret;
	}

	_type = h.type;
	if (h.tag == 15)
	{
		if ((ret= is.peekBuf(&_tag, 1, 1)) != JCE_SUCCESS)
		{	
			return ret;
		}

		*n += 1;
	}
	else
	{
		_tag = h.tag;
	}

	return JCE_SUCCESS;
}

/**
 * 写入数据头信息
 * @param os ： 模板类型参数。
 */
void DataHead::writeTo(JceOutputStream & os)
{
	helper h;
	h.type = _type;

	if (_tag < 15)
	{
		//标签小于15时，头信息只占1字节的大小。
		h.tag = _tag;
		os.writeBuf(&h, 1);
	}
	else
	{
		//标签大于等于15时，头信息占2字节大小。 第一个字节用于type和tag1占用。
		//第二个字节用于tag2占用，即真正的标签值
		h.tag = 15;
		os.writeBuf(&h, 1);
		os.writeBuf(&_tag, 1);
	}
}

/************************************************************************/
/* JceInputStream 类实现                                                */
/************************************************************************/
JceInputStream::JceInputStream()
{

}

JceInputStream::~JceInputStream()
{

}

JceInputStream* JceInputStream::NewLC()
{
    JceInputStream* self = new JceInputStream();
    if(self == NULL)
	{
		return NULL;
    }

    return self;
}

JceInputStream* JceInputStream::NewL()
{
    JceInputStream* self=JceInputStream::NewLC();
    return self;
}

void JceInputStream::ConstructL()
{

}

void JceInputStream::error(const String &t1, UInt8 tag, unsigned char type, const String &t2, Int32 i)
{
    char s[128];

    String sTag;    
    sTag.ToString(tag);
    
    String sInfo;    
    sInfo.ToString(i);

    String sTypeH;  
    sTypeH.ToString(type);

    strncpy(s, (const char*)t1.data(), sizeof(s)-1);
    strncat(s, ", tag:", sizeof(s)-1);
    strncat(s, (const char*)sTag.data(), sizeof(s)-strlen(s)-1);
    strncat(s, ", type:", sizeof(s)-strlen(s)-1);
    strncat(s, (const char*)sTypeH.data(), sizeof(s)-strlen(s)-1);
    strncat(s, ", ", sizeof(s)-strlen(s)-1);
    strncat(s, (const char*)t2.data(), sizeof(s)-strlen(s)-1);
    strncat(s, ": ", sizeof(s)-strlen(s)-1);
    strncat(s, (const char*)sInfo.data(), sizeof(s)-strlen(s)-1);

    _sLastError = (signed char*)s;
}

void JceInputStream::errorRequire(UInt8 tag)
{
    String sTag;   
    sTag.ToString(tag);
    
    char s[64];
    strncpy(s, "require field not exist, tag: ", sizeof(s)-1 );
    strncat(s, (const char*)sTag.data(), sizeof(s)-strlen(s)-1);

    _sLastError = (signed char*)s;
}

bool JceInputStream::skipToTag(UInt8 tag)
{
    Int32 ret;
    DataHead* dataHead = DataHead::NewL();
    if(dataHead == NULL)return false;
    while(true)
    {
        size_t len = 0;
        ret = dataHead->peekFrom(*this,&len);
		if (JCE_SUCCESS!=ret)
		{
			delete dataHead;
			return false;
		}

		if (tag <= dataHead->getTag() || dataHead->getType() == DataHead::eStructEnd)
        {
			UInt8 findTag = dataHead->getTag();
			delete dataHead;
			return tag == findTag;
		}

		BufferReader::skip(len);

		skipField(dataHead->getType());
	}

	delete dataHead;
    return false;
}

void JceInputStream::skipToStructEnd()
{
    DataHead* dataHead = DataHead::NewL();
    if (dataHead == NULL) return;
    
    do
	{
		dataHead->readFrom(*this);
		skipField(dataHead->getType());
    }while(dataHead->getType() != DataHead::eStructEnd);

	delete dataHead;
}

void JceInputStream::skipField()
{
    DataHead* dataHead = DataHead::NewL();
    if(dataHead == NULL)return;
    
    dataHead->readFrom(*this);
    skipField(dataHead->getType());
    delete dataHead;

}

void JceInputStream::skipField(UInt8 type)
{
    switch(type)
	{
		case DataHead::eChar:
			this->skip(sizeof(Char));
			break;
		case DataHead::eShort:
			this->skip(sizeof(Short));
			break;
		case DataHead::eInt32:
			this->skip(sizeof(Int32));
			break;
		case DataHead::eInt64:
			this->skip(sizeof(Int64));
			break;
		case DataHead::eFloat:
			this->skip(sizeof(Float));
			break;
		case DataHead::eDouble:
			this->skip(sizeof(Double));
			break;
		case DataHead::eString1:
        {
            UInt8 n;
            readBuf(&n, sizeof(n));
            size_t len = n;
            this->skip(len);
        }
			break;
		case DataHead::eString4:
        {
            UInt32 n;
            readBuf(&n, sizeof(n));
            size_t len =  jce_ntohl(n);
            this->skip(len);
        }
			break;
		case DataHead::eMap:
        {
            Int32 size;
            read(size, 0);
            for(Int32 i = 0; i < size * 2; ++i)
                skipField();
        }
			break;
		case DataHead::eList:
        {
            Int32 size;
            read(size, 0);
            for(Int32 i = 0; i < size; ++i)
                skipField();
        }
			break;
		case DataHead::eSimpleList:
        {
            DataHead* dataHead = DataHead::NewL();
            if(dataHead == NULL)break;           
            dataHead->readFrom(*this);
            if(dataHead->getType() != DataHead::eChar)
            {
				error((Char*)"skipField with invalid type", 0, type, (Char*)"type", dataHead->getType());
				delete dataHead;
				return;
            }
            Int32 size;
            read(size, 0);
            this->skip(size);
			delete dataHead;
        }
			break;
		case DataHead::eStructBegin:
			skipToStructEnd();
			break;
		case DataHead::eStructEnd:
		case DataHead::eZeroTag:
			break;
		default:
        {
            error((Char*)"skipField with invalid type", 0, type, (Char*)"type", 0);
        }
    }
}

Char JceInputStream::readChar()
{
    Char n;
    readBuf(&n, sizeof(n));
    return n;
}

Short JceInputStream::readShort()
{
    Short n;
    readBuf(&n, sizeof(n));
    return n;
}

Int32 JceInputStream::readInt32()
{
    Int32 n;
    readBuf(&n, sizeof(n));
    return n;
}

Int32 JceInputStream::read(Bool& b, UInt8 tag, bool isRequire)
{
    Int32 ret;
    Char c = b;
    ret = read(c, tag, isRequire);
    b = c ? true : false;
    return ret;
}

Int32 JceInputStream::read(Char& c, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
        DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}
        dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				c = 0;
				break;
			case DataHead::eChar:
				this->readBuf(&c, sizeof(c));
				break;
			default:
            {
				error((Char*)"read 'Char' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }

		delete dataHead;
	}
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

    return JCE_SUCCESS;
}

Int32 JceInputStream::read(Short& n, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
		DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}
        dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				n = 0;
				break;
			case DataHead::eChar:
				n = readChar();
				break;
			case DataHead::eShort:
				this->readBuf(&n, sizeof(n));
				n = jce_ntohs(n);
				break;
			default:
            {
                error((Char*)"read 'Short' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }
		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

	return JCE_SUCCESS;
}

Int32 JceInputStream::read(Int32& n, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
        DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}
        dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				n = 0;
				break;
			case DataHead::eChar:
				n = readChar();
				break;
			case DataHead::eShort:
				n = (Short) readShort();
				n = (Short) jce_ntohs(n);
				break;
			case DataHead::eInt32:
				this->readBuf(&n, sizeof(n));
				n = jce_ntohl(n);
				break;
			default:
            {
				error((Char*)"read 'Int32' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }

		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

    return JCE_SUCCESS;
}

Int32 JceInputStream::read(Int64& n, UInt8 tag, bool isRequire)
{
    if (skipToTag(tag))
    {
		DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}        

		dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				n = 0;
				break;
			case DataHead::eChar:
				n = readChar();
				break;
			case DataHead::eShort:
				n = (Short) readShort();
				n = (Short) jce_ntohs(n);
				break;
			case DataHead::eInt32:
				n = (Int32) readInt32();
				n = (Int32) jce_ntohl(n);
				break;
			case DataHead::eInt64:
				this->readBuf(&n, sizeof(n));
				n = jce_ntohll(n);
				break;
			default:
            {
				error((Char*)"read 'Int64' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }

		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

    return JCE_SUCCESS;
}

Int32 JceInputStream::read(Float& n, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
		DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}             

		dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				n = 0;
				break;
			case DataHead::eFloat:
				this->readBuf(&n, sizeof(n));
				n = jce_ntohf(n);
				break;
			default:
            {
				error((Char*)"read 'Float' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }

		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

	return JCE_SUCCESS;
}

Int32 JceInputStream::read(Double& n, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
		DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		} 

		dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eZeroTag:
				n = 0;
				break;
			case DataHead::eFloat:
            {
                Float n1;
                readBuf(&n1, sizeof(n1));
                n = n1;
                break;
            }
			case DataHead::eDouble:
				this->readBuf(&n, sizeof(n));
				n = jce_ntohd(n);
				break;
			default:
            {
				error((Char*)"read 'Double' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }
		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

	return JCE_SUCCESS;
}

Int32 JceInputStream::read(UInt8& c, UInt8 tag, bool isRequire)
{
	Int32 ret = JCE_SUCCESS;

	Short i = c;
    if ((ret = read(i,tag,isRequire)) != JCE_SUCCESS) return ret;

    c = (UInt8)i;
	return JCE_SUCCESS;
}
    
Int32 JceInputStream::read(UInt16& n, UInt8 tag, bool isRequire)
{
	Int32 ret = JCE_SUCCESS;

	Int32 i = n;
    if ((ret = read(i,tag,isRequire)) != JCE_SUCCESS) return ret;

	n = (UInt16)i;
	return JCE_SUCCESS;
}
    
Int32 JceInputStream::read(UInt32& n, UInt8 tag, bool isRequire)
{   
	Int32 ret = JCE_SUCCESS;

	Int64 i = n;
    if ((ret = read(i,tag,isRequire)) != JCE_SUCCESS) return ret;

	n = (UInt32)i;
	return JCE_SUCCESS;
}

Int32 JceInputStream::read(String& s, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
		DataHead* dataHead = DataHead::NewL();   
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		} 
        dataHead->readFrom(*this);
        switch(dataHead->getType())
        {
			case DataHead::eString1:
            {
                UInt8 n;
                this->readBuf(&n, sizeof(n));
                size_t len = n;
                Char ss[256];
                this->readBuf(ss, len);
                s.assign(ss, ss + len);
            }
				break;

			case DataHead::eString4:
            {
                UInt32 len;
                readBuf(&len, sizeof(len));
                len = jce_ntohl(len);
                if(len > JCE_MAX_STRING_LENGTH)
                {
                    error((Char*)"invalid string size", tag, dataHead->getType(), (Char*)"size", len);
					delete dataHead;
					return JCE_DECODE_ERROR;
                }
                Char *ss = new Char[len];
				if(ss)
				{	
	 	    	  Int32 ret;
				  ret = this->readBuf(ss, len);
				  if(JCE_SUCCESS!=ret)
				  {
					  delete[] ss;
					  return JCE_DECODE_ERROR;
				  }
				  s.assign(ss, ss + len);
				  delete[] ss;
				}
            }
				break;
			default:
            {
                error((Char*)"read 'string' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
				delete dataHead;
				return JCE_DECODE_ERROR;
            }
        }

		delete dataHead;
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

    return JCE_SUCCESS;
}

Int32 JceInputStream::read(JceStructBase& v, UInt8 tag, bool isRequire)
{
    if(skipToTag(tag))
    {
        DataHead* dataHead = DataHead::NewL();
		if(dataHead == NULL)
		{
			return JCE_MALLOC_ERROR;
		}                    
        dataHead->readFrom(*this);
        if(dataHead->getType() != DataHead::eStructBegin)
        {
			error((Char*)"read 'struct' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
			delete dataHead;
			return JCE_DECODE_ERROR;
        }
        v.readFrom(*this);
        skipToStructEnd();
		delete dataHead;        
    }
    else if(isRequire)
    {
        errorRequire(tag);
		return JCE_TAG_NOTFOUND;
    }

	return JCE_SUCCESS;
}

bool JceInputStream::readVectorChar(UInt8 tag, Int32 &size)
{
	DataHead* dataHead = DataHead::NewL();
	if(dataHead == NULL)
	{
		return false;
	}
	dataHead->readFrom(*this);
	switch(dataHead->getType())
	{
		case DataHead::eSimpleList:
	    {
	        DataHead* dataHead2 = DataHead::NewL();
			if(dataHead2 == NULL)
			{
				delete dataHead;
				return false;
			}
	        dataHead2->readFrom(*this);
	        if(dataHead2->getType() != DataHead::eChar)
	        {
	            error((Char*)"type mismatch", tag, dataHead->getType(), (Char*)"type", dataHead2->getType());
				delete(dataHead2);
				delete(dataHead);
				return false;	   
			}
	        read(size, 0);
	        if(size < 0)
	        {
	            error((Char*)"invalid vector size", tag, dataHead->getType(), (Char*)"size", size);
				delete(dataHead2);
				delete(dataHead);
				return false;	   
			}
			delete(dataHead2);
			delete(dataHead);	
		}
		return true;   

		default:
	    {
			error((Char*)"type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
			delete(dataHead);	
			return false;
	    }
	}

	delete(dataHead);	
	return false;
}

bool JceInputStream::readVector(UInt8 tag, Int32 &size)
{
	DataHead* dataHead = DataHead::NewL();
	if(dataHead == NULL)
	{
		return false;
	}        
	dataHead->readFrom(*this);
	switch(dataHead->getType())
	{
		case DataHead::eList:
	    {
	        read(size, 0);
	        if(size < 0)
	        {
				error((Char*)"invalid size", tag, dataHead->getType(), (Char*)"size", size);
				delete(dataHead);
				return false;	
	        }
			delete(dataHead);
	    }
	    return true;

		default:
	    {
	        error((Char*)"read 'vector' type mismatch", tag, dataHead->getType(), (Char*)"size", 0);
			delete(dataHead);
			return false;	
	    }
	}
 	delete(dataHead);
	return false;
}

bool JceInputStream::readMap(UInt8 tag, Int32 &size)
{
	DataHead* dataHead = DataHead::NewL();
	if(dataHead == NULL)
	{
		return false;
	}       

	dataHead->readFrom(*this);
	switch(dataHead->getType())
	{
		case DataHead::eMap:
	    {
	        read(size, 0);
	        if(size < 0)
	        {
				error((Char*)"invalid map size", tag, dataHead->getType(), (Char*)"size", size);
				delete(dataHead);
				return false;	
	        } 
			delete(dataHead);
	        return true;
	    }
			break;
		default:
	    {
			error((Char*)"invalid map type", tag, dataHead->getType(), (Char*)"size", 0);
			delete(dataHead);
			return false;	
	    }
	}
	delete(dataHead);
	return false;
}

/************************************************************************/
/*  JceOutputStream 类实现                                              */
/************************************************************************/
JceOutputStream::JceOutputStream()
{
    // No implementation required
}

JceOutputStream::~JceOutputStream()
{

}

JceOutputStream* JceOutputStream::NewLC()
{
	JceOutputStream* self = new JceOutputStream();
	if(self == NULL)
	{
		return NULL;
	}

	return self;
}

JceOutputStream* JceOutputStream::NewL()
{
    JceOutputStream* self = JceOutputStream::NewLC();
    return self;
}

Int32 JceOutputStream::write(Bool b, UInt8 tag)
{
	Char c = (Char) (b ? 1 : 0);
	write(c, tag);
	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Char n, UInt8 tag)
{
	DataHead* dataHead = DataHead::NewL(DataHead::eChar, tag);
	if (dataHead == NULL)return JCE_MALLOC_ERROR;

	if (0 == n)
	{
		//为0情况，没有紧跟数据。只写头。
		dataHead->setType(DataHead::eZeroTag);
		dataHead->writeTo(*this);
	}
	else
	{
	    //不为0，写完头，后紧跟数据
		dataHead->writeTo(*this);
		this->writeBuf(&n, sizeof(n));
	}

	delete dataHead;
	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Short n, UInt8 tag)
{
	if (n >= (-128) && n <= 127)
	{
	    return write((Char) n, tag);
	}
	else
	{
	    DataHead* dataHead = DataHead::NewL(DataHead::eShort, tag);
	    if(dataHead == NULL) return JCE_MALLOC_ERROR;
	    dataHead->writeTo(*this);
	    delete dataHead;
	    n = jce_htons(n);
		this->writeBuf(&n, sizeof(n));
	}

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Int32 n, UInt8 tag)
{
	if(n >= (-32768) && n <= 32767)
	{
	    return write((Short) n, tag);
	}
	else
	{
	    DataHead* dataHead = DataHead::NewL(DataHead::eInt32, tag);
	    if(dataHead == NULL)return JCE_MALLOC_ERROR;
		dataHead->writeTo(*this);
	    delete dataHead;

		n = jce_htonl(n);
		this->writeBuf(&n, sizeof(n));
	}

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Int64 n, UInt8 tag)
{
	if(n >= (-2147483647-1) && n <= 2147483647)
	{
		return write((Int32) n, tag);
	}
	else
	{
		DataHead* dataHead = DataHead::NewL(DataHead::eInt64, tag);
		if (dataHead == NULL) return JCE_MALLOC_ERROR;

		dataHead->writeTo(*this);

		delete dataHead;
		n = jce_htonll(n);
		this->writeBuf(&n, sizeof(n));
	}

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Float n, UInt8 tag)
{
	DataHead* dataHead = DataHead::NewL(DataHead::eFloat, tag);  
	if(dataHead == NULL) return JCE_MALLOC_ERROR;

	dataHead->writeTo(*this);

	n = jce_htonf(n);
	delete dataHead;
	this->writeBuf(&n, sizeof(n));

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(Double n, UInt8 tag)
{
	DataHead * dataHead = DataHead::NewL(DataHead::eDouble, tag);
	if (dataHead == NULL) return JCE_MALLOC_ERROR;

	dataHead->writeTo(*this);

	delete dataHead;
	n = jce_htond(n);
	this->writeBuf(&n, sizeof(n));

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(UInt8 n, UInt8 tag)
{    
    return write((Short)n,tag);
}

Int32 JceOutputStream::write(UInt16 n, UInt8 tag)
{
	return write((Int32)n,tag);
}

Int32 JceOutputStream::write(UInt32 n, UInt8 tag)
{
	return write((Int64)n,tag);
}

Int32 JceOutputStream::write(const String & s, UInt8 tag)
{
	DataHead* dataHead = NULL;
	if (s.size() > JCE_MAX_STRING_LENGTH)
	{
		String sTag;    
		sTag.ToString(tag);
		
		String sSize;  
		sSize.ToString(s.size());
	        
		char ss[128];
		strncpy(ss, "invalid string size, tag:", sizeof(ss)-1 );
		strncat(ss, (const char*)sTag.data(), sizeof(ss)-strlen(ss)-1);
		strncat(ss, ", size: ", sizeof(ss)-strlen(ss)-1);
		strncat(ss, (const char*)sSize.data(), sizeof(s)-strlen(ss)-1);                   
		_sLastError = s;
		return JCE_RUNTIME_ERROR;
	}

	if ((dataHead = DataHead::NewL(DataHead::eString1, tag)) == NULL) return JCE_MALLOC_ERROR;

	if (s.size() > 255)
	{
		dataHead->setType(DataHead::eString4);
	    dataHead->writeTo(*this);
	    UInt32 n = jce_htonl(s.size());
	    this->writeBuf(&n, sizeof(n));
	    this->writeBuf(s.data(), s.size());
	}
	else
	{
	    dataHead->writeTo(*this);
	    UInt8 n = s.size();
	    this->writeBuf(&n, sizeof(n));
	    this->writeBuf(s.data(), s.size());
	}
	delete dataHead;

	return JCE_SUCCESS;
}

Int32 JceOutputStream::write(const JceStructBase& v, UInt8 tag)
{
	DataHead* dataHead = DataHead::NewL(DataHead::eStructBegin, tag);
	if(dataHead == NULL) return JCE_MALLOC_ERROR;

	dataHead->writeTo(*this);
	v.writeTo(*this);
	dataHead->setType(DataHead::eStructEnd);
	dataHead->setTag(0);
	dataHead->writeTo(*this);
	delete dataHead;

	return JCE_SUCCESS;
}

Int32 JceOutputStream::writeHead(UInt8 type, UInt8 tag)
{
	DataHead* dataHead = DataHead::NewL(type, tag);
	if(dataHead == NULL) return JCE_MALLOC_ERROR;

	dataHead->writeTo(*this);
	delete dataHead;

	return JCE_SUCCESS;
}

