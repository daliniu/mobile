#include "element.h"
#include <iostream>
#include <cassert>
#include "parse.h"
#include "util/tc_common.h"
#include "util/tc_file.h"

/*************************************************************************************************/
const char* Builtin::builtinTable[] =
    {
        "void",
        "bool",
        "byte",
        "short",
        "int",
        "long",
        "float",
        "double",
        "string",
        "binary",
        "vector",
        "map"
    };

Builtin::Builtin(Kind kind) : _kind(kind)
{
}

Builtin::Kind Builtin::kind() const
{
    return _kind;
}

bool Builtin::isSimple() const
{
	switch(_kind)
	{
	case Builtin::KindBool:
	case Builtin::KindByte:
	case Builtin::KindShort:
	case Builtin::KindInt:
	case Builtin::KindLong:
	case Builtin::KindFloat:
	case Builtin::KindDouble:
        return true;
    default:
        return false;
	}

    return true;
}

/*************************************************************************************************/
Vector::Vector(const TypePtr& ptr) : _ptr(ptr)
{

}

/*************************************************************************************************/
Map::Map(const TypePtr& pleft, const TypePtr& pright): _pleft(pleft), _pright(pright)
{

}


/*************************************************************************************************/

TypeId::TypeId(const TypePtr& ptr, const string&id) : _ptr(ptr), _id(id)
{
}

/*************************************************************************************************/
NamespacePtr Container::createNamespace(const string &id)
{
    NamespacePtr np = NamespacePtr::dynamicCast(this);
    if(np)
    {
        g_parse->error("namespace can't be nested!");
    }

    _ns.push_back(new Namespace(id));

    g_parse->currentContextPtr()->addNamespacePtr(_ns.back());
    return _ns.back();
}

/*************************************************************************************************/
ParamDeclPtr Operation::createParamDecl(const TypeIdPtr &typeIdPtr)
{
    _ps.push_back(new ParamDecl(typeIdPtr));
    return _ps.back();
}

/*************************************************************************************************/
OperationPtr Interface::createOperation(const string &id, const TypePtr &typePtr)
{
    _ops.push_back(new Operation(id, typePtr));
    return _ops.back();
}

/*************************************************************************************************/

InterfacePtr Namespace::createInterface(const string &id)
{
    for(size_t i = 0; i < _is.size(); i++)
    {
        if(_is[i]->getId() == id)
        {
            g_parse->error("interface '" + id + "' exists in namespace '" + _id + "'");
            return NULL;
        }
    }

    _is.push_back(new Interface(id));
    return _is.back();
}

StructPtr Namespace::createStruct(const string& id)
{
    g_parse->checkConflict(_id + "::" + id);

    _ss.push_back(new Struct(id, _id + "::" + id));
    g_parse->addStructPtr(_ss.back());

    return _ss.back();
}

ConstPtr Namespace::createConst(TypeIdPtr &pPtr, ConstTokPtr &cPtr)
{
    for(size_t i = 0; i < _cs.size(); i++)
    {
        if(_cs[i]->getTypeIdPtr()->getId() == pPtr->getId())
        {
            g_parse->error("const type '" + pPtr->getId() + "' exists in namespace '" + _id + "'");
            return NULL;
        }
    }    

    g_parse->checkConstValue(pPtr, cPtr->t);
    _cs.push_back(new Const(pPtr, cPtr));

    return _cs.back();
}

/*************************************************************************************************/

void Struct::addTypeId(const TypeIdPtr &typeIdPtr)
{
    StructPtr sp = StructPtr::dynamicCast(typeIdPtr->getTypePtr());
    if(sp)
    {
        if(sp->getSid() == getSid())
        {
            g_parse->error("struct can't take self as member data");
        }
    }

    for(size_t i = 0; i < _members.size(); i++)
    {
        if(_members[i]->getId() == typeIdPtr->getId())
        {
            g_parse->error("data member '" + typeIdPtr->getId() + "' duplicate definition");
        }
    }

    _members.push_back(typeIdPtr);
}

/***********************************************************************************/

void Context::addInclude(const string &incl) 
{ 
    if(incl == _filename)
    {
        g_parse->error("can't include self");
    }
    _includes.push_back(taf::TC_File::excludeFileExt(incl) + ".h"); 
}


