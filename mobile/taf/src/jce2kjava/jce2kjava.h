#ifndef _Jce2KJava_H
#define _Jce2KJava_H

#include "parse/parse.h"

#include <cassert>
#include <string>

#define JCE_PACKAGE     ".taf.jce"
#define PROXY_PACKAGE   ".taf.proxy"
#define HOLDER_PACKAGE  ".taf.holder"
#define SERVER_PACKAGE  ".taf.server"
#define WUP_PACKAGE     ".jce.wup"

/**
 * 根据jce生成java文件
 * 包括结构的编解码以及生成Proxy和Servant
 */
class Jce2KJava
{
public:
	Jce2KJava();

    /**
     * 设置代码生成的根目录
     * @param dir
     */
    void setBaseDir(const string &dir);

    /**
     * 设置包前缀
     * @param prefix
     */
    void setBasePackage(const string &prefix);

    /**
     * 设置是否需要服务端代码
     */
    void setWithServant(bool bWithServant) { _bWithServant = bWithServant;}

    /**
     * 生成
     * @param file
     * @param isFramework 是否是框架
     */
    void createFile(const string &file);

	/**
	 * 设置是否传送默认值
	 */
	void setCheckDefault(bool bCheck) 	{ _bCheckDefault = bCheck; }

	/**
	 * 设置是否生成比较函数
	 */
	void setEquals(bool bEquals) 		{ _bEquals = bEquals; }
	

	/**
	 * 设置是否根据结构体的名称，来选择生成readFrom或者writeTo函数
	 */
	void setByname(bool bByname) 		{ _bByname = bByname; }
	
    /**
     * 设置TAF库的报名
     */
    void setTafPacket(const std::string & sPacket) 
    {
        s_JCE_PACKAGE 		= sPacket + JCE_PACKAGE;
        s_PROXY_PACKAGE		= sPacket + PROXY_PACKAGE;
        s_HOLDER_PACKAGE	= sPacket + HOLDER_PACKAGE;
        s_SERVER_PACKAGE	= sPacket + SERVER_PACKAGE;
        s_WUP_PACKAGE		= sPacket + WUP_PACKAGE;
    }
    
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
    bool   _bWithServant;

	bool   _bCheckDefault;

	bool   _bEquals;

	bool   _bByname;
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

    //下面是类型描述的源码生成
protected:

    /*
     * 生成某类型的初始化字符串
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr &pPtr) const;

    /*
     * 生成某类型对应对象的初始化字符串
     * @param pPtr
     *
     * @return string
     */


    string toObjTypeInit(const TypePtr &pPtr) const;

    /**
     * 生成某类型的对应对象的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string toObjStr(const TypePtr &pPtr) const;

    /**
     * 判断是否是对象类型
     */
    bool isObjType(const TypePtr &pPtr) const;

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

    //以下是h和java文件的具体生成
protected:
    /**
     * 结构的md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * 生成结构的Holder类，用于引用传递
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateHolder(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 放置默认元素用于识别map/list类型
     * @param pPtr
     * @param sElemName 元素名称
     *
     * @return string
     */
    string generateDefautElem(const TypePtr &pPtr, const string & sElemName) const;

    /**
     * 生成结构的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成容器的java源码
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ContainerPtr &pPtr) const;

    /**
     * 生成参数声明的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ParamDeclPtr &pPtr) const;

    /**
     * 生成参数holder的java文件内容
     * @param pPtr
     *
     * @return string
     */
    string generateHolder(const ParamDeclPtr &pPtr) const;

    /**
     * 生成作操作的proxy的java文件内容
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateJava(const OperationPtr &pPtr, const string &cn) const;

    /**
     * 生成操作java文件函数调用分发的源码
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchJava(const OperationPtr &pPtr, const string &cn) const;


    /**
     * 生成接口的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateJava(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成Proxy接口的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrx(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成Proxy帮助类的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxHelper(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成Proxy回调类的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxCallback(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成服务框架类的java文件的源码
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateServant(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成枚举的头文件源码
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const EnumPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成常量java源码
     * @param pPtr
     * 
     * @return string
     */
    string generateJava(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * 生成名字空间java文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const NamespacePtr &pPtr) const;

    /**
     * 生成每个jce文件的java文件源码
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const ContextPtr &pPtr) const;
	
	
	void dependCompute(const StructPtr & sPtr, int flag) const;

	/**
	 * 计算结构体依赖关系的结构体 
	 * */ 
	//0x01: 生成readFrom;response生成
	//0x02：生成writeTo ;request生成

	mutable std::map<std::string, int> _mapDepend;
    
private:
    std::string s_JCE_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
};

#endif


