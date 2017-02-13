#ifndef _JCE2SYMCPP_H
#define _JCE2SYMCPP_H

#include "parse/parse.h"
#include "servant/BaseF.h"

#include <cassert>
#include <string>

using namespace taf;

/**
 * 根据jce生成symbain 
 * c文件 包括结构的编解码以及生成Proxy和Servant 
 */
class Jce2Cpp
{
public:

    /**
     * 生成
     * @param file
     */
    void createFile(const string &file);

	/**
	* 设置生成文件的目录
	*
	*/
	void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }

	/**
	 * 设置是否传递默认值
	 * 
	 * @author kevintian (2010-10-8)
	 * 
	 * @param bCheck 
	 */
	void setCheckDefault(const bool bCheck) { m_bCheckDefault = bCheck; }
	/**
	 * 设置是否传递默认值
	 * 
	 * @author kevintian (2011-04-20)
	 * 
	 * @param bCheck 
	 */
	void setWithImportC(const bool bWith) { m_bWithImportC = bWith; }


    //下面是编解码的源码生成
protected:

    /**
     * 生成某类型的解码源码
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr &pPtr) const;

    /**
     * 生成某类型的编码源码
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string display(const TypeIdPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string displaySimple(const TypeIdPtr &pPtr, bool bSep) const;

    //下面是类型描述的源码生成
protected:
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
    string tostrMap(const MapPtr &pPtr) const;

    /**
     * 生成某种结构的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;

    /**
     * 生成某种枚举的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrEnum(const EnumPtr &pPtr) const;

    /**
     * 生成类型变量的解码源码
     * @param pPtr
     *
     * @return string
     */
    string decode(const TypeIdPtr &pPtr) const;

    /**
     * 生成类型变量的编码源码
     * @param pPtr
     *
     * @return string
     */
    string encode(const TypeIdPtr &pPtr) const;

    //以下是h和cpp文件的具体生成
protected:
    /**
     * 结构的md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * 生成结构的头文件内容
     * @param pPtr
     * @param namespaceId 命名空间
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成参数声明的头文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string generateOutH(const ParamDeclPtr &pPtr) const;



    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr) const;

    /**
     * 生成常量头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ConstPtr &pPtr) const;


    /**
     * 生成结构的cpp文件内容
     * @param pPtr
     * @param namespaceId 命名空间
     *
     * @return string
     */
    string generateCpp(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * 生成枚举的cpp文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const EnumPtr &pPtr) const;




    /**
     * 
     * 生成接口编解码代码
     * @param pPtr 
     * @param interface 
     */
    void generateCoder(const ContextPtr &pPtr) const;

    string generateCoder(const NamespacePtr &pPtr) const;

    string generateCoder(const InterfacePtr &pPtr) const;

    string generateCoder(const OperationPtr &pPtr) const;

    string generateCoderCpp(const NamespacePtr &pPtr) const;


	std::string m_sBaseDir;


	bool m_bCheckDefault;
	bool m_bWithImportC;
};

#endif


