#include "jce2mcarecpp.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
//
string Jce2Cpp::writeTo(const TypeIdPtr &pPtr) const
{
	ostringstream s;
	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());

	if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s << TAB << "_os.write((Int32)" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
	}
	else if (m_bCheckDefault == false || pPtr->isRequire() || (!pPtr->hasDefault() && !mPtr && !vPtr) || sPtr)
	{
		s << TAB << "_os.write(" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
	}
	else
	{
		std::string sCheckCond = "true";

		if (mPtr || vPtr)
		{
			sCheckCond = pPtr->getId() + ".size() > 0";
		}

		BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
		if (bPtr && bPtr->kind() == Builtin::KindString && pPtr->hasDefault())
		{
			string tmp = taf::TC_Common::replace(pPtr->def(), "\"", "\\\"");
			s << TAB << "String static" << pPtr->getId() << "((const signed char *)" << "\"" << tmp << "\");" << endl;
			sCheckCond = "!(" + pPtr->getId() + " == static" + pPtr->getId() + ")"; 
		}
		else if (pPtr->hasDefault())
		{
			sCheckCond = pPtr->getId() + " != " + pPtr->def();
		}
		
		s << TAB << "if (" << sCheckCond << ")" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "_os.write(" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
		DEL_TAB;
		s << TAB << "}" << endl;
	}

	return s.str();
}

string Jce2Cpp::readFrom(const TypeIdPtr &pPtr) const
{
	ostringstream s;
	if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s << TAB << "_is.read((Int32&)" << pPtr->getId();
	}
	else
	{
		s << TAB << "_is.read(" << pPtr->getId();
	}

	s << ", " << pPtr->getTag() << ", " << (pPtr->isRequire()?"true":"false") << ");" << endl;

	return s.str();
}

string Jce2Cpp::display(const TypeIdPtr &pPtr) const
{
	ostringstream s;
	if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s << TAB << "_ds.display((Int32)" << pPtr->getId() << ",\"" << pPtr->getId() << "\");" << endl;;
	}
	else
	{
		s << TAB << "_ds.display(" << pPtr->getId() << ",\"" << pPtr->getId() << "\");" << endl;;
	}

	return s.str();
}

string Jce2Cpp::displaySimple(const TypeIdPtr &pPtr, bool bSep) const
{
	ostringstream s;
	if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s << TAB << "_ds.displaySimple((Int32)" << pPtr->getId() << ", "
		<<(bSep ? "true" : "false") << ");" << endl;;
	}
	else
	{
		s << TAB << "_ds.displaySimple(" << pPtr->getId() << ", "
		<<(bSep ? "true" : "false") << ");" << endl;;
	}

	return s.str();
}

/*******************************BuiltinPtr********************************/
string Jce2Cpp::tostr(const TypePtr &pPtr) const
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr) return tostrBuiltin(bPtr);

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr) return tostrVector(vPtr);

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr) return tostrMap(mPtr);

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr) return tostrStruct(sPtr);

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
	if (ePtr) return tostrEnum(ePtr);

	if (!pPtr) return "void";

	assert(false);
	return "";
}

string Jce2Cpp::tostrBuiltin(const BuiltinPtr &pPtr) const
{
	string s;

	switch (pPtr->kind())
	{
	case Builtin::KindBool:
		s = "Bool";
		break;
	case Builtin::KindByte:
		s = "Char";
		break;
	case Builtin::KindShort:
		s = pPtr->isUnsigned()?"UInt8":"Short";
		break;
	case Builtin::KindInt:
		s = pPtr->isUnsigned()?"UInt16":"Int32";
		break;
	case Builtin::KindLong:
		s = pPtr->isUnsigned()?"UInt32":"Int64";
		break;
	case Builtin::KindFloat:
		s = "Float";
		break;
	case Builtin::KindDouble:
		s = "Double";
		break;
	case Builtin::KindString:
		s = "String";
		break;
	case Builtin::KindVector:
		s = "JArray";
		break;
	case Builtin::KindMap:
		s = "JMapWrapper";
		break;
	default:
		assert(false);
		break;
	}

	return s;
}
/*******************************VectorPtr********************************/
string Jce2Cpp::tostrVector(const VectorPtr &pPtr) const
{   

	string s = "JArray" + string("<") + tostr(pPtr->getTypePtr());
	if (MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s += " >";
	}
	else
	{
		s += ">";
	}
	return s;
}
/*******************************MapPtr********************************/
string Jce2Cpp::tostrMap(const MapPtr &pPtr) const
{
	string s = "JMapWrapper" + string("<") + tostr(pPtr->getLeftTypePtr()) + ", " + tostr(pPtr->getRightTypePtr());
	if (MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
	{
		s += " >";
	}
	else
	{
		s += ">";
	}
	return s;
}

/*******************************StructPtr********************************/
string Jce2Cpp::tostrStruct(const StructPtr &pPtr) const
{
	return pPtr->getSid();
}

string Jce2Cpp::MD5(const StructPtr &pPtr) const
{
	string s;
	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	for (size_t j = 0; j < member.size(); j++)
	{
		s += "_" + tostr(member[j]->getTypePtr());
	}

	return "\"" + taf::TC_MD5::md5str(s) + "\"";
}

/////////////////////////////////////////////////////////////////////
string Jce2Cpp::tostrEnum(const EnumPtr &pPtr) const
{
	return pPtr->getSid();
}
///////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateH(const StructPtr &pPtr, const string& namespaceId) const
{
	ostringstream s;

	s << TAB << "struct " << pPtr->getId() <<" : public JceStructBase" << endl;
	s << TAB << "{" << endl;
	s << TAB << "public:" << endl;

	INC_TAB;

	s << TAB << "String className();" << endl;
	s << TAB << "String structName() const;" << endl;
	////////////////////////////////////////////////////////////
	//定义缺省构造函数
	s << TAB << pPtr->getId() << "();" << endl;

	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	//定义reset操作
	s << TAB << "void reset();" << endl;
	////////////////////////////////////////////////////////////
	s << TAB << "void writeTo(JceOutputStream& _os) const;" << endl;

	///////////////////////////////////////////////////////////
	s << TAB << "void readFrom(JceInputStream& _is);" << endl;

	/*
	s << TAB << "void display(CConsoleBase * _con, int _level=0) const" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "taf::JceDisplayer _ds(_con, _level);" << endl;

	for(size_t j = 0; j < member.size(); j++)
	{
		s << display(member[j]);
	}
	DEL_TAB;
	s << TAB << "}" << endl;


	s << TAB << "void displaySimple(CConsoleBase * _con, int _level=0) const" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "taf::JceDisplayer _ds(_con, _level);" << endl;

	for(size_t j = 0; j < member.size(); j++)
	{
		s << displaySimple(member[j], (j!=member.size()-1 ? true : false));
	}
	DEL_TAB;
	s << TAB << "}" << endl;
	*/

	DEL_TAB;
	s << TAB << "public:" << endl;
	INC_TAB;

	//定义成员变量
	for (size_t j = 0; j < member.size(); j++)
	{
		s << TAB << tostr(member[j]->getTypePtr()) << " " << member[j]->getId() << ";" << endl;
	}
	s << endl;

	//定义 = 操作
	s << TAB << pPtr->getId() << "& operator=(const " << pPtr->getId() << "& r);" << endl;
	DEL_TAB;

	s << TAB << "};" << endl;


	//定义==操作
	s << TAB << "bool operator==(const " << pPtr->getId() << "& l, const " << pPtr->getId() << "& r);" << endl;

	//定义!=
	s << TAB << "bool operator!=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r);" << endl;

	vector<string> key = pPtr->getKey();
	//定义<
	if (key.size() > 0)
	{
		s << TAB << "bool operator<(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r);" << endl;

		//定义<=
		s << TAB << "bool operator<=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r);" << endl;

		//定义>
		s << TAB << "bool operator>(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r);" << endl;

		//定义>=
		s << TAB << "inline bool operator>=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r);" << endl;
	}
	return s.str();
}


///////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateCpp(const StructPtr &pPtr, const string& namespaceId) const
{
	ostringstream s;

	s << TAB << "String " <<pPtr->getId()<<"::className()" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	//s << TAB << "static taf::String sClassName((const signed char *)" << "\"" << namespaceId << "." << pPtr->getId() << "\");" << endl;
	//s << TAB << "return sClassName;" << endl;
    s << TAB << "return (const signed char *)" << "\"" << namespaceId << "." << pPtr->getId() << "\";" << endl;
    DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;

	s << TAB << "String " <<pPtr->getId()<<"::structName() const" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	//s << TAB << "static taf::String sStructName((const signed char *)" << "\"" << namespaceId << "." << pPtr->getId() << "\"" << ");" << endl;
	//s << TAB << "return sStructName;" << endl;
    s << TAB << "return (const signed char *)" << "\"" << namespaceId << "." << pPtr->getId() << "\";" << endl;
    DEL_TAB;
	s << TAB << "}" << endl;

	////////////////////////////////////////////////////////////
	//定义缺省构造函数
	s << endl;
	s << TAB <<pPtr->getId()<<"::"<< pPtr->getId() << "()" << endl;

	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	bool b = false;
	for (size_t j = 0; j < member.size(); j++)
	{
		if (member[j]->hasDefault())
		{
			if (!b)
			{
				s << TAB << ": ";
			}
			else
			{
				s << ",";
			}
			BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
			//string值要转义
			if (bPtr && bPtr->kind() == Builtin::KindString)
			{
				string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
				s << member[j]->getId() << "(String((const signed char *)\"" << tmp << "\"))";
			}
			else
			{
				s << member[j]->getId() << "(" << member[j]->def() << ")";
			}
			b = true;
		}
	}
	if (b)
	{
		s << endl;
	}

	s << TAB << "{" << endl;
	s << TAB << "}" << endl;

	s << endl;
	//定义reset操作
	s << TAB << "void "<<pPtr->getId()<<"::reset()" << endl;

	s << TAB << "{" << endl;
	INC_TAB;
	for (size_t j = 0; j < member.size(); j++)
	{
		if (member[j]->hasDefault())
		{
			BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
			//string值要转义
			if (bPtr && bPtr->kind() == Builtin::KindString)
			{
				string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
				s << TAB << member[j]->getId() << " = String((const signed char *)\"" << tmp << "\");" << endl;
			}
			else
			{
				s << TAB << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
			}
		}

		VectorPtr vPtr  = VectorPtr::dynamicCast(member[j]->getTypePtr());
		if (vPtr)
		{
			s << TAB << member[j]->getId() << ".clear();" << endl;
		}

		MapPtr mPtr  = MapPtr::dynamicCast(member[j]->getTypePtr());
		if (mPtr)
		{
			s << TAB << member[j]->getId() << ".clear();" << endl;
		}
	}

	DEL_TAB;
	s << TAB << "}" << endl;


	////////////////////////////////////////////////////////////
	s << endl;
	s << TAB << "void "<<pPtr->getId()<<"::writeTo(JceOutputStream& _os) const" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	for (size_t j = 0; j < member.size(); j++)
	{
		s << writeTo(member[j]);
	}
	DEL_TAB;
	s << TAB << "}" << endl;

	///////////////////////////////////////////////////////////
	s << endl;
	s << TAB << "void "<<pPtr->getId()<<"::readFrom(JceInputStream& _is)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	for (size_t j = 0; j < member.size(); j++)
	{
		s << readFrom(member[j]);
	}

	DEL_TAB;
	s << TAB << "}" << endl;

	//定义 = 操作
	s << endl;
	s << TAB << pPtr->getId() << "& "<<pPtr->getId()<<"::operator=(const " << pPtr->getId() << "& r)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	for (size_t j = 0; j < member.size(); j++)
	{
		s << TAB << "this->" << member[j]->getId() << " = r." << member[j]->getId() << ";" << endl;
	}
	s << TAB << "return *this;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;


	//定义==操作
	s << endl;
	s << TAB << "bool operator==(const " << pPtr->getId() << "& l, const " << pPtr->getId() << "& r)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "return ";
	for (size_t j = 0; j < member.size(); j++)
	{
		s << "l." << member[j]->getId() << " == r." << member[j]->getId();
		if (j != member.size() - 1)
		{
			s << " && ";
		}
	}
	s << ";" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	//定义!=
	s << endl;
	s << TAB << "bool operator!=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "return !(l == r);" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	vector<string> key = pPtr->getKey();
	//定义<
	if (key.size() > 0)
	{
		s << endl;
		s << TAB << "bool operator<(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		for (size_t i=0; i < key.size(); i++)
		{
			s << TAB << "if(!(l." << key[i] << " == r." << key[i] << ")) return (l." << key[i] << " < r." << key[i] << ");" << endl;
		}

		s << TAB << "return false;" << endl;
		DEL_TAB;
		s << TAB << "}" << endl;

		//定义<=
		s << endl;
		s << TAB << "bool operator<=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "return !(r < l);" << endl;
		DEL_TAB;
		s << TAB << "}" << endl;

		//定义>
		s << endl;
		s << TAB << "bool operator>(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "return r < l;" << endl;
		DEL_TAB;
		s << TAB << "}" << endl;

		//定义>=
		s << endl;
		s << TAB << "bool operator>=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "return !(l < r);" << endl;
		DEL_TAB;
		s << TAB << "}" << endl;
	}

	return s.str();
}



/******************************ParamDeclPtr***************************************/
string Jce2Cpp::generateH(const ParamDeclPtr &pPtr) const
{
	ostringstream s;

	//输出参数, 或简单类型
	if (pPtr->isOut() || pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
	{
		s << tostr(pPtr->getTypeIdPtr()->getTypePtr());
	}
	else
	{
		//结构, map, vector, string
		s << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " &";
	}

	if (pPtr->isOut())
	{
		s << " &";
	}
	else
	{
		s << " ";
	}
	s << pPtr->getTypeIdPtr()->getId();

	return s.str();
}

string Jce2Cpp::generateOutH(const ParamDeclPtr &pPtr) const
{
	if (!pPtr->isOut())	return "";

	ostringstream s;

	//输出参数, 或简单类型
	if (pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
	{
		s << tostr(pPtr->getTypeIdPtr()->getTypePtr())<<" ";
	}
	else
	{
		//结构, map, vector, string
		s << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " &";
	}
	s << pPtr->getTypeIdPtr()->getId();

	return s.str();
}


struct SortOperation
{
	bool operator()(const OperationPtr &o1, const OperationPtr &o2)
	{
		return o1->getId() < o2->getId();
	}
};


/******************************EnumPtr***************************************/

string Jce2Cpp::generateH(const EnumPtr &pPtr) const
{
	ostringstream s;
	s << TAB << "enum " << pPtr->getId() << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << member[i]->getId();
		if (member[i]->hasDefault())
		{
			s << " = " << member[i]->def();
		}
		s << "," << endl;
	}
	DEL_TAB;
	s << TAB << "};" << endl;

	//生成枚举转字符串函数
	s << TAB << "inline String etos" << "(const " <<  pPtr->getId() << " & e);" << endl;

	//生成字符串转枚举函数
	s << TAB << "inline Int32 stoe" << "(const String & s, " <<  pPtr->getId() << " & e);" << endl;
	return s.str();
}


string Jce2Cpp::generateCpp(const EnumPtr &pPtr) const
{
	ostringstream s;
	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

	//生成枚举转字符串函数
	s << TAB << "String etos" << "(const " <<  pPtr->getId() << " & e)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;

	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << "String s" <<  member[i]->getId() << "Name((const signed char *)" << "\"" << member[i]->getId() << "\");" << endl;
	}

	s << endl;

	s << TAB << "switch(e)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;

	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << "case " <<  member[i]->getId() << ": return s" << member[i]->getId() << "Name;" << endl;
	}
	s << TAB << "default: return String((const signed char *)\"\");" << endl;


	DEL_TAB;
	s << TAB << "}" << endl;
	//s << TAB << "return taf::String((const signed char *)\"\");" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	//生成字符串转枚举函数
	s << endl;
	s << TAB << "Int32 stoe" << "(const String & s, " <<  pPtr->getId() << " & e)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;

	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << "String s" <<  member[i]->getId() << "Name((const signed char *)" << "\"" << member[i]->getId() << "\");" << endl;
	}

	s << endl;

	for (size_t i = 0; i < member.size(); i++)
	{
		s << TAB << "if(s == s" << member[i]->getId() << "Name)  { e="<< member[i]->getId() << "; return 0; }" << endl;
	}
	s << endl;
	s << TAB << "return -1;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	return s.str();
}

/******************************ConstPtr***************************************/
string Jce2Cpp::generateH(const ConstPtr &pPtr) const
{
	ostringstream s;

	if (pPtr->getConstTokPtr()->t == ConstTok::STRING)
	{
		string tmp = taf::TC_Common::replace(pPtr->getConstTokPtr()->v, "\"", "\\\"");
		s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";"<< endl;
	}
	else
	{
		s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstTokPtr()->v << ";" << endl;
	}

	return s.str();
}




/******************************Jce2Cpp***************************************/




void Jce2Cpp::createFile(const string &file)
{
	std::vector<ContextPtr> contexts = g_parse->getContexts();
	for (size_t i = 0; i < contexts.size(); i++)
	{
		if (file == contexts[i]->getFileName())
		{
			generateCoder(contexts[i]);
		}
	}
}


////////////////////////////////
//for coder generating
////////////////////////////////

string Jce2Cpp::generateCoder(const NamespacePtr &pPtr) const
{
	ostringstream s;
	vector<StructPtr>       &ss    = pPtr->getAllStructPtr();
	vector<EnumPtr>         &es    = pPtr->getAllEnumPtr();
	vector<ConstPtr>        &cs    = pPtr->getAllConstPtr();

	s << endl;
	//s << TAB << "namespace " << pPtr->getId() << endl;
	//s << TAB << "{" << endl;
	//INC_TAB;

	for (size_t i = 0; i < cs.size(); i++)
	{
		s << generateH(cs[i]) << endl;
	}

	for (size_t i = 0; i < es.size(); i++)
	{
		s << generateH(es[i]) << endl;
	}

	for (size_t i = 0; i < ss.size(); i++)
	{
		s << generateH(ss[i],  pPtr->getId()) << endl;
	}

	s << endl;

	//DEL_TAB;
	//s << "}";

	s << endl << endl;

	return s.str();
}

string Jce2Cpp::generateCoderCpp(const NamespacePtr &pPtr) const
{
	ostringstream s;
	vector<StructPtr>       &ss    = pPtr->getAllStructPtr();
	vector<EnumPtr>         &es    = pPtr->getAllEnumPtr();

	s << endl;
	//s << TAB << "namespace " << pPtr->getId() << endl;
	//s << TAB << "{" << endl;
	//INC_TAB;

	for (size_t i = 0; i < es.size(); i++)
	{
		s << generateCpp(es[i]) << endl;
	}

	for (size_t i = 0; i < ss.size(); i++)
	{
		s << generateCpp(ss[i],  pPtr->getId()) << endl;
	}

	s << endl;

	//DEL_TAB;
	//s << "}";

	s << endl << endl;

	return s.str();
}

string Jce2Cpp::generateCoder(const InterfacePtr &pPtr) const
{
	ostringstream s;

	vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

	//生成编解码类
	s << TAB << "// encode and decode for client" << endl;
	s << TAB << "class " << pPtr->getId() << "Coder" << endl;
	s << TAB << "{" << endl;
	s << TAB << "public:" << endl << endl;
	INC_TAB;
	s << TAB << "typedef JMapWrapper<String, String> TAF_CONTEXT;" << endl << endl;

	s << TAB << "enum enumResult" << endl;
	s << TAB << "{" << endl;
	INC_TAB;

	s << TAB << "eJceServerSuccess      = 0," << endl;
	s << TAB << "eJcePacketLess         = 1," << endl;
	s << TAB << "eJcePacketErr          = 2," << endl;
	s << TAB << "eJceServerDecodeErr    = -1," << endl;
	s << TAB << "eJceServerEncodeErr    = -2," << endl;
	s << TAB << "eJceServerNoFuncErr    = -3," << endl;
	s << TAB << "eJceServerNoServantErr = -4," << endl;
	s << TAB << "eJceServerQueueTimeout = -6," << endl;
	s << TAB << "eJceAsyncCallTimeout   = -7," << endl;
	s << TAB << "eJceProxyConnectErr    = -8," << endl;
	s << TAB << "eJceServerUnknownErr   = -99," << endl;

	DEL_TAB;
	s << TAB << "};" << endl << endl;

	for (size_t i = 0; i < vOperation.size(); i++)
	{
		s << generateCoder(vOperation[i]) << endl;
	}

	DEL_TAB;
	s << TAB << "protected:" << endl << endl;
	INC_TAB;
	s << TAB << "Int32 fetchPacket(const String & in, String & out)" << endl;
	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "if(in.length() < sizeof(Int32)) return eJcePacketLess;" << endl;

	s << TAB << "Int32 iHeaderLen;" << endl;
	s << TAB << "memcpy(&iHeaderLen, in.c_str(), sizeof(Int32));" << endl;

	s << TAB << "iHeaderLen = ntohl(iHeaderLen);" << endl;
	s << TAB << "if((Int32)in.length() < iHeaderLen) return eJcePacketLess;" << endl;
	s << TAB << "if(iHeaderLen < (Int32)sizeof(Int32) || iHeaderLen > 100000000) return eJcePacketErr;" << endl;

	s << TAB << "out.assign(in.data()+sizeof(Int32), iHeaderLen - sizeof(Int32)); " << endl;
	s << TAB << "return 0;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;
	s << TAB << "String encodeBasePacket(const String & sServantName, const String & sFuncName, const JArray<char> & buffer, "
	<<"const JMapWrapper<String, String>& context = TAF_CONTEXT())" <<endl;
	s << TAB << "{" << endl;
	INC_TAB;

	s << TAB << "JceOutputStream os;" << endl;
	s << TAB << "os.write(" << JCEVERSION << ", 1);" << endl;
	s << TAB << "os.write(" << (int)JCENORMAL  << ", 2);" << endl;
	s << TAB << "os.write(" << JCEMESSAGETYPENULL << ", 3);" << endl; 
	s << TAB << "os.write(0, 4);" << endl;
	s << TAB << "os.write(sServantName, 5);" << endl;
	s << TAB << "os.write(sFuncName, 6);" << endl;
	s << TAB << "os.write(buffer, 7);" << endl;
	s << TAB << "os.write(60, 8);" << endl;
	s << TAB << "os.write(context, 9);" << endl;
	s << TAB << "os.write(JMapWrapper<String, String>(), 10);" << endl;

	s << TAB << "Int32 iHeaderLen;" << endl;
	s << TAB << "iHeaderLen = htonl(sizeof(Int32) + os.getLength());" << endl;
	s << TAB << "String s;" << endl;
	s << TAB << "s.append((const char*)&iHeaderLen, sizeof(Int32));" << endl;
	s << TAB << "s.append(os.getBuffer(), os.getLength());" << endl;

	s << TAB << "return s;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;
	s << TAB << "Int32 decodeBasePacket(const String & in, Int32 & iServerRet, JArray<char> & buffer)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;

	s << TAB << "JceInputStream is;" << endl;
	s << TAB << "is.setBuffer(in.c_str(), in.length());" << endl;
	s << TAB << "is.read(iServerRet, 5, true);" << endl;
	s << TAB << "is.read(buffer, 6, true);" << endl;

	s << TAB << "return 0;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;

	DEL_TAB;
	s << TAB << "};" << endl;

	return s.str();
}

string Jce2Cpp::generateCoder(const OperationPtr &pPtr) const
{
	ostringstream s;
	vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

	//编码函数
	s << TAB << "//encode & decode function for '" << pPtr->getId() << "()'"<< endl << endl;
	s << TAB << "String encode_" << pPtr->getId() << "(const String & sServantName, ";

	for (size_t i = 0; i < vParamDecl.size(); i++)
	{
		if (!vParamDecl[i]->isOut())
		{
			s << generateH(vParamDecl[i]) << ",";
		}
	}
	s << endl;
	s << TAB << "    const JMapWrapper<String, String>& context = TAF_CONTEXT())" << endl;
	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "try" << endl;
	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "JceOutputStream _os;" << endl;

	for (size_t i = 0; i < vParamDecl.size(); i++)
	{
		if (vParamDecl[i]->isOut())	continue;
		s << writeTo(vParamDecl[i]->getTypeIdPtr());
	}
	s << TAB << "JArray<char> buffer;"<<endl;
	s << TAB << "buffer.assign(_os.getBuffer(),_os.getLength());"<<endl;
	s << TAB << "return encodeBasePacket(sServantName, \""<< pPtr->getId() << "\", buffer, context);" <<endl;

	DEL_TAB;

	s << TAB << "}" << endl;
	s << TAB << "catch (JceException & ex)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "return String((const signed char *)\"\");" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	s << endl;

	//解码函数

	s << TAB << "Int32 decode_" << pPtr->getId() << "(const String & in ";

	if (pPtr->getReturnPtr()->getTypePtr())
	{
		s << ", " << tostr(pPtr->getReturnPtr()->getTypePtr()) << " & _ret ";
	}
	for (size_t i = 0; i < vParamDecl.size(); i++)
	{
		if (!vParamDecl[i]->isOut())
			continue;

		s << ", " << generateH(vParamDecl[i]);
	}
	s << ")" <<endl;

	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "try" << endl;
	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "String out;" << endl;
	s << TAB << "Int32 iRet = 0;" << endl;
	s << TAB << "if((iRet = fetchPacket(in, out)) != 0) return iRet;" << endl;

	s << TAB << "JceInputStream _is;" << endl;
	s << TAB << "Int32 iServerRet=0;" << endl;
	s << TAB << "JArray<char> buffer;" << endl;
	s << TAB << "decodeBasePacket(out, iServerRet, buffer);" << endl;
	s << TAB << "if(iServerRet != 0)  return iServerRet;" << endl;

	s << TAB << "_is.setBuffer(buffer.data(),buffer.size());" << endl;

	if (pPtr->getReturnPtr()->getTypePtr())
	{
		s << readFrom(pPtr->getReturnPtr());
	}

	for (size_t i = 0; i < vParamDecl.size(); i++)
	{
		if (vParamDecl[i]->isOut())
		{
			s << readFrom(vParamDecl[i]->getTypeIdPtr());
		}
	}


	s << TAB << "return 0;" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;
	s << TAB << "catch (JceException & ex)" << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "return eJcePacketErr;" << endl;
	DEL_TAB;
	s << TAB << "}" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;


	s << endl;

	return s.str();
}

void Jce2Cpp::generateCoder(const ContextPtr &pPtr) const
{   
	string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));

	string fileH    = m_sBaseDir + "/" + n + ".h";

	string fileCpp  = m_sBaseDir + "/" + n + ".cpp";

	string define   = taf::TC_Common::upper("__" + n + "_h_");

	ostringstream s;

	s << "#ifndef " << define << endl;
	s << "#define " << define << endl;
	s << endl;
	//s << "#include \"Jce_sym.h\"" << endl;
	//s << "#include \"sys/types.h\"" << endl;
	//s << "#include \"netinet/in.h\"" << endl;
    s << "#include \"Jce_mcare.h\"" << endl;

	vector<string> include = pPtr->getIncludes();
	for (size_t i = 0; i < include.size(); i++)
	{
		s << "#include \"" << g_parse->getHeader() 
		<< taf::TC_File::extractFileName(include[i])<< "\"" << endl;
	}

	vector<NamespacePtr> namespaces = pPtr->getNamespaces();

	s << endl;

	for (size_t i = 0; i < namespaces.size(); i++)
	{
		s << generateCoder(namespaces[i]) << endl;
	}

	s << endl;
	s << "#endif" << endl;

	taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
	taf::TC_File::save2file(fileH, s.str());

	s.str("");

	s << "#include \""+fileH+"\"" << endl;

	s << endl;

	for (size_t i = 0; i < namespaces.size(); i++)
	{
		s << generateCoderCpp(namespaces[i]) << endl;
	}

	s << endl;

	taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
	taf::TC_File::save2file(fileCpp, s.str());

	return;
}
