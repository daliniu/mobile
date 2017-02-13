#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2kjavap.h"

Jce2KJavaP::Jce2KJavaP():_bWithServant(false),_bRead(true), _bWrite(true), _bEvent(true)
{
	s_JCE_PACKAGE	= std::string("com.qq") + JCE_PACKAGE;
	s_PROXY_PACKAGE	= std::string("com.qq") + PROXY_PACKAGE;
	s_HOLDER_PACKAGE= std::string("com.qq") + HOLDER_PACKAGE;
	s_SERVER_PACKAGE= std::string("com.qq") + SERVER_PACKAGE;
	s_WUP_PACKAGE	= std::string("com.qq") + WUP_PACKAGE;
}

const std::string Jce2KJavaP::toTypeName(const TypePtr & pType) const
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pType);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool:		return "boolean";
			case Builtin::KindByte:		return "byte";
			case Builtin::KindShort:	return "short";
			case Builtin::KindInt:		return "int";
			case Builtin::KindLong:		return "long";
			case Builtin::KindFloat:	return "float";
			case Builtin::KindDouble:	return "double";
			case Builtin::KindString:	return "String";
			default:					return "";
		}
	}

	EnumPtr ePtr	= EnumPtr::dynamicCast(pType);
	if (ePtr)
	{
		return "int";
	}

	return "";
}

const std::string Jce2KJavaP::toReadFunction(const TypePtr & pType) const
{
	BuiltinPtr bPtr	= BuiltinPtr::dynamicCast(pType);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool:		return "readBoolean";
			case Builtin::KindByte:		return "readByte";
			case Builtin::KindShort:	return "readShort";
			case Builtin::KindInt:		return "readInt";
			case Builtin::KindLong:		return "readLong";
			case Builtin::KindFloat:	return "readFloat";
			case Builtin::KindDouble:	return "readDouble";
			case Builtin::KindString:	return "readString";
			default:					return "";
		}
	}

	EnumPtr ePtr	= EnumPtr::dynamicCast(pType);
	if (ePtr)
	{
		return "readInt";
	}

	return "read";
}

const std::string Jce2KJavaP::toTypeInit(const TypePtr & pType) const
{
	BuiltinPtr bPtr	= BuiltinPtr::dynamicCast(pType);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool:		return "false";
			case Builtin::KindByte:		return "0";
			case Builtin::KindShort:	return "0";
			case Builtin::KindInt:		return "0";
			case Builtin::KindLong:		return "0L";
			case Builtin::KindFloat:	return "0.0f";
			case Builtin::KindDouble:	return "0.0";
			case Builtin::KindString:	return "\"\"";
			default:					return "null";
		}
	}

	EnumPtr ePtr	= EnumPtr::dynamicCast(pType);
	if (ePtr)
	{
		return "0";
	}

	return "null";
}

void Jce2KJavaP::setBaseDir(const string &dir)
{
    _baseDir = dir;
}

void Jce2KJavaP::setBasePackage(const string &prefix)
{
    _packagePrefix = prefix;
    if(_packagePrefix.length() != 0 && _packagePrefix.substr(_packagePrefix.length()-1, 1) != ".")
    {
        _packagePrefix += ".";
    }
}

string Jce2KJavaP::getFilePath(const string &ns) const
{
    return _baseDir + "/" + taf::TC_Common::replace(_packagePrefix, ".", "/") + "/" + ns + "/";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	生成结构体的分析树
void Jce2KJavaP::createTree(const TypePtr &pPtr, CNode & node) const
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr) createTreeForBuiltin(bPtr, node);

	EnumPtr ePtr	= EnumPtr::dynamicCast(pPtr);
	if (ePtr) createTreeForEnum(ePtr, node);

	StructPtr sPtr 	= StructPtr::dynamicCast(pPtr);
	if (sPtr) createTreeForStruct(sPtr, node);

	VectorPtr vPtr 	= VectorPtr::dynamicCast(pPtr);
	if (vPtr) createTreeForVector(vPtr, node);

	MapPtr mPtr 	= MapPtr::dynamicCast(pPtr);
	if (mPtr) createTreeForMap(mPtr, node);
}

void Jce2KJavaP::createTreeForBuiltin(const BuiltinPtr & bPtr, CNode & thisnode) const
{
	thisnode.sFinalName	= thisnode.sFinalName.substr(0, thisnode.sFinalName.length() - 1);

	if (!thisnode.sInitValue.empty())
	{
		if(bPtr->kind() == Builtin::KindString)
		{
			thisnode.sTypeValue = "\"" + taf::TC_Common::replace(thisnode.sInitValue, "\"", "\\\"") + "\"";
			thisnode.sInitValue = "= (" + toTypeName(bPtr) + ")" + thisnode.sTypeValue;

		}
		else
		{
			thisnode.sTypeValue = thisnode.sInitValue;
			thisnode.sInitValue = "= (" + toTypeName(bPtr) + ")" + thisnode.sInitValue;
		}
	}
	else
	{
		thisnode.sTypeValue = toTypeInit(bPtr);
		thisnode.sInitValue = "= " + toTypeInit(bPtr);
	}

	if (!thisnode.sArray.empty())
	{
		thisnode.sInitValue = "= null";
	}
}

void Jce2KJavaP::createTreeForEnum(const EnumPtr & ePtr, CNode & thisnode) const
{
	thisnode.sFinalName	= thisnode.sFinalName.substr(0, thisnode.sFinalName.length() - 1);

	thisnode.sTypeValue = toTypeInit(ePtr);
	thisnode.sInitValue	= "= " + toTypeInit(ePtr);

	if (!thisnode.sArray.empty())
	{
		thisnode.sInitValue = "= null";
	}
}

void Jce2KJavaP::createTreeForStruct(const StructPtr & sPtr, CNode & thisnode) const
{
	std::vector<TypeIdPtr> & vecMembers = sPtr->getAllMemberPtr();
	for (std::vector<TypeIdPtr>::size_type i = 0; i < vecMembers.size(); i++)
	{
		CNode * subnode = new CNode();
		subnode->sInitName 		= vecMembers[i]->getId();
		subnode->sInitValue		= vecMembers[i]->def();
		subnode->iTag			= vecMembers[i]->getTag();
		subnode->bIsRequire		= vecMembers[i]->isRequire();
		subnode->pType			= vecMembers[i]->getTypePtr();
		subnode->sArray			= thisnode.sArray;
		subnode->sFinalName		= thisnode.sFinalName + subnode->sInitName + "_";
		subnode->sLoopPrefix	= thisnode.sLoopPrefix + "_" + taf::TC_Common::tostr<int>(subnode->iTag);
		subnode->sLocator		= thisnode.sLocator;

		thisnode.vecNodes.push_back(subnode);

		createTree(vecMembers[i]->getTypePtr(), *subnode);
	}
}

void Jce2KJavaP::createTreeForVector(const VectorPtr & vPtr, CNode & thisnode) const
{
	CNode * subnode = new CNode();
	subnode->sInitName		= "";
	subnode->sInitValue		= "";
	subnode->iTag			= 0;
	subnode->pType			= vPtr->getTypePtr();
	subnode->bIsRequire		= thisnode.bIsRequire;
	subnode->sFinalName		= thisnode.sFinalName;
	subnode->sArray			= thisnode.sArray + "[]";
	subnode->sLocator		= thisnode.sLocator + "[" + thisnode.sLoopPrefix + "]";
	subnode->sLoopPrefix	= thisnode.sLoopPrefix + "_" + taf::TC_Common::tostr<int>(subnode->iTag);
	thisnode.vecNodes.push_back(subnode);

	createTree(vPtr->getTypePtr(), *subnode);
}

void Jce2KJavaP::createTreeForMap(const MapPtr & mPtr, CNode & thisnode) const
{
	CNode * firstnode = new CNode();
	firstnode->sInitName 	= thisnode.sInitName + "First";
	firstnode->sInitValue	= "";
	firstnode->iTag			= 0;
	firstnode->bIsRequire	= thisnode.bIsRequire;
	firstnode->pType		= mPtr->getLeftTypePtr();
	firstnode->sArray		= thisnode.sArray + "[]";
	firstnode->sFinalName	= thisnode.sFinalName + "First" + "_";
	firstnode->sLocator		= thisnode.sLocator + "[" + thisnode.sLoopPrefix + "]";
	firstnode->sLoopPrefix	= thisnode.sLoopPrefix + "_" + taf::TC_Common::tostr<int>(0);
	thisnode.vecNodes.push_back(firstnode);

	CNode * secondnode = new CNode();
	secondnode->sInitName 	= thisnode.sInitName + "Second";
	secondnode->sInitValue	= "";
	secondnode->iTag		= 1;
	secondnode->bIsRequire	= thisnode.bIsRequire;
	secondnode->pType		= mPtr->getRightTypePtr();
	secondnode->sArray		= thisnode.sArray + "[]";
	secondnode->sFinalName	= thisnode.sFinalName + "Second" + "_";
	secondnode->sLocator	= thisnode.sLocator + "[" + thisnode.sLoopPrefix + "]";
	secondnode->sLoopPrefix	= thisnode.sLoopPrefix + "_" + taf::TC_Common::tostr<int>(0);
	thisnode.vecNodes.push_back(secondnode);

	createTree(firstnode->pType, *firstnode);
	createTree(secondnode->pType, *secondnode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	获取参数列表
void Jce2KJavaP::createParams(const CNode & thisnode, std::vector<std::pair<std::string, std::pair<std::string, std::string> > > & vec_params) const
{
	if (thisnode.vecNodes.size() == 0)
	{
		std::string sFinalName = thisnode.sFinalName;
		std::string sFinalType = toTypeName(thisnode.pType) + thisnode.sArray;
		std::string sFinalInit = thisnode.sInitValue;

		vec_params.push_back(std::make_pair(sFinalName, std::make_pair(sFinalType, sFinalInit)));
		return ;
	}

	for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
	{
		createParams(*thisnode.vecNodes[i], vec_params);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	生成解码代码
const std::string Jce2KJavaP::generateReadVector(const CNode & thisnode, const CNode & firstnode, const std::string & sSize) const
{
	std::ostringstream s;

	if (thisnode.vecNodes.size() == 0)
	{
		std::string sArray = thisnode.sArray.substr(firstnode.sArray.length() + 2);
		s << TAB << thisnode.sFinalName << firstnode.sLocator << " = new " << toTypeName(thisnode.pType) << "[" << sSize << "]" << sArray << ";" << std::endl;

		return s.str();
	}

	for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
	{
		s << generateReadVector(*thisnode.vecNodes[i], firstnode, sSize);
	}

	return s.str();
}

const std::string Jce2KJavaP::generateRead(const CNode & thisnode) const
{
	std::ostringstream s;

	static int iTag 		= 0;
	std::string sRequire 	= "isRequire_" + taf::TC_Common::tostr<int>(iTag++);
	std::string sHead 		= "aHead_" + taf::TC_Common::tostr<int>(iTag);
	std::string sSize 		= "iSize_" + taf::TC_Common::tostr<int>(iTag);

	if (thisnode.vecNodes.size() == 0)
	{
		s << TAB << thisnode.sFinalName << thisnode.sLocator << " = _is." << toReadFunction(thisnode.pType) << "(" << taf::TC_Common::tostr<int>(thisnode.iTag) << ", " << std::string(thisnode.bIsRequire?"true":"false") << ");" << std::endl;
	}

	if (StructPtr::dynamicCast(thisnode.pType)) //struct
	{
		s << TAB << "boolean " << sRequire << " = " << (thisnode.bIsRequire?"true":"false") << ";" << std::endl;
		s << TAB << "try" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "if (_is.skipToTag(" << thisnode.iTag << "))" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "int[] " << sHead << " = new int[2];" << std::endl;
		s << TAB << "_is.readHead(" << sHead << ");" << std::endl;
		s << TAB << "if (" << sHead << "[0] != " << s_JCE_PACKAGE << ".JceStruct.STRUCT_BEGIN)" << std::endl;
		s << TAB << "    throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"type mismatch.\");" << std::endl;

		for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
		{
			s << generateRead(*thisnode.vecNodes[i]);
		}

		s << TAB << "_is.skipToStructEnd();" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "else if (" << sRequire << ")" << std::endl;
		s << TAB << "{" << std::endl;
		s << TAB << "    throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"" << sRequire << " field not exist.\");" << std::endl;
		s << TAB << "}" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "catch (Exception e)" << std::endl;
		s << TAB << "{" << std::endl;
		s << TAB << "    throw new Exception(\" read msg decode error \");" << std::endl;
		s << TAB << "}" << std::endl;

		return s.str();
	}

	if (VectorPtr::dynamicCast(thisnode.pType))	//vector
	{
		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(VectorPtr::dynamicCast(thisnode.pType)->getTypePtr());
		EnumPtr ePtr = EnumPtr::dynamicCast(VectorPtr::dynamicCast(thisnode.pType)->getTypePtr());

		if ((bPtr && !toTypeName(bPtr).empty()) || (ePtr && !toTypeName(ePtr).empty()))
		{
			s << TAB << thisnode.vecNodes[0]->sFinalName << thisnode.sLocator << " = _is." << toReadFunction(VectorPtr::dynamicCast(thisnode.pType)->getTypePtr()) << "Array(" << thisnode.iTag << ", " << (thisnode.bIsRequire?"true":"false") << ");" << std::endl;
			return s.str();
		}

		s << TAB << "boolean " << sRequire << " = " << (thisnode.bIsRequire?"true":"false") << ";" << std::endl;
		s << TAB << "if (_is.skipToTag(" << thisnode.iTag << "))" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "int[] " << sHead << " = new int[2];" << std::endl;
		s << TAB << "_is.readHead(" << sHead << ");" << std::endl;
		s << TAB << "switch (" << sHead << "[0])" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "case " << s_JCE_PACKAGE << ".JceStruct.LIST:" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "int " << sSize << " = _is.readInt(0, true);" << std::endl;
		s << TAB << "if (" << sSize << " < 0)" << std::endl;
		s << TAB << "    throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"" << sSize << " invalid: \" + " << sSize << ");" << std::endl;

		s << generateReadVector(*thisnode.vecNodes[0], thisnode, sSize);

		s << TAB << "for (int " << thisnode.sLoopPrefix << " = 0; " << thisnode.sLoopPrefix << " < " << sSize << "; " << thisnode.sLoopPrefix << "++)" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << generateRead(*thisnode.vecNodes[0]);
		DEL_TAB;
		s << TAB << "}" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "break;" << std::endl;
		s << TAB << "default:	throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"type mismatch.\");" << std::endl;

		DEL_TAB;
		s << TAB << "}" << std::endl;

		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "else if (" << sRequire << ")" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"" << sRequire << " field not exist...\");" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;

		return s.str();
	}

	if (MapPtr::dynamicCast(thisnode.pType))	//map
	{
		s << TAB << "boolean " << sRequire << " = " << (thisnode.bIsRequire?"true":"false") << ";" << std::endl;
		s << TAB << "if (_is.skipToTag(" << thisnode.iTag << "))" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "int[] " << sHead << " = new int[2];" << std::endl;
		s << TAB << "_is.readHead(" << sHead << ");" << std::endl;
		s << TAB << "switch (" << sHead << "[0])" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "case " << s_JCE_PACKAGE << ".JceStruct.MAP:" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "int " << sSize << " = _is.readInt(0, true);" << std::endl;
		s << TAB << "if (" << sSize << " < 0)" << std::endl;
		s << TAB << "    throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"" << sSize << " invalid: \" + " << sSize << ");" << std::endl;

		s << generateReadVector(*thisnode.vecNodes[0], thisnode, sSize);
		s << generateReadVector(*thisnode.vecNodes[1], thisnode, sSize);

		s << TAB << "for (int " << thisnode.sLoopPrefix << " = 0; " << thisnode.sLoopPrefix << " < " << sSize << "; " << thisnode.sLoopPrefix << "++)" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << generateRead(*thisnode.vecNodes[0]);
		s << generateRead(*thisnode.vecNodes[1]);
		DEL_TAB;
		s << TAB << "}" << std::endl;

		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "break;" << std::endl;
		s << TAB << "default:	throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"type mismatch.\");" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;

		DEL_TAB;
		s << TAB << "}" << std::endl;
		s << TAB << "else if (" << sRequire << ")" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << TAB << "throw new " << s_JCE_PACKAGE << ".JceDecodeException(\"" << sRequire << " field not exist...\");" << std::endl;
		DEL_TAB;
		s << TAB << "}" << std::endl;

		return s.str();
	}

	for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
	{
		s<< generateRead(*thisnode.vecNodes[i]);
	}

	return s.str();
}

const std::string Jce2KJavaP::generateWrite(const CNode & thisnode) const
{
	std::ostringstream s;

	static int iTag 	= 0;
	std::string sSize	= "iSize_" + taf::TC_Common::tostr<int>(iTag++);

	if (thisnode.vecNodes.size() == 0)
	{
		s << TAB << "_os.write(" << thisnode.sFinalName << thisnode.sLocator << ", " << thisnode.iTag << ");" << std::endl;
	}

	if (StructPtr::dynamicCast(thisnode.pType)) //struct
	{
		s << TAB << "_os.writeHead(" << s_JCE_PACKAGE << ".JceStruct.STRUCT_BEGIN, " << thisnode.iTag << ");" << std::endl;

		for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
			s << generateWrite(*thisnode.vecNodes[i]);

		s << TAB << "_os.writeHead(" << s_JCE_PACKAGE << ".JceStruct.STRUCT_END, 0);" << std::endl;

		return s.str();
	}

	if (VectorPtr::dynamicCast(thisnode.pType)) //vector
	{
		std::string sFirst;
		for (CNode * node = thisnode.vecNodes[0]; ; )
		{
			if (node->vecNodes.size() == 0)
			{
				sFirst = node->sFinalName;
				break;
			}

			node = node->vecNodes[0];
		}

		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(VectorPtr::dynamicCast(thisnode.pType)->getTypePtr());
		if (bPtr && !toTypeName(bPtr).empty())
		{
			s << TAB << "_os.write(" << sFirst << thisnode.sLocator << ", " << thisnode.iTag << ");" << std::endl;
			return s.str();
		}

		s << TAB << "int " << sSize << " = " << sFirst << " == null?0:" << sFirst << ".length;" << std::endl;
		s << TAB << "_os.writeHead(" << s_JCE_PACKAGE << ".JceStruct.LIST, " << thisnode.iTag << ");" << std::endl;
		s << TAB << "_os.write(" << sSize << ", 0);" << std::endl;

		s << TAB << "for (int " << thisnode.sLoopPrefix << " = 0; " << thisnode.sLoopPrefix << " < " << sSize << "; " << thisnode.sLoopPrefix << "++)" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << generateWrite(*thisnode.vecNodes[0]);
		DEL_TAB;
		s << TAB << "}" << std::endl;

		return s.str();
	}

	if (MapPtr::dynamicCast(thisnode.pType))	//map
	{
		std::string sFirst;
		for (CNode * node = thisnode.vecNodes[0]; ; )
		{
			if (node->vecNodes.size() == 0)
			{
				sFirst = node->sFinalName;
				break;
			}

			node = node->vecNodes[0];
		}

		s << TAB << "int " << sSize << " = " << sFirst << " == null?0:" << sFirst << ".length;" << std::endl;

		s << TAB << "_os.writeHead(" << s_JCE_PACKAGE << ".JceStruct.MAP, " << thisnode.iTag << ");" << std::endl;
		s << TAB << "_os.write(" << sSize << ", 0);" << std::endl;

		s << TAB << "for (int " << thisnode.sLoopPrefix << " = 0; " << thisnode.sLoopPrefix << " < " << sSize << "; " << thisnode.sLoopPrefix << "++)" << std::endl;
		s << TAB << "{" << std::endl;
		INC_TAB;
		s << generateWrite(*thisnode.vecNodes[0]);
		s << generateWrite(*thisnode.vecNodes[1]);
		DEL_TAB;
		s << TAB << "}" << std::endl;

		return s.str();
	}

	for (std::vector<CNode *>::size_type i = 0; i < thisnode.vecNodes.size(); i++)
	{
		s << generateWrite(*thisnode.vecNodes[i]);
	}

	return s.str();
}

const std::string Jce2KJavaP::generateJava(const StructPtr &pPtr, const NamespacePtr &nPtr) const
{
	//生成该结构体的语法分析树以及最后生成的所有总结变量
	CNode root;
	root.sLoopPrefix = "iLoopTag";
	createTreeForStruct(pPtr, root);

	std::vector<std::pair<std::string, std::pair<std::string, std::string> > > vec_params;
	createParams(root, vec_params);

	std::ostringstream sEvent;
	std::ostringstream sFileText;
	sFileText << g_parse->printHeaderRemark() << std::endl;

	sFileText << "package " << _packagePrefix << nPtr->getId() << ";" << std::endl;
	sFileText << std::endl;

	sFileText << "public class " << pPtr->getId() << std::endl;
	sFileText << "{" << std::endl;

	if (_bRead == false) goto GEN_WRITE_LABEL;

	INC_TAB;
	sFileText << TAB << "public static void read" << pPtr->getId() << "(byte[] bodys, EventHandler handler)" << std::endl;
	sFileText << TAB << "{" << std::endl;

	INC_TAB;
	//定义并接初始化成员变量的代码
	sFileText << TAB << "//" << pPtr->getId() << "--成员变量定义以及初始化代码段" << std::endl;
	for (std::vector<std::pair<std::string, std::pair<std::string, std::string> > >::size_type i = 0; i < vec_params.size(); i++)
	{
		sFileText << TAB << vec_params[i].second.first << " " << vec_params[i].first << " " << vec_params[i].second.second << ";" << std::endl;
	}

	//下面开始生成读取参数代码
	sFileText << std::endl;
	sFileText << TAB << "//" << pPtr->getId() << "--结构体解码代码段" << std::endl;
	sFileText << TAB << "try" << std::endl;
	sFileText << TAB << "{" << std::endl;
	INC_TAB;
	sFileText << TAB << s_JCE_PACKAGE << ".JceInputStream _is = new " << s_JCE_PACKAGE << ".JceInputStream(bodys);" << std::endl;
	sFileText << generateRead(root) << std::endl;

	//调用EventHanlder代码段
    //add by edwardsu 10.11.19
    sFileText << TAB << "if(null != handler)"<<endl;
    sFileText << TAB << "{"<<endl;
    INC_TAB;
	sFileText << TAB <<"handler.executeRead" << pPtr->getId() << "(";
	INC_TAB;
	for (std::vector<std::pair<std::string, std::pair<std::string, std::string> > >::size_type i = 0; i < vec_params.size(); i++)
	{
		sFileText << vec_params[i].first << (i == vec_params.size() - 1?"":", ");
		if (i%2 == 1 && i != vec_params.size() - 1) sFileText << std::endl << TAB;
	}
	sFileText << ");" << std::endl;
    DEL_TAB;
	DEL_TAB;
    sFileText <<TAB << "}" << endl;
    //end add

	DEL_TAB;
	sFileText << TAB << "}" << std::endl;

	sFileText << TAB << "catch (Exception e)" << std::endl;
	sFileText << TAB << "{" << std::endl;
	INC_TAB;
	sFileText << TAB << "e.printStackTrace();" << std::endl;
	DEL_TAB;
	sFileText << TAB << "}" << std::endl;

	DEL_TAB;
	sFileText << TAB << "}" << std::endl << std::endl; //read的结束位置

	//生成EventHandler类的内容

	sEvent << TAB << "public static void executeRead" << pPtr->getId() << "(";
	INC_TAB;
	for (std::vector<std::pair<std::string, std::pair<std::string, std::string> > >::size_type i = 0; i < vec_params.size(); i++)
	{
		sEvent << vec_params[i].second.first << " " << vec_params[i].first << (i == vec_params.size() - 1?"":", ");
		if (i%2 == 1 && i != vec_params.size() - 1) sEvent << std::endl << TAB;
	}
	sEvent << ")" << std::endl;
	DEL_TAB;

	sEvent << TAB<< "{" << std::endl;
	INC_TAB;
	sEvent << TAB << "////TODO:" << std::endl << std::endl;
	DEL_TAB;
	sEvent << TAB << "}" << std::endl << std::endl;


GEN_WRITE_LABEL:
	//Wrirt区域代码段
	sFileText << TAB << "public static void write" << pPtr->getId() << "(" << s_JCE_PACKAGE << ".JceOutputStream _os, ";
	INC_TAB;
	for (std::vector<std::pair<std::string, std::pair<std::string, std::string> > >::size_type i = 0; i < vec_params.size(); i++)
	{
		sFileText << vec_params[i].second.first << " " << vec_params[i].first << (i == vec_params.size() - 1?"":", ");
		if (i%2 == 1 && i != vec_params.size() - 1) sFileText << std::endl << TAB;
	}
	sFileText << ")" << std::endl;
	DEL_TAB;

	sFileText << TAB << "{" << std::endl;

	INC_TAB;
	sFileText << TAB << "try" << std::endl;
	sFileText << TAB << "{" << std::endl;
	INC_TAB;
	sFileText << generateWrite(root);
	DEL_TAB;
	sFileText << TAB << "}" << std::endl;
	sFileText << TAB << "catch (Exception e)" << std::endl;
	sFileText << TAB << "{" << std::endl;
	INC_TAB;
	sFileText << TAB << "e.printStackTrace();" << std::endl;
	DEL_TAB;
	sFileText << TAB << "}" << std::endl;

	DEL_TAB;
	sFileText << TAB << "}" << std::endl;	//读结束

	DEL_TAB;
	sFileText << TAB << "}" << std::endl;	//命名空间结束

	//将生成的内容写到文件中
    std::string sFilePath  = getFilePath(nPtr->getId()) + pPtr->getId() + ".java";
    taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
    taf::TC_File::save2file(sFilePath, sFileText.str());

	return sEvent.str();
}

void Jce2KJavaP::generateConst(const vector<ConstPtr> &cs,const NamespacePtr & pPtr) const
{

    ostringstream s;
    s << g_parse->printHeaderRemark();
    s << TAB << "public class Const " << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    //-----------------const类型开始------------------------------------
    for(size_t i = 0; i < cs.size(); i++)
    {
        if(cs[i]->getConstTokPtr()->t == ConstTok::STRING)
        {
            string tmp = taf::TC_Common::replace(cs[i]->getConstTokPtr()->v, "\"", "\\\"");
            s  << TAB << "public static final "<< toTypeName(cs[i]->getTypeIdPtr()->getTypePtr()) << " "<<cs[i]->getTypeIdPtr()->getId()<< " = "
                << tmp << "\";"<< endl;
        }
        else if(toTypeName(cs[i]->getTypeIdPtr()->getTypePtr()) == "long")
        {
            s  << TAB << "public static final "<< toTypeName(cs[i]->getTypeIdPtr()->getTypePtr()) << " "<<cs[i]->getTypeIdPtr()->getId()<< " = "
                << cs[i]->getConstTokPtr()->v << "L;" << endl;
        }
        else
        {
            s  << TAB << "public static final "<< toTypeName(cs[i]->getTypeIdPtr()->getTypePtr()) <<" "<<cs[i]->getTypeIdPtr()->getId()<< " = "
               << cs[i]->getConstTokPtr()->v << ";" << endl;
        }
    }
    DEL_TAB;
    s << TAB << "}" << endl;

    string fileCs  = getFilePath(pPtr->getId()) + "Const.java";
    taf::TC_File::makeDirRecursive(getFilePath(pPtr->getId()), 0755);
    taf::TC_File::save2file(fileCs, s.str());

    return;
}
void Jce2KJavaP::generateJava(const NamespacePtr & pPtr) const
{
	//vector<EnumPtr>         &es    = pPtr->getAllEnumPtr();
	vector<ConstPtr>        &cs    = pPtr->getAllConstPtr();
	if(cs.size()>0)//如果有const变量就单独生成一个Const.java文件
	{
		generateConst(cs,pPtr);
	}
    std::ostringstream s;
	s << "package " << _packagePrefix << pPtr->getId() << ";" << std::endl << std::endl;
	s << TAB << "class EventHandler" << std::endl;
	s << TAB << "{" << std::endl;

	std::vector<StructPtr> & vecStructs = pPtr->getAllStructPtr();
    for (std::vector<StructPtr>::size_type i = 0; i < vecStructs.size(); i++)
    {
        s << generateJava(vecStructs[i], pPtr);
    }

	s << TAB << "}" << std::endl;

	if (_bEvent)
	{
		std::string sFilePath  = getFilePath(pPtr->getId()) + "EventHandler.java";
		taf::TC_File::makeDirRecursive(getFilePath(pPtr->getId()), 0755);
		taf::TC_File::save2file(sFilePath, s.str());
	}
}

void Jce2KJavaP::generateJava(const ContextPtr & pPtr) const
{
    std::ostringstream s;

	std::vector<NamespacePtr> vecNamespaces = pPtr->getNamespaces();

    for (std::vector<NamespacePtr>::size_type i = 0; i < vecNamespaces.size(); i++)
    {
        generateJava(vecNamespaces[i]);
    }
}

void Jce2KJavaP::createFile(const std::string & sJceFile) const
{
    std::vector<ContextPtr> vecContextPtr = g_parse->getContexts();

    for (std::vector<ContextPtr>::size_type i = 0; i < vecContextPtr.size(); i++)
    {
        if (sJceFile == vecContextPtr[i]->getFileName())
        {
            generateJava(vecContextPtr[i]);
        }
    }
}

