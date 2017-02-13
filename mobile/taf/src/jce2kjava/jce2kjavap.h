#ifndef __JCE2KJAVAP_H__
#define __JCE2KJAVAP_H__

#include <string>
#include "parse/parse.h"

#define JCE_PACKAGE     ".taf.jce"
#define PROXY_PACKAGE   ".taf.proxy"
#define HOLDER_PACKAGE  ".taf.holder"
#define SERVER_PACKAGE  ".taf.server"
#define WUP_PACKAGE     ".jce.wup"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

struct CNode
{
	std::string sInitName;
	std::string sInitValue;
	int iTag;
	bool bIsRequire;
	TypePtr pType;					//当前node的类型

	std::string sFinalName;			//最后生成成员的名称
	std::string sArray;				//最后与iType组成sFinalName的类型
	std::string sLocator;			//sFinalName[iTag_5][iTag_5_1] = readInt
	
	std::string sLoopPrefix;		//循环变量的前缀
	std::string sTypeValue;			//原子类型的初始值

	std::vector<CNode *> vecNodes;
	CNode():iTag(0), bIsRequire(false), pType(NULL){}
};

class Jce2KJavaP
{
public:
	Jce2KJavaP();
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
	void createFile(const string &file) const;


	/**
	 * 设定是否生成read接口
	 */
	void setRead(const bool bRead) { _bRead = bRead; }

	/**
	 * 设定是否生成Write接口
	 */
	void setWrite(const bool bWrite) { _bWrite = bWrite; }

	/**
	 * 设定是否生成EventHandler示范
	 */
	void setEventHandler(const bool bEvent) { _bEvent = bEvent; }

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
	bool   _bRead;
	bool   _bWrite;
	bool   _bEvent;

private:
	/**
     * 生成结构的java文件内容
     * @param pPtr
     *
     * @return string
     */
    const std::string generateJava(const StructPtr & pPtr, const NamespacePtr & nPtr) const;
    
	/**
     * 生成Const变量的java文件内容
     * @param pPtr
     * @param cs
     * @return void
     */
    void generateConst(const vector<ConstPtr> &cs,const NamespacePtr & pPtr) const;
    
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

	const std::string toTypeName(const TypePtr & pType) const;
	const std::string toTypeInit(const TypePtr & pType) const;
	const std::string toReadFunction(const TypePtr & pType) const;
	
	void createTree(const TypePtr &in_tPtr, CNode & node) const ;
	void createTreeForBuiltin(const BuiltinPtr & bPtr, CNode & thisnode) const;
	void createTreeForEnum(const EnumPtr & ePtr, CNode & thisnode) const;
	void createTreeForStruct(const StructPtr & sPtr, CNode & thisnode) const;
	void createTreeForVector(const VectorPtr & vPtr, CNode & thisnode) const;
	void createTreeForMap(const MapPtr & mPtr, CNode & thisnode) const;

	void createParams(const CNode & thisnode, std::vector<std::pair<std::string, std::pair<std::string, std::string> > > & vec_params) const;

	const std::string generateRead(const CNode & thisnode) const;
	const std::string generateReadVector(const CNode & thisnode, const CNode & firstnode, const std::string & sSize) const;
	const std::string generateWrite(const CNode & thisnode) const;

private:
    std::string s_JCE_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
};

#endif

