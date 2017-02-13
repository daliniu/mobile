#ifndef _HCE2HESSIAN_H
#define _HCE2HESSIAN_H

#include "parse.h"

#include <cassert>
#include <string>

/**
 * 根据hce生成c++文件
 */
class Hce2Cpp
{
public:

    /**
     * 生成源文件
     * @param file
     */
    void createFile(const string &file);

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
     * 结构的md5
     * @param pPtr
     *
     * @return string
     */
	string MD5(const StructPtr &pPtr) const;

    //以下头文件和cpp文件具体
protected:
    /**
     * 生成结构的头文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateH(const StructPtr &pPtr) const;

    /**
     * 生成结构的cpp文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const StructPtr &pPtr) const;

    /**
     * 生成容器的头文件源码
     * @param pPtr
     *
     * @return string
     */
	string generateH(const ContainerPtr &pPtr) const;

    /**
     * 生成容器的cpp源码
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const ContainerPtr &pPtr) const;

    /**
     * 生成参数声明的头文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateH(const ParamDeclPtr &pPtr) const;

    /**
     * 生成参数声明的cpp文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const ParamDeclPtr &pPtr) const;

    /**
     * 生成作操作的proxy的cpp文件内容
     * @param pPtr
     * @param cn
     *
     * @return string
     */
	string generateCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * 生成操作头文件内容
     * @param pPtr
     *
     * @return string
     */
	string generateH(const OperationPtr &pPtr) const;

    /**
     * 生成接口的头文件源码
     * @param pPtr
     *
     * @return string
     */
	string generateH(const InterfacePtr &pPtr) const;

    /**
     * 生的接口的cpp文件的源码
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const InterfacePtr &pPtr) const;

    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
	string generateH(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间cpp文件源码
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const NamespacePtr &pPtr) const;

    /**
     * 生成名字空间的头文件源码
     * @param pPtr
     *
     * @return string
     */
	void generateH(const ContextPtr &pPtr) const;

    /**
     * 生成名字空间cpp文件源码
     * @param pPtr
     *
     * @return string
     */
	void generateCpp(const ContextPtr &pPtr) const;
};

#endif

