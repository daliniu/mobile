#include "jce2js.h"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

std::string Jce2Js::toFunctionName(const TypeIdPtr & pPtr, const std::string &sAction)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	:	return sAction + "Boolean";
			case Builtin::KindString:	return sAction + "String";
			case Builtin::KindByte: 	return sAction + "Int8";
			case Builtin::KindShort:	return sAction + (bPtr->isUnsigned()?"UInt8":"Int16");
			case Builtin::KindInt:		return sAction + (bPtr->isUnsigned()?"UInt16":"Int32");
			case Builtin::KindLong: 	return sAction + (bPtr->isUnsigned()?"UInt32":"Int64");
			case Builtin::KindFloat:	return sAction + "Float";
			case Builtin::KindDouble:	return sAction + "Double";
			default:					assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return sAction + "Bytes";
		}

		return sAction + "Vector";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return sAction + "Struct";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		return sAction + "Int32";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return sAction + "Map";
	}

	return "";
}


string Jce2Js::generateJS(const EnumPtr &pPtr, const std::string &sNamespace)
{
	ostringstream s;
    s << TAB << sNamespace << "." << pPtr->getId() << " = " << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //成员变量
    int nenum = -1;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t i = 0; i < member.size(); i++)
    {
		if (member[i]->hasDefault())
		{
			nenum = taf::TC_Common::strto<int>(member[i]->def());
		}
		else
		{
			nenum++;
		}

        s << TAB << member[i]->getId()<< " : " << taf::TC_Common::tostr(nenum) << (i == member.size() - 1?"":",") << endl;
    }

	DEL_TAB;
	s << TAB << "};" << endl;

    return s.str();
}

string Jce2Js::generateJS(const ConstPtr &pPtr, const std::string &sNamespace)
{
    ostringstream s;

    if (pPtr->getConstTokPtr()->t == ConstTok::STRING)
    {
        string tmp = taf::TC_Common::replace(pPtr->getConstTokPtr()->v, "\"", "\\\"");
        s << TAB << sNamespace << "." << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";";
    }
    else
    {
        s << TAB << sNamespace << "." << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstTokPtr()->v << ";";
    }

    return s.str();
}

string Jce2Js::getDataType(const TypePtr & pPtr)
{
	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool	:	return "new Taf.BOOLEAN()";
			case Builtin::KindString:	return "new Taf.STRING()";
			case Builtin::KindShort:	return "new Taf.INT16()";
			case Builtin::KindInt:		return "new Taf.INT32()";
			case Builtin::KindLong: 	return "new Taf.INT64()";
			case Builtin::KindFloat:	return "new Taf.FLOAT()";
			case Builtin::KindDouble:	return "new Taf.DOUBLE()";
			default:					assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "new Taf.BinBuffer()";
		}

		return "new Taf.Vector(" + getDataType(vPtr) + ")";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr);
	if (sPtr)
	{
		return "new " + taf::TC_Common::replace(sPtr->getSid(), "::", ".") + "()";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr);
	if (mPtr)
	{
		return "new Taf.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ")";
	}

	assert(false);
	return "";
}


string Jce2Js::getDefault(const TypeIdPtr & pPtr, const std::string &sDefault)
{
	std::cout << "GETDEFTA:" << sDefault << endl;


	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
	if (bPtr)
	{
		switch (bPtr->kind())
		{
			case Builtin::KindBool		: return sDefault.empty()?"false":sDefault;
			case Builtin::KindString	: return "\"" + taf::TC_Common::replace(sDefault, "\"", "\\\"") +  "\"";
			case Builtin::KindByte		:
			case Builtin::KindShort		:
			case Builtin::KindInt		:
			case Builtin::KindLong		: return sDefault.empty()?"0":sDefault;
			case Builtin::KindFloat		:
			case Builtin::KindDouble	: return sDefault.empty()?"0.0":sDefault;
			default						: assert(false);
		}
	}

	VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
	if (vPtr)
	{
		BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		if (vbPtr && vbPtr->kind() == Builtin::KindByte)
		{
			return "new Taf.BinBuffer()";
		}

		return "new Taf.Vector(" + getDataType(vPtr->getTypePtr()) + ")";
	}

	StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
	if (sPtr)
	{
		return "new " + taf::TC_Common::replace(sPtr->getSid(), "::", ".") + "()";
	}

	MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
	if (mPtr)
	{
		return "new Taf.Map(" + getDataType(mPtr->getLeftTypePtr()) + ", " + getDataType(mPtr->getRightTypePtr()) + ")";
	}

	EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
	if (ePtr)
	{
		return sDefault.empty()?"0":(taf::TC_Common::replace(ePtr->getSid(), "::", ".") + "." + sDefault);
	}

	return sDefault;
}

string Jce2Js::generateJS(const StructPtr & pPtr, const NamespacePtr & nPtr)
{
    ostringstream s;
    vector<string> key = pPtr->getKey();
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    s << TAB << nPtr->getId() << "." << pPtr->getId() << " = function()"<< endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "this." << member[i]->getId() << " = " << getDefault(member[i], member[i]->def()) << ";" << endl;
    }

	DEL_TAB;
	s << TAB << "};" << endl;

	std::string sProto = taf::TC_Common::replace(pPtr->getSid(), "::", ".") + ".prototype.";
	s << TAB << sProto << "_clone = function () { return new " + taf::TC_Common::replace(pPtr->getSid(), "::", ".") + "(); }" << endl;
	s << TAB << sProto << "_write = function (os, tag, value) { os.writeStruct(tag, value); }" << endl;
	s << TAB << sProto << "_read  = function (is, tag, def) { return is.readStruct(tag, true, def); }" << endl << endl;

    //writeTo()
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype.writeTo = function (os)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::string sFuncName = toFunctionName(member[i], "write");

       	s << TAB << "os." << sFuncName << "(" << member[i]->getTag() << ", this." << member[i]->getId() << ");" << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;
    s << endl;

    //readFrom()
    s << TAB << nPtr->getId() << "." << pPtr->getId() << ".prototype.readFrom = function (is)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::string sFuncName = toFunctionName(member[i], "read");
		s << TAB << "this." << member[i]->getId() << "= is." << sFuncName << "(" << member[i]->getTag()
            << ", " << (member[i]->isRequire()?"true":"false") << ", this." << member[i]->getId() << ");" << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    return s.str();
}

string Jce2Js::generateJS(const NamespacePtr &pPtr)
{
	std::ostringstream str;
	str << "var " << pPtr->getId() << " = " << pPtr->getId() << " || {};" << endl << endl;

	vector<ConstPtr> &cs = pPtr->getAllConstPtr();
	for (size_t i = 0; i < cs.size(); i++)
	{
		str << generateJS(cs[i], pPtr->getId()) << endl;
	}
	str << endl;

	vector<EnumPtr> & es = pPtr->getAllEnumPtr();
    for (size_t i = 0; i < es.size(); i++)
    {
        str << generateJS(es[i], pPtr->getId()) << endl;
    }

	vector<StructPtr> & ss = pPtr->getAllStructPtr();
	for (size_t i = 0; i < ss.size(); i++)
	{
		str << generateJS(ss[i], pPtr) << endl;
	}

	return str.str();
}

void Jce2Js::generateJS(const ContextPtr &pPtr)
{
	std::string str = g_parse->printHeaderRemark();

	vector<NamespacePtr> namespaces = pPtr->getNamespaces();
	for(size_t i = 0; i < namespaces.size(); i++)
	{
		str += generateJS(namespaces[i]);
	}

	std::string sFileName = _baseDir + "/" + taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName())) + ".js";
	taf::TC_File::makeDirRecursive(_baseDir, 0755);
	taf::TC_File::save2file(sFileName, str.c_str());
}
void Jce2Js::createFile(const string &file)
{
	std::vector<ContextPtr> contexts = g_parse->getContexts();
	for(size_t i = 0; i < contexts.size(); i++)
	{
		if(file == contexts[i]->getFileName())
		{
			generateJS(contexts[i]);
		}
	}
}

