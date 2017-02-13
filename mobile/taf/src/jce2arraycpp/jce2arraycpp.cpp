#include "jce2arraycpp.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <string>

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

string jce2arraycpp::dumpInfo(const TypeIdPtr &pPtr) const
{
		ostringstream s;
    if(!pPtr)
    {
        assert(false);
        return s.str();
    }
	
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if(bPtr){
        s << TAB << "PRINTF_INFO(\"| " << pPtr->getId() << " = %d\\n\", (int)" << pPtr->getId() << ");" << endl;
        return s.str();
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if(vPtr){
        if(!vPtr->isArray()){
            cerr << "vecotr no size!" << endl;
            assert(false);
            return s.str();
        }

        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if(vbPtr){
        		s << TAB << "PRINTF_INFO(\"| " << pPtr->getId() << "_Size = %d\\n\", (int)" << pPtr->getId() << "_Size);" << endl;
        		s << TAB << "PRINTF_INFO(\"-------------------\\n\");" << endl;
        		string name = pPtr->getId();
        		if(name[0] == 's' && name[1] == 't' && name[2] == 'r') //以str开头的数组为字符串
        		{
        			  s << TAB << "    " << "PRINTF_INFO(\"| " << pPtr->getId() << " = %s\", " << pPtr->getId() << ");" << endl;
        		}
        		else
        		{
        	      s << TAB << "for(i=0; i<" << pPtr->getId() << "_Size; i++) {" << endl;
                s << TAB << "    " << "PRINTF_INFO(\"| " << pPtr->getId() << "[%d] = %d\", i, (int)" << pPtr->getId() << "[i]);" << endl;
                s << TAB << "}" << endl;
             }
            s << TAB << "PRINTF_INFO(\"\\n-------------------\\n\");" << endl;
            return s.str();
        }

        StructPtr vsPtr = StructPtr::dynamicCast(vPtr->getTypePtr());
        if(vsPtr){
            s << TAB << "PRINTF_INFO(\"| " << pPtr->getId() << "_Size = %d\\n\", (int)" << pPtr->getId() << "_Size);" << endl;
        		s << TAB << "PRINTF_INFO(\"-------------------\\n\");" << endl;
        	  s << TAB << "for(i=0; i<" << pPtr->getId() << "_Size; i++) {" << endl;
            s << TAB << "    " << pPtr->getId() << "[i].dumpInfo();" << endl;
            s << TAB << "}" << endl;
            s << TAB << "PRINTF_INFO(\"-------------------\\n\");" << endl;
            return s.str();
        }

        cerr << "vector unkonw type!" << endl;
        assert(false);
        return s.str();
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if(sPtr){
        s << TAB << "PRINTF_INFO(\"-------------------\\n\");" << endl;
        s << TAB << pPtr->getId() << ".dumpInfo();" << endl;
        s << TAB << "PRINTF_INFO(\"-------------------\\n\");" << endl;
        return s.str();
    }

	return s.str();
}

//////////////////////////////////////////////////////////////////////////////////
//
string jce2arraycpp::writeTo(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "write") << "(bw, (jceCode::Int32)" << pPtr->getId();
    }
    else if (toSuffix(pPtr).size()>0)
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "write") << "(bw, " << pPtr->getId() << ", " << pPtr->getId() <<"_Size";
    }
    else
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "write") << "(bw, " << pPtr->getId();
    }

    s << ", " << pPtr->getTag() << ");" << endl;

    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return ret;" << endl;

    return s.str();
}

string jce2arraycpp::readFrom(const TypeIdPtr &pPtr, bool bIsRequire) const
{
    ostringstream s;
    if(EnumPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "read") << "(br, (jceCode::Int32)" << pPtr->getId();
    }
    else if (toSuffix(pPtr).size()>0)
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "read") << "(br, " << pPtr->getId() << ", " << pPtr->getTypePtr()->getSize() << ", " << pPtr->getId() <<"_Size";
    }
    else
    {
        s << TAB << "ret = " << toFunctionName(pPtr, "read") << "(br, " << pPtr->getId();
    }

    s << ", " << pPtr->getTag() << ");" << endl;

    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return ret;" << endl;

    return s.str();
}

string jce2arraycpp::toFunctionName(const TypeIdPtr &pPtr,string action) const
{
    ostringstream s;
    if(!pPtr)
    {
        assert(false);
        return s.str();
    }

    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if(bPtr){
        s << "jceCode::BuiltinType::" << action;
        if(bPtr->kind() == Builtin::KindString ){
            s << "String";
            if(!bPtr->isArray()){
                cerr << "string no size!" << endl;
                assert(false);
                return s.str();
            }
        }
        else if (bPtr->isArray()){
            cerr << "builtin type can't have size!" << endl;
            assert(false);
            return s.str();
        }
        return s.str();
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if(vPtr){
        if(!vPtr->isArray()){
            cerr << "vecotr no size!" << endl;
            assert(false);
            return s.str();
        }

        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if(vbPtr){
            s << "jceCode::BuiltinType::" << action << "Vector";
            //vector 里面 不能套string 也不能再套数组了
            if(vbPtr->kind() == Builtin::KindString ){
                cerr << "vector string error!" << endl;
                assert(false);
                return s.str();
            }
            if(vbPtr->isArray()){
                cerr << "vector array error!" << endl;;
                assert(false);
                return s.str();
            }
            if(vbPtr->kind() == Builtin::KindByte )
                s << "Char";

            return s.str();
        }

        StructPtr vsPtr = StructPtr::dynamicCast(vPtr->getTypePtr());
        if(vsPtr){
            s << vsPtr->getSid() << "::" << action << "VectorStruct";
            return s.str();
        }

        cerr << "vector unkonw type!" << endl;
        assert(false);
        return s.str();
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if(sPtr){
        s << sPtr->getSid() << "::" << action << "Struct";
        return s.str();
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if(ePtr) {
        s << "jceCode::BuiltinType::" << action;
        return s.str();
    }

    cerr << "unknow type!" << endl;
    assert(false);
    return s.str();
}

string jce2arraycpp::toTypeName(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(!pPtr)
    {
        assert(false);
        return s.str();
    }

    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if(bPtr){
        if(bPtr->kind() == Builtin::KindString ){
            if(!bPtr->isArray()){
                cerr << "string no size!" << endl;
                assert(false);
                return s.str();
            }
        }
        else if (bPtr->isArray()){
            cerr << "builtin type can't have size!" << endl;
            assert(false);
            return s.str();
        }
        return tostrBuiltin(bPtr);
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if(vPtr){
        if(!vPtr->isArray()){
            cerr << "vecotr no size!" << endl;
            assert(false);
            return s.str();
        }

        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if(vbPtr){
            //vector 里面 不能套string 也不能再套数组了
            if(vbPtr->kind() == Builtin::KindString ){
                cerr << "vector string error!" << endl;
                assert(false);
                return s.str();
            }
            if(vbPtr->isArray()){
                cerr << "vector array error!" << endl;;
                assert(false);
                return s.str();
            }
            return tostrBuiltin(vbPtr);
        }

        StructPtr vsPtr = StructPtr::dynamicCast(vPtr->getTypePtr());
        if(vsPtr)
            return vsPtr->getSid();

        cerr << "vector unkonw type!" << endl;
        assert(false);
        return s.str();
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if(sPtr){
        return  sPtr->getSid();
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if(ePtr) {
        return  sPtr->getSid();
    }

    cerr << "unknow type!" << endl;
    assert(false);
    return s.str();
}

string jce2arraycpp::toSuffix(const TypeIdPtr &pPtr) const
{
    ostringstream s;
    if(!pPtr)
    {
        assert(false);
        return s.str();
    }

    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr->getTypePtr());
    if(bPtr){
        if(bPtr->kind() == Builtin::KindString ){
            if(!bPtr->isArray()){
                cerr << "string no size!" << endl;
                assert(false);
                return s.str();
            }
            s << "[" << bPtr->getSize() << "]";
        }
        else if (bPtr->isArray()){
            cerr << "builtin type can't have size!" << endl;
            assert(false);
            return s.str();
        }
        return s.str();
    }

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr->getTypePtr());
    if(vPtr){
        if(!vPtr->isArray()){
            cerr << "vecotr no size!" << endl;
            assert(false);
            return s.str();
        }

        s << "[" << vPtr->getSize() << "]";

        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
        if(vbPtr){
            //vector 里面 不能套string 也不能再套数组了
            if(vbPtr->kind() == Builtin::KindString ){
                cerr << "vector string error!" << endl;
                assert(false);
                return s.str();
            }
            if(vbPtr->isArray()){
                cerr << "vector array error!" << endl;;
                assert(false);
                return s.str();
            }
            return s.str();
        }

        StructPtr vsPtr = StructPtr::dynamicCast(vPtr->getTypePtr());
        if(vsPtr)
            return s.str();

        cerr << "vector unkonw type!" << endl;
        assert(false);
        return s.str();
    }

    StructPtr sPtr = StructPtr::dynamicCast(pPtr->getTypePtr());
    if(sPtr){
        return s.str();
    }

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr->getTypePtr());
    if(ePtr) {
        return s.str();
    }

    cerr << "unknow type!" << endl;
    assert(false);
    return s.str();
}

/*******************************BuiltinPtr********************************/
string jce2arraycpp::tostr(const TypePtr &pPtr) const
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if(bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if(vPtr) return tostrVector(vPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if(sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if(ePtr) return tostrEnum(ePtr);

    if(!pPtr) return "void";

    assert(false);
    return "";
}

string jce2arraycpp::tostrBuiltin(const BuiltinPtr &pPtr) const
{
    string s;

    switch(pPtr->kind())
    {
    case Builtin::KindBool:
        s = "jceCode::Bool";
        break;
    case Builtin::KindByte:
        s = "jceCode::Char";
        break;
    case Builtin::KindShort:
        s = "jceCode::Short";
        break;
    case Builtin::KindInt:
        s = "jceCode::Int32";
        break;
    case Builtin::KindLong:
        s = "jceCode::Int64";
        break;
    case Builtin::KindFloat:
        s = "jceCode::Float";
        break;
    case Builtin::KindDouble:
        s = "jceCode::Double";
        break;
    case Builtin::KindString:
        s = "jceCode::Char";
        break;
    default:
        assert(false);
        break;
    }

    return s;
}

/*******************************VectorPtr********************************/
string jce2arraycpp::tostrVector(const VectorPtr &pPtr) const
{
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

/*******************************StructPtr********************************/
string jce2arraycpp::tostrStruct(const StructPtr &pPtr) const
{
    return pPtr->getSid();
}

/////////////////////////////////////////////////////////////////////
string jce2arraycpp::tostrEnum(const EnumPtr &pPtr) const
{
    return pPtr->getSid();
}

///////////////////////////////////////////////////////////////////////
string jce2arraycpp::generateH(const StructPtr &pPtr, const string& namespaceId) const
{
    ostringstream s;

    s << TAB << "class " << pPtr->getId() << " : public jceCode::JceClassBase" << endl;
    s << TAB << "{" << endl;
    s << TAB << "public:" << endl;

    INC_TAB;

    ////////////////////////////////////////////////////////////
    //定义缺省构造函数
    s << TAB << pPtr->getId() << "()" << endl;

    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();
    bool b = false;
    for(size_t j = 0; j < member.size(); j++)
    {
        BuiltinPtr bPtr  = BuiltinPtr::dynamicCast(member[j]->getTypePtr());
        //string值要转义
        if(bPtr && bPtr->kind() == Builtin::KindString)
        {
            continue;
        }

        if(member[j]->hasDefault())
        {
            if(!b)
            {
                s << TAB << ": ";
            }
            else
            {
                s << ",";
            }
            s << member[j]->getId() << "(" << member[j]->def() << ")";
            b = true;
        }

        string suffix;
        suffix = toSuffix(member[j]);

        if(!suffix.empty())
        {
            if(!b)
            {
                s << TAB << ":";
            }
            else
            {
                s << ",";
            }
            s << member[j]->getId() << "_Size" << "(" << taf::TC_Common::strto<int>(suffix.substr(1)) << ")" ;
            b = true;
        }
    }
    if(b)
    {
        s << endl;
    }

    s << TAB << "{" << endl;
    
    // wese 在构造函数里面对基本数组做初始化操作
    for(size_t j = 0; j < member.size(); j++)
    {
        VectorPtr vPtr = VectorPtr::dynamicCast(member[j]->getTypePtr());
		    if(vPtr)
		    {
		        if(!vPtr->isArray()){
		            cerr << "vecotr no size!" << endl;
		            assert(false);
		            return s.str();
		        }
		
		        BuiltinPtr vbPtr = BuiltinPtr::dynamicCast(vPtr->getTypePtr());
		        	
		        if(vbPtr)
		        {
		            s << TAB << "    memset(" << member[j]->getId() <<", 0, " << "sizeof(" << member[j]->getId() << "));" << endl;
		        }
		    }
    }
    
    s << TAB << "}" << endl;
    
    //wese 输出结构体内容，直接打印
    ////////////////////////////////////////////////////////////
    s << TAB << "void dumpInfo() const" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 i = 0;" << endl;
    for(size_t j = 0; j < member.size(); j++)
    {
        s << dumpInfo(member[j]);
    }
    DEL_TAB;
    s << TAB << "}" << endl;

    ////////////////////////////////////////////////////////////
    s << TAB << "jceCode::Int32 writeTo(jceCode::BufferWriter& bw) const" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    for(size_t j = 0; j < member.size(); j++)
    {
        s << writeTo(member[j]);
    }
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ///////////////////////////////////////////////////////////
    s << TAB << "jceCode::Int32 readFrom(jceCode::BufferReader& br)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    for(size_t j = 0; j < member.size(); j++)
    {
        s << readFrom(member[j]);
    }
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ////////////////////////////////////////////////////////////
    s << TAB << "static jceCode::Int32 writeStruct(jceCode::BufferWriter& bw, const " << pPtr->getId() << "& t, jceCode::UInt8 tag)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    s << TAB << "ret = jceCode::DataHead::writeTo(bw, jceCode::DataHead::eStructBegin, tag);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "ret = t.writeTo(bw);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "ret = jceCode::DataHead::writeTo(bw, jceCode::DataHead::eStructEnd, 0);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ///////////////////////////////////////////////////////////
    s << TAB << "static jceCode::Int32 readStruct(jceCode::BufferReader& br, " << pPtr->getId() << "& t, jceCode::UInt8 tag, bool isRequire = true)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    s << TAB << "if(jceCode::BuiltinType::skipToTag(br,tag)){;" << endl;
    INC_TAB;
    s << TAB << "jceCode::DataHead h;" << endl;
    s << TAB << "ret = h.readFrom(br);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "if(h.getType() != jceCode::DataHead::eStructBegin){" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "ret = t.readFrom(br);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "ret = jceCode::BuiltinType::skipToStructEnd(br);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    DEL_TAB;
    s << TAB << "}else if(isRequire){" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ////////////////////////////////////////////////////////////
    s << TAB << "static jceCode::Int32 writeVectorStruct(jceCode::BufferWriter& bw, const " << pPtr->getId() << " v[], jceCode::Int32 len, jceCode::UInt8 tag)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    s << TAB << "ret = jceCode::DataHead::writeTo(bw, jceCode::DataHead::eList, tag);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "ret = jceCode::BuiltinType::write(bw, len, 0);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    s << TAB << "for(jceCode::Int32 i = 0;i<len;i++){" << endl;
    INC_TAB;
    s << TAB << "ret = writeStruct(bw, v[i], 0);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return false;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    ////////////////////////////////////////////////////////////
    s << TAB << "static jceCode::Int32 readVectorStruct(jceCode::BufferReader& br, " << pPtr->getId() << " v[], jceCode::Int32 buffSize, jceCode::Int32& readLen, jceCode::UInt8 tag, bool isRequire = true)" << endl;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "jceCode::Int32 ret;" << endl;
    s << TAB << "if(jceCode::BuiltinType::skipToTag(br,tag)){" << endl;
    INC_TAB;
    s << TAB << "jceCode::DataHead h;" << endl;
    s << TAB << "ret = h.readFrom(br);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return ret;" << endl;
    s << TAB << "switch(h.getType()){" << endl;
    s << TAB << "case jceCode::DataHead::eList:" << endl;
    INC_TAB;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "ret = jceCode::BuiltinType::read(br, readLen, 0);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return ret;" << endl;
    s << TAB << "if(readLen < 0){" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "if(readLen > buffSize){" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "for(jceCode::Int32 i = 0; i < readLen; ++i){" << endl;
    INC_TAB;
    s << TAB << "ret = readStruct(br, v[i], 0);" << endl;
    s << TAB << "if(jceCode::JCE_SUCCESS != ret) return ret;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "break;" << endl;
    DEL_TAB;
    s << TAB << "default:" << endl;
    INC_TAB;
    s << TAB << "{" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    DEL_TAB;
    s << TAB << "}else if(isRequire){" << endl;
    INC_TAB;
    s << TAB << "return jceCode::JCE_CODE_ERROR;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;
    s << TAB << "return jceCode::JCE_SUCCESS;" << endl;
    DEL_TAB;
    s << TAB << "}" << endl;

    DEL_TAB;
    s << TAB << "public:" << endl;
    INC_TAB;

    //定义成员变量
    for(size_t j = 0; j < member.size(); j++)
    {
        if(toSuffix(member[j]).size()>0){
            s<<endl;
            s << TAB << "jceCode::Int32 " << member[j]->getId() << "_Size" << ";" << endl;
        }
        s << TAB << toTypeName(member[j]) << " " << member[j]->getId() << toSuffix(member[j]) << ";" << endl;
    }
    DEL_TAB;
    s << TAB << "};" << endl;

    return s.str();
}

/******************************EnumPtr***************************************/
string jce2arraycpp::generateH(const EnumPtr &pPtr) const
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
    s << TAB << "return \"\";" << endl;
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
string jce2arraycpp::generateH(const ConstPtr &pPtr) const
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
            << ((tostr(pPtr->getTypeIdPtr()->getTypePtr()) == "jceCode::Int64") ? "LL;" : ";" )<< endl;
    }

    return s.str();
}

void jce2arraycpp::createFile(const string &file)
{
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for(size_t i = 0; i < contexts.size(); i++)
    {
        if(file == contexts[i]->getFileName())
        {
            generateCoder(contexts[i]);
        }
    }
}

////////////////////////////////
//for coder generating
////////////////////////////////

string jce2arraycpp::generateCoder(const NamespacePtr &pPtr) const
{
    ostringstream s;
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

    DEL_TAB;
    s << "}";

    s << endl << endl;

    return s.str();
}

void jce2arraycpp::generateCoder(const ContextPtr &pPtr) const
{
    string n        = taf::TC_File::excludeFileExt(taf::TC_File::extractFileName(pPtr->getFileName())) + "Coder";

    string fileH    = n + ".h";

    string define   = taf::TC_Common::upper("__" + n + "_h_");

    ostringstream s;

    s << g_parse->printHeaderRemark();

    s << "#ifndef " << define << endl;
    s << "#define " << define << endl;
    s << endl;
    s << "#include \"jce_arraycpp.h\"" << endl;

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
        s << generateCoder(namespaces[i]) << endl;
    }

    s << endl;
    s << "#endif" << endl;

    taf::TC_File::save2file(fileH, s.str());

    return;
}
