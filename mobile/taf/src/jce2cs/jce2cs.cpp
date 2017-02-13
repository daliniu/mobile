#include "jce2cs.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>
#include <string.h>

#define JCE_PACKAGE     "Wup.Jce"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
string Jce2Cs::toTypeInit(const TypePtr &pPtr) const
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
            case Builtin::KindLong:     return "0L;";
            case Builtin::KindFloat:    return "0.0f;";
            case Builtin::KindDouble:   return "0.0;";
            case Builtin::KindString:   return "\"\";";
            default:                    return "";
        }
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr)
    {
        //数组特殊处理
        string sType;
        size_t iPosBegin, iPosEnd;
        sType = tostr(vPtr->getTypePtr());
        if ((iPosBegin = sType.find("<")) != string::npos && (iPosEnd = sType.rfind(">")) != string::npos)
        {
            sType = sType.substr(0, iPosBegin) +  sType.substr(iPosEnd+1);
        }
        //[] (数组)的数组变为[1]
        sType = taf::TC_Common::replace(sType, "[]" , "[1]");
        return "(" + tostr(vPtr->getTypePtr()) + "[]) new " + sType + "[1];";;
    }

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if (mPtr) return "new " + tostrMap(mPtr, true) + "();";

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return "new " + tostrStruct(sPtr) + "();";

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return "0;";

    return "";
}

string Jce2Cs::toObjStr(const TypePtr &pPtr) const
{
    string sType = tostr(pPtr);

    if (sType == "bool") return "bool";
    if (sType == "byte")    return "byte";
    if (sType == "short" )  return "short";
    if (sType == "int" )    return "int";
    if (sType == "long" )   return "long";
    if (sType == "float" )  return "float";
    if (sType == "double" ) return "double";

    return sType;
}

string Jce2Cs::tostr(const TypePtr &pPtr) const
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

/*******************************BuiltinPtr********************************/
string Jce2Cs::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch (pPtr->kind())
    {
        case Builtin::KindBool:     s = "bool";  break;
        case Builtin::KindByte:     s = "byte";     break;
        case Builtin::KindShort:    s = "short";    break;
        case Builtin::KindInt:      s = "int";      break;
        case Builtin::KindLong:     s = "long";     break;
        case Builtin::KindFloat:    s = "float";    break;
        case Builtin::KindDouble:   s = "double";   break;
        case Builtin::KindString:   s = "string";   break;
        case Builtin::KindVector:   s = "System.Collections.Generic.List";  break;
        case Builtin::KindMap:      s = "System.Collections.Generic.Dictionary";    break;
        default:                    assert(false);  break;
    }

    return s;
}

string Jce2Cs::tostrVector(const VectorPtr &pPtr) const
{
    string s = "System.Collections.Generic.List" + string("<") + toObjStr(pPtr->getTypePtr()) + ">";
    return s;
}

string Jce2Cs::tostrMap(const MapPtr &pPtr, bool bNew) const
{
    string s;
    if (!bNew)
    {
        s = "System.Collections.Generic.Dictionary";
    }
    else
    {
        s = "System.Collections.Generic.Dictionary";
    }
    s += string("<") + toObjStr(pPtr->getLeftTypePtr()) + ", " + toObjStr(pPtr->getRightTypePtr()) + ">";

    return s;
}

string Jce2Cs::tostrStruct(const StructPtr &pPtr) const
{
    return taf::TC_Common::replace(pPtr->getSid(), "::", ".").substr(taf::TC_Common::replace(pPtr->getSid(), "::", ".").find(".")+1);
}

string Jce2Cs::tostrEnum(const EnumPtr &pPtr) const
{
    return "int";
}

/******************************StructPtr***************************************/
string Jce2Cs::generateCs(const StructPtr &pPtr, const NamespacePtr &nPtr) const
{
    ostringstream s;
    s << g_parse->printHeaderRemark();

    vector<string> key = pPtr->getKey();
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    s << TAB << "using System;"<<endl;
    s << TAB << "namespace "<< nPtr->getId()<< endl;
    s << "{"<<endl;
    INC_TAB;
    s << endl;

    s << TAB << "public sealed class " << pPtr->getId() << " : "JCE_PACKAGE".JceStruct"<< endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //定义成员变量set;get函数
    for (size_t i = 0; i < member.size(); i++)
    {
        string sDefault;
        if (member[i]->hasDefault())
        {
            BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[i]->getTypePtr());

            if (bPtr && bPtr->kind() == Builtin::KindString)
            {
                sDefault = taf::TC_Common::replace(member[i]->def(), "\"", "\\\"");
                sDefault = " = \"" + sDefault + "\"";
            }
            else
            {
                sDefault = " = " + member[i]->def();
            }
            
            s << TAB << tostr(member[i]->getTypePtr()) << " _" << member[i]->getId() << sDefault << ";" << endl;
            s << TAB << "public " << tostr(member[i]->getTypePtr()) << " "  << member[i]->getId() << endl;
            s << TAB << "{" << endl;

            INC_TAB;
            s << TAB << "get" << endl;
            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << " return _" << member[i]->getId() << ";" << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
            s << TAB << "set" << endl;
            s << TAB << "{" << endl;
            INC_TAB;
            s << TAB << "_" << member[i]->getId() << " = value; " << endl;
            DEL_TAB;
            s << TAB << "}" << endl;
            DEL_TAB;
            
            s << TAB << "}" << endl;
        }
        else
        {
            sDefault = " {get; set;} ";
            s << TAB << "public " << tostr(member[i]->getTypePtr()) << " " << member[i]->getId() << sDefault << endl;
        }

        s << endl;
    }

    //writeTo()
    s << TAB << "public override void WriteTo("JCE_PACKAGE".JceOutputStream _os)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "_os.Write(" << member[i]->getId()
        << ", " << member[i]->getTag() << ");" << endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    //readFrom()
    s << TAB << "public override void ReadFrom("JCE_PACKAGE".JceInputStream _is)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << member[i]->getId() << " = (" + tostr(member[i]->getTypePtr()) + ")"
            << " _is.Read(" << member[i]->getId() << ", " << member[i]->getTag() << ", " 
            << (member[i]->isRequire() ? "true" : "false") << ");" << endl << endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    //display()
    s << TAB << "public override void Display(System.Text.StringBuilder _os, int _level)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << JCE_PACKAGE".JceDisplayer _ds = new "JCE_PACKAGE".JceDisplayer(_os, _level);" << endl;
    for (size_t i = 0; i < member.size(); i++)
    {
        s << TAB << "_ds.Display(" << member[i]->getId() << ", \"" << member[i]->getId() << "\");" << endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s << endl;

    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB; 
    s << TAB << "}" << endl;
    s << endl;

    string fileCs  = getFilePath(nPtr->getId()) + pPtr->getId() + ".cs";
    taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
    taf::TC_File::save2file(fileCs, s.str());

    return s.str();
}

/******************************ConstPtr***************************************/
void Jce2Cs::generateCs(const vector<EnumPtr> &es,const vector<ConstPtr> &cs,const NamespacePtr &nPtr) const
{
    if (es.size()==0 && cs.size()==0)
    {
        return;
    }
    ostringstream s;
    s << g_parse->printHeaderRemark();

    s << TAB << "using System;"<<endl;
    //s << TAB << "namespace " << _packagePrefix << nPtr->getId() << endl;
    s << TAB << "namespace " << nPtr->getId()<< endl;
    s << "{"<<endl;
    INC_TAB;

    if (cs.size()>0)
    {
        s << TAB << "class Const " << endl;
        s << TAB << "{" << endl;
        INC_TAB;
        //-----------------const类型开始------------------------------------
        for (size_t i = 0; i < cs.size(); i++)
        {
            if (cs[i]->getConstTokPtr()->t == ConstTok::STRING)
            {
                string tmp = taf::TC_Common::replace(cs[i]->getConstTokPtr()->v, "\"", "\\\"");
                s  << TAB << "public const "<< tostr(cs[i]->getTypeIdPtr()->getTypePtr()) << " "<<cs[i]->getTypeIdPtr()->getId()<< " = \""
                << tmp << "\";"<< endl;
            }
            else if (tostr(cs[i]->getTypeIdPtr()->getTypePtr()) == "long")
            {
                s  << TAB << "public const "<< tostr(cs[i]->getTypeIdPtr()->getTypePtr()) << " "<<cs[i]->getTypeIdPtr()->getId()<< " = "
                << cs[i]->getConstTokPtr()->v << "L;" << endl;
            }
            else
            {
                s  << TAB << "public const "<< tostr(cs[i]->getTypeIdPtr()->getTypePtr()) <<" "<<cs[i]->getTypeIdPtr()->getId()<< " = "
                << cs[i]->getConstTokPtr()->v << ";" << endl;
            }
        }
        DEL_TAB;
        s << TAB << "}" << endl;
    }
    //-----------------const类型结束--------------------------------
    if (es.size()>0)
    {
        //-----------------枚举类型开始---------------------------------
        for (size_t i = 0; i < es.size(); i++)
        {
            s << TAB << "public enum "<<es[i]->getId()<<endl;
            s<< TAB <<"{"<<endl;
            INC_TAB;
            vector<TypeIdPtr>& member = es[i]->getAllMemberPtr();
            for (size_t i = 0; i < member.size(); i++)
            {
                s << TAB << member[i]->getId();
				if(member[i]->hasDefault())
				{
					s << " = " << member[i]->def();
				}

				s <<","<<endl;
            }
            DEL_TAB;
            s<< TAB <<"}"<<endl;
        }
    }
    //-----------------枚举类型结束---------------------------------
    DEL_TAB;
    s << TAB << "}" << endl;

    string fileCs  = getFilePath(nPtr->getId()) + nPtr->getId()+"_const.cs";
    taf::TC_File::makeDirRecursive(getFilePath(nPtr->getId()), 0755);
    taf::TC_File::save2file(fileCs, s.str());

    return;
}

/******************************NamespacePtr***************************************/
void Jce2Cs::generateCs(const NamespacePtr &pPtr) const
{
    vector<StructPtr>       &ss    = pPtr->getAllStructPtr();
    vector<EnumPtr>         &es    = pPtr->getAllEnumPtr();
    vector<ConstPtr>        &cs    = pPtr->getAllConstPtr();

    for (size_t i = 0; i < ss.size(); i++)
    {
        generateCs(ss[i], pPtr);
    }

    generateCs(es,cs,pPtr);//c#里面的枚举、const都放到一起。

    return;
}

void Jce2Cs::generateCs(const ContextPtr &pPtr) const
{
    ostringstream s;

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    for (size_t i = 0; i < namespaces.size(); i++)
    {
        generateCs(namespaces[i]);
    }

}

void Jce2Cs::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for (size_t i = 0; i < contexts.size(); i++)
    {
        if (file == contexts[i]->getFileName())
        {
            generateCs(contexts[i]);
        }
    }
}

void Jce2Cs::setBaseDir(const string &dir)
{
    _baseDir = dir;
}

void Jce2Cs::setBasePackage(const string &prefix)
{
    _packagePrefix = prefix;
    if (_packagePrefix.length() != 0 && _packagePrefix.substr(_packagePrefix.length()-1, 1) != ".")
    {
        _packagePrefix += ".";
    }
}

string Jce2Cs::getFilePath(const string &ns) const
{
    return _baseDir + "/" + taf::TC_Common::replace(_packagePrefix, ".", "/") + "/" + ns + "/";
}

