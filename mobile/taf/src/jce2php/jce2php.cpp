#include "jce2php.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

//////////////////////////////////////////////////////////////////////////////////
//
Jce2Php::Jce2Php():m_bPDU(false), m_bCheckDefault(false)
{

}

string Jce2Php::writeTo(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    s << TAB << "$this->" << pPtr->getId() << "->write($_out," << pPtr->getTag() << ");" << endl;
    return s.str();
}
string Jce2Php::readFrom(const TypeIdPtr &pPtr, bool bIsRequire) const
{
    ostringstream s;
    s<<TAB<<"$this->"<<pPtr->getId()<<"->read($_in ";
    s << ", " << pPtr->getTag() << ", " << ((pPtr->isRequire() && bIsRequire)?"true":"false") << ");" << endl;
    return s.str();
}


/*******************************获取定长数组坐标********************************/
int Jce2Php::getSuffix(const TypeIdPtr &pPtr) const
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

string Jce2Php::toStrSuffix(const TypeIdPtr &pPtr) const
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

string Jce2Php::tostr(const TypePtr &pPtr) const
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

string Jce2Php::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch(pPtr->kind())
    {
    case Builtin::KindBool:
        s = "c_char";
        break;
    case Builtin::KindByte:
        s = (pPtr->isUnsigned()?"c_short":"c_char");
        break;
    case Builtin::KindShort:
        s = (pPtr->isUnsigned()?"c_short":"c_short");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?"c_int":"c_int");
        break;
    case Builtin::KindLong:
        s =  "c_int64";
        break;
    case Builtin::KindFloat:
        s = "c_float";
        break;
    case Builtin::KindDouble:
        s = "c_double";
        break;
    case Builtin::KindString:
        s = "c_string";
        break;
    case Builtin::KindVector:
        s = "c_vector";
        break;
    case Builtin::KindMap:
        s = "c_map";
        break;
    default:
        assert(false);
        break;
    }

    return s;
}
/*******************************VectorPtr********************************/
string Jce2Php::tostrVector(const VectorPtr &pPtr) const
{
    string s = string("c_vector (new ") + tostr(pPtr->getTypePtr()) + string(")");
    return s;
}
/*******************************MapPtr********************************/
string Jce2Php::tostrMap(const MapPtr &pPtr) const
{
    string s = string("c_map (new ") + tostr(pPtr->getLeftTypePtr()) + ",new " + tostr(pPtr->getRightTypePtr()) + string(")");
    return s;
}

/*******************************StructPtr********************************/
string Jce2Php::tostrStruct(const StructPtr &pPtr) const
{
    return pPtr->getId();
}


/////////////////////////////////////////////////////////////////////
string Jce2Php::tostrEnum(const EnumPtr &pPtr) const
{
    return "c_int";//pPtr->getId();
}
///////////////////////////////////////////////////////////////////////
string Jce2Php::generatePHP(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;

	vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    s << TAB << "class " << pPtr->getId() << " extends c_struct" << endl;
    s << TAB << "{" << endl;
    INC_TAB;

    //变量声明
    for(size_t k = 0;k < member.size();k++)
    {
        s<< TAB << "public $" <<member[k]->getId()<<";"<<endl;
    }
    s<<endl;
    s << TAB << "public function __clone()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for(size_t k = 0;k < member.size();k++)
    {
        s<< TAB << "$this->" <<member[k]->getId()<<" = clone $this->"<<member[k]->getId()<<";"<<endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s <<endl;

    s << TAB << "public function __construct()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    for(size_t k = 0;k < member.size();k++)
    {
        s<< TAB << "$this->" <<member[k]->getId()<<" = new  "<< tostr(member[k]->getTypePtr())<<";"<<endl;
    }
    DEL_TAB;
    s << TAB << "}" << endl;
    s <<endl;
    ////////////////////////////////////////////////////////////
    s << TAB << "public function get_class_name()" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return " << "\"" << namespaceId << "." << pPtr->getId() << "\"" << ";" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s<<endl;

	//write begin
	if(m_bPDU)
	{
	    s << TAB << "public function writeTo(&$_out,$tag)" << endl;
	    s << TAB << "{" << endl;
	    INC_TAB;
	    //s << TAB << "jce_header::_pack_header($_out,'c_struct_begin',$tag);" << endl;
	}
	else
	{
		s << TAB << "public function write(&$_out,$tag)" << endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "jce_header::_pack_header($_out,'c_struct_begin',$tag);" << endl;
	}

    for(size_t j = 0; j < member.size(); j++)
    {
        s << writeTo(member[j]);
    }

    if(m_bPDU)
    {
        //s << TAB << "jce_header::_pack_header($_out,'c_struct_end',0);" << endl;
    }
	else
	{
		 s << TAB << "jce_header::_pack_header($_out,'c_struct_end',0);" << endl;
	}

    DEL_TAB;
    s << TAB << "}" << endl;
	//write end


	///read begin
	if(m_bPDU)
	{
		s << TAB << "public function readFrom(&$_in,$tag,$isRequire = true)" << endl;
		s << TAB << "{" << endl;

		INC_TAB;
#if 0

		s << TAB << "jce_header::_peek_header($_in,$type,$this_tag);"<<endl;

		s << TAB << "if($tag != $this_tag)" <<endl;
		s << TAB << "{" << endl;
		INC_TAB;
		s << TAB << "if($isRequire == true){"<<endl;
		s << TAB << "throw new JceException(__LINE__ ,TAG_NOT_MATCH);"<<endl;
		s << TAB << "}else{" <<endl;
		INC_TAB;
		s << TAB << "return;"<<endl;
		DEL_TAB;
		s << TAB << "}" <<endl;
		DEL_TAB;
		s << TAB << "}" <<endl;

		s << TAB << "if($type != 'c_struct_begin')"<<endl;
		INC_TAB;
		s << TAB << "throw new JceException(__LINE__ ,TYPE_NOT_MATCH);"<<endl;
		DEL_TAB;

		s<< TAB << "jce_header::_unpack_header($_in,$type,$this_tag);"<<endl;
#endif
	}
	else
	{
		s << TAB << "public function read(&$_in,$tag,$isRequire = true)" << endl;
		s << TAB << "{" << endl;

		INC_TAB;

		s << TAB << "jce_header::_check_struct($_in,$type,$tag,$isRequire);"<<endl;
		s << TAB << "jce_header::_unpack_header($_in,$type,$this_tag);"<<endl;

	}
    for(size_t j = 0; j < member.size(); j++)
    {
        s << readFrom(member[j]);
    }

	if(!m_bPDU)
	{
    	s<< TAB << "$this->_skip_struct($_in);" << endl;
	}

    DEL_TAB;
    s << TAB << "}" << endl;
	//read end

    DEL_TAB;
    s << TAB << "}" << endl;
    return s.str();
}


/******************************NamespacePtr***************************************/
string Jce2Php::generatePHP(const NamespacePtr &pPtr) const
{
    ostringstream s;

    vector<StructPtr> &ss = pPtr->getAllStructPtr();

    for(size_t i = 0; i < ss.size(); i++)
    {
        s << generatePHP(ss[i], pPtr->getId()) << endl;
    }

    return s.str();
}

void Jce2Php::generatePHP(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));
    string fileH    = m_sBaseDir + "/" + n + "_wup.php";

    string define = "<?php";

    ostringstream s;
    s << define << endl;
    s << endl;
    s << g_parse->printHeaderRemark();
    s << endl;

    s << "require_once('jce.php');" <<endl;
	s <<endl;

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generatePHP(namespaces[i]) << endl;
    }

    s << "?>"<<endl;

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileH, s.str());
}

void Jce2Php::generatePHP_Pdu(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName()));
    string fileH    = m_sBaseDir + "/" + n + "_pdu.php";

    string define = "<?php";

    ostringstream s;
    s << define << endl;
    s << endl;
    s << g_parse->printHeaderRemark();
    s << endl;
    s << "require_once('jce.php');" <<endl;

    vector<NamespacePtr> namespaces = pPtr->getNamespaces();

    for(size_t i = 0; i < namespaces.size(); i++)
    {
        s << generatePHP(namespaces[i]) << endl;
    }
    s << "?>";

    taf::TC_File::makeDirRecursive(m_sBaseDir, 0755);
    taf::TC_File::save2file(fileH, s.str());
}

void Jce2Php::createFile(const string &file, const vector<string> &vsCoder)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for(size_t i = 0; i < contexts.size(); i++)
    {
        if(file == contexts[i]->getFileName())
        {
            if(m_bPDU) //pdu版本的
            {
                generatePHP_Pdu(contexts[i]);
            }
			else //wup版本的
			{
                generatePHP(contexts[i]);
            }
        }
    }
}



