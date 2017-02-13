#include "parse.h"
#include "hce.tab.hpp"
#include <errno.h>
#include <fstream>
#include <string.h>
#include "util/tc_common.h"

HceParsePtr g_parse = new HceParse();

void yyerror(char const *msg)
{
    g_parse->error(msg);
}

HceParse::HceParse()
{
    _itab = 0;
    initScanner();
}

void HceParse::clear()
{
    while(!_contexts.empty()) _contexts.pop();
    while(!_contains.empty()) _contains.pop();
    _vcontexts.clear();
    _structs.clear();
    _namespaces.clear();
}

void HceParse::parse(const string& sFileName)
{
    clear();

    _contains.push(new Container(""));
    if(!(yyin = fopen(sFileName.c_str(), "r")))
    {
        error("open file '" + sFileName + "' error :" + string(strerror(errno)));
    }

    pushFile(sFileName);

    yyparse();
}

void HceParse::pushFile(const string &file)
{
    ContextPtr c = new Context(file); 
    _contexts.push(c);
    _vcontexts.push_back(c);
}

ContextPtr HceParse::popFile()
{
    ContextPtr c = _contexts.top();
    _contexts.pop();
    return c;
}

bool HceParse::getFilePath(const string &s, string &file)
{
    if(s.length() < 2 || s[0] != '\"' || s[s.length()-1] != '\"')
    {
        error("#include need \"FILENAME\"");
    }

    file = s.substr(1, s.length() - 2);

    for(size_t i = 0; i < _vcontexts.size(); i++)
    {
        if(_vcontexts[i]->getFileName() == file)
        {
            return false;
        }
    }

    return true;
}

string HceParse::getCurrFileName()
{
	return _contexts.top()->getFileName();
}

void HceParse::nextLine()
{
   _contexts.top()->nextLine();
}

ContextPtr HceParse::currentContextPtr()
{
    return _contexts.top();
}

void HceParse::error(const string &msg)
{
    cerr <<  _contexts.top()->getFileName() << ": " << _contexts.top()->getCurrLine() << ": error: " << msg << endl;
    exit(-1);
}

int HceParse::checkKeyword(const string& s)
{
    std::map<std::string, int>::const_iterator it = _keywordMap.find(s);
    if(it != _keywordMap.end())
    {
        return it->second;
    }

    return HCE_IDENTIFIER;
}

void HceParse::initScanner()
{
    _keywordMap["void"]     = HCE_VOID;
    _keywordMap["struct"]   = HCE_STRUCT;
    _keywordMap["bool"]     = HCE_BOOL;
    _keywordMap["byte"]     = HCE_BYTE;
    _keywordMap["short"]    = HCE_SHORT;
    _keywordMap["int"]      = HCE_INT;
    _keywordMap["double"]   = HCE_DOUBLE;
    _keywordMap["float"]    = HCE_FLOAT;
    _keywordMap["long"]     = HCE_LONG;
    _keywordMap["string"]   = HCE_STRING;
    _keywordMap["binary"]   = HCE_BINARY;
    _keywordMap["vector"]   = HCE_VECTOR;
    _keywordMap["map"]      = HCE_MAP;
    _keywordMap["module"]   = HCE_NAMESPACE;
    _keywordMap["interface"]= HCE_INTERFACE;
    _keywordMap["false"]    = HCE_FALSE;
    _keywordMap["true"]     = HCE_TRUE;
    _keywordMap["const"]    = HCE_CONST;
}

string HceParse::getTab()
{
    ostringstream s;
    for(int i = 0; i < _itab; i++)
    {
        s << "    ";
    }

    return s.str();
}

BuiltinPtr HceParse::createBuiltin(Builtin::Kind kind)
{
    return new Builtin(kind);
}

VectorPtr HceParse::createVector(const TypePtr &ptr)
{
    return new Vector(ptr);
}

MapPtr HceParse::createMap(const TypePtr &pleft, const TypePtr &pright)
{
    return new Map(pleft, pright);
}

void HceParse::addNamespacePtr(const NamespacePtr &nPtr)
{
    _namespaces.push_back(nPtr);
}

NamespacePtr HceParse::findNamespace(const string &id)
{
    for(size_t i = 0; i < _namespaces.size(); i++)
    {
        if(_namespaces[i]->getId() == id)
        {
            return _namespaces[i];
        }
    }

    return NULL;
}

NamespacePtr HceParse::currentNamespace()
{
    return _namespaces.back();
}

void HceParse::addStructPtr(const StructPtr &sPtr)
{
    _structs.push_back(sPtr);
}

StructPtr HceParse::findStruct(const string &sid)
{
    string ssid = sid;

    //在当前namespace中查找
    NamespacePtr np = currentNamespace();
    if(ssid.find("::") == string::npos)
    {
        ssid = np->getId() + "::" + ssid;
    }

    for(size_t i = 0; i < _structs.size(); i++)
    {
        if(_structs[i]->getSid() == ssid)
        {
            return _structs[i];
        }
    }

    return NULL;
}

void HceParse::checkConflict(const string &sid)
{
    //是否和结构重名
    if(findStruct(sid))
    {
        error("conflicts with struct '" + sid + "'");
    }
}

TypePtr HceParse::findUserType(const string &sid)
{
    StructPtr sPtr = findStruct(sid);
    if(sPtr) return sPtr;

    return NULL;
}

ContainerPtr HceParse::currentContainer()
{
    return _contains.top();
}

void HceParse::pushContainer(const ContainerPtr &c)
{
    _contains.push(c);
    NamespacePtr np = NamespacePtr::dynamicCast(c);
    if(np)
    {
        addNamespacePtr(np);
    }
}

ContainerPtr HceParse::popContainer()
{
    ContainerPtr c = _contains.top();
    _contains.pop();

    return c;
}

void HceParse::checkConstValue(TypeIdPtr &tPtr, int c)
{
    //只有内建类型才能有缺省值
    BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(tPtr->getTypePtr());
    if(!bPtr)
    {
        error("only base type can have default value");
    }

    int b = bPtr->kind();

    if(c == ConstTok::VALUE)
    {
        if(b == Builtin::KindBool)
        {
            error("default value of bool can only be true or false");
        }
        if(b == Builtin::KindString)
        {
            error("default value of string can only be \"string\"");
        }
    }
    else if(c == ConstTok::BOOL)
    {
        if(b != Builtin::KindBool)
        {
            error("only bool type can be true or false");
        }
    }
    else if(c == ConstTok::STRING)
    {
        if(b != Builtin::KindString)
        {
            error("only string type can be \"string\"");
        }
    }
}


