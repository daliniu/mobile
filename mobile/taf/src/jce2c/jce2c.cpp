#include "jce2c.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
//
string Jce2C::writeTo(const TypeIdPtr &pPtr,const string& namespaceId) const
{
    ostringstream s;
    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    MapPtr mPtr = MapPtr::dynamicCast(pPtr->getTypePtr());

    if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = JceOutputStream_writeInt32(os, (Int32)taf_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (toClassName(VectorPtr::dynamicCast(pPtr->getTypePtr())) == "list<char>" )
    {
        s << TAB << "ret = JceOutputStream_writeVectorChar(os, taf_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (sPtr)
    {
        s << TAB << "ret = JceOutputStream_writeStruct(os, taf_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else if (m_bCheckDefault == false || pPtr->isRequire() || (!pPtr->hasDefault() && !mPtr && !vPtr))
    {
        s << TAB << "ret = JceOutputStream_write" << toFuncName(pPtr->getTypePtr()) << "(os, taf_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
    }
    else
    {
        std::string sCheckCond = "true";

        if (mPtr)
        {
            sCheckCond = "JMapWrapper_size(taf_st->" + pPtr->getId() + ") > 0";
        }

        if (vPtr)
        {
            sCheckCond = "JArray_size(taf_st->" + pPtr->getId() + ") > 0";
        }

        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
        if (bPtr && bPtr->kind() == Builtin::KindString && pPtr->hasDefault())
        {
            string tmp = taf::TC_Common::replace(pPtr->def(), "\"", "\\\"");
            sCheckCond = "JString_size(taf_st->" + pPtr->getId() + ") != strlen(\"" + tmp + "\") || strncmp(JString_data(taf_st->" + pPtr->getId() + "), \"" + tmp + "\", JString_size(taf_st->" + pPtr->getId() + ")) != 0";
        }
        else if (pPtr->hasDefault())
        {
            sCheckCond = "taf_st->" + pPtr->getId() + " != " + pPtr->def();
        }

        s << TAB << "if (" << sCheckCond << ")" << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        s << TAB << "ret = JceOutputStream_write" << toFuncName(pPtr->getTypePtr()) << "(os, taf_st->" << pPtr->getId() << ", " << pPtr->getTag() << ");" << endl;
        DEL_TAB;
        s << TAB << "}" << endl;
    }


    s << TAB <<"if (JCE_SUCCESS != ret) return ret;" << endl;
    s <<endl;

    return s.str();
}

string Jce2C::readFrom(const TypeIdPtr &pPtr, bool bIsRequire,const string& namespaceId) const
{
    ostringstream s;

    bool isPtr = false;
    string type = toFuncName(pPtr->getTypePtr());
    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if (type == "String" || type == "Vector" || type == "Map" || type == "Struct")
    {
        isPtr = true;
    }

    if (EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = JceInputStream_readInt32(is, & taf_st->" << pPtr->getId();
    }
    else if (toClassName(VectorPtr::dynamicCast(pPtr->getTypePtr())) == "list<char>" )
    {
        s << TAB << "ret = JceInputStream_readVectorChar(is, taf_st->" << pPtr->getId();
    }
    else if (sPtr)
    {
        s << TAB << "ret = JceInputStream_readStruct(is, taf_st->" << pPtr->getId();
    }
    else
    {
        s << TAB << "ret = JceInputStream_read" << toFuncName(pPtr->getTypePtr()) << "(is, "<< (isPtr ? "" : "&") <<"taf_st->" << pPtr->getId();
    }

    s << ", " << pPtr->getTag() << ", " << ((pPtr->isRequire() && bIsRequire)?"true":"false") << ");"<<endl;
    s << TAB<<"if(JCE_SUCCESS != ret) return ret;" << endl;

    return s.str();
}

string Jce2C::toClassName(const TypePtr &pPtr) const
{
    string s;
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     s = "bool";     break;
            case Builtin::KindByte:     s = "char";     break;
            case Builtin::KindShort:    s = "short";    break;
            case Builtin::KindInt:      s = "int32";    break;
            case Builtin::KindLong:     s = "int64";    break;
            case Builtin::KindFloat:    s = "float";    break;
            case Builtin::KindDouble:   s = "double";   break;
            case Builtin::KindString:   s = "string";   break;
            default:                    assert(false);  break;
        }
        
        return s;
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return TC_Common::replace(sPtr->getSid(), "::", ".");

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}


string Jce2C::toFuncName(const TypePtr &pPtr) const
{
    string s;
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr)
    {
        switch (bPtr->kind())
        {
            case Builtin::KindBool:     s = "Bool";     break;
            case Builtin::KindByte:     s = "Char";     break;
            case Builtin::KindShort:    s = bPtr->isUnsigned()?"UInt8" :"Short";    break;
            case Builtin::KindInt:      s = bPtr->isUnsigned()?"UInt16":"Int32";    break;
            case Builtin::KindLong:     s = bPtr->isUnsigned()?"UInt32":"Int64";    break;
            case Builtin::KindFloat:    s = "Float";    break;
            case Builtin::KindDouble:   s = "Double";   break;
            case Builtin::KindString:   s = "String";   break;
            default:                    assert(false);  break;
        }
        
        return s;
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return "Vector";

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "Map";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return "Struct";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}


/*******************************BuiltinPtr********************************/
string Jce2C::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        if (toClassName(vPtr) == "list<char>")
            return "JString * ";
        else
            return "JArray * ";
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "JMapWrapper * ";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);

    if (sPtr) return tostrStruct(sPtr)+ " *";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return "Int32";//tostrEnum(ePtr);

    if (!pPtr) return "void";

    assert(false);
    return "";
}

string Jce2C::tostrBuiltin(const BuiltinPtr &pPtr) const
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
        s = (pPtr->isUnsigned()?"UInt8":"Short");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?"UInt16":"Int32");
        break;
    case Builtin::KindLong:
        s =  (pPtr->isUnsigned()?"UInt32":"Int64");
        break;
    case Builtin::KindFloat:
        s = "Float";
        break;
    case Builtin::KindDouble:
        s = "Double";
        break;
    case Builtin::KindString:
        s = "JString * ";
        break;
    default:
        assert(false);
        break;
    }

    return s;
}
/*******************************VectorPtr********************************/
string Jce2C::tostrVector(const VectorPtr &pPtr) const
{
    string s = string("list<") + toClassName(pPtr->getTypePtr());

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
string Jce2C::tostrMap(const MapPtr &pPtr) const
{
    string s = string("map<") + toClassName(pPtr->getLeftTypePtr()) + "," + toClassName(pPtr->getRightTypePtr());
    if (MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
    {
        s += ">";
    }
    else
    {
        s += ">";
    }
    return s;
}

/*******************************StructPtr********************************/
string Jce2C::tostrStruct(const StructPtr &pPtr) const
{
    return TC_Common::replace(pPtr->getSid(), "::", "_");
}

string Jce2C::MD5(const StructPtr &pPtr) const
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
string Jce2C::tostrEnum(const EnumPtr &pPtr) const
{
    return TC_Common::replace(pPtr->getSid(), "::", "_");
}
///////////////////////////////////////////////////////////////////////
string Jce2C::generateH(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    string sStructName = namespaceId + "_" + pPtr->getId();

    s << TAB << "typedef struct " << sStructName << " " << sStructName << ";" << endl;
    s << TAB << "struct " << sStructName << endl;
    s << TAB << "{" << endl;
    //s << TAB << "public:" << endl;

    INC_TAB;

    s << TAB << "char * className;" << endl;
    s << TAB << "Int32 (*writeTo)( const "<<sStructName<<"*, JceOutputStream *);" << endl;
    s << TAB << "Int32 (*readFrom)( "<<sStructName<<"*, JceInputStream *);" << endl;
    //s << TAB << "char * MD5;" << endl;
    //s << TAB << "return " << MD5(pPtr) << ";" << endl;

    //定义成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    for (size_t j = 0; j < member.size(); j++)
    {
        s << TAB << tostr(member[j]->getTypePtr()) << " " << member[j]->getId() << ";" << endl;
    }


    s << endl;

    DEL_TAB;
    s << TAB << "};" << endl;
    s << endl;

    ////////////////////////////////////////////////////////////
    //s << TAB << "template<typename WriterT>" << endl;
    s << TAB << "Int32 " << sStructName << "_writeTo(const " << sStructName << "* taf_st, JceOutputStream * os);" << endl; 
    s << TAB << "Int32 " << sStructName << "_readFrom(" << sStructName << "* taf_st, JceInputStream *is);" << endl;
    s << TAB << "void " <<  sStructName << "_del(" << sStructName << " ** handle);" << endl;
    s << TAB << "Int32 " << sStructName << "_init(" << sStructName << " * handle);" << endl;
    s << TAB << sStructName << " * " << sStructName << "_new();" << endl;
    s << TAB <<endl;

    return s.str();
}

///////////////////////////////////////////////////////////////////////
string Jce2C::generateC(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    string sStructName = namespaceId + "_" + pPtr->getId();

    //定义成员变量
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    ////////////////////////////////////////////////////////////
    s << TAB << "Int32 " << sStructName << "_writeTo(const " << sStructName << "* taf_st, JceOutputStream * os)" << endl; 
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        s << writeTo(member[j],namespaceId);
    }

    s << endl;
    s << TAB << "return JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ///////////////////////////////////////////////////////////
    s << endl;
    s << TAB << "Int32 " << sStructName << "_readFrom(" << sStructName << "* taf_st, JceInputStream *is)" << endl;;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        s << readFrom(member[j],true,namespaceId);
    }

    s << endl;
    s << TAB << "return JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "void " << sStructName << "_del(" << sStructName << " ** handle)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << sStructName << " ** this = (" << sStructName << "**)handle;" << endl;

    for (size_t j = 0; j < member.size(); j++)
    {
        string type = toFuncName(member[j]->getTypePtr());
        if (type == "String")
            s << TAB << "if((*this)->" << member[j]->getId() << ") JString_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Vector")
            if (toClassName(member[j]->getTypePtr()) == "list<char>")
                s << TAB << "if((*this)->" << member[j]->getId() << ") JString_del(&(*this)->" << member[j]->getId() << ");" << endl;
            else
                s << TAB << "if((*this)->" << member[j]->getId() << ") JArray_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Map")
            s << TAB << "if((*this)->" << member[j]->getId() << ") JMapWrapper_del(&(*this)->" << member[j]->getId() << ");" << endl;
        else if (type == "Struct")
        {
            StructPtr sPtr = StructPtr::dynamicCast(member[j]->getTypePtr());
            if (sPtr)
                s << TAB << "if((*this)->" << member[j]->getId() << ") "<< namespaceId + "_" + sPtr->getId()<<"_del(&(*this)->" << member[j]->getId() << ");" << endl;
        }

    }
    s << TAB << "if((*this)->className) JceFree((*this)->className);" << endl;
    s << TAB << "JceFree(*this);" << endl;
    s << TAB << "*this = NULL;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    s << TAB << "Int32 " << sStructName << "_init(" << sStructName << " * handle)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << sStructName << " * this = (" << sStructName << "*) handle;" << endl;
    s << endl;

    s << TAB << "char * tafClassName = \"" << namespaceId << "." << pPtr->getId() << "\";" << endl;
    s << endl;

    s << TAB << "this->className = JceMalloc(strlen(tafClassName)+1);" << endl;
    s << TAB << "this->writeTo = "<< sStructName << "_writeTo;" <<endl;
    s << TAB << "this->readFrom = "<< sStructName << "_readFrom;" << endl;
    s << endl;

    string sCleanCondition = "!this->className ";
    for (size_t j = 0; j < member.size(); j++)
    {
        BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
        //string值要转义
        if (bPtr && bPtr->kind() == Builtin::KindString)
        {
            s << TAB << "this->" << member[j]->getId() << " = JString_new();" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        VectorPtr vPtr = VectorPtr::dynamicCast(member[j]->getTypePtr());
        if (vPtr)
        {
            if (toClassName(vPtr) == "list<char>")
            {
                s << TAB << "this->" << member[j]->getId() << " = JString_new();" << endl;
                sCleanCondition += " || !this->" + member[j]->getId();
            }
            else
            {
                s << TAB << "this->" << member[j]->getId() << " = JArray_new(\"" 
                << toClassName(vPtr->getTypePtr()) << "\");" << endl;
                sCleanCondition += " || !this->" + member[j]->getId();
            }
            continue;
        }

        MapPtr mPtr = MapPtr::dynamicCast(member[j]->getTypePtr());
        if (mPtr)
        {
            s << TAB << "this->" << member[j]->getId() << " = JMapWrapper_new(\""
            << toClassName(mPtr->getLeftTypePtr()) << "\", \"" << toClassName(mPtr->getRightTypePtr()) << "\");" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        StructPtr sPtr = StructPtr::dynamicCast(member[j]->getTypePtr());
        if (sPtr)
        {
            s << TAB << "this->" << member[j]->getId() << " = "<< namespaceId + "_" + sPtr->getId()+"_new();" << endl;
            sCleanCondition += " || !this->" + member[j]->getId();
            continue;
        }

        if (tostr(member[j]->getTypePtr()) == "Int64")
        {
            s << "#ifdef __MTK_64"<<endl;
            s << TAB << "this->" << member[j]->getId() << ".high = " << member[j]->def() << ";" << endl;
            s << TAB << "this->" << member[j]->getId() << ".low = " << member[j]->def() << ";" << endl;
            s << "#else"<<endl;
            s << TAB << "this->" << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
            s << "#endif"<<endl;
            continue;
        }
        if (member[j]->hasDefault())
        {
            s << TAB << "this->" << member[j]->getId() << " = " << member[j]->def() << ";" << endl;
        }
    }
    s << endl;

    s << TAB << "if(" << sCleanCondition << ")" << endl;
    s << TAB << "{"<<endl;
    INC_TAB;
    s << TAB <<  sStructName << "_del(&this);"<<endl;
    s << TAB <<  "return JCE_MALLOC_ERROR;"<<endl;
    DEL_TAB;
    s << TAB <<"}" << endl;

    s << TAB << "memcpy(this->className, tafClassName, strlen(tafClassName)+1);" << endl;
    s << endl;

    //string default
    for (size_t j = 0; j < member.size(); j++)
    {
        BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
        //string值要转义
        if (bPtr && bPtr->kind() == Builtin::KindString)
        {
            if (member[j]->hasDefault())
            {
                string tmp = taf::TC_Common::replace(member[j]->def(), "\"", "\\\"");
                s << TAB << "JString_assign(this->" << member[j]->getId() << ", \"" 
                << tmp << "\", strlen(\"" << tmp << "\"));" << endl;
            }
            continue;
        }
    }
    s << endl;

    s << TAB << "return JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;


    s << TAB << sStructName << " * " << sStructName << "_new()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "Int32 ret=0;" << endl;
    s << TAB << sStructName << " *this = (" << sStructName << " *)JceMalloc(sizeof("<< sStructName << "));" << endl;
    s << TAB << "if(!this) return NULL;" << endl;
    s << TAB << "ret = " << sStructName << "_init(this);" << endl;
    s << TAB << "if(JCE_SUCCESS != ret) { return NULL; }" << endl;
    s << TAB << "return this;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}


/*******************************ContainerPtr********************************/
string Jce2C::generateH(const ContainerPtr &pPtr) const
{
    ostringstream s;
    for (size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
    {
        s << generateH(pPtr->getAllNamespacePtr()[i]) << endl;
        s << endl;
    }
    return s.str();
}

/*
string Jce2C::generateCpp(const ContainerPtr &pPtr) const
{
    ostringstream s;
    for(size_t i = 0; i < pPtr->getAllNamespacePtr().size(); i++)
    {
        s << generateCpp(pPtr->getAllNamespacePtr()[i]) << endl;
        s << endl;
    }
    return s.str();
}
*/
/******************************ParamDeclPtr***************************************/
/*
string Jce2C::generateH(const ParamDeclPtr &pPtr) const
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

string Jce2C::generateOutH(const ParamDeclPtr &pPtr) const
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

string Jce2C::generateCpp(const ParamDeclPtr &pPtr) const
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
*/

/******************************OperationPtr***************************************/
/*
string Jce2C::generateCpp(const OperationPtr &pPtr, const string &cn) const
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

    s << TAB << "taf::JceOutputStream<taf::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        //if(vParamDecl[i]->isOut()) continue;
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "taf::ResponsePacket rep;" << endl;

    s << TAB << "std::map<string, string> status;" << endl;

    if (!routekey.empty())
    {
        ostringstream os; 
        
        os << routekey;

        s << TAB << "status.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;
    }

    s << TAB << "taf_invoke(taf::JCENORMAL,\"" << pPtr->getId() << "\", _os.getByteBuffer(), context, status, rep);" << endl;

    if(vParamDecl.size() > 0 || pPtr->getReturnPtr()->getTypePtr())
    {
        s << TAB << "taf::JceInputStream<taf::BufferReader> _is;" << endl;
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
*/
/*
string Jce2C::generateCppAsync(const OperationPtr &pPtr, const string &cn) const
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
    s << TAB << "taf::JceOutputStream<taf::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut())
        {
            continue;
        }
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "std::map<string, string> status;" << endl;

    if (!routekey.empty())
    {
        ostringstream os; 
        
        os << routekey;

        s << TAB << "status.insert(std::make_pair(ServantProxy::STATUS_GRID_KEY, " << os.str() << "));" << endl;
    }

    s << TAB << "taf_invoke_async(taf::JCENORMAL,\"" << pPtr->getId() << "\", _os.getByteBuffer(), context, status, callback);" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    return s.str();
}
///////////////////////////////////////////////////////////////////////////////////////////////
string Jce2C::generateDispatchAsyncCpp(const OperationPtr &pPtr, const string &cn) const
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

    s << TAB << "taf::JceInputStream<taf::BufferReader> _is;" << endl;
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
string Jce2C::generateHAsync(const OperationPtr &pPtr) const
{
    ostringstream s;
    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();
    s << TAB << "virtual void " << "callback_" << pPtr->getId() << "(";

    string sParams;
    if (pPtr->getReturnPtr()->getTypePtr())
    {
        if(pPtr->getReturnPtr()->getTypePtr()->isSimple())
        {
            sParams = tostr(pPtr->getReturnPtr()->getTypePtr()) + " ret, ";
        }
        else
        {
            //结构, map, vector, string
            sParams =  "const " + tostr(pPtr->getReturnPtr()->getTypePtr()) + "& ret, ";
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
                sParams += " const " + tostr(pPtr->getTypeIdPtr()->getTypePtr()) + "&";
            }
            sParams += " " + pPtr->getTypeIdPtr()->getId() + ", ";
        }
    }
    s << taf::TC_Common::trimright(sParams, ", ", false) << ")" << endl;

    s << TAB << "{ throw std::runtime_error(\"callback_" << pPtr->getId() << "() overloading incorrect.\"); }" << endl;
    s << TAB << "virtual void " << "callback_" << pPtr->getId() << "_exception(taf::Int32 ret)" << endl;
    s << TAB << "{ throw std::runtime_error(\"callback_" << pPtr->getId() << "_exception() overloading incorrect.\"); }";
    s << endl;

    return s.str();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
string Jce2C::generateDispatchCpp(const OperationPtr &pPtr, const string &cn) const
{
    ostringstream s;
    s << TAB << "taf::JceInputStream<taf::BufferReader> _is;" << endl;
    s << TAB << "_is.setBuffer(_current->getRequestBuffer());" << endl;

    vector<ParamDeclPtr>& vParamDecl = pPtr->getAllParamDeclPtr();

    //对输入输出参数编码
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        s << TAB << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) << " "
            << vParamDecl[i]->getTypeIdPtr()->getId() << ";" << endl;
    }

    //WUP的支持
    s << TAB << "if (_current->getRequestVersion() == WUPVERSION)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "UniAttribute<taf::BufferWriter, taf::BufferReader>  tafAttr;" << endl;
    s << TAB << "tafAttr.decode(_current->getRequestBuffer());" << endl;
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        string sParamName =  vParamDecl[i]->getTypeIdPtr()->getId();
        string sEnum2Int = (EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) ? "(taf::Int32)" : "";
        if(!vParamDecl[i]->isOut())
        {
            //枚举类型转成int
            if(EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr()))
            {
                s << TAB << sParamName << " = (" << tostr(vParamDecl[i]->getTypeIdPtr()->getTypePtr()) 
                    << ") tafAttr.get<taf::Int32>(\"" << sParamName << "\");" << endl;
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
                    << ") tafAttr.getByDefault<taf::Int32>(\"" << sParamName << "\", " << sEnum2Int << sParamName << ");" << endl;
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
    s << TAB << "if (_current->getRequestVersion() == WUPVERSION)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "UniAttribute<taf::BufferWriter, taf::BufferReader>  tafAttr;" << endl;
    if(pPtr->getReturnPtr()->getTypePtr())
    {
        string sEnum2Int = (EnumPtr::dynamicCast(pPtr->getReturnPtr()->getTypePtr())) ? "(taf::Int32)" : "";
        s << TAB << "tafAttr.put(\"\", " << sEnum2Int << "_ret);" << endl;
    }
    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        string sParamName = vParamDecl[i]->getTypeIdPtr()->getId();
        string sEnum2Int = (EnumPtr::dynamicCast(vParamDecl[i]->getTypeIdPtr()->getTypePtr())) ? "(taf::Int32)" : "";
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
    s << TAB << "taf::JceOutputStream<taf::BufferWriter> _os;" << endl;

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
string Jce2C::generateHAsync(const OperationPtr &pPtr, const string& interfaceId) const
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
*/
/////////////////////////////////////////////////////////////////////////////////////////////////
/*
string Jce2C::generateH(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const
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
*/
/******************************InterfacePtr***************************************/
/*
string Jce2C::generateH(const InterfacePtr &pPtr) const
{
    ostringstream s;
    vector<OperationPtr>& vOperation = pPtr->getAllOperationPtr();

    //生成异步回调Proxy
    s << TAB << " //callback of async proxy for client " << endl;
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

    //生成客户端代理
    s << TAB << "// proxy for client " << endl;
    s << TAB << "class " << pPtr->getId() << "Proxy : public taf::ServantProxy" << endl;
    s << TAB << "{" << endl;
    s << TAB << "public:" << endl;
    INC_TAB;
    s << TAB << "typedef map<string, string> TAF_CONTEXT;" << endl;

    for(size_t i = 0; i < vOperation.size(); i++)
    {
        s << generateH(vOperation[i], false, pPtr->getId());// << endl;
        s << generateHAsync(vOperation[i], pPtr->getId()) << endl;
    }
    s << TAB << pPtr->getId() << "Proxy* taf_hash(int64_t key);" << endl;

    DEL_TAB;
    s << TAB << "};" << endl;

    s << TAB << "typedef taf::TC_AutoPtr<" << pPtr->getId() << "Proxy> " << pPtr->getId() << "Prx;" << endl;
    s << endl;

    //生成服务端Servant
    s << TAB <<  "// servant for server /" << endl;
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

string Jce2C::generateAsyncResponseCpp(const OperationPtr &pPtr, const string &cn) const
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

    s << TAB << "taf::JceOutputStream<taf::BufferWriter> _os;" << endl;
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

    return s.str();
}

struct SortOperation
{
    bool operator()(const OperationPtr &o1, const OperationPtr &o2)
    {
        return o1->getId() < o2->getId();
    }
};

string Jce2C::generateCpp(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const
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
*/

/******************************EnumPtr***************************************/

string Jce2C::generateH(const EnumPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;
    s << TAB << "enum " << namespaceId << "_" << pPtr->getId() << endl;
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
        s << (i==member.size()-1 ? "" : ",") << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    return s.str();
}

/******************************ConstPtr***************************************/
string Jce2C::generateH(const ConstPtr &pPtr) const
{
    ostringstream s;

    if (pPtr->getConstTokPtr()->t == ConstTok::STRING)
    {
        string tmp = taf::TC_Common::replace(pPtr->getConstTokPtr()->v, "\"", "\\\"");
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = \"" << tmp << "\";"<< endl;
    }
    else
    {
        s << TAB << "const " << tostr(pPtr->getTypeIdPtr()->getTypePtr()) << " " << pPtr->getTypeIdPtr()->getId() << " = " << pPtr->getConstTokPtr()->v 
        << ((tostr(pPtr->getTypeIdPtr()->getTypePtr()) == "taf::Int64") ? "LL;" : ";" )<< endl;
    }

    return s.str();
}
/******************************NamespacePtr***************************************/

string Jce2C::generateH(const NamespacePtr &pPtr) const
{
    ostringstream s;
    //vector<InterfacePtr>    &is    = pPtr->getAllInterfacePtr();
    vector<StructPtr>        &ss    = pPtr->getAllStructPtr();
    vector<EnumPtr>            &es    = pPtr->getAllEnumPtr();
    //vector<ConstPtr>        &cs    = pPtr->getAllConstPtr();

    s << endl;
    //s << TAB << "namespace " << pPtr->getId() << endl;
    //s << TAB << "{" << endl;
    //INC_TAB;

    /*
    for(size_t i = 0; i < cs.size(); i++)
    {
        s << generateH(cs[i]) << endl;
    }
    */

    for (size_t i = 0; i < es.size(); i++)
    {
        s << generateH(es[i], pPtr->getId()) << endl;
    }

    for (size_t i = 0; i < ss.size(); i++)
    {
        s << generateH(ss[i], pPtr->getId()) << endl;
    }

    /*
    for(size_t i = 0; i < is.size(); i++)
    {
        s << generateH(is[i]) << endl;
        s << endl;
    }
    */

    //DEL_TAB;
    //s << "}";

    s << endl;

    //定义结构拷贝宏
    /*
    for(size_t i = 0; i < ss.size(); i++)
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
    */

    return s.str();
}


string Jce2C::generateC(const NamespacePtr &pPtr) const
{
    ostringstream s;
    vector<StructPtr>        &ss    = pPtr->getAllStructPtr();
    s << endl;
    for (size_t i = 0; i < ss.size(); i++)
    {
        s << generateC(ss[i], pPtr->getId()) << endl;
    }
    s << endl;
    return s.str();
}


/******************************Jce2C***************************************/

void Jce2C::generateH(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));

    string fileH    = m_sBaseDir + "/" + n + ".h";
    //string fileCpp  = n + ".cpp";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    s << "#ifndef " << define << endl;
    s << "#define " << define << endl;
    s << endl;
    //s << "#include <map>" << endl;
    //s << "#include <string>" << endl;
    //s << "#include <vector>" << endl;
    s << "#include \"Jce_c.h\"" << endl;
    //s << "using namespace std;" << endl;

    vector<string> include = pPtr->getIncludes();
    for (size_t i = 0; i < include.size(); i++)
    {
        s << "#include \"" << g_parse->getHeader() << taf::TC_File::extractFileName(include[i]) << "\"" << endl;
    }

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    //名字空间有接口
    /*
    for(size_t i = 0; i < namespaces.size(); i++)
    {
        if(namespaces[i]->hasInterface())
        {
            s << "#include \"servant/ServantProxy.h\"" << endl;
            s << "#include \"servant/Servant.h\"" << endl;
            break;
        }
    }
    */

    s << endl;

    for (size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateH(namespaces[i]) << endl;
    }

    s << endl;
    s << "#endif" << endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileH, s.str());
}

void Jce2C::generateC(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));
    string fileC    = m_sBaseDir + "/" + n + ".c";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    s << "#include \"" << g_parse->getHeader() << n << ".h\"" << endl;
    s << endl;
    //s << "using namespace wup;" << endl;

    for (size_t i = 0; i < namespaces.size(); i++)
    {
        s << generateC(namespaces[i]) << endl;
    }

    s << endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileC, s.str());

}


void Jce2C::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        if (file == contexts[i]->getFileName())
        {
            generateH(contexts[i]);
            generateC(contexts[i]);
        }
    }
}

StructPtr Jce2C::findStruct(const ContextPtr &pPtr,const string &id)
{
    string sid = id;

    //在当前namespace中查找
    vector<NamespacePtr> namespaces = pPtr->getNamespaces();
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        NamespacePtr np = namespaces[i];
        vector<StructPtr> structs = np->getAllStructPtr();

        for (size_t i = 0; i < structs.size(); i++)
        {
            if (structs[i]->getSid() == sid)
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
/*
string Jce2C::generateCoder(const NamespacePtr &pPtr,const string & sInterface) const
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

string Jce2C::generateCoder(const InterfacePtr &pPtr) const
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
    s << TAB << "static taf::Int32 fetchPacket(const string & in, string & out)" << endl;
    s << TAB << "{" << endl;

    INC_TAB;
    s << TAB << "if(in.length() < sizeof(taf::Int32)) return eJcePacketLess;" << endl;

    s << TAB << "taf::Int32 iHeaderLen;" << endl;
    s << TAB << "memcpy(&iHeaderLen, in.c_str(), sizeof(taf::Int32));" << endl;

    s << TAB << "iHeaderLen = ntohl(iHeaderLen);" << endl;
    s << TAB << "if(iHeaderLen < (taf::Int32)sizeof(taf::Int32) || iHeaderLen > 100000000) return eJcePacketErr;" << endl;
    s << TAB << "if((taf::Int32)in.length() < iHeaderLen) return eJcePacketLess;" << endl;

    s << TAB << "out = in.substr(sizeof(taf::Int32), iHeaderLen - sizeof(taf::Int32)); " << endl;
    s << TAB << "return 0;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "static string encodeBasePacket(const string & sServantName, const string & sFuncName, const vector<char> & buffer, "
        <<"const map<string, string>& context = TAF_CONTEXT())" <<endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "taf::JceOutputStream<taf::BufferWriter> os;" << endl;
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

    s << TAB << "taf::Int32 iHeaderLen;" << endl;
    s << TAB << "iHeaderLen = htonl(sizeof(taf::Int32) + os.getLength());" << endl;
    s << TAB << "string s;" << endl;
    s << TAB << "s.append((const char*)&iHeaderLen, sizeof(taf::Int32));" << endl;
    s << TAB << "s.append(os.getBuffer(), os.getLength());" << endl;

    s << TAB << "return s;" << endl;

    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;
    s << TAB << "static taf::Int32 decodeBasePacket(const string & in, taf::Int32 & iServerRet, vector<char> & buffer)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    s << TAB << "taf::JceInputStream<taf::BufferReader> is;" << endl;
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

string Jce2C::generateCoder(const OperationPtr &pPtr) const
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
    s << TAB << "taf::JceOutputStream<taf::BufferWriter> _os;" << endl;

    for(size_t i = 0; i < vParamDecl.size(); i++)
    {
        if(vParamDecl[i]->isOut()) continue;
        s << writeTo(vParamDecl[i]->getTypeIdPtr());
    }

    s << TAB << "return encodeBasePacket(sServantName, \""<< pPtr->getId() << "\", _os.getByteBuffer(), context);" <<endl;

    DEL_TAB;

    s << TAB << "}" << endl;
    s << TAB << "catch (taf::JceException & ex)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return \"\";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    s << endl;

    //解码函数

    s << TAB << "static taf::Int32 decode_" << pPtr->getId() << "(const string & in ";

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
    s << TAB << "taf::Int32 iRet = 0;" << endl;
    s << TAB << "if((iRet = fetchPacket(in, out)) != 0) return iRet;" << endl;

    s << TAB << "taf::JceInputStream<taf::BufferReader> _is;" << endl;
    s << TAB << "taf::Int32 iServerRet=0;" << endl;
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
    s << TAB << "catch (taf::JceException & ex)" << endl;
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

void Jce2C::generateCoder(const ContextPtr &pPtr,const string &sInterface) const
{
    cout<< "Interface:" << sInterface <<endl;
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName())) + "Coder";

    string fileH    = n + ".h";

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

    taf::TC_File::save2file(fileH, s.str());

    return;
}
*/
