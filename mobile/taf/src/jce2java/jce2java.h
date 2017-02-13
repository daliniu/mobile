#ifndef _JCE2JAVA_H
#define _JCE2JAVA_H

#include "parse/parse.h"

#include <cassert>
#include <string>

#define JCE_PACKAGE     ".taf.jce"
#define PROXY_PACKAGE   ".taf.proxy"
#define HOLDER_PACKAGE  ".taf.holder"
#define SERVER_PACKAGE  ".taf.server"
#define WUP_PACKAGE     ".jce.wup"
#define WSP_PACKAGE     ".tac2.wsp.build.WspStruct"

/**
 * 根据jce生成java文件
 * 包括结构的编解码以及生成Proxy和Servant
 */
class Jce2Java
{
public:
    Jce2Java();

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
     * 设置是否支持wsp
     */
    void setWithWsp(bool bWsp) { _bWithWsp = bWsp;}

	/**
	* 设置是否生成///@Async标签
	*/
	void setWithAysnc(bool bAsync) { _bWithAsync = bAsync; }

	/**
	* 默认将非byte的vector转list 加上该参数后转数组
	*/
	void setForceArray(bool bArray) { _bForceArray = bArray; }

	/**
	 * 设置是否生成Holder类
	 *
	 * @author kevintian (2010-9-3)
	 *
	 * @param bHolder
	 */
	void setHolder(bool bHolder) { _bHolder = bHolder; }


    void setCheckDefault(bool bCheck) { _bCheckDefault = bCheck; }

	/*支持自定义字符集，by johnson*/
    void setCharset(string charset) { _sCharset = charset; }
	/*支持javabean规范，by johnson*/
    void setWithJbr(bool bJbr) { _bWithJbr = bJbr;}


    void setGenSetterGetter(bool bGen)
    {
        _bGenSetterGetter=bGen;
    }

    /**
     * 生成
     * @param file
     * @param isFramework 是否是框架
     */
    void createFile(const string &file);

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
        s_WSP_PACKAGE		= sPacket + WSP_PACKAGE;
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
    bool _bWithServant;
    bool _bWithWsp;
    bool _bWithAsync;
    bool _bForceArray;
    bool _bHolder;
    bool _bCheckDefault;
    string _sCharset;
    bool _bWithJbr;
    bool _bGenSetterGetter;
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
    void generateJava(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

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

private:
    std::string s_JCE_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
    std::string s_WSP_PACKAGE;
};

#endif


