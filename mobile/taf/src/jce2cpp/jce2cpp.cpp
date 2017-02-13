#include "jce2cpp.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
//
Jce2Cpp::Jce2Cpp():
m_bPDU(false),
m_bCheckDefault(false),
m_bLuaSupport(false),
m_bJsonSupport(false),
m_bOnlyStruct(false),
m_bTafMSF(false),
m_sNamespace("taf"),
m_bUnknownField(false),
m_bEnableDataConstructor(true)
{

}

string Jce2Cpp::writeToJson(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "p->value[\"" << pPtr->getId() << "\"] = " + m_sNamespace+ "::JsonOutput::writeJson((" + m_sNamespace+ "::Int32)"
            << pPtr->getId() << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << "p->value[\"" << pPtr->getId() << "\"] = " + m_sNamespace+ "::JsonOutput::writeJson((const "
            << tostr(pPtr->getTypePtr()) << " *)" << pPtr->getId() << "Len"  << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isPointer())
    {
        s << TAB << "p->value[\"" << pPtr->getId() << "\"] = " + m_sNamespace+ "::JsonOutput::writeJson((const "
            << tostr(pPtr->getTypePtr()) << " )" << pPtr->getId() << "Len"  << ");" << endl;
    }
    else
    {
        MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
        VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());

        if (!m_bCheckDefault || pPtr->isRequire() || (!pPtr->hasDefault() && !mPtr && !vPtr))
        {
            s << TAB << "p->value[\"" << pPtr->getId() << "\"] = " + m_sNamespace+ "::JsonOutput::writeJson("
                << pPtr->getId() << ");" << endl;
        }
        else
        {
            string sDefault = pPtr->def();

            BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
            if (bPtr && bPtr->kind() == Builtin::KindString)
            {
                sDefault = "\"" + taf::TC_Common::replace(pPtr->def(), "\"", "\\\"") + "\"";
            }

            if (mPtr || vPtr)
            {
                s << TAB << "if (" << pPtr->getId() << ".size() > 0)" << endl;
            }
            else
            {
                s << TAB << "if (" << pPtr->getId() << " != " << sDefault << ")" << endl;
            }

            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << "p->value[\"" << pPtr->getId() << "\"] = " + m_sNamespace+ "::JsonOutput::writeJson("
                << pPtr->getId() << ");" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
        }
    }

    return s.str();
}

string Jce2Cpp::writeTo(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "_os.write((" + m_sNamespace+ "::Int32)" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << "_os.write((const " << tostr(pPtr->getTypePtr()) << " *)"<< pPtr->getId() <<", "<<pPtr->getId() << "Len"<< ", " << pPtr->getTag() << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isPointer())
    {
        s << TAB << "_os.write((const " << tostr(pPtr->getTypePtr()) << ")"<< pPtr->getId() <<", "<<pPtr->getId() << "Len"<< ", " << pPtr->getTag() << ");" << endl;
    }
    else
    {
        MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());
        VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());

        if (!m_bCheckDefault || pPtr->isRequire() || (!pPtr->hasDefault() && !mPtr && !vPtr))
        {
            s << TAB << "_os.write("<< pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
        }
        else
        {
            string sDefault = pPtr->def();

            BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
            if (bPtr && bPtr->kind() == Builtin::KindString)
            {
                sDefault = "\"" + taf::TC_Common::replace(pPtr->def(), "\"", "\\\"") + "\"";
            }

            if (mPtr || vPtr)
            {
                s << TAB << "if (" << pPtr->getId() << ".size() > 0)" << endl;
            }
            else
            {
                s << TAB << "if (" << pPtr->getId() << " != " << sDefault << ")" << endl;
            }

            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << "_os.write("<< pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
        }
    }

    return s.str();
}

string Jce2Cpp::readFromJson(const TypeIdPtr &pPtr, bool bIsRequire) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << m_sNamespace+ "::JsonInput::readJson((" + m_sNamespace+ "::Int32&)" << pPtr->getId() <<",pObj->value[\"" << pPtr->getId() << "\"]";
    }
    else if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << m_sNamespace+ "::JsonInput::readJson(" << pPtr->getId() << "Len" <<",pObj->value[\"" << pPtr->getId() << "\"]" << getSuffix(pPtr);
    }
    else if(pPtr->getTypePtr()->isPointer())
    {
#if 0
        s << TAB << pPtr->getId() <<" = ("<<tostr(pPtr->getTypePtr())<<")_is.cur();"<<endl;
        s << TAB << "_is.read("<< pPtr->getId()<<", _is.left(), "<< pPtr->getId() << "Len";
#endif
        s << TAB << "not support";
    }
    else
    {
        s << TAB << m_sNamespace+ "::JsonInput::readJson(" << pPtr->getId() << ",pObj->value[\"" << pPtr->getId() << "\"]";
    }
    s << ", " << ((pPtr->isRequire() && bIsRequire)?"true":"false") << ");" << endl;

#if 0
    if(pPtr->getTypePtr()->isPointer())
        s << TAB <<"_is.mapBufferSkip("<<pPtr->getId() << "Len);"<<endl;
#endif

    return s.str();
}


string Jce2Cpp::readFrom(const TypeIdPtr &pPtr, bool bIsRequire) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "_is.read((" + m_sNamespace+ "::Int32&)" << pPtr->getId();
    }
    else if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << "_is.read("<< pPtr->getId()<<", "<<getSuffix(pPtr)<<", "<< pPtr->getId() << "Len";
    }
    else if(pPtr->getTypePtr()->isPointer())
    {
        s << TAB << pPtr->getId() <<" = ("<<tostr(pPtr->getTypePtr())<<")_is.cur();"<<endl;
        s << TAB << "_is.read("<< pPtr->getId()<<", _is.left(), "<< pPtr->getId() << "Len";
    }
    else
    {
        s << TAB << "_is.read("<< pPtr->getId();
    }

    s << ", " << pPtr->getTag() << ", " << ((pPtr->isRequire() && bIsRequire)?"true":"false") << ");" << endl;

    if(pPtr->getTypePtr()->isPointer())
    s << TAB <<"_is.mapBufferSkip("<<pPtr->getId() << "Len);"<<endl;

    return s.str();
}

string Jce2Cpp::readUnknown(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    s << TAB << "_is.readUnknown(sUnknownField, "<<  pPtr->getTag() << ");" <<endl;
    return s.str();
}
string Jce2Cpp::writeUnknown() const
{
    ostringstream s;
    s << TAB << "_os.writeUnknown(sUnknownField);" <<endl;
    return s.str();
}

string Jce2Cpp::display(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "_ds.display((" + m_sNamespace+ "::Int32)" << pPtr->getId() << ",\"" << pPtr->getId() << "\");" << endl;;
    }
    else if(pPtr->getTypePtr()->isArray()||pPtr->getTypePtr()->isPointer())
    {
        s << TAB << "_ds.display(" << pPtr->getId() <<", "<< pPtr->getId() << "Len"<< ",\"" << pPtr->getId() << "\");" << endl;
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
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "_ds.displaySimple((" + m_sNamespace+ "::Int32)" << pPtr->getId() << ", "
            <<(bSep ? "true" : "false") << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << "_ds.displaySimple("<<pPtr->getId() <<", "<< pPtr->getId() << "Len"<< ","
            <<(bSep ? "true" : "false") << ");" << endl;
    }
    else if(pPtr->getTypePtr()->isPointer())
    {
        s << TAB << "_ds.displaySimple(";
        s <<(bSep ?"":("(const "+tostr(pPtr->getTypePtr())+")"));
        s << pPtr->getId() <<", "<< pPtr->getId() << "Len"<< "," << (bSep ? "true" : "false") << ");" << endl;
    }
    else
    {
        s << TAB << "_ds.displaySimple(" << pPtr->getId() << ", "
            <<(bSep ? "true" : "false") << ");" << endl;
    }

    return s.str();
}


string Jce2Cpp::generateCollection(const TypeIdPtr &pPtr, bool bSep) const
{
    ostringstream s;
    if(pPtr->getTypePtr()->isArray())
    {
        s << TAB << "_jj.generateCollection(" << "\"" << pPtr->getId()  <<  "\""  << ", " <<pPtr->getId() <<", "<< pPtr->getId() << "Len"
            << ");" << "\n" << (bSep ? TAB + "_jj.append(\",\", false);":"") <<endl;
    }
    else
    {
        s << TAB << "_jj.generateCollection(" << "\"" << pPtr->getId()  <<  "\"" << ", " << pPtr->getId()
            << ");" << "\n" << (bSep ? TAB + "_jj.append(\",\", false);":"") <<endl;
    }

    return s.str();
}


/*******************************获取定长数组坐标********************************/
int Jce2Cpp::getSuffix(const TypeIdPtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if(bPtr && bPtr->kind() == Builtin::KindString && bPtr->isArray())
    {
        return bPtr->getSize();
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if(vPtr && vPtr->isArray())
    {
        return vPtr->getSize();
    }

    return -1;
}

/*******************************定长数组坐标********************************/

string Jce2Cpp::toStrSuffix(const TypeIdPtr &pPtr) const
{
    ostringstream s;

    int i = getSuffix(pPtr);

    if(i >= 0)
    {
         s << "[" << i << "]";
    }
    return s.str();
}
/*******************************BuiltinPtr********************************/

string Jce2Cpp::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if(bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if(vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if(mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if(sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if(ePtr) return tostrEnum(ePtr);

    if(!pPtr) return "void";

    assert(false);
    return "";
}

string Jce2Cpp::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch(pPtr->kind())
    {
    case Builtin::KindBool:
        s = m_sNamespace+ "::Bool";
        break;
    case Builtin::KindByte:
        s = m_sNamespace+ "::Char";
        break;
    case Builtin::KindShort:
    //为了兼容java无unsigned, 编结码时把jce问件中 unsigned char 对应到short
    //c++中需要还原回来
        s = (pPtr->isUnsigned()?m_sNamespace+ "::UInt8":m_sNamespace+ "::Short");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?m_sNamespace+ "::UInt16":m_sNamespace+ "::Int32");
        break;
    case Builtin::KindLong:
        s =  (pPtr->isUnsigned()?m_sNamespace+ "::" + (m_bTafMSF?"taf":"") + "UInt32":m_sNamespace+ "::Int64");
        break;
    case Builtin::KindFloat:
        s = m_sNamespace+ "::Float";
        break;
    case Builtin::KindDouble:
        s = m_sNamespace+ "::Double";
        break;
    case Builtin::KindString:
        if(pPtr->isArray())
            s = m_sNamespace+ "::Char"; //char a [8];
        else
            s = "std::string";//string a;
        break;
    case Builtin::KindVector:
        s = "std::vector";
        break;
    case Builtin::KindMap:
        s = "std::map";
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
    //数组类型
    if(pPtr->isArray())
    {
        return tostr(pPtr->getTypePtr());
    }

    //指针类型
    if(pPtr->isPointer())
    {
        return tostr(pPtr->getTypePtr())+" *";
    }

    string s = Builtin::builtinTable[Builtin::KindVector] + string("<") + tostr(pPtr->getTypePtr());

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
/*******************************MapPtr********************************/
string Jce2Cpp::tostrMap(const MapPtr &pPtr) const
{
    string s = Builtin::builtinTable[Builtin::KindMap] + string("<") + tostr(pPtr->getLeftTypePtr()) + ", " + tostr(pPtr->getRightTypePtr());
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

/*******************************StructPtr********************************/
string Jce2Cpp::tostrStruct(const StructPtr &pPtr) const
{
    return pPtr->getSid();
}

string Jce2Cpp::MD5(const StructPtr &pPtr) const
{
    string s;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for(size_t j = 0; j < member.size(); j++)
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

    s << TAB << "struct " << pPtr->getId() << " : public " + m_sNamespace+ "::JceStructBase" << endl;
    s << TAB << "{" << endl;
    if(m_bLuaSupport)
    {
        s << TAB << "DECLARE_SCRIPT_STRUCT(" << pPtr->getId() << ")" << endl;
    }
    s << TAB << "public:" << endl;

    INC_TAB;

    s << TAB << "static string className()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return " << "\"" << namespaceId << "." << pPtr->getId() << "\"" << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "static string MD5()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return " << MD5(pPtr) << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    ////////////////////////////////////////////////////////////
    //定义构造函数
    if(m_bEnableDataConstructor)
    {
        s << TAB << pPtr->getId()<<"(";
        bool b = false;
        for(size_t j = 0; j < member.size(); j++)
        {
            if(member[j]->getTypePtr()->isArray() || member[j]->getTypePtr()->isPointer())
            {
                continue;
            }
            if(b) s<<",";
            if(member[j]->getTypePtr()->isSimple())
            {
                s << tostr(member[j]->getTypePtr())<<" "<<member[j]->getId();
            }
            else
            {
                s << "const "<< tostr(member[j]->getTypePtr())<<" &"<<member[j]->getId();
            }
            b=true;
        }
        s << ")" <<endl;
        INC_TAB;
        b =false;
        for(size_t j = 0; j < member.size(); j++)
        {
            if(!b) s<<TAB<<":"; else s<<",";
            if(member[j]->getTypePtr()->isArray())
            {
                s << member[j]->getId() << "Len(0)";
                b = true;
                continue;
            }
            if( member[j]->getTypePtr()->isPointer())
            {
                s << member[j]->getId() << "Len(0),"<<member[j]->getId()<<"(NULL)";
                b = true;
                continue;
            }
            s << member[j]->getId()<<"("<<member[j]->getId()<<")";
            b=true;
        }
        if(b)
        {
            if (m_bUnknownField)
            {
                s << ",sUnknownField(\"\")";
            }
            s << endl;
        }
        
        DEL_TAB;
        s << TAB << "{" << endl;
        INC_TAB;
        for(size_t j = 0; j < member.size(); j++)
        {
            VectorPtr vPtr   = VectorPtr::dynamicCast(member[j]->getTypePtr());
            if(vPtr)
            {
                BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
                if(!bPtr ||  (bPtr && bPtr->kind() == Builtin::KindString) ) //非内建类型或者string 类型不能memset
                {
                    continue;
                }
            }
            if( !member[j]->getTypePtr()->isArray() )
            {
                continue;
            }
            s << TAB << "memset(" << member[j]->getId() <<", 0, " << "sizeof(" << member[j]->getId() << "));" << endl;
        }
        DEL_TAB;
        s << TAB << "}" << endl;
    }

    ////////////////////////////////////////////////////////////
    //定义缺省构造函数
    s << TAB << pPtr->getId() << "()" << endl;

    bool b = false;
    for(size_t j = 0; j < member.size(); j++)
    {

        if(member[j]->getTypePtr()->isArray())
        {
            if(!b) s << TAB << ":"; else s << ",";
            s << member[j]->getId() << "Len(0)";
            b = true;
            continue;
        }

        if( member[j]->getTypePtr()->isPointer())
        {
            if(!b) s << TAB << ":"; else s << ",";
            s << member[j]->getId() << "Len(0),"<<member[j]->getId()<<"(NULL)";
            b = true;
            continue;
        }

        if(member[j]->hasDefault())
        {
            if(!b) s << TAB << ":"; else s << ",";

            BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
            //string值要转义
            if(bPtr && bPtr->kind() == Builtin::KindString)
            {
                string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
                s << member[j]->getId() << "(\"" << tmp << "\")";
            }
            else
            {
                s << member[j]->getId() << "(" << member[j]->def() << ")";
            }
            b = true;
        }
    }
    if(b)
    {
        if (m_bUnknownField)
        {
            s << ",sUnknownField(\"\")";
        }
        s << endl;
    }

    s << TAB << "{" << endl;
    INC_TAB;
    for(size_t j = 0; j < member.size(); j++)
    {
        VectorPtr vPtr   = VectorPtr::dynamicCast(member[j]->getTypePtr());
        if(vPtr)
        {
            BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
            if(!bPtr ||  (bPtr && bPtr->kind() == Builtin::KindString) ) //非内建类型或者string 类型不能memset
            {
                continue;
            }
        }
        if( !member[j]->getTypePtr()->isArray() )
        {
            continue;
        }
        s << TAB << "memset(" << member[j]->getId() <<", 0, " << "sizeof(" << member[j]->getId() << "));" << endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;

    //resetDefault()函数
    s << TAB << "void resetDefautlt()" <<  endl;
    s << TAB << "{" << endl;
    INC_TAB;

    member = pPtr->getAllMemberPtr();
    for(size_t j = 0; j < member.size(); j++)
    {
        if(member[j]->getTypePtr()->isArray())
        {
            s << TAB << member[j]->getId() << "Len = 0;" << endl;
            VectorPtr vPtr   = VectorPtr::dynamicCast(member[j]->getTypePtr());
            if(vPtr)
            {
                BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
                if(bPtr &&  bPtr->kind() != Builtin::KindString) //非内建类型或者string 类型不能memset
                {
                    s << TAB << "memset(" << member[j]->getId() <<", 0, " << "sizeof(" << member[j]->getId() << "));" << endl;
                }
            }
            continue;
        }

        if( member[j]->getTypePtr()->isPointer())
        {
            s << TAB << member[j]->getId() << "Len = 0;" << endl;
            s << TAB << member[j]->getId() <<" = NULL;" << endl;
            continue;
        }

        if(member[j]->hasDefault())
        {
            BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
            //string值要转义
            if(bPtr && bPtr->kind() == Builtin::KindString)
            {
                string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
                s << TAB << member[j]->getId() << " = \"" << tmp << "\";" << endl;
            }
            else
            {
                s << TAB << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
            }
        }
    }

    DEL_TAB;
    s << TAB << "}" << endl;




    //added by forrestliu@20111127 定义lua导出要求构造方法
    if(m_bLuaSupport)
    {
        s << TAB << pPtr->getId() << "(const Luavatar::LuaValueList& params)" << endl;
        vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
        bool b = false;
        for(size_t j = 0; j < member.size(); j++)
        {

            if(member[j]->getTypePtr()->isArray())
            {
                if(!b) s << TAB << ":"; else s << ",";
                s << member[j]->getId() << "Len(0)";
                b = true;
                continue;
            }

            if( member[j]->getTypePtr()->isPointer())
            {
                if(!b) s << TAB << ":"; else s << ",";
                s << member[j]->getId() << "Len(0),"<<member[j]->getId()<<"(NULL)";
                b = true;
                continue;
            }

            if(member[j]->hasDefault())
            {
                if(!b) s << TAB << ":"; else s << ",";

                BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
                //string值要转义
                if(bPtr && bPtr->kind() == Builtin::KindString)
                {
                    string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
                    s << member[j]->getId() << "(\"" << tmp << "\")";
                }
                else
                {
                    s << member[j]->getId() << "(" << member[j]->def() << ")";
                }
                b = true;
            }
        }
        if(b)
        {
            s << endl;
        }

        s << TAB << "{" << endl;
        INC_TAB;
        for(size_t j = 0; j < member.size(); j++)
        {
            VectorPtr vPtr   = VectorPtr::dynamicCast(member[j]->getTypePtr());
            if(vPtr)
            {
                BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
                if(!bPtr ||  (bPtr && bPtr->kind() == Builtin::KindString) ) //非内建类型或者string 类型不能memset
                {
                    continue;
                }
            }
            if( !member[j]->getTypePtr()->isArray() )
            {
                continue;
            }
            s << TAB << "memset(" << member[j]->getId() <<", 0, " << "sizeof(" << member[j]->getId() << "));" << endl;
        }
        DEL_TAB;
        s << TAB << "}" << endl;
    }

    ////////////////////////////////////////////////////////////
    s << TAB << "template<typename WriterT>" << endl;
    s << TAB << "void writeTo(" + m_sNamespace+ "::JceOutputStream<WriterT>& _os) const" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for(size_t j = 0; j < member.size(); j++)
    {
        s << writeTo(member[j]);
    }
    if (m_bUnknownField)
    {
        s << writeUnknown();
    }
    DEL_TAB;
    s << TAB << "}" << endl;

    ///////////////////////////////////////////////////////////
    s << TAB << "template<typename ReaderT>" << endl;
    s << TAB << "void readFrom(" + m_sNamespace+ "::JceInputStream<ReaderT>& _is)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "resetDefautlt();" << endl;
    for(size_t j = 0; j < member.size(); j++)
    {
        s << readFrom(member[j]);
    }
    if (m_bUnknownField)
    {
        s << readUnknown(member[member.size() - 1]);
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    if(m_bJsonSupport)
    {
        s << TAB << "taf::JsonValueObjPtr writeToJson() const" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "taf::JsonValueObjPtr p = new taf::JsonValueObj();" << endl;
        for(size_t j = 0; j < member.size(); j++)
        {
            s << writeToJson(member[j]);
        }
        s << TAB << "return p;" <<endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        s << TAB << "string writeToJsonString()" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "return taf::TC_Json::writeValue(writeToJson());" <<endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        s << TAB << "void readFromJson(const taf::JsonValuePtr & p, bool isRequire = true)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "resetDefautlt();" << endl;
        s << TAB << "if(NULL == p || p->getType() != taf::eJsonTypeObj)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "char s[128];" << endl;
        s << TAB << "snprintf(s, sizeof(s), \"read 'struct' type mismatch, get type: %d.\", p->getType());" << endl;
        s << TAB << "throw taf::TC_Json_Exception(s);" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
        s << TAB << "taf::JsonValueObjPtr pObj=taf::JsonValueObjPtr::dynamicCast(p);" << endl;
        for(size_t j = 0; j < member.size(); j++)
        {
            s << readFromJson(member[j]);
        }
        DEL_TAB;
        s << TAB << "}" << endl;

        s << TAB << "void readFromJsonString(const string & str)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "readFromJson(taf::TC_Json::getValue(str));" <<endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }

    if (!m_bTafMSF) // msf不需要display接口
    {
        s << TAB << "ostream& display(ostream& _os, int _level=0) const" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << m_sNamespace+ "::JceDisplayer _ds(_os, _level);" << endl;

        for(size_t j = 0; j < member.size(); j++)
        {
            s << display(member[j]);
        }
        s << TAB << "return _os;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;


        s << TAB << "ostream& displaySimple(ostream& _os, int _level=0) const" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << m_sNamespace+ "::JceDisplayer _ds(_os, _level);" << endl;

        for(size_t j = 0; j < member.size(); j++)
        {
            s << displaySimple(member[j], (j!=member.size()-1 ? true : false));
        }
        s << TAB << "return _os;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
        if(m_bPDU)
        {
            //jonneywang想提的需求
            s << TAB << "int Encode(uint8_t * pui_buff, int32_t * pi_buff_len, pdu_protocol_header * pdu_header)" << endl;
            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << "try" << endl;
            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << "int i_len = pdu_header?*pi_buff_len:0;" << endl;
            s << TAB << "int i_ret = 0;" << endl;
            s << TAB << "if (pdu_header && (i_ret = ::pdu_header_pack1(pdu_header, pui_buff, (uint32_t *)&i_len)) != 0) { return i_ret; }" << endl << endl;

            s << TAB << m_sNamespace+ "::JceOutputStream<taf::BufferWriterBuff> os;" << endl;
            s << TAB << "os.setBuffer(reinterpret_cast<char *>(pui_buff + i_len), *pi_buff_len - i_len);" << endl;
            s << TAB << "this->writeTo(os);" << endl;
            s << TAB << "i_len = i_len + static_cast<int>(os.getLength());" << endl;

            s << TAB << "if (pdu_header && (i_ret = pdu_header_pack2(pui_buff, (uint32_t *)pi_buff_len, (uint32_t *)&i_len)) != 0) { return i_ret; }" << endl;
            s << TAB << "*pi_buff_len = i_len;" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
            s << TAB << "catch (" + m_sNamespace+ "::JceNotEnoughBuff & e)" << endl;
            s << TAB << "{" << endl;
            s << TAB << "    return -100;" << endl;
            s << TAB << "}" << endl;
            s << TAB << "catch (" + m_sNamespace+ "::JceDecodeInvalidValue & e)" << endl;
            s << TAB << "{" << endl;
            s << TAB << "    return -104;" << endl;
            s << TAB << "}" << endl;
            s << TAB << "catch (...)" << endl;
            s << TAB << "{" << endl;
            s << TAB << "    return -1;" << endl;
            s << TAB << "}" << endl;
            s << TAB << "return 0;" << endl;
            DEL_TAB;
            s << TAB << "}" << endl << endl;


            s << TAB << "int Decode(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header)" << endl;
            s << TAB << "{" << endl;
           INC_TAB;
           s << TAB << "try" << endl;
           s << TAB << "{" << endl;
           INC_TAB;
           s << TAB << "int i_ret = 0;" << endl;
           s << TAB << "int i_wup_len = pdu_header?*pi_buff_len:0;" << endl;
           s << TAB << "int i_end_len = 0;" << endl;
           s << TAB << "if (pdu_header && (i_ret = pdu_header_unpack(pui_buff, (uint32_t *)&i_wup_len, pdu_header, (uint32_t *)&i_end_len)) != 0) return i_ret;" << endl;
           s << endl;

           s << TAB << m_sNamespace+ "::JceInputStream<taf::MapBufferReader> is;" << endl;
           s << TAB << "is.setBuffer(reinterpret_cast<const char*>(pui_buff + i_wup_len), static_cast<size_t>(*pi_buff_len - i_wup_len - i_end_len));" << endl;
           s << endl;
           s << TAB << "pst_struct->readFrom(is);" << endl;
           DEL_TAB;
           s << TAB << "}" << endl;
           s << TAB << "catch (" + m_sNamespace+ "::JceDecodeMismatch & e)" << endl;
           s << TAB << "{" << endl;
           s << TAB << "    return -101;" << endl;
           s << TAB << "}" << endl;
           s << TAB << "catch (" + m_sNamespace+ "::JceDecodeInvalidValue & e)" << endl;
           s << TAB << "{" << endl;
           s << TAB << "    return -104;" << endl;
           s << TAB << "}" << endl;
           s << TAB << "catch (" + m_sNamespace+ "::JceDecodeRequireNotExist & e)" << endl;
           s << TAB << "{" << endl;
           s << TAB << "    return -103;" << endl;
           s << TAB << "}" << endl;
           s << TAB << "catch (...)" << endl;
           s << TAB << "{" << endl;
           s << TAB << "    return -1;" << endl;
           s << TAB << "}" << endl;

           s << TAB << "return 0;" << endl;
           DEL_TAB;
           s << TAB << "}" << endl;
        }
    }
    if(m_bLuaSupport)
    {
        s << TAB << "void cs2table(Luavatar::LuaVariable lv) const" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "lv.emptyTable();" << endl;

        for(size_t j = 0; j < member.size(); j++)
        {
            EnumPtr ePtr = EnumPtr::dynamicCast(member[j]->getTypePtr());
            if(ePtr)
            {
                s << TAB << "LuaAttrHelper<int >::copy(" << member[j]->getId() << ",lv[\"" << member[j]->getId() << "\"]);" << endl;
            }
            else
            {
                s << TAB << "LuaAttrHelper<" << tostr(member[j]->getTypePtr()) << " >::copy(" << member[j]->getId() << ",lv[\"" << member[j]->getId() << "\"]);" << endl;
            }
            
        }
        s << TAB << "return;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }

    DEL_TAB;
    s << TAB << "public:" << endl;
    INC_TAB;

    //定义成员变量
    for(size_t j = 0; j < member.size(); j++)
    {
        if(member[j]->getTypePtr()->isArray() || member[j]->getTypePtr()->isPointer()) //数组类型、指针类型需要定义长度
        {
            s << TAB << m_sNamespace+ "::" << (m_bTafMSF?"taf":"") << "UInt32 " << member[j]->getId() << "Len" << ";" << endl;
        }
        s << TAB << tostr(member[j]->getTypePtr()) << " " << member[j]->getId() << toStrSuffix(member[j])<< ";" << endl;
        //added by forrestliu@20111127 for lua support
        if(m_bLuaSupport)
        {
            EnumPtr ePtr = EnumPtr::dynamicCast(member[j]->getTypePtr());
            if(ePtr)
            {
                s << TAB << "int get_" << member[j]->getId() << "(lua_State* L){return LuaAttrHelper<int >::get(L,(int)" <<member[j]->getId() << ");}" << endl;
                s << TAB << "int set_" << member[j]->getId() << "(lua_State* L){return LuaAttrHelper<int >::set(L,(int&)" <<member[j]->getId() << ");}" << endl;
            }
            else
            {
                s << TAB << "int get_" << member[j]->getId() << "(lua_State* L){return LuaAttrHelper<" << tostr(member[j]->getTypePtr()) << " >::get(L," <<member[j]->getId() << ");}" << endl;
                s << TAB << "int set_" << member[j]->getId() << "(lua_State* L){return LuaAttrHelper<" << tostr(member[j]->getTypePtr()) << " >::set(L," <<member[j]->getId() << ");}" << endl;
            }
        }
    }
    if  (m_bUnknownField)
    {
        s << TAB << "std::string sUnknownField; //工具带--unknown参数自动生成字段,存放未知tag数据." << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    //定义==操作
    s << TAB << "inline bool operator==(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return ";
    for(size_t j = 0; j < member.size(); j++)
    {
        if(member[j]->getTypePtr()->isArray() || member[j]->getTypePtr()->isPointer()) //数组类型、指针类型
        {
            s << "!memcmp(l." << member[j]->getId() << ",r." << member[j]->getId()<<",l."<<member[j]->getId() << "Len)";
        }
        else
        {
            s << "l." << member[j]->getId() << " == r." << member[j]->getId();
        }
        if(j != member.size() - 1)
        {
            s << " && ";
        }
    }
    s << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    //定义!=
    s << TAB << "inline bool operator!=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return !(l == r);" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    vector<string> key = pPtr->getKey();
    //定义<
    if(key.size() > 0)
    {
        s << TAB << "inline bool operator<(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        for(size_t i=0; i < key.size(); i++)
        {
            s << TAB << "if(l." << key[i] << " != r." << key[i] << ") ";
            for(size_t z =0; z < member.size(); z++)
            {
                if( key[i] == member[z]->getId() && (member[z]->getTypePtr()->isArray() || member[z]->getTypePtr()->isPointer())) //数组类型、指针类型
                {
                    s << "memcmp(l." << key[i] << ",r." <<key[i] <<",l."<<key[i] << "Len)< 0";
                }
            }
            s << " return (l." << key[i] << " < r." << key[i] << ");" << endl;

        }

        s << TAB << "return false;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        //定义<=
        s << TAB << "inline bool operator<=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "return !(r < l);" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        //定义>
        s << TAB << "inline bool operator>(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "return r < l;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        //定义>=
        s << TAB << "inline bool operator>=(const " << pPtr->getId() << "&l, const " << pPtr->getId() << "&r)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "return !(l < r);" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }

    if (m_bPDU)
    {
        s << TAB << "int struct_" << pPtr->getId() << "_pack(" << pPtr->getId() << " * pst_struct, uint8_t * pui_buff, int32_t * pi_buff_len, pdu_protocol_header * pdu_header);" << endl;
        s << TAB << "int struct_" << pPtr->getId() << "_unpack(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header);" << endl;
        s << TAB << "int struct_" << pPtr->getId() << "_unpack(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header, uint8_t * pui_mapbuff, int32_t * pi_mapbuff_len);" << endl;
    }

    return s.str();
}

string Jce2Cpp::generateCpp(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;

    //特殊用途：定义互联网的编解码函数
    //if (!m_bPDU) return s.str();

    //生成编码函数
    if(m_bPDU)
    {
        s << TAB << "int struct_" << pPtr->getId() << "_pack(" << pPtr->getId() << " * pst_struct, uint8_t * pui_buff, int32_t * pi_buff_len, pdu_protocol_header * pdu_header)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "try" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "int i_len = pdu_header?*pi_buff_len:0;" << endl;
        s << TAB << "int i_ret = 0;" << endl;
        s << TAB << "if (pdu_header && (i_ret = ::pdu_header_pack1(pdu_header, pui_buff, (uint32_t *)&i_len)) != 0) { return i_ret; }" << endl << endl;

        s << TAB << m_sNamespace+ "::JceOutputStream<taf::BufferWriterBuff> os;" << endl;
        s << TAB << "os.setBuffer(reinterpret_cast<char *>(pui_buff + i_len), *pi_buff_len - i_len);" << endl;
        s << TAB << "pst_struct->writeTo(os);" << endl;
        s << TAB << "i_len = i_len + static_cast<int>(os.getLength());" << endl;

        s << TAB << "if (pdu_header && (i_ret = pdu_header_pack2(pui_buff, (uint32_t *)pi_buff_len, (uint32_t *)&i_len)) != 0) { return i_ret; }" << endl;
        s << TAB << "*pi_buff_len = i_len;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
        s << TAB << "catch (" + m_sNamespace+ "::JceNotEnoughBuff & e)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -100;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "catch (" + m_sNamespace+ "::JceDecodeInvalidValue & e)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -104;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "catch (...)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -1;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "return 0;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl << endl;

        //生成解码函数
        s << TAB << "static int struct_" << pPtr->getId() << "_unpack__(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header, uint8_t * pui_mapbuff, int32_t * pi_mapbuff_len)" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "try" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "int i_ret = 0;" << endl;
        s << TAB << "int i_wup_len = pdu_header?*pi_buff_len:0;" << endl;
        s << TAB << "int i_end_len = 0;" << endl;
        s << TAB << "if (pdu_header && (i_ret = pdu_header_unpack(pui_buff, (uint32_t *)&i_wup_len, pdu_header, (uint32_t *)&i_end_len)) != 0) return i_ret;" << endl;
        s << endl;

        s << TAB << m_sNamespace+ "::JceInputStream<taf::MapBufferReader> is;" << endl;
        s << TAB << "is.setBuffer(reinterpret_cast<const char*>(pui_buff + i_wup_len), static_cast<size_t>(*pi_buff_len - i_wup_len - i_end_len));" << endl;
        s << TAB << "if (pui_mapbuff && pi_mapbuff_len) is.setMapBuffer((char *)pui_mapbuff, *pi_mapbuff_len);" << endl;
        s << endl;
        s << TAB << "pst_struct->readFrom(is);" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
        s << TAB << "catch (" + m_sNamespace+ "::JceDecodeMismatch & e)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -101;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "catch (" + m_sNamespace+ "::JceDecodeInvalidValue & e)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -104;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "catch (" + m_sNamespace+ "::JceDecodeRequireNotExist & e)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -103;" << endl;
        s << TAB << "}" << endl;
        s << TAB << "catch (...)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return -1;" << endl;
        s << TAB << "}" << endl;

        s << TAB << "return 0;" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;

        s << TAB << "int struct_" << pPtr->getId() << "_unpack(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return struct_" << pPtr->getId() << "_unpack__(pui_buff, pi_buff_len, pst_struct, pdu_header, NULL, NULL);" << endl;
        s << TAB << "}" << endl;
        s << endl;

        s << TAB << "int struct_" << pPtr->getId() << "_unpack(uint8_t * pui_buff, int32_t * pi_buff_len, " << pPtr->getId() << " * pst_struct, pdu_protocol_header * pdu_header, uint8_t * pui_mapbuff, int32_t * pi_mapbuff_len)" << endl;
        s << TAB << "{" << endl;
        s << TAB << "    return struct_" << pPtr->getId() << "_unpack__(pui_buff, pi_buff_len, pst_struct, pdu_header, pui_mapbuff, pi_mapbuff_len);" << endl;
        s << TAB << "}" << endl;
    }

    //added by forrestliu@20111127 for lua support
    if(m_bLuaSupport)
    {
        s << TAB << "IMPLEMENT_SCRIPT_STRUCT(" << pPtr->getId() << ")" << endl;
        s << TAB << "BEGIN_SCRIPT_ATTR(" << pPtr->getId() << ")" << endl;
        INC_TAB;
        vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
        for(size_t j = 0; j < member.size(); j++)
        {
            if(member[j]->getTypePtr()->isArray() || member[j]->getTypePtr()->isPointer()) //数组类型、指针类型
            {
                //do nothing for not supported attributes
            }
            else
            {
                s << TAB << "LUNAR_DECLARE_ATTR(" << pPtr->getId() << ", get_" << member[j]->getId() << ")," << endl;
                s << TAB << "LUNAR_DECLARE_ATTR(" << pPtr->getId() << ", set_" << member[j]->getId() << ")," << endl;
            }
        }
        DEL_TAB;
        s << TAB << "END_SCRIPT_ATTR" << endl;
    }

    return s.str();
}


/*******************************ContainerPtr********************************/
string Jce2Cpp::generateH(const ContainerPtr &pPtr) const
{
    ostringstream s;
    for(size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
    {
        s << generateH(pPtr->getAllNamespacePtr()[i]) << endl;
        s << endl;
    }
    return s.str();
}

string Jce2Cpp::generateCpp(const ContainerPtr &pPtr) const
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
string Jce2Cpp::generateH(const ParamDeclPtr &pPtr) const
{
    ostringstream s;

    //输出参数, 或简单类型
    if(pPtr->isOut() || pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
    {
        s << tostr(pPtr->getTypeIdPtr()->getTypePtr());
    }
    else
    {
        //结构, map, vector, string
        s << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " &";
    }

    if(pPtr->isOut())
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
    if(!pPtr->isOut()) return "";

    ostringstream s;

    //输出参数, 或简单类型
    if(pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
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

string Jce2Cpp::generateCpp(const ParamDeclPtr &pPtr) const
{
    ostringstream s;

    if(pPtr->isOut() || pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
    {
        s << tostr(pPtr->getTypeIdPtr()->getTypePtr());

        if(pPtr->isOut()) s << " &";
        else s << " ";
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
string Jce2Cpp::generateCpp(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;

    //生成函数声明
    s << TAB << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << cn << "Proxy::" << pPtr->getId() << "(" ;

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    string routekey = "";

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << generateCpp(vParamDecl[i]);
        if(i != vParamDecl.size() - 1)
        {
            s << ", ";
        }
        else
        {
            s << ", const map<string, string> &context)";
        }
        if (routekey.empty() && vParamDecl[i]->isRouteKey())
        {
            routekey = vParamDecl[i]->getTypeIdPtr()->getId();
        }
    }
    if(vParamDecl.size() == 0)
    {
        s << "const map<string, string> &context)";
    }

    s << endl;

    s << TAB << "{" << endl;

    INC_TAB;

    s << TAB << m_sNamespace+ "::JceOutputStream<" + m_sNamespace+ "::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        //if(vParamDecl[i]->isOut()) continue;
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "" + m_sNamespace + "::ResponsePacket rep;" << endl;

    s << TAB << "std::map<string, string> _mStatus;" << endl;

    if (!routekey.empty())
    {
        ostringstream os;

        os << routekey;

        s << TAB << "_mStatus.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;
    }

    s << TAB << "taf_invoke(taf::JCENORMAL,\"" << pPtr->getId() << "\", _os.getByteBuffer(), context, _mStatus, rep);" << endl;

    if(vParamDecl.size() > 0 || pPtr->getReturnPtr()->getTypePtr())
    {
        s << TAB <<  m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> _is;" << endl;
        s << TAB << "_is.setBuffer(rep.sBuffer);" << endl;
        if(pPtr->getReturnPtr()->getTypePtr())
        {
            s << TAB << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getReturnPtr()->getId() << ";"  << endl;
            s << readFrom(pPtr->getReturnPtr());
        }
        for(size_t i = 0; i < vParamDecl.size(); i++)
        {
            if(vParamDecl[i]->isOut())
            {
                s << readFrom(vParamDecl[i]->getTypeIdPtr());
            }
        }
        if(pPtr->getReturnPtr()->getTypePtr())
        {
            s << TAB << "return " << pPtr->getReturnPtr()->getId() << ";" << endl;
        }
    }
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}

string Jce2Cpp::generateCppAsync(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;
    //生成函数声明
    s << TAB << "void "<< cn << "Proxy::async_" << pPtr->getId() << "(" ;
    s << cn << "PrxCallbackPtr callback,";

    string routekey = "";

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut())
        {
            s << generateCpp(vParamDecl[i]) << ",";
        }
        if (routekey.empty() && vParamDecl[i]->isRouteKey())
        {
            routekey = vParamDecl[i]->getTypeIdPtr()->getId();
        }
    }
    s << "const map<string, string>& context)";
    s << endl;

    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut())
        {
            continue;
        }
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "std::map<string, string> _mStatus;" << endl;

    if (!routekey.empty())
    {
        ostringstream os;

        os << routekey;

        s << TAB << "_mStatus.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;
    }

    s << TAB << "taf_invoke_async(taf::JCENORMAL,\"" << pPtr->getId() << "\", _os.getByteBuffer(), context, _mStatus, callback);" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}
///////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateDispatchAsyncCpp(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;
    s << TAB << "if (msg->response.iRet != taf::JCESERVERSUCCESS)" << endl
      << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "callback_" << pPtr->getId() << "_exception(msg->response.iRet);" << endl;
    s << endl;

    s << TAB << "return msg->response.iRet;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> _is;" << endl;
    s << endl;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    s << TAB << "_is.setBuffer(msg->response.sBuffer);" << endl;

    //对输出参数编码
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << TAB << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getReturnPtr()->getId() << ";" << endl;
        s << readFrom(pPtr->getReturnPtr()) << endl;
    }

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut())
        {
            s << TAB << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << " "
              << vParamDecl[i]->getTypeIdPtr()->getId() << ";" << endl;
            s << readFrom(vParamDecl[i]->getTypeIdPtr());
        }
    }

    //异步回调都无返回值
    s << TAB << "callback_" << pPtr->getId() << "(";
    string sParams;
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        sParams = pPtr->getReturnPtr()->getId() + ", ";
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (vParamDecl[i]->isOut())
        {
            sParams += vParamDecl[i]->getTypeIdPtr()->getId() + ", ";
        }
    }
    s << taf::TC_Common::trimright(sParams, ", ", false) <<  ");" << endl;
    s << TAB << "return taf::JCESERVERSUCCESS;" << endl;

    return s.str();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateHAsync(const OperationPtr &pPtr, bool bEnableLambda) const
{
    ostringstream s;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();
    s << TAB << "virtual void " << "callback_" << pPtr->getId() << "(";

    string sParams;
    string sParamsNoType;
    string sParamsNoValue;

    if (pPtr->getReturnPtr()->getTypePtr())
    {
        if(pPtr->getReturnPtr()->getTypePtr()->isSimple())
        {
            sParams = tostr(pPtr->getReturnPtr()->getTypePtr()) + " ret, ";
            sParamsNoValue = tostr(pPtr->getReturnPtr()->getTypePtr()) + ", ";
        }
        else
        {
            //结构, map, vector, string
            sParams =  "const " + tostr(pPtr->getReturnPtr()->getTypePtr()) + "& ret, ";
            sParamsNoValue = "const " + tostr(pPtr->getReturnPtr()->getTypePtr()) + "&, ";
        }

        sParamsNoType = "ret,";
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        ParamDeclPtr& pPtr = vParamDecl[i];
        if (pPtr->isOut())
        {
            //输出参数, 或简单类型
            if(pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
            {
                sParams += tostr(pPtr->getTypeIdPtr()->getTypePtr());
            }
            else
            {
                //结构, map, vector, string
                sParams += "const " + tostr(pPtr->getTypeIdPtr()->getTypePtr()) + "&";
            }
            sParamsNoValue += ", ";
            sParams += " " + pPtr->getTypeIdPtr()->getId() + ", ";
            sParamsNoType += pPtr->getTypeIdPtr()->getId() +",";
        }
    }
    s << taf::TC_Common::trimright(sParams, ", ", false) << ")" << endl;

    if(bEnableLambda == true)
    {
         s << TAB << "{ tafAsync::TafCallback<Args...>::doCalls(" << taf::TC_Common::trimright(sParamsNoType, ",", false) <<");}" << endl;
    }
    else
    {
        s << TAB << "{ throw std::runtime_error(\"callback_" << pPtr->getId() << "() overloading incorrect.\"); }" << endl;
    }
    s << TAB << "virtual void " << "callback_" << pPtr->getId() << "_exception(" + m_sNamespace + "::Int32 ret)" << endl;

    if(bEnableLambda == true)
    {
        s << TAB << "{ tafAsync::TafCallback<Args...>::handleException(ret);}";
    }
    else
    {
         s << TAB << "{ throw std::runtime_error(\"callback_" << pPtr->getId() << "_exception() overloading incorrect.\"); }";
    }
    s << endl;

    return s.str();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateHLambdaTypeDef(const OperationPtr &pPtr,const string& sInterfaceId) const
{

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    string sParams;

    if (pPtr->getReturnPtr()->getTypePtr())
    {
        if(pPtr->getReturnPtr()->getTypePtr()->isSimple())
        {
            sParams = tostr(pPtr->getReturnPtr()->getTypePtr()) + ", ";
        }
        else
        {
            //结构, map, vector, string
            sParams = "const " + tostr(pPtr->getReturnPtr()->getTypePtr()) + "&, ";
        }
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        ParamDeclPtr& pPtr = vParamDecl[i];
        if (pPtr->isOut())
        {
            //输出参数, 或简单类型
            if(pPtr->getTypeIdPtr()->getTypePtr()->isSimple())
            {
                sParams += tostr(pPtr->getTypeIdPtr()->getTypePtr());
            }
            else
            {
                //结构, map, vector, string
                sParams += "const " + tostr(pPtr->getTypeIdPtr()->getTypePtr()) + "&";
            }
            sParams += ", ";
        }
    }
    ostringstream s;
    s <<"typedef "<<sInterfaceId<<"PrxCallbackImp<"<<taf::TC_Common::trimright(sParams, ", ", false) <<"> taf_"<<pPtr->getId()<<"Cb;";
    //s <<TAB<<"typedef taf::TC_AutoPtr<" << pPtr->getId() << "Cb> " << pPtr->getId() << "CbPtr;"<<endl;
    return s.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateDispatchCpp(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;
    s << TAB << m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> _is;" << endl;
    s << TAB << "_is.setBuffer(_current->getRequestBuffer());" << endl;

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    string routekey;
    //对输入输出参数编码
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << TAB << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << " "
            << vParamDecl[i]->getTypeIdPtr()->getId() << ";" << endl;

        if (routekey.empty() && vParamDecl[i]->isRouteKey())
        {
            routekey = vParamDecl[i]->getTypeIdPtr()->getId();
        }
    }


    //WUP的支持
    s << TAB << "if (_current->getRequestVersion() == WUPVERSION || _current->getRequestVersion() == WUPVERSION2)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "UniAttribute<" + m_sNamespace + "::BufferWriter, " + m_sNamespace + "::BufferReader>  tafAttr;" << endl;
    s << TAB << "tafAttr.setVersion(_current->getRequestVersion());" << endl;
    s << TAB << "tafAttr.decode(_current->getRequestBuffer());" << endl;
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        string sParamName =  vParamDecl[i]->getTypeIdPtr()->getId();
        string sEnum2Int = (EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) ? "(" + m_sNamespace + "::Int32)" : "";
        if(!vParamDecl[i]->isOut())
        {
            //枚举类型转成int
            if(EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
            {
                s << TAB << sParamName << " = (" << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr())
                    << ") tafAttr.get<" + m_sNamespace + "::Int32>(\"" << sParamName << "\");" << endl;
            }
            else
            {
                s << TAB << "tafAttr.get(\"" << sParamName << "\", " << sParamName << ");" << endl;
            }
        }
        else
        {
            //枚举类型转成int
            if(EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
            {
                s << TAB << sParamName << " = (" << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr())
                    << ") tafAttr.getByDefault<" + m_sNamespace + "::Int32>(\"" << sParamName << "\", " << sEnum2Int << sParamName << ");" << endl;
            }
            else
            {
                s << TAB << "tafAttr.getByDefault(\"" << sParamName << "\", " << sEnum2Int << sParamName << ", "
                    << sEnum2Int << sParamName << ");" << endl;
            }
        }
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "else" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //普通taf请求
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << readFrom(vParamDecl[i]->getTypeIdPtr(), (!vParamDecl[i]->isOut()) );
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    if(!routekey.empty())
    {
        ostringstream os;

        //os << routekey;

        //s << TAB << "status.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;

        //s << TAB <<"SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID);"<<endl;

        s << TAB << "if( doGridRouter(_current,"<<routekey<<") == 0 ) return taf::JCESERVERSUCCESS;"<<endl;
    }

    if(pPtr->getReturnPtr()->getTypePtr())
    {
        //有返回值的函数调用
        s << TAB << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getReturnPtr()->getId() << " = " << pPtr->getId() << "(";
    }
    else
    {
        //无返回值的函数调用
        s << TAB << pPtr->getId() << "(";
    }

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << vParamDecl[i]->getTypeIdPtr()->getId();
        if(i != vParamDecl.size() - 1)
            s << ",";
        else
            s << ", _current);" << endl;
    }
    if(vParamDecl.size() == 0)
    {
        s << "_current);";
    }
    s << TAB << "if(_current->isResponse())" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //WUP输出参数
    s << TAB << "if (_current->getRequestVersion() == WUPVERSION || _current->getRequestVersion() == WUPVERSION2)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "UniAttribute<" + m_sNamespace + "::BufferWriter, " + m_sNamespace + "::BufferReader>  tafAttr;" << endl;
    s << TAB << "tafAttr.setVersion(_current->getRequestVersion());" << endl;
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        string sEnum2Int = (EnumPtr::dynamicCast(pPtr->getReturnPtr()->getTypePtr())) ? "(" + m_sNamespace + "::Int32)" : "";
        s << TAB << "tafAttr.put(\"\", " << sEnum2Int << "_ret);" << endl;
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        string sParamName = vParamDecl[i]->getTypeIdPtr()->getId();
        string sEnum2Int = (EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) ? "(" + m_sNamespace + "::Int32)" : "";
        if(vParamDecl[i]->isOut())
        {
            s << TAB << "tafAttr.put(\"" << sParamName << "\", " << sEnum2Int << sParamName << ");" << endl;
        }
    }
    s << TAB << "tafAttr.encode(_sResponseBuffer);"<< endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "else" << endl;

    //普通taf调用输出参数
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> _os;" << endl;

    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << writeTo(pPtr->getReturnPtr());
    }
    //解码输出参数
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut())
        {
             s << writeTo(vParamDecl[i]->getTypeIdPtr());
        }
    }
    s << TAB << "_os.swap(_sResponseBuffer);" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "return taf::JCESERVERSUCCESS;" << endl;

    return s.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateHAsync(const OperationPtr &pPtr, const string& interfaceId) const
{
    ostringstream s;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    s << TAB << "void async_" << pPtr->getId() << "(" << interfaceId << "PrxCallbackPtr callback,";

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut())
        {
            s << generateH(vParamDecl[i]) << ",";
        }
    }
    s << "const map<string, string> &context = TAF_CONTEXT());";
    s << endl;
    return s.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateHLambdaAsync(const OperationPtr &pPtr, const string& interfaceId) const
{
    ostringstream s;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    s << TAB << "tafAsync::AutoRef<taf_"<<pPtr->getId()<<"Cb> async_" << pPtr->getId() << "(";

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut())
        {
            s << generateH(vParamDecl[i]) << ",";
        }
    }
    s << "const map<string, string> &context = TAF_CONTEXT())"<<endl;

    s << TAB << "{ ";
    s << "TC_AutoPtr<taf_"<<pPtr->getId()<<"Cb> cb=new taf_"<<pPtr->getId()<<"Cb;";
    s <<" async_"<< pPtr->getId()<<"(cb,";
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if (!vParamDecl[i]->isOut())
        {
            s <<vParamDecl[i]->getTypeIdPtr()->getId()<< ",";
        }
    }
    s<<"context);";
    s<<"return cb;";
    s<< " }" << endl; 
     
    return s.str();
}


/////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2Cpp::generateH(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const
{
    ostringstream s;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    s << TAB;

    if(bVirtual) s << "virtual ";

    s << tostr(pPtr->getReturnPtr()->getTypePtr()) << " " << pPtr->getId() << "(";

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << generateH(vParamDecl[i]) << ",";
    }

    if(bVirtual)
        s << "taf::JceCurrentPtr current) = 0;";
    else
        s << "const map<string, string> &context = TAF_CONTEXT());";

    s << endl;

    if(bVirtual)
    {
        //异步回调
        s << TAB << "static void async_response_" << pPtr->getId() << "(taf::JceCurrentPtr current";
        if(pPtr->getReturnPtr()->getTypePtr())
        {
            s << ", ";
            if(pPtr->getReturnPtr()->getTypePtr()->isSimple())
            {
                s << tostr(pPtr->getReturnPtr()->getTypePtr()) << " ";
            }
            else
            {
                //结构, map, vector, string
                s << "const " << tostr(pPtr->getReturnPtr()->getTypePtr()) << " &";
            }
            s << pPtr->getReturnPtr()->getId();
        }
        for(size_t i = 0; i < vParamDecl.size(); i++)
        {
            if(!vParamDecl[i]->isOut())
                continue;

            s << ", ";
            s << generateOutH(vParamDecl[i]);
        }
        s << ");" << endl;
    }
    return s.str();
}
/******************************InterfacePtr***************************************/
string Jce2Cpp::generateH(const InterfacePtr &pPtr) const
{
    ostringstream s;
    vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

    //生成异步回调Proxy
    s << TAB << "/* callback of async proxy for client */" << endl;
    s << TAB << "class " << pPtr->getId() << "PrxCallback: public taf::ServantProxyCallback" << endl;
    s << TAB << "{" << endl;
    s << TAB << "public:" << endl;
    INC_TAB;
    s << TAB << "virtual ~" << pPtr->getId() << "PrxCallback(){}" << endl;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateHAsync(vOperation[i]) << endl;
    }

    DEL_TAB;
    s << TAB << "public:" << endl;
    INC_TAB;

    s << TAB << "int onDispatch(taf::ReqMessagePtr msg);" << endl;
    DEL_TAB;
    s << TAB << "};" << endl;

    s << TAB << "typedef taf::TC_AutoPtr<" << pPtr->getId() << "PrxCallback> " << pPtr->getId() << "PrxCallbackPtr;" << endl;
    s << endl;

    //支持lamda的异步回调
    if(m_bEnableLambda == true)
    {
        s << TAB << "/* callback of lambda_async support proxy for client */" << endl;
        s << TAB <<"template<typename ...Args>"<<endl;
        s << TAB << "class " << pPtr->getId() << "PrxCallbackImp:public "<<pPtr->getId()<<"PrxCallback,public tafAsync::TafCallback<Args...>" << endl;
        s << TAB << "{" << endl;
        s << TAB << "public:" << endl;
        INC_TAB;
        //s << TAB << "virtual ~" << pPtr->getId() << "CallbackImp(){}" << endl;
    
        for(size_t i = 0; i < vOperation.size(); i++)
        {
            s << generateHAsync(vOperation[i],true) << endl;
        }
    
        DEL_TAB;
        s << TAB << "};" << endl;
    }


    //生成客户端代理
    s << TAB << "/* proxy for client */" << endl;
    s << TAB << "class " << pPtr->getId() << "Proxy : public taf::ServantProxy" << endl;
    s << TAB << "{" << endl;
    s << TAB << "public:" << endl;
    INC_TAB;
    s << TAB << "typedef map<string, string> TAF_CONTEXT;" << endl;

    if(m_bEnableLambda == true)
    {
        for(size_t i = 0; i < vOperation.size(); i++)
        {
            s << TAB<< generateHLambdaTypeDef(vOperation[i],pPtr->getId()) << endl;
        }
    }

    s<<endl;
    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateH(vOperation[i], false, pPtr->getId());// << endl;
        s << generateHAsync(vOperation[i], pPtr->getId())<<endl;
        if(m_bEnableLambda == true)
        {
            s << generateHLambdaAsync(vOperation[i], pPtr->getId()) << endl;
        }
    }
    s << TAB << pPtr->getId() << "Proxy* taf_hash(int64_t key);" << endl;

    DEL_TAB;
    s << TAB << "};" << endl;

    s << TAB << "typedef taf::TC_AutoPtr<" << pPtr->getId() << "Proxy> " << pPtr->getId() << "Prx;" << endl;
    s << endl;

    //生成服务端Servant
    s << TAB <<  "/* servant for server */" << endl;
    s << TAB << "class " << pPtr->getId() << " : public taf::Servant" << endl;
    s << TAB << "{" << endl;
    s << TAB << "public:" << endl;
    INC_TAB;
    s << TAB << "virtual ~" << pPtr->getId() << "(){}" << endl;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateH(vOperation[i], true, pPtr->getId()) << endl;
    }

    DEL_TAB;
    s << TAB << "public:" << endl;
    INC_TAB;

    s << TAB << "int onDispatch(taf::JceCurrentPtr _current, vector<char> &_sResponseBuffer);" << endl;
    DEL_TAB;
    s << TAB << "};" << endl;



    return s.str();
}

string Jce2Cpp::generateAsyncResponseCpp(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;
    //异步回调
    s << TAB << "void " << cn << "::async_response_" << pPtr->getId() << "(taf::JceCurrentPtr current" ;

    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << ", ";
        if(pPtr->getReturnPtr()->getTypePtr()->isSimple())
        {
            s << tostr(pPtr->getReturnPtr()->getTypePtr()) << " ";
        }
        else
        {
            //结构, map, vector, string
            s << "const " << tostr(pPtr->getReturnPtr()->getTypePtr()) << " &";
        }
        s << pPtr->getReturnPtr()->getId();
    }

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(!vParamDecl[i]->isOut())
            continue;

        s << ", ";
        s << generateOutH(vParamDecl[i]);
    }
    s << ")" << endl;

    s << TAB << "{" << endl;
    INC_TAB;

    //WUP输出参数
    s << TAB << "if (current->getRequestVersion() == WUPVERSION || current->getRequestVersion() == WUPVERSION2)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "UniAttribute<" + m_sNamespace + "::BufferWriter, " + m_sNamespace + "::BufferReader>  tafAttr;" << endl;
    s << TAB << "tafAttr.setVersion(current->getRequestVersion());" << endl;
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        string sEnum2Int = (EnumPtr::dynamicCast(pPtr->getReturnPtr()->getTypePtr())) ? "(" + m_sNamespace + "::Int32)" : "";
        s << TAB << "tafAttr.put(\"\", " << sEnum2Int << "_ret);" << endl;
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        string sParamName = vParamDecl[i]->getTypeIdPtr()->getId();
        string sEnum2Int = (EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) ? "(" + m_sNamespace + "::Int32)" : "";
        if(vParamDecl[i]->isOut())
        {
            s << TAB << "tafAttr.put(\"" << sParamName << "\", " << sEnum2Int << sParamName << ");" << endl;
        }
    }
    s << endl;
    s << TAB << "vector<char> sWupResponseBuffer;" << endl;
    s << TAB << "tafAttr.encode(sWupResponseBuffer);"<< endl;
    s << TAB << "current->sendResponse(taf::JCESERVERSUCCESS, sWupResponseBuffer);" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "else" << endl;
    s << TAB << "{" << endl;

    INC_TAB;

    s << TAB <<  m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> _os;" << endl;
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << writeTo(pPtr->getReturnPtr()) << endl;
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(!vParamDecl[i]->isOut())
            continue;

        s << writeTo(vParamDecl[i]->getTypeIdPtr()) << endl;
    }


//    s << TAB << "current->sendResponse(taf::JCESERVERSUCCESS, string(_os.getBuffer(), _os.getLength()));" << endl;
    s << TAB << "current->sendResponse(taf::JCESERVERSUCCESS, _os.getByteBuffer());" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}

struct SortOperation
{
    bool operator()(const OperationPtr &o1, const OperationPtr &o2)
    {
        return o1->getId() < o2->getId();
    }
};

string Jce2Cpp::generateCpp(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const
{
    ostringstream s;
    vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

    std::sort(vOperation.begin(), vOperation.end(), SortOperation());

    //生成客户端接口的实现
    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateCpp(vOperation[i], pPtr->getId()) << endl;
        s << generateAsyncResponseCpp(vOperation[i], pPtr->getId()) << endl;
        s << generateCppAsync(vOperation[i], pPtr->getId()) << endl;
    }

    //生成Hash实现
    s << TAB << pPtr->getId() << "Proxy* " << pPtr->getId() << "Proxy::taf_hash(int64_t key)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return (" << pPtr->getId() + "Proxy*)ServantProxy::taf_hash(key);" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    string dname    = "__" + nPtr->getId() + "__" + pPtr->getId() + "_all";
    string dispatch =  "static ::std::string " + dname;
    s << TAB << dispatch << "[]=" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << TAB << "\"" << vOperation[i]->getId() << "\"";
        if(i != vOperation.size() - 1)
        {
            s << ",";
        }
        s << endl;
    }

    DEL_TAB;
    s << TAB << "};" << endl;

    s << endl;

    //生成异步回调接口
    s << TAB << "int " << pPtr->getId() << "PrxCallback::onDispatch(taf::ReqMessagePtr msg)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "pair<string*, string*> r = equal_range(" << dname << ", " << dname << "+" << vOperation.size() << ", msg->request.sFuncName);" << endl;

    s << TAB << "if(r.first == r.second) return taf::JCESERVERNOFUNCERR;" << endl;

    s << TAB << "switch(r.first - " << dname << ")" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << TAB << "case " << i << ":" << endl;
        s << TAB << "{" << endl;
        INC_TAB;

        s << generateDispatchAsyncCpp(vOperation[i], pPtr->getId()) << endl;

        DEL_TAB;
        s << TAB << "}" << endl;
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "return taf::JCESERVERNOFUNCERR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;

    //生成服务端接口
    s << TAB << "int " << pPtr->getId() << "::onDispatch(taf::JceCurrentPtr _current, vector<char> &_sResponseBuffer)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "pair<string*, string*> r = equal_range(" << dname << ", " << dname << "+" << vOperation.size() << ", _current->getFuncName());" << endl;

    s << TAB << "if(r.first == r.second) return taf::JCESERVERNOFUNCERR;" << endl;

    s << TAB << "switch(r.first - " << dname << ")" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << TAB << "case " << i << ":" << endl;
        s << TAB << "{" << endl;
        INC_TAB;

        s << generateDispatchCpp(vOperation[i], pPtr->getId()) << endl;

        DEL_TAB;
        s << TAB << "}" << endl;
    }

    DEL_TAB;
    s << TAB << "}" << endl;

    s << TAB << "return taf::JCESERVERNOFUNCERR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}

/******************************EnumPtr***************************************/

string Jce2Cpp::generateH(const EnumPtr &pPtr) const
{
    ostringstream s;
    s << TAB << "enum " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for(size_t i = 0; i < member.size(); i++)
    {
        s << TAB << member[i]->getId();
        if(member[i]->hasDefault())
        {
            s << " = " << member[i]->def();
        }
        s << "," << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    //生成枚举转字符串函数
    s << TAB << "inline string etos" << "(const " <<  pPtr->getId() << " & e)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "switch(e)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "case " <<  member[i]->getId() << ": return "
            << "\"" << member[i]->getId() << "\";" << endl;
    }
    s << TAB << "default: return \"\";" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    //s << TAB << "return \"\";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    //生成字符串转枚举函数
    s << TAB << "inline int stoe" << "(const string & s, " <<  pPtr->getId() << " & e)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "if(s == \"" << member[i]->getId() << "\")  { e="<< member[i]->getId() << "; return 0;}" << endl;
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

    if(pPtr->getConstTokPtr()->t == ConstTok::STRING)
    {
        string tmp = taf::TC_Common::replace(pPtr->getConstTokPtr()->v, "\"", "\\\"");
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";"<< endl;
    }
    else
    {
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstTokPtr()->v
            << ((tostr(pPtr->getTypeIdPtr()->getTypePtr()) ==  m_sNamespace + "::Int64") ? "LL;" : ";" )<< endl;
    }

    return s.str();
}
/******************************NamespacePtr***************************************/

string Jce2Cpp::generateH(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<InterfacePtr>        &is    = pPtr->getAllInterfacePtr();
    vector<StructPtr>           &ss    = pPtr->getAllStructPtr();
    vector<EnumPtr>             &es    = pPtr->getAllEnumPtr();
    vector<ConstPtr>            &cs    = pPtr->getAllConstPtr();

    s << endl;
    s << TAB << "namespace " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < cs.size(); i++)
    {
        s << generateH(cs[i]) << endl;
    }

    for(size_t i = 0; i < es.size(); i++)
    {
        s << generateH(es[i]) << endl;
    }

    for(size_t i = 0; i < ss.size(); i++)
    {
        s << generateH(ss[i], pPtr->getId()) << endl;
    }

    s << endl;

    for(size_t i = 0; i < is.size() && m_bOnlyStruct == false; i++)
    {
        s << generateH(is[i]) << endl;
        s << endl;
    }

    DEL_TAB;
    s << "}";

    s << endl << endl;

    //定义结构拷贝宏
    for(size_t i = 0; i < ss.size() && !m_bPDU; i++)
    {
        s << "#define "<< pPtr->getId() << "_" << ss[i]->getId() << "_" << "JCE_COPY_STRUCT_HELPER   \\" << endl;
        s << "        ";

        vector<TypeIdPtr>& member = ss[i]->getAllMemberPtr();
        for(size_t j = 0; j < member.size(); j++)
        {
            s << "jce_copy_struct(a." << member[j]->getId() << ",b." << member[j]->getId() << ");" ;
        }
        s << endl << endl;
    }

    return s.str();
}

string Jce2Cpp::generateCpp(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<InterfacePtr>    &is    = pPtr->getAllInterfacePtr();

    s << endl;
    s << TAB << "namespace " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    s << endl;
    INC_TAB;

    for (size_t i = 0; i < is.size(); i++)
    {
        s << generateCpp(is[i], pPtr) << endl;
        s << endl;
    }

    //如果是互联网特殊应用生成struct的读写.cpp
    vector<StructPtr> & vecStruct = pPtr->getAllStructPtr();
    for (size_t i = 0; i < vecStruct.size() && (m_bPDU || m_bLuaSupport); i++)
    {
        s << generateCpp(vecStruct[i], pPtr->getId()) << endl;
        s << endl;
    }

    DEL_TAB;
    s << "}";

    return s.str();
}

/******************************Jce2Cpp***************************************/

void Jce2Cpp::generateH(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));

    string fileH    = m_sBaseDir + "/" + n + ".h";
    string fileCpp  = m_sBaseDir + "/" + n + ".cpp";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    s << "#ifndef " << define << endl;
    s << "#define " << define << endl;
    s << endl;
    s << "#include <map>" << endl;
    s << "#include <string>" << endl;
    s << "#include <vector>" << endl;
    s << "#include \"jce/Jce.h\"" << endl;

    if(m_bJsonSupport)
        s << "#include \"jce/Jce_json.h\"" << endl;
    if(m_bPDU)
        s << "#include \"pdu_header.h\""<<endl;

    s << "using namespace std;" << endl;

    vector<string> include = pPtr->getIncludes();
    for(size_t i = 0; i < include.size(); i++)
    {
        s << "#include \"" << g_parse->getHeader() << taf::TC_File::extractFileName(include[i]) << "\"" << endl;
    }

    if(m_bLuaSupport)
    {
        s << "#include \"luaregistry.h\"" << endl;
    }

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    //名字空间有接口
    for(size_t i = 0; i < namespaces.size() && m_bOnlyStruct == false; i++)
    {
        if(namespaces[i]->hasInterface())
        {
            s << "#include \"servant/ServantProxy.h\"" << endl;
            s << "#include \"servant/Servant.h\"" << endl;
            if(m_bEnableLambda== true)
            {
                s<< "#include \"servant/TafCallbacks.h\"" << endl;
            }
            break;
        }

    }


    s << endl;

    if (m_bPDU)
    {
        s << endl;
        s << TAB << "struct pdu_protocol_header;" << endl;
    }
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateH(namespaces[i]) << endl;
    }

    s << endl;
    s << "#endif" << endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileH, s.str());
}

void Jce2Cpp::generateCpp(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));
    string fileCpp  = m_sBaseDir + "/" + n + ".cpp";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    s << "#include \"" << g_parse->getHeader() << n << ".h\"" << endl;
    if (!m_bPDU)
    {
        s << "#include \"jce/wup.h\"" << endl;
        s << "#include \"servant/BaseF.h\"" << endl;
        s << endl;
        s << "using namespace wup;" << endl;
    }
    else
    {
        s << endl;
        s << TAB << "extern int pdu_header_pack1(pdu_protocol_header * pdu_header, uint8_t * pui_buff, uint32_t * pi_buff_len);" << endl;
        s << TAB << "extern int pdu_header_pack2(uint8_t * pui_buff, uint32_t * pi_buff_len, uint32_t * pi_used_len);" << endl;
        s << TAB << "extern int pdu_header_unpack(uint8_t * pui_buff, uint32_t * pi_buff_len, pdu_protocol_header * pdu_header, uint32_t * pi_end_len);" << endl;
    }

    bool bHasInterface = false;
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateCpp(namespaces[i]) << endl;

        vector<InterfacePtr>    &is    = namespaces[i]->getAllInterfacePtr();
        if(is.size() > 0)
        {
            bHasInterface = true;
        }
    }
    //没有接口的jce不用生成cpp，并且不是互联网特殊应用
    if(!bHasInterface && !m_bPDU && !m_bLuaSupport)
    {
        return;
    }

    s << endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileCpp, s.str());

}

void Jce2Cpp::createFile(const string &file, const vector<string> &vsCoder)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for(size_t i = 0; i < contexts.size(); i++)
    {
        if(file == contexts[i]->getFileName())
        {
            if(vsCoder.size() == 0)
            {
                generateH(contexts[i]);
                if(m_bOnlyStruct == false)
                {
                    generateCpp(contexts[i]);
                }
            }
            else
            {
                for(size_t j = 0 ; j < vsCoder.size(); j++)
                {
                    generateCoder(contexts[i], vsCoder[j]);
                }
            }
        }
    }
}

StructPtr Jce2Cpp::findStruct(const ContextPtr &pPtr,const string &id)
{
    string sid = id;

    //在当前namespace中查找
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        NamespacePtr np = namespaces[i];
        vector<StructPtr> structs = np->getAllStructPtr();

        for(size_t i = 0; i < structs.size(); i++)
        {
            if(structs[i]->getSid() == sid)
            {
                return structs[i];
            }
        }
    }

    return NULL;
}


////////////////////////////////
//for coder generating
////////////////////////////////

string Jce2Cpp::generateCoder(const NamespacePtr &pPtr,const string & sInterface) const
{
    ostringstream s;
    vector<InterfacePtr>    &is    = pPtr->getAllInterfacePtr();
    vector<StructPtr>       &ss    = pPtr->getAllStructPtr();
    vector<EnumPtr>         &es    = pPtr->getAllEnumPtr();
    vector<ConstPtr>        &cs    = pPtr->getAllConstPtr();

    s << endl;
    s << TAB << "namespace " << pPtr->getId() << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    for(size_t i = 0; i < cs.size(); i++)
    {
        s << generateH(cs[i]) << endl;
    }

    for(size_t i = 0; i < es.size(); i++)
    {
        s << generateH(es[i]) << endl;
    }

    for(size_t i = 0; i < ss.size(); i++)
    {
        s << generateH(ss[i], pPtr->getId()) << endl;
    }

    s << endl;

    for(size_t i = 0; i < is.size(); i++)
    {
        if(pPtr->getId() + "::" + is[i]->getId() == sInterface)
        {
            s << generateCoder(is[i]) << endl;
            s << endl;
        }
    }

    DEL_TAB;
    s << "}";

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
    s << TAB << "typedef map<string, string> TAF_CONTEXT;" << endl << endl;

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

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateCoder(vOperation[i]) << endl;
    }

    DEL_TAB;
    s << TAB << "protected:" << endl << endl;
    INC_TAB;
    s << TAB << "static " + m_sNamespace + "::Int32 fetchPacket(const string & in, string & out)" << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "if(in.length() < sizeof(" + m_sNamespace + "::Int32)) return eJcePacketLess;" << endl;

    s << TAB << "" + m_sNamespace + "::Int32 iHeaderLen;" << endl;
    s << TAB << "memcpy(&iHeaderLen, in.c_str(), sizeof(" + m_sNamespace + "::Int32));" << endl;

    s << TAB << "iHeaderLen = ntohl(iHeaderLen);" << endl;
    s << TAB << "if(iHeaderLen < (" + m_sNamespace + "::Int32)sizeof(" + m_sNamespace + "::Int32) || iHeaderLen > 100000000) return eJcePacketErr;" << endl;
    s << TAB << "if((" + m_sNamespace + "::Int32)in.length() < iHeaderLen) return eJcePacketLess;" << endl;

    s << TAB << "out = in.substr(sizeof(" + m_sNamespace + "::Int32), iHeaderLen - sizeof(" + m_sNamespace + "::Int32)); " << endl;
    s << TAB << "return 0;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "static string encodeBasePacket(const string & sServantName, const string & sFuncName, const vector<char> & buffer, "
        <<"const map<string, string>& context = TAF_CONTEXT())" <<endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> os;" << endl;
    s << TAB << "os.write(1, 1);" << endl;
    s << TAB << "os.write(0, 2);" << endl;
    s << TAB << "os.write(0, 3);" << endl;
    s << TAB << "os.write(0, 4);" << endl;
    s << TAB << "os.write(sServantName, 5);" << endl;
    s << TAB << "os.write(sFuncName, 6);" << endl;
    s << TAB << "os.write(buffer, 7);" << endl;
    s << TAB << "os.write(60, 8);" << endl;
    s << TAB << "os.write(context, 9);" << endl;
    s << TAB << "os.write(map<string, string>(), 10);" << endl;

    s << TAB << m_sNamespace + "::Int32 iHeaderLen;" << endl;
    s << TAB << "iHeaderLen = htonl(sizeof(" + m_sNamespace + "::Int32) + os.getLength());" << endl;
    s << TAB << "string s;" << endl;
    s << TAB << "s.append((const char*)&iHeaderLen, sizeof(" + m_sNamespace + "::Int32));" << endl;
    s << TAB << "s.append(os.getBuffer(), os.getLength());" << endl;

    s << TAB << "return s;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "static " + m_sNamespace + "::Int32 decodeBasePacket(const string & in, " + m_sNamespace + "::Int32 & iServerRet, vector<char> & buffer)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> is;" << endl;
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
    s << TAB << "static string encode_" << pPtr->getId() << "(const string & sServantName, ";

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(!vParamDecl[i]->isOut())
        {
            s << generateH(vParamDecl[i]) << ",";
        }
    }
    s << endl;
    s << TAB << "    const map<string, string>& context = TAF_CONTEXT())" << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "try" << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << m_sNamespace + "::JceOutputStream<" + m_sNamespace + "::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut()) continue;
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "return encodeBasePacket(sServantName, \""<< pPtr->getId() << "\", _os.getByteBuffer(), context);" <<endl;

    DEL_TAB;

    s << TAB << "}" << endl;
    s << TAB << "catch (" + m_sNamespace + "::JceException & ex)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return \"\";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;

    //解码函数

    s << TAB << "static " + m_sNamespace + "::Int32 decode_" << pPtr->getId() << "(const string & in ";

    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << ", " << tostr(pPtr->getReturnPtr()->getTypePtr()) << " & _ret ";
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(!vParamDecl[i]->isOut())
            continue;

        s << ", " << generateH(vParamDecl[i]);
    }
    s << ")" <<endl;

    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "try" << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "string out;" << endl;
    s << TAB << m_sNamespace + "::Int32 iRet = 0;" << endl;
    s << TAB << "if((iRet = fetchPacket(in, out)) != 0) return iRet;" << endl;

    s << TAB << m_sNamespace + "::JceInputStream<" + m_sNamespace + "::BufferReader> _is;" << endl;
    s << TAB << m_sNamespace + "::Int32 iServerRet=0;" << endl;
    s << TAB << "vector<char> buffer;" << endl;
    s << TAB << "decodeBasePacket(out, iServerRet, buffer);" << endl;
    s << TAB << "if(iServerRet != 0)  return iServerRet;" << endl;

    s << TAB << "_is.setBuffer(buffer);" << endl;

    if(pPtr->getReturnPtr()->getTypePtr())
    {
        s << readFrom(pPtr->getReturnPtr());
    }

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut())
        {
            s << readFrom(vParamDecl[i]->getTypeIdPtr());
        }
    }


    s << TAB << "return 0;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "catch (" + m_sNamespace + "::JceException & ex)" << endl;
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

void Jce2Cpp::generateCoder(const ContextPtr &pPtr,const string &sInterface) const
{
    cout<< "Interface:" << sInterface <<endl;
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName())) + "Coder";

    string fileH    = m_sBaseDir + "/" + n + ".h";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    s << "#ifndef " << define << endl;
    s << "#define " << define << endl;
    s << endl;
    s << "#include <map>" << endl;
    s << "#include <string>" << endl;
    s << "#include <vector>" << endl;
    s << "#include \"jce/Jce.h\"" << endl;

    s << "using namespace std;" << endl;

    vector<string> include = pPtr->getIncludes();
    for(size_t i = 0; i < include.size(); i++)
    {
        s << "#include \"" << g_parse->getHeader()
            << taf::TC_Common::replace(taf::TC_File::extractFileName(include[i]), ".h", "Coder.h") << "\"" << endl;
    }

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    s << endl;

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateCoder(namespaces[i], sInterface) << endl;
    }

    s << endl;
    s << "#endif" << endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileH, s.str());

    return;
}
