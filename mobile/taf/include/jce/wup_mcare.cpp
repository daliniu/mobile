// **********************************************************************
// WUP version 1.0.0 by WSRD Tencent.
// **********************************************************************
#include <stddef.h>
#include "wup_mcare.h"

UniAttribute::UniAttribute()
{
    _is = JceInputStream::NewL();
    _os = JceOutputStream::NewL();
}

UniAttribute::~UniAttribute()
{
    if (_is)
    {
        delete _is;
        _is = NULL;
    }

    if (_os)
    {
        delete _os;
        _os = NULL;
    }
}

Int32 UniAttribute::putAttr(JceOutputStream &os, const String &key, const String & type_name)
{
    JArray<JMapPair<String, JMapWrapper<String, JArray<signed char> > > > & array1 = _data.getArray();
    for (unsigned int i = 0; i < array1.size(); ++i)
    {
        if (array1[i].first == key)
        {
            JArray<JMapPair<String, JArray<signed char> > > & array2  = array1[i].second.getArray();
            String type = type_name;

            for (unsigned int j = 0; j < array1.size(); ++j)
            {
                if (array2[j].first == type)
                {
                    array2[j].second = JArray<signed char>(os.getBuffer(), os.getLength());
                    return WUP_SUCCESS;
                }
            }

            array2.append(JMapPair<String, JArray<signed char> >(type_name, JArray<signed char>(os.getBuffer(), os.getLength())));

            return WUP_SUCCESS;
        }
    }

    JMapWrapper<String, JArray<signed char> > map1;
    map1.getArray().append(JMapPair<String, JArray<signed char> >(type_name, JArray<signed char>(os.getBuffer(), os.getLength())));
    _data.getArray().append(JMapPair<String, JMapWrapper<String, JArray<signed char> > >(key, map1));

    return WUP_SUCCESS;
}

Int32 UniAttribute::put(const String& name, const Bool &t)          {DO_PUT}
Int32 UniAttribute::put(const String& name, const Char &t)          {DO_PUT}
Int32 UniAttribute::put(const String& name, const Short &t)         {DO_PUT}
Int32 UniAttribute::put(const String& name, const Int32 &t)         {DO_PUT}
Int32 UniAttribute::put(const String& name, const Int64 &t)         {DO_PUT}
Int32 UniAttribute::put(const String& name, const Float &t)         {DO_PUT}
Int32 UniAttribute::put(const String& name, const Double &t)        {DO_PUT}
Int32 UniAttribute::put(const String& name, const String &t)        {DO_PUT}
Int32 UniAttribute::put(const String& name, const JceStructBase &t) {DO_PUT}
Int32 UniAttribute::put(const String& name, const UInt8 &t)         {DO_PUT}
Int32 UniAttribute::put(const String& name, const UInt16 &t)        {DO_PUT}
Int32 UniAttribute::put(const String& name, const UInt32 &t)        {DO_PUT}

Int32 UniAttribute::getAttr(JceInputStream &is, const String &key, const String & type_name)
{
    JArray<JMapPair<String, JMapWrapper<String, JArray<signed char> > > > & array1 = _data.getArray();
    for (unsigned int i = 0; i < array1.size(); ++i)
    {
        if (array1[i].first == key)
        {
            JArray<JMapPair<String, JArray<signed char> > > & array2 = array1[i].second.getArray();

            for (unsigned int j = 0; j < array1.size(); ++j)
            {
                if (array2[j].first == type_name)
                {
                    is.reset();
                    is.setBuffer(array2[j].second.data(), array2[j].second.size());

                    return WUP_SUCCESS;
                }
            }
            
            _sLastError.assign((Char *)"UniAttribute type match fail,key:", strlen("UniAttribute type match fail,key:"));
            _sLastError.append(key);
            _sLastError.append((Char *)",type:");
            _sLastError.append(type_name);
            if (array1.size() > 0)
            {
                _sLastError.append((Char *)",may be:");
                _sLastError.append(array2[0].first);
            }
            
            return WUP_DECODE_ERROR;
        }
    }

    _sLastError = (Char*)"UinRequest not found key:";
    _sLastError.AppendData((unsigned char*)key.data(), key.size());
    return WUP_ATTR_NOT_FOUND;
}

Int32 UniAttribute::get(const String& name,  Bool &t)           {DO_GET}
Int32 UniAttribute::get(const String& name,  Char &t)           {DO_GET}
Int32 UniAttribute::get(const String& name,  Short &t)          {DO_GET}
Int32 UniAttribute::get(const String& name,  Int32 &t)          {DO_GET}
Int32 UniAttribute::get(const String& name,  Int64 &t)          {DO_GET}
Int32 UniAttribute::get(const String& name,  Float &t)          {DO_GET}
Int32 UniAttribute::get(const String& name,  Double &t)         {DO_GET}
Int32 UniAttribute::get(const String& name,  String &t)         {DO_GET}
Int32 UniAttribute::get(const String& name,  JceStructBase &t)  {DO_GET}
Int32 UniAttribute::get(const String& name,  UInt8 &t)          {DO_GET}
Int32 UniAttribute::get(const String& name,  UInt16 &t)         {DO_GET}
Int32 UniAttribute::get(const String& name,  UInt32 &t)         {DO_GET}

void UniAttribute::clear() 
{ 
    _data.clear(); 
}

Int32 UniAttribute::encode(String& buff)
{
    _os->reset();

    _os->write(_data, 0);

    buff.assign(_os->getBuffer(), (const signed char *)(_os->getBuffer() + _os->getLength()));

    return WUP_SUCCESS;
}

Int32 UniAttribute::encode(JArray<Char>& buff)
{
    _os->reset();

    _os->write(_data, 0);

    buff.assign(_os->getBuffer(), _os->getLength());

    return WUP_SUCCESS;
}

Int32 UniAttribute::encode(Char * buff, size_t & len)
{   
    _os->reset();

    _os->write(_data, 0);

    if (len < _os->getLength())
    {
        _sLastError = (Char*)"encode error, buffer length too short";
        return WUP_ENCODE_ERROR;
    }
    memcpy(buff, _os->getBuffer(), _os->getLength());
    len =  _os->getLength();

    return WUP_SUCCESS;
}

Int32 UniAttribute::decode(const Char* buff, size_t len)
{
    Int32 ret =0;
    _is->reset();

    _is->setBuffer(buff, len);

    _data.clear();

    ret = _is->read(_data, 0, true);
    if (ret != 0)
    {
        _sLastError = _is->getLastError();
        return WUP_DECODE_ERROR;
    }

    return WUP_SUCCESS;
}

Int32 UniAttribute::decode(const JArray<Char>& buff)
{
    Int32 ret =0;
    _is->reset();

    _is->setBuffer(buff.data(), buff.size());

    _data.clear();

    ret = _is->read(_data, 0, true);
    if (ret!=0)
    {
        _sLastError = _is->getLastError();
        return WUP_DECODE_ERROR;
    }
    return WUP_SUCCESS;
}

const JMapWrapper<String, JMapWrapper<String, JArray<Char> > >& UniAttribute::getData() const
{
    return _data;
}

bool UniAttribute::isEmpty()
{
    return _data.empty();
}

size_t UniAttribute::size()
{
    return _data.size();
}

String UniAttribute::getLastError()
{
    return _sLastError;
}

////////////////////////////////////////////////////////////////////////////////////////////////
UniPacket::UniPacket() 
{
    iVersion = 2; cPacketType = 0; 

    iMessageType = 0; iRequestId = 0; 

    sServantName = (Char*)""; sFuncName = (Char*)""; 

    iTimeout = 0; sBuffer.clear(); 

    context.clear(); status.clear(); 

    UniAttribute::_data.clear();
}

UniPacket* UniPacket::createResponseLC()
{
    UniPacket* self = new UniPacket();
    if (self == NULL) return NULL;
 
    self->sServantName = sServantName;
    self->sFuncName    = sFuncName;
    self->iRequestId   = iRequestId;

    return self;
}

UniPacket* UniPacket::createResponseL()
{
    UniPacket* self=UniPacket::createResponseLC();

    return self;
}

Int32 UniPacket::encode(String& buff)
{
    Int32 ret =0;
    _os->reset();

    ret = doEncode(*_os);
    if (WUP_SUCCESS!=ret)
    {
        return ret;
    }
    Int32 iHeaderLen = jce_htonl(sizeof(Int32) + _os->getLength());
    buff.assign((const Char*)&iHeaderLen, (const Char*) ((const Char*)&iHeaderLen + sizeof(Int32)));

    buff.AppendData((unsigned char*)_os->getBuffer(), _os->getLength());

    return WUP_SUCCESS;
}

Int32 UniPacket::encode(JArray<Char>& buff)
{
    Int32 ret =0;
    _os->reset();

    ret = doEncode(*_os);
    if (WUP_SUCCESS!=ret)
    {
        return ret;
    }
    Int32 iHeaderLen = jce_htonl(sizeof(Int32) + _os->getLength());

    buff.resize(sizeof(Int32) + _os->getLength());
    memcpy(&buff[0], &iHeaderLen, sizeof(Int32));
    memcpy(&buff[sizeof(Int32)], _os->getBuffer(), _os->getLength());
    return WUP_SUCCESS;
}

Int32 UniPacket::encode(Char * buff, size_t & len)
{
    Int32 ret =0;
    _os->reset();

    ret = doEncode(*_os);
    if (WUP_SUCCESS!=ret)
    {
        return ret;
    }
    Int32 iHeaderLen = jce_htonl(sizeof(Int32) + _os->getLength());
    if (len < sizeof(Int32) + _os->getLength())
    {
        _sLastError = (Char*)"encode error, buffer length too short";
        return WUP_ENCODE_ERROR;
    }

    memcpy(buff, &iHeaderLen, sizeof(Int32));
    memcpy(buff + sizeof(Int32), _os->getBuffer(), _os->getLength());

    len = sizeof(Int32) + _os->getLength();

    return WUP_SUCCESS;
}

Int32 UniPacket::decode(const signed char* buff, size_t len)
{
    Int32 ret =0;
    if (len < sizeof(Int32))
    {
        _sLastError = (Char*)"packet length too short";
        return WUP_DECODE_ERROR;
    }

    _is->reset();

    _is->setBuffer(buff + sizeof(Int32), len - sizeof(Int32));

    readFrom(*_is);

    _is->reset();

    _is->setBuffer(sBuffer.data(), sBuffer.size());

    UniAttribute::_data.clear();

    ret = _is->read(UniAttribute::_data, 0, true);

    if (ret!=0)
    {
        _sLastError = _is->getLastError();
        return WUP_DECODE_ERROR;
    }
    return WUP_SUCCESS;
}

Int32 UniPacket::getRequestId() const { return iRequestId;}

void UniPacket::setRequestId(Int32 value) { iRequestId = value;}

const String& UniPacket::getServantName() const { return sServantName;}

void UniPacket::setServantName(const String& value) { sServantName = value;}

const String& UniPacket::getFuncName() const { return sFuncName;}

void UniPacket::setFuncName(const String& value) { sFuncName = value;}

Int32 UniPacket::doEncode(JceOutputStream & os)
{
    //ServantName¡¢FuncName²»ÄÜÎª¿Õ
    if (sServantName.size() == 0)
    {
        _sLastError = (Char*)"ServantName must not be empty";
        return WUP_ENCODE_ERROR;
    }
    
    if (sFuncName.size() == 0)
    {
        _sLastError = (Char*)"FuncName must not be empty";
        return WUP_ENCODE_ERROR;
    }

    os.reset();

    os.write(UniAttribute::_data, 0);

    sBuffer.assign(os.getBuffer(), os.getLength());

    os.reset();

    writeTo(os);
    return WUP_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////
TafUniPacket::TafUniPacket() {}

TafUniPacket::~TafUniPacket() {}

void TafUniPacket::setTafVersion(Short value) { iVersion = value;}

void TafUniPacket::setTafPacketType(Char value) { cPacketType = value;}

void TafUniPacket::setTafMessageType(Int32 value) { iMessageType = value;}

void TafUniPacket::setTafTimeout(Int32 value) { iTimeout = value;}

void TafUniPacket::setTafBuffer(const JArray<Char>& value) { sBuffer = value;}

void TafUniPacket::setTafContext(const JMapWrapper<String, String>& value) { context = value;}

void TafUniPacket::setTafStatus(const JMapWrapper<String, String>& value) { status = value;}

Short TafUniPacket::getTafVersion() const { return iVersion;}

Char TafUniPacket::getTafPacketType() const { return cPacketType;}

Int32 TafUniPacket::getTafMessageType() const { return iMessageType;}

Int32 TafUniPacket::getTafTimeout() const { return iTimeout;}

const JArray<Char>& TafUniPacket::getTafBuffer() const { return sBuffer;}

const JMapWrapper<String, String>& TafUniPacket::getTafContext() const { return context;}

const JMapWrapper<String, String>& TafUniPacket::getTafStatus() const { return status;}

Int32 TafUniPacket::getTafResultCode() const
{
    const JArray<JMapPair<String, String> > & arr = this->status.getArray();
    for (unsigned int i = 0; i < arr.size(); ++i)
    {
        if (arr[i].first == STATUS_RESULT_CODE)
        {
            return atoi((const char*)arr[i].second.c_str());
        }
    }
    return 0;
}

String TafUniPacket::getTafResultDesc() const
{
    const JArray<JMapPair<String, String> > & arr = this->status.getArray();
    for (unsigned int i = 0; i < arr.size(); ++i)
    {
        if (arr[i].first == STATUS_RESULT_DESC)
        {
            return arr[i].second;
        }
    }
    return(Char*)"";
}




