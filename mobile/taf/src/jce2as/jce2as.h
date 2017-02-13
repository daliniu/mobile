#ifndef _JCE2AS_H
#define _JCE2AS_H
#include "parse/parse.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"


class Jce2as
{
public:
    /**
     * 设置代码生成的根目录
     * @param dir
     */
    void setBaseDir(const string &dir)
    {
        _baseDir = dir;
    }

    /**
     * 设置包前缀
     * @param prefix
     */
    void setBasePackage(const string &prefix);
	void createFile(const string &file);
protected:
    /**
     * 根据命名空间获取文件路径
     * @param ns 命名空间
     *
     * @return string
     */
    string getFilePath(const string &ns) const;

    string _packagePrefix;
    string _baseDir;
protected:
    /*
     * 生成某类型的初始化字符串
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr &pPtr) const;

    void toFunctionName(const TypeIdPtr & pPtr, const std::string & sAction, std::pair<string, string> & pairFunc) const;

    /**
     * 生成某类型的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) const;

    /**
     * 生成内建类型的字符串源码
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) const;
    /**
     * 生成vector的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) const;

    /**
     * 生成map的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr, bool bNew = false) const;

    /**
     * 生成某种结构的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;

protected:
    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateAS(const EnumPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成结构的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateAS(const StructPtr & pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成常量的头文件源码
     * @param pPtr
     *
     * @return
     */
    void generateAS(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成名字空间java文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateAS(const NamespacePtr &pPtr) const;

    /**
     * 生成每个jce文件的java文件源码
     * @param pPtr
     *
     * @return string
     */
	void generateAS(const ContextPtr &pPtr) const;


};

#endif

