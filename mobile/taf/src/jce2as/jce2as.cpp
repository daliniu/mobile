#include "jce2as.h"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
string Jce2as::toTypeInit(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     return "false;";
            case Builtin::KindByte:     return "0;";
            case Builtin::KindShort:    return "0;";
            case Builtin::KindInt:      return "0;";
            case Builtin::KindLong:     return "0;";
            case Builtin::KindFloat:    return "0.0f;";
            case Builtin::KindDouble:   return "0.0;";
            case Builtin::KindString:   return "\"\";";
            default:                    return "";
        }
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (bPtr && bPtr->kind() == Builtin::KindByte)
        {
            return "(" + tostr(vPtr->getTypePtr()) + "[]) new " + tostr(vPtr->getTypePtr()) + "[1];";
        }
        return "new " + tostrVector(vPtr)+"();";
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return "new " + tostrStruct(sPtr) + "();";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return "0;";

    return "";
}

void Jce2as::toFunctionName(const TypeIdPtr & pPtr, const std::string & sAction, std::pair<string, string> & pairFunc) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool  :   pairFunc.first = "DataHelp." + sAction + "Boolean"; break;
            case Builtin::KindString:   pairFunc.first = "DataHelp." + sAction + "String";  break;
            case Builtin::KindByte:     pairFunc.first = "DataHelp." + sAction + "Int8";    break;
            case Builtin::KindShort:    pairFunc.first = "DataHelp." + sAction + "Int16";   break;
            case Builtin::KindInt:      pairFunc.first = "DataHelp." + sAction + "Int32";   break;
            case Builtin::KindLong:     pairFunc.first = "DataHelp." + sAction + "Int64";   break;
            case Builtin::KindFloat:    pairFunc.first = "DataHelp." + sAction + "Float";   break;
            case Builtin::KindDouble:   pairFunc.first = "DataHelp." + sAction + "Double";  break;
            default:                    assert(false);
        }
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if (sPtr)
    {
        std::string sid = sPtr->getSid();
        sid = sid.substr(sid.find("::") + 2);
        pairFunc.first = sid + "." + sAction + "Struct";
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if (ePtr)
    {
        pairFunc.first = "DataHelp." + sAction + "Int32";
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if (vPtr)
    {
        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if (vbPtr)
        {
            switch (vbPtr->kind())
            {
                case Builtin::KindString:   pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "String";          break;
                case Builtin::KindByte:     pairFunc.first = "DataHelp." + sAction + "VectorByte";      break;
                case Builtin::KindShort:    pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "Int16";           break;
                case Builtin::KindInt:      pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "Int32";           break;
                case Builtin::KindLong:     pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "Int64";           break;
                case Builtin::KindFloat:    pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "Float";           break;
                case Builtin::KindDouble:   pairFunc.first = "DataHelp." + sAction + "VectorObject";
                                            pairFunc.second= "DataHelp." + sAction + "Double";          break;
                default:                    assert(false);
            }
        }

        StructPtr vsPtr = StructPtr::dynamicCast(vPtr->getTypePtr());
        if (vsPtr)
        {
            std::string sid = vsPtr->getSid();
            sid = sid.substr(sid.find("::") + 2);

            pairFunc.first = "DataHelp." + sAction + "VectorObject";
            pairFunc.second= sid + "." + sAction + "Struct";
        }
    }
}

string Jce2as::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return tostrVector(vPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return "int";

    if (!pPtr) return "void";

    assert(false);
    return "";
}

/*******************************BuiltinPtr********************************/
string Jce2as::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    switch (pPtr->kind())
    {
        case Builtin::KindBool:     return "Boolean";
        case Builtin::KindByte:     return "int";
        case Builtin::KindShort:    return "int";
        case Builtin::KindInt:      return "int";
        case Builtin::KindLong:     return "Number";
        case Builtin::KindFloat:    return "Number";
        case Builtin::KindDouble:   return "Number";
        case Builtin::KindString:   return "String";
        case Builtin::KindVector:   return "Array";
        default:                    assert(false);
    }

    return "";
}

string Jce2as::tostrVector(const VectorPtr &pPtr) const
{
    BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if (vbPtr && vbPtr->kind() == Builtin::KindByte)
    {
        return "ByteArray";
    }

    return "Array";
}

void Jce2as::setBasePackage(const string &prefix)
{
	_packagePrefix = prefix;
	if (_packagePrefix.length() != 0 && _packagePrefix.substr(_packagePrefix.length()-1, 1) != ".")
	{
		_packagePrefix += ".";
	}
}

string Jce2as::tostrStruct(const StructPtr &pPtr) const
{
    return _packagePrefix + taf::TC_Common::replace(pPtr->getSid(), "::", ".");
}

string Jce2as::getFilePath(const string &ns) const
{
	return _baseDir + "/" + taf::TC_Common::replace(_packagePrefix, ".", "/") + "/" + ns + "/";
}


/******************************ConstPtr***************************************/
void Jce2as::generateAS(const ConstPtr &pPtr,const NamespacePtr &nPtr) const
{
	ostringstream s;
	s << g_parse->printHeaderRemark();

	s << TAB << "package " << _packagePrefix << nPtr->getId() << endl;
	s << TAB << "{" << endl;

	INC_TAB;
	s << TAB << "public final class " << taf::TC_Common::lower(pPtr->getTypeIdPtr()->getId()) << endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "public static const " << pPtr->getTypeIdPtr()->getId() << ":" << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " = ";

	BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypeIdPtr()->getTypePtr());
	if(bPtr->kind() == Builtin::KindString)
	{
		s << "\"" << pPtr->getConstTokPtr()->v << "\";" << endl;
	}
	else
	{
		s << pPtr->getConstTokPtr()->v << ";" << endl;
	}

	DEL_TAB;
	s << TAB << "}" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

	string fileAS  = getFilePath(nPtr->getId()) +  taf::TC_Common::lower(pPtr->getTypeIdPtr()->getId()) + ".as";
	taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
	taf::TC_File::save2file(fileAS, s.str());

	return;
}



string Jce2as::generateAS(const EnumPtr &pPtr, const NamespacePtr &nPtr) const
{
    ostringstream s;
    s << g_parse->printHeaderRemark();

	s << TAB << "package " << _packagePrefix << nPtr->getId() << endl;
	s << TAB << "{" << endl;

	INC_TAB;
    s << TAB << "public final class " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    string sValue = "0";
	bool bHasV = false;
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "public static const " << member[i]->getId()<< ":int";
        if (member[i]->hasDefault())
        {
            sValue = member[i]->def();
            s << " = " << sValue;
			bHasV = true;
        }
		else
		{
			if(bHasV)
			{
				int v = taf::TC_Common::strto<int>(sValue);

				s << " = " << taf::TC_Common::tostr<int>(v+1);
			}
			else
			{
				s << " = " << taf::TC_Common::tostr<int>(i);
			}
		}
        s <<";"<<endl;
    }

    DEL_TAB;
    s << TAB << "}" << endl;

	DEL_TAB;
	s << TAB << "}" << endl;

    string file  = getFilePath(nPtr->getId()) + pPtr->getId() + ".as";
    taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
    taf::TC_File::save2file(file, s.str());

    return s.str();
}


string Jce2as::generateAS(const StructPtr & pPtr, const NamespacePtr & nPtr) const
{
    ostringstream s;
    s << g_parse->printHeaderRemark();

    vector<string> key = pPtr->getKey();
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    s << TAB << "package " << _packagePrefix << nPtr->getId() << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "import flash.utils.ByteArray;" << endl;
    s << TAB << "import com.qq.taf.jce.DataHelp;" << endl;
    s << TAB << "import com.qq.taf.jce.DataHead;" << endl;
    s << endl;

    s << TAB << "public class " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //定义成员变量
    for (size_t i = 0; i < member.size(); i++)
    {
        std::string sDefault;

        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(member[i]->getTypePtr());
        if (bPtr)
        {
            if(member[i]->hasDefault())
            {
                BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[i]->getTypePtr());
                if (bPtr && bPtr->kind() == Builtin::KindString)
                {
                    sDefault = "\"" + taf::TC_Common::replace(member[i]->def(), "\"", "\\\"") +  "\";";
                }
                else
                {
                    sDefault = member[i]->def() + ";";
                }
            }
        }

        VectorPtr vPtr = VectorPtr::dynamicCast(member[i]->getTypePtr());
        if (vPtr)
        {
            sDefault = "new " + tostr(member[i]->getTypePtr()) + "();";
        }

        StructPtr sPtr = StructPtr::dynamicCast(member[i]->getTypePtr());
        if (sPtr)
        {
            sDefault = "new " + tostr(member[i]->getTypePtr()) + "();";
        }

		EnumPtr ePtr = EnumPtr::dynamicCast(member[i]->getTypePtr());
		if (ePtr)
		{

			if(member[i]->hasDefault())
			{
				std::string sdef = member[i]->def();
				std::string::size_type pos = sdef.find_last_of("::");
				if (pos != std::string::npos && pos + 2 < sdef.size())
				{
					sdef = sdef.substr(pos + 2);
				}

				sDefault = _packagePrefix + taf::TC_Common::replace(ePtr->getSid(), "::", ".") + "." + sdef + ";";
			}
			else
			{
				sDefault = toTypeInit(ePtr);
			}
		}

		if(!sDefault.empty())
		{
			s << TAB << "public var " << member[i]->getId() << ":" << tostr(member[i]->getTypePtr()) << " = " << sDefault << endl;
		}
		else
		{
			s << TAB << "public var " << member[i]->getId() << ":" << tostr(member[i]->getTypePtr()) << ";" << endl;
		}

    }

    //writeTo()
    s << TAB << "public static function writeTo(ost:ByteArray, value:" << pPtr->getId() << "):void" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::pair<string, string> pairFunc;
        toFunctionName(member[i], "write", pairFunc);

        if (pairFunc.second.empty())
        {
            s << TAB << pairFunc.first << "(ost, value." << member[i]->getId() << ", " << member[i]->getTag() << ");" << endl;
        }
        else
        {
            s << TAB << pairFunc.first << "(ost, value." << member[i]->getId() << ", " << member[i]->getTag() << ", " << pairFunc.second << ");" << endl;
        }
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

	//writeStruct
	s << TAB << "public static function writeStruct(ost:ByteArray, value:Object, tag:int):void"<<endl;
	s << TAB << "{" << endl;
	INC_TAB;
	s << TAB << "DataHead.writeTo(ost, tag, DataHead.EM_STRUCTBEGIN);"<<endl;
	s << TAB << pPtr->getId() << ".writeTo(ost,value as " << pPtr->getId() << ");"<<endl;
	s << TAB << "DataHead.writeTo(ost,   0, DataHead.EM_STRUCTEND);"<<endl;
	DEL_TAB;
	s << TAB << "}" << endl;
	s << endl;


    //readFrom()
    s << TAB << "public static function readFrom(ist:ByteArray):" << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "var value:" << pPtr->getId() << " = new " << pPtr->getId() << "();" << endl;
    for (size_t i = 0; i < member.size(); i++)
    {
        std::pair<string, string> pairFunc;
        toFunctionName(member[i], "read", pairFunc);

        if (pairFunc.second.empty())
        {
            s << TAB << "value." << member[i]->getId() << "=" << pairFunc.first << "(ist, " << member[i]->getTag()
            << ", " << (member[i]->isRequire()?"true":"false") << ");" << endl;
        }
        else
        {
            s << TAB << "value." << member[i]->getId() << "=" << pairFunc.first << "(ist, " << member[i]->getTag()
            << ", " << (member[i]->isRequire()?"true":"false") << ", " << pairFunc.second << ");" << endl;
        }
    }

    s << TAB << "return value;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    //readStruct()
    s << TAB << "public static function readStruct(ist:ByteArray, tag:int, require:Boolean):" << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return DataHelp.readStruct(ist, tag, require, readFrom) as " << pPtr->getId() << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s<<endl;

    string file  = getFilePath(nPtr->getId()) + pPtr->getId() + ".as";
    taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
    taf::TC_File::save2file(file, s.str());

    return s.str();
}

void Jce2as::createFile(const string &file)
{
	std::vector<ContextPtr> contexts = g_parse->getContexts();
	for(size_t i = 0; i < contexts.size(); i++)
	{
		if(file == contexts[i]->getFileName())
		{
			generateAS(contexts[i]);
		}
	}
}
void Jce2as::generateAS(const ContextPtr &pPtr) const
{
	ostringstream s;
	vector<NamespacePtr> namespaces = pPtr->getNamespaces();

	for(size_t i = 0; i < namespaces.size(); i++)
	{
		generateAS(namespaces[i]);
	}
}

void Jce2as::generateAS(const NamespacePtr &pPtr) const
{
	vector<StructPtr> & ss = pPtr->getAllStructPtr();

	vector<ConstPtr>  &cs  = pPtr->getAllConstPtr();

	vector<EnumPtr>  &es  = pPtr->getAllEnumPtr();

	for(size_t i = 0; i < ss.size(); i++)
	{
		generateAS(ss[i], pPtr);
	}

	for (size_t i = 0; i < es.size(); i++)
	{
		generateAS(es[i], pPtr);
	}

	for (size_t i = 0; i < cs.size(); i++)
	{
		generateAS(cs[i], pPtr);
	}

}


