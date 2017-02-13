#include "hce2cpp.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

////////////////////////////////////////////////////////////////
//
string Hce2Cpp::MD5(const StructPtr &pPtr) const
{
    string s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for(size_t j = 0; j < member.size(); j++)
    {
        s += "_" + tostr(member[j]->getTypePtr());
    }

    return "\"" + taf::TC_MD5::md5str(s) + "\"";
}

string Hce2Cpp::tostr(const TypePtr &pPtr) const
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if(bPtr) return tostrBuiltin(bPtr);

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if(vPtr) return tostrVector(vPtr);

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if(mPtr) return tostrMap(mPtr);

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if(sPtr) return tostrStruct(sPtr);

	assert(false);
    return "";
}

string Hce2Cpp::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    assert(pPtr->kind() != Builtin::KindVector);
    assert(pPtr->kind() != Builtin::KindMap);

	if(pPtr->kind() == Builtin::KindByte)
	{
		return "char";
	}
	else if(pPtr->kind() == Builtin::KindLong)
	{
		return "long long";
	}
	else if(pPtr->kind() == Builtin::KindBinary)
	{
		return "taf::BinaryWrapper";
	}
	else
	{
		return Builtin::builtinTable[pPtr->kind()];
	}
}

string Hce2Cpp::tostrVector(const VectorPtr &pPtr) const
{
	string s = string("vector<") + tostr(pPtr->getTypePtr());

	if(MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s += " >";
	}
	else
	{
		s += ">";
	}

	return s;
}

string Hce2Cpp::tostrMap(const MapPtr &pPtr) const
{
	string s = string("map<") + tostr(pPtr->getLeftTypePtr()) + ", " + tostr(pPtr->getRightTypePtr());

    if(MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
  	{
		s += " >";
	}
	else
	{
		s += ">";
	}

	return s;
}

string Hce2Cpp::tostrStruct(const StructPtr &pPtr) const
{
	return pPtr->getSid();
}

//////////////////////////////////////////////////////////////////
//
string Hce2Cpp::generateH(const StructPtr &pPtr) const
{
	ostringstream s;

	s << g_parse->getTab() << "struct " << pPtr->getId() << endl;
	s << g_parse->getTab() << "{" << endl;
	s << g_parse->getTab() << "public:" << endl;

	g_parse->incTab();

	s << g_parse->getTab() << "static string MD5();" << endl;
	////////////////////////////////////////////////////////////
	s << g_parse->getTab() << pPtr->getId() << "(){};" << endl;

	////////////////////////////////////////////////////////////
	s << g_parse->getTab() << pPtr->getId() << "(const " << pPtr->getId() << " &t);" << endl;

    ////////////////////////////////////////////////////////////
    s << g_parse->getTab() << pPtr->getId() << "& operator=(const " << pPtr->getId() << " &t);" << endl;

	////////////////////////////////////////////////////////////
	s << g_parse->getTab() << "void decode(const taf::HObjectPtr &po);" << endl;

	///////////////////////////////////////////////////////////
	s << g_parse->getTab() << "taf::HObjectPtr encode() const;" << endl;

	g_parse->delTab();
	s << g_parse->getTab() << "public:" << endl;
	g_parse->incTab();

    //定义成员变量
	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	for(size_t j = 0; j < member.size(); j++)
	{
		s << g_parse->getTab() << tostr(member[j]->getTypePtr()) << " " << member[j]->getId() << ";" << endl;
	}
	g_parse->delTab();
	s << g_parse->getTab() << "};" << endl;

    //定义操作
    s << g_parse->getTab() << "bool operator==(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    s << g_parse->getTab() << "bool operator!=(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    /*
    s << g_parse->getTab() << "bool operator<(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    s << g_parse->getTab() << "bool operator<=(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    s << g_parse->getTab() << "bool operator>(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    s << g_parse->getTab() << "bool operator>=(const " << pPtr->getId() << "&, const " << pPtr->getId() << "&);" << endl;
    */

	return s.str();
}

string Hce2Cpp::generateCpp(const StructPtr &pPtr) const
{
	ostringstream s;

    //定义MD5函数
	s << g_parse->getTab() << "string " << pPtr->getId() << "::MD5()" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
	s << g_parse->getTab() << "return " << MD5(pPtr) << ";" << endl;
	g_parse->delTab();

	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

	s << g_parse->getTab() << "}" << endl;

	//定义构造函数
	s << g_parse->getTab() << pPtr->getId() << "::" << pPtr->getId() << "(const " << pPtr->getId() << " &t)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();

    for(size_t j = 0; j < member.size(); j++)
    {
        s << g_parse->getTab() << member[j]->getId() << " = t." << member[j]->getId() << ";" << endl;
    }

	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义赋值函数
    s << g_parse->getTab() << pPtr->getId() << "& " << pPtr->getId() << "::operator=(const " << pPtr->getId() << " &t)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "if(this != &t)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    for(size_t j = 0; j < member.size(); j++)
    {
        s << g_parse->getTab() << member[j]->getId() << " = t." << member[j]->getId() << ";" << endl;
    }
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;
    s << g_parse->getTab() << "return (*this);" << endl;

	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

	//定义解码函数
	s << g_parse->getTab() << "void " << pPtr->getId() << "::decode(const taf::HObjectPtr &po)" << endl;

	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();

    s << g_parse->getTab() << "taf::HMapPtr tPtr = taf::HMapPtr::dynamicCast(po);" << endl;
	for(size_t j = 0; j < member.size(); j++)
	{
        if(StructPtr::dynamicCast(member[j]->getTypePtr()))
        {
            s << g_parse->getTab() << "this->" << member[j]->getId() << ".decode(tPtr->find(new taf::HString(\"" << member[j]->getId() << "\")));" << endl;
        }
        else
        {
            s << g_parse->getTab() << "taf::decode(tPtr->find(new taf::HString(\"" << member[j]->getId() << "\")), this->" << member[j]->getId() << ");" << endl;
        }
	}

	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

	//定义编码函数
	s << g_parse->getTab() << "taf::HObjectPtr " << pPtr->getId() << "::encode() const" << endl;
	s << g_parse->getTab() << "{" << endl;

	g_parse->incTab();
    s << g_parse->getTab() << "taf::HMapPtr oPtr = new taf::HMap();" << endl;
	for(size_t j = 0; j < member.size(); j++)
	{
        if(StructPtr::dynamicCast(member[j]->getTypePtr()))
        {
            s << g_parse->getTab()<<"oPtr->value()[new taf::HString(\""<<member[j]->getId()<<"\")] = " << member[j]->getId() << ".encode();" << endl;
        }
        else
        {
            s << g_parse->getTab()<<"oPtr->value()[new taf::HString(\""<<member[j]->getId()<<"\")] = taf::encode(" << member[j]->getId() << ");" << endl;
        }
	}

	s << g_parse->getTab() << "return oPtr;" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义==操作
    s << g_parse->getTab() << "bool operator==(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
	s << g_parse->getTab() << "return ";
	for(size_t j = 0; j < member.size(); j++)
	{
		s << "l." << member[j]->getId() << " == r." << member[j]->getId();
		if(j != member.size() - 1)
		{
			s << " && ";
		}
	}
	s << ";" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义!=
    s << g_parse->getTab() << "bool operator!=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "return !(l == r);" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;
/*
    //定义<
    s << g_parse->getTab() << "bool operator<(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "return &l < &r;" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义<=
    s << g_parse->getTab() << "bool operator<=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "return l < r || l == r;" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义>
    s << g_parse->getTab() << "bool operator>(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "return !(l < r) && !(l == r);" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

    //定义>=
    s << g_parse->getTab() << "bool operator>=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();
    s << g_parse->getTab() << "return !(l < r);" << endl;
	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;
*/
	return s.str();
}

/*******************************ContainerPtr********************************/

string Hce2Cpp::generateH(const ContainerPtr &pPtr) const
{
	ostringstream s;
	for(size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
	{
		s << generateH(pPtr->getAllNamespacePtr()[i]) << endl;
		s << endl;
	}
	return s.str();
}

string Hce2Cpp::generateCpp(const ContainerPtr &pPtr) const
{
	ostringstream s;
	for(size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
	{
		s << generateCpp(pPtr->getAllNamespacePtr()[i]) << endl;
		s << endl;
	}
	return s.str();
}

/******************************ParamDeclPtr***************************************/
string Hce2Cpp::generateH(const ParamDeclPtr &pPtr) const
{
	ostringstream s;

	//输出参数, 或者是内建类型
	if(pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
	{
        s << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " ";
	}
	else
	{

		//输入参数
		s << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " &";
	}

	s << pPtr->getTypeIdPtr()->getId();

	return s.str();
}

string Hce2Cpp::generateCpp(const ParamDeclPtr &pPtr) const
{
	ostringstream s;

	if(pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
	{
		s << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " ";
	}
	else
	{
		//输入参数
		s << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " &";
	}

	s << pPtr->getTypeIdPtr()->getId();

	return s.str();
}

/******************************OperationPtr***************************************/

string Hce2Cpp::generateCpp(const OperationPtr &pPtr, const string &cn) const
{
	ostringstream s;

	//生成客户端代理源码
	s << g_parse->getTab();
	//生成函数声明
	if(!pPtr->getTypePtr())
	{
		s << "void";
	}
	else
	{
		s << tostr(pPtr->getTypePtr());
	}

	s << " " << cn << "Proxy::" << pPtr->getId() ;

	s << "(";
	vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

	for(size_t i = 0; i < vParamDecl.size(); i++)
	{
		s << generateCpp(vParamDecl[i]);
		if(i != vParamDecl.size() -1)
		{
			s << ", ";
		}
		else
		{
			s << ")";
		}
	}
	if(vParamDecl.size() == 0)
	{
		s << ")";
	}

	s << endl;

	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();

    //todo
	s << g_parse->getTab() << "vector<taf::HObjectPtr> vParam;" << endl;
	for(size_t i = 0; i < vParamDecl.size(); i++)
	{
        if(StructPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
        {
            s << g_parse->getTab() << "vParam.push_back(" << vParamDecl[i]->getTypeIdPtr()->getId() << ".encode());" << endl;
        }
        else
        {
            s << g_parse->getTab() << "vParam.push_back(taf::encode(" << vParamDecl[i]->getTypeIdPtr()->getId() << "));" << endl;
        }
	}

	s << g_parse->getTab() << "taf::HObjectPtr oPtr = call(\"" << pPtr->getId() << "\", vParam);" << endl;

    if(pPtr->getTypePtr())
    {
        s << g_parse->getTab() << tostr(pPtr->getTypePtr()) << " ret;" << endl;
        if(StructPtr::dynamicCast(pPtr->getTypePtr()))
        {
            s << g_parse->getTab() << "ret.decode(oPtr);" << endl;
        }
        else
        {
            s << g_parse->getTab() << "decode(oPtr, ret);" << endl;
        }
        s << g_parse->getTab() << "return ret;" << endl;
    }

	g_parse->delTab();
	s << g_parse->getTab() << "}" << endl;

	return s.str();
}

string Hce2Cpp::generateH(const OperationPtr &pPtr) const
{
	ostringstream s;
	vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

	s << g_parse->getTab();
	if(pPtr->getTypePtr())
	{
		s << tostr(pPtr->getTypePtr()) << " " << pPtr->getId();
	}
	else
	{
		s << "void " << pPtr->getId();
	}

	s << "(";
	for(size_t i = 0; i < vParamDecl.size(); i++)
	{
		s << generateH(vParamDecl[i]);
		if(i != vParamDecl.size() -1)
		{
			s << ", ";
		}
		else
		{
			s << ");";
		}
	}

	if(vParamDecl.size() == 0)
	{
		s << ");";
	}

	s << endl;

	return s.str();
}

/******************************InterfacePtr***************************************/

string Hce2Cpp::generateH(const InterfacePtr &pPtr) const
{
	ostringstream s;
	vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

	//生成客户端代理
	s << g_parse->getTab() << "class " << pPtr->getId() << "Proxy : public taf::HessianProxy" << endl;
	s << g_parse->getTab() << "{" << endl;
	s << g_parse->getTab() << "public:" << endl;
	g_parse->incTab();

	for(size_t i = 0; i < vOperation.size(); i++)
	{
		s << generateH(vOperation[i]) << endl;
	}

	g_parse->delTab();
	s << g_parse->getTab() << "};" << endl;

//	s << g_parse->getTab() << "typedef taf::TC_AutoPtr<" << pPtr->getId() << "Proxy> " << pPtr->getId() << "Prx;" << endl;

	return s.str();
}

string Hce2Cpp::generateCpp(const InterfacePtr &pPtr) const
{
	ostringstream s;
	vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

	//生成客户端接口的实现
	for(size_t i = 0; i < vOperation.size(); i++)
	{
		s << generateCpp(vOperation[i], pPtr->getId()) << endl;
	}
	return s.str();
}

/******************************NamespacePtr***************************************/

string Hce2Cpp::generateH(const NamespacePtr &pPtr) const
{
	ostringstream s;
	vector<InterfacePtr>	&is	= pPtr->getAllInterfacePtr();
	vector<StructPtr>		&ss	= pPtr->getAllStructPtr();

	s << endl;
	s << g_parse->getTab() << "namespace " << pPtr->getId() << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();

	for(size_t i = 0; i < ss.size(); i++)
	{
		s << generateH(ss[i]) << endl;
	}

	s << endl;

	for(size_t i = 0; i < is.size(); i++)
	{
		s << generateH(is[i]) << endl;
		s << endl;
	}

	g_parse->delTab();
	s << "}";

	return s.str();
}

string Hce2Cpp::generateCpp(const NamespacePtr &pPtr) const
{
	ostringstream s;
	vector<InterfacePtr>	&is	= pPtr->getAllInterfacePtr();
	vector<StructPtr>		&ss	= pPtr->getAllStructPtr();

	s << endl;
	s << g_parse->getTab() << "namespace " << pPtr->getId() << endl;
	s << g_parse->getTab() << "{" << endl;
	g_parse->incTab();

	for(size_t i = 0; i < ss.size(); i++)
	{
		s << generateCpp(ss[i]) << endl;
	}

	s << endl;

	for(size_t i = 0; i < is.size(); i++)
	{
		s << generateCpp(is[i]) << endl;
		s << endl;
	}

	g_parse->delTab();
	s << "}";

	return s.str();
}


/******************************Hce2Cpp***************************************/

void Hce2Cpp::generateH(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(pPtr->getFileName());

    string fileH    = n + ".h";
    string fileCpp  = n + ".cpp";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

	s << "#ifndef " << define << endl;
	s << "#define " << define << endl;
	s << endl;
	s << "#include <map>" << endl;
	s << "#include <string>" << endl;
	s << "#include <vector>" << endl;
    s << "#include \"hessian/HessianProxy.h\"" << endl;

    vector<string> include = pPtr->getIncludes();
    for(size_t i = 0; i < include.size(); i++)
    {
        s << "#include \"" << taf::TC_File::extractFileName(include[i]) << "\"" << endl;
    }

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateH(namespaces[i]) << endl;
    }

	s << endl;
	s << "#endif" << endl;

    taf::TC_File::save2file(fileH, s.str());
}


void Hce2Cpp::generateCpp(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(pPtr->getFileName());
    string fileCpp  = n + ".cpp";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

	s << "#include \"" << n << ".h\"" << endl;

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateCpp(namespaces[i]) << endl;
    }

	s << endl;

    taf::TC_File::save2file(fileCpp, s.str());
}

void Hce2Cpp::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for(size_t i = 0; i < contexts.size(); i++)
    {
        if(file == contexts[i]->getFileName())
        {
            generateH(contexts[i]);
            generateCpp(contexts[i]);
        }
    }
}


