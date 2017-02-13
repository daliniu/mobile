#ifndef _JCE2AS_H
#define _JCE2AS_H
#include "parse/parse.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"

class Jce2Js
{
public:
    void setBaseDir(const string &dir)
    {
        _baseDir = dir;
    }

    void setBasePackage(const string &prefix)
    {
        _packagePrefix = prefix;
        if (_packagePrefix.length() != 0 && _packagePrefix.substr(_packagePrefix.length()-1, 1) != ".")
        {
            _packagePrefix += ".";
        }
    }

public:
    string _packagePrefix;
    string _baseDir;

    string toFunctionName(const TypeIdPtr & pPtr, const std::string &sAction);

	string getDataType(const TypePtr & pPtr);

	string getDefault(const TypeIdPtr & pPtr, const std::string &sDefault);

	string generateJS(const ConstPtr &pPtr, const std::string &sNamespace);

	string generateJS(const EnumPtr &pPtr, const std::string &sNamespace);

    string generateJS(const StructPtr & pPtr, const NamespacePtr &nPtr);

    string generateJS(const NamespacePtr &pPtr);

    void generateJS(const ContextPtr &pPtr) ;
	void createFile(const string &file);

};

#endif

