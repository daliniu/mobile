#include "util/tc_json.h"

#include <math.h>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace taf
{

#define FILTER_SPACE while(isspace((int)c)) {c=reader.read();}

JsonValuePtr TC_Json::getValue(BufferJsonReader & reader)
{
    char c=reader.read();
    FILTER_SPACE;

    switch(c)
    {
        case '{':
            return getObj(reader);
            break;
        case '[':
            return getArray(reader);
            break;
        case '"':
        //case '\'':
            return getString(reader,c);
            break;
        case 'T':
        case 't':
        case 'F':
        case 'f':
            return getBoolean(reader,c);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return getNum(reader,c);
            break;
        case 'n':
        case 'N':
            return getNull(reader,c);
        default:
            char s[64];
            snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)(uint32_t)reader.getCur());
            throw TC_Json_Exception(s);
    }
}

JsonValueObjPtr TC_Json::getObj(BufferJsonReader & reader)
{
    JsonValueObjPtr p = new JsonValueObj();
    bool bFirst=true;
    while(1)
    {
        char c=reader.read();
        FILTER_SPACE;
        if(c == '}' && bFirst)
        {
            return p;
        }
        bFirst=false;

        if(c != '"')
        {
            char s[64];
            snprintf(s, sizeof(s), "get obj error(key is not string)[pos:%u]", (uint32_t)reader.getCur());
            throw TC_Json_Exception(s);
        }
        JsonValueStringPtr pString=getString(reader);
        c=reader.read();
        FILTER_SPACE;
        if(c != ':')
        {
            char s[64];
            snprintf(s, sizeof(s), "get obj error(: not find)[pos:%u]", (uint32_t)reader.getCur());
            throw TC_Json_Exception(s);
        }
        JsonValuePtr pValue=getValue(reader);
        p->value[pString->value]=pValue;

        c=reader.read();
        FILTER_SPACE;

        if(c == ',')
            continue;
        if(c == '}')
            return p;

        char s[64];
        snprintf(s, sizeof(s), "get obj error(, not find)[pos:%u]", (uint32_t)reader.getCur());
        throw TC_Json_Exception(s);
    }
}

JsonValueArrayPtr TC_Json::getArray(BufferJsonReader & reader)
{
    JsonValueArrayPtr p = new JsonValueArray();
    bool bFirst=true;
    while(1)
    {
        char c;
        if(bFirst)
        {
            c=reader.read();
            FILTER_SPACE;
            if(c == ']')
            {
                return p;
            }
            reader.back();
        }
        bFirst=false;

        JsonValuePtr pValue=getValue(reader);
        p->push_back(pValue);

        c=reader.read();
        FILTER_SPACE;
        if(c == ',')
            continue;
        if(c == ']')
            return p;

        char s[64];
        snprintf(s, sizeof(s), "get vector error(, not find )[pos:%u]", (uint32_t)reader.getCur());
        throw TC_Json_Exception(s);
    }
}

JsonValueStringPtr TC_Json::getString(BufferJsonReader & reader,char head)
{
    JsonValueStringPtr p = new JsonValueString();
    const char * pChar=reader.getPoint();
    char c;
    uint32_t i=0;
    while(1)
    {
        c=reader.read(); 
        if(c == '\\')
        {
            p->value.append(pChar,i);
            pChar=pChar+i+2;
            i=0;
            c=reader.read();
            if(c == '\\' || c == '\"' || c == '/')
                p->value.append(1,c);
            else if(c == 'b')
                p->value.append(1,'\b');
            else if(c == 'f')
                p->value.append(1,'\f');
            else if(c == 'n')
                p->value.append(1,'\n');
            else if(c == 'r')
                p->value.append(1,'\r');
            else if(c == 't')
                p->value.append(1,'\t');
            else if(c == 'u')
            {
                uint16_t iCode=getHex(reader);
                if(iCode>0xff)
                {
                    char s[64];
                    snprintf(s, sizeof(s), "get string error(\\u)[pos:%u]", (uint32_t)reader.getCur());
                    throw TC_Json_Exception(s);
                }
                pChar+=4;
                p->value.append(1,(char)iCode);
#if 0
                //还要再读一个
                if(iCode<0xd800)
                {
                    p->value.append(1,(char)(iCode>>2&0x00ff));
                    p->value.append(1,(char)(iCode&0x00ff));
                }
                else
                {
                    uint16_t iCodeTwelve=0;
                    c=reader.read();
                    if(c == '\\' && (c=reader.read(),c=='u'))
                    {
                        iCodeTwelve=getHex();
                    }
                    if(iCodeTwelve<0xdc00)
                    {
                        char s[64];
                        snprintf(s, sizeof(s), "get string error(\\u)[pos:%u]", (uint32_t)reader.getCur());
                        throw TC_Json_Exception(s);
                    }
                    int iBuf=0;
                    iCode=iCode&0x03ff;
                    iBuf=(iCode<10);
                    iBuf+=(iCodeTwelve&0x03ff);
                    iBuf+=0x10000;
                }
#endif
            }
        }
        else if(c==head)
            break;
        else
            i++;
    }
    p->value.append(pChar,i);
    return p;
}

JsonValueNumPtr TC_Json::getNum(BufferJsonReader & reader,char head)
{
    bool bOk=true;
    bool bFloat=false;
    bool bExponential=false;
    bool bNegative=false;
    bool bExponentialNegative=false;
    int64_t iInt=0;
    double dFloat=0;
    double dFloatRat;
    int64_t iExponential=0;
    if(head == '-')
    {
        bOk=false;
        bNegative=true;
    }
    else
        iInt=head-0x30;
    char c;
    bool bNeedBack=false;
    while(1)
    {
        if(reader.hasEnd())
            break;
        c=reader.read();
        if(c>=0x30 && c<=0x39)
        {
            bOk=true;
            if(bExponential)
                iExponential=iExponential*10+c-0x30;
            else if(bFloat)
            {
                dFloat=dFloat+dFloatRat*(c-0x30);
                dFloatRat=dFloatRat*0.1;
            }
            else
                iInt=iInt*10+c-0x30;
        }
        else if(c == '.' && !bFloat && !bExponential && bOk)
        {
            bOk=false;
            bFloat=true;
            dFloatRat=0.1;
        }
        else if((c == 'e' || c == 'E') && !bExponential && bOk)
        {
            bOk=false;
            bExponential=true;
            iExponential=0;
            if(reader.hasEnd())
                break;
            c=reader.read();
            if(c == '-')
                bExponentialNegative=true;
            else if(c == '+')
                bExponentialNegative=false;
            else if(c>=0x30 && c<=0x39)
            {
                bOk=true;
                bExponential=c-0x30;
            }
            else
            {
                bNeedBack=true;
                break;
            }
        }
        else
        {
            bNeedBack=true;
            break;
        }
    }
    if(!bOk)
    {
        char s[64];
        snprintf(s, sizeof(s), "get num error[pos:%u]", (uint32_t)reader.getCur());
        throw TC_Json_Exception(s);
    }
    if(bNeedBack)
        reader.back();
    if(bExponentialNegative)
        iExponential=0-iExponential;
    double dResult=(iInt+dFloat)*pow(10,iExponential);
    if(bNegative)
        dResult=0-dResult;
    JsonValueNumPtr p = new JsonValueNum();
    p->value=dResult;
    return p;
}

//为了提高效率和代码好写就先这么写了
JsonValueBooleanPtr TC_Json::getBoolean(BufferJsonReader & reader,char c)
{
    bool bOk=false;
    bool bValue;
    if(c=='t'||c=='T')
    {
        c=reader.read();
        if(c=='r'||c=='R')
        {
            c=reader.read();
            if(c=='u'||c=='U')
            {
                c=reader.read();
                if(c=='e'||c=='E')
                {
                    bValue=true;
                    bOk=true;
                }
            }
        }
    }
    else if(c=='f'||c=='F')
    {
        c=reader.read();
        if(c=='a'||c=='A')
        {
            c=reader.read();
            if(c=='l'||c=='L')
            {
                c=reader.read();
                if(c=='s'||c=='S')
                {
                    c=reader.read();
                    if(c=='e'||c=='E')
                    {
                        bValue=false;
                        bOk=true;
                    }
                }
            }
        }
    }

    if(!bOk)
    {
        char s[64];
        snprintf(s, sizeof(s), "get bool error[pos:%u]", (uint32_t)reader.getCur());
        throw TC_Json_Exception(s);
    }

    JsonValueBooleanPtr p = new JsonValueBoolean();
    p->value=bValue;
    return p;
}

JsonValuePtr TC_Json::getNull(BufferJsonReader & reader,char c)
{
    assert(c=='n' || c=='N');
    bool bOk=false;
    c=reader.read();
    if(c=='u'||c=='U')
    {
        c=reader.read();
        if(c=='l'||c=='L')
        {
            c=reader.read();
            if(c=='l'||c=='L')
            {
                bOk=true;
            }
        }
    }
    if(!bOk)
    {
        char s[64];
        snprintf(s, sizeof(s), "get NULL error[pos:%u]", (uint32_t)reader.getCur());
        throw TC_Json_Exception(s);
    }
    return NULL;
}

uint16_t TC_Json::getHex(BufferJsonReader & reader)
{
    uint16_t iCode=0;
    char c;
    for(int iLoop=0;iLoop<4;iLoop++)
    {
        c=reader.read();
        if(c>='a'&&c<='f')
            iCode=iCode*16+c-'a'+10;
        else if(c>='A'&&c<='F')
            iCode=iCode*16+c-'A'+10;
        else if(c>='0'&&c<='9')
            iCode=iCode*16+c-'0';
        else
        {
            char s[64];
            snprintf(s, sizeof(s), "get string error(\\u)[pos:%u]", (uint32_t)reader.getCur());
            throw TC_Json_Exception(s);
        }
    }
    return iCode;
}

string TC_Json::writeValue(const JsonValuePtr & p)
{
    if(!p)
    {
        return "null";
    }
    switch(p->getType())
    {
        case eJsonTypeString :
            return writeString(JsonValueStringPtr::dynamicCast(p));
        case eJsonTypeNum:
            return writeNum(JsonValueNumPtr::dynamicCast(p));
        case eJsonTypeObj:
            return writeObj(JsonValueObjPtr::dynamicCast(p));
        case eJsonTypeArray:
            return writeArray(JsonValueArrayPtr::dynamicCast(p));
        case eJsonTypeBoolean:
            return writeBoolean(JsonValueBooleanPtr::dynamicCast(p));
        default:
            assert(false);
    }
}

string TC_Json::writeString(const JsonValueStringPtr & p)
{
    return writeString(p->value);
}

string TC_Json::writeString(const string & s)
{
    string sReturn;
    string sReplace;
    sReturn="\"";
    uint32_t iBegin=0;
    uint32_t i;
    for(i=0;i<s.size();i++)
    {
        if(s[i]=='"')
            sReplace="\\\"";
        else if(s[i]=='\\')
            sReplace="\\\\";
        else if(s[i]=='/')
            sReplace="\\/";
        else if(s[i]=='\b')
            sReplace="\\b";
        else if(s[i]=='\f')
            sReplace="\\f";
        else if(s[i]=='\n')
            sReplace="\\n";
        else if(s[i]=='\r')
            sReplace="\\r";
        else if(s[i]=='\t')
            sReplace="\\t";
        else if((unsigned char)s[i]<0x20)
        {
            sReplace="\\u";
            char buf[16];
            snprintf(buf,sizeof(buf),"%04x",(unsigned char)s[i]);
            sReplace=sReplace+string(buf,4);
        }
        else
            continue;
        sReturn=sReturn+s.substr(iBegin,i-iBegin);
        sReturn=sReturn+sReplace;
        iBegin=i+1;
    }
    sReturn=sReturn+s.substr(iBegin,i-iBegin);
    sReturn=sReturn+"\"";
    return sReturn;
}

string TC_Json::writeNum(const JsonValueNumPtr & p)
{
    ostringstream sBuffer;
    if(p->isInt)
        sBuffer<<setprecision(30)<<p->value;
    else
        sBuffer<<p->value;
    return sBuffer.str();
}

string TC_Json::writeObj(const JsonValueObjPtr & p)
{
    string s;
    s="{ ";
    map<string,JsonValuePtr>::iterator iter;
    iter=p->value.begin();
    for(;iter!=p->value.end();++iter)
    {
        if(iter != p->value.begin())
            s=s+", ";
        s=s+writeString(iter->first) + ": " + writeValue(iter->second);
    }
    s=s+" }";
    return s;
}

string TC_Json::writeArray(const JsonValueArrayPtr & p)
{
    string s;
    s="[ ";
    for(size_t i=0;i<p->value.size();++i)
    {
        if(i!=0)
            s=s+", ";
        s=s+writeValue(p->value[i]);
    }
    s=s+" ]";
    return s;
}

string TC_Json::writeBoolean(const JsonValueBooleanPtr & p)
{
    if(p->value)
        return "true";
    else
        return "false";
}

JsonValuePtr TC_Json::getValue(const string & str)
{
    BufferJsonReader reader;
    reader.setBuffer(str.c_str(),str.length());
    return getValue(reader);
}

//json里面定义的空白字符
bool TC_Json::isspace(char c)
{
    if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
        return true;
     return false;
}

}
