%{
#include <iostream>
#include <memory>
#include <cassert>

using namespace std;

#include "parse.h"
#define YYDEBUG 1
%}

%defines
%debug

//keyword token
%token HCE_VOID
%token HCE_STRUCT
%token HCE_BOOL
%token HCE_BYTE
%token HCE_SHORT
%token HCE_INT
%token HCE_DOUBLE
%token HCE_FLOAT
%token HCE_LONG
%token HCE_STRING
%token HCE_BINARY
%token HCE_VECTOR
%token HCE_MAP
%token HCE_NAMESPACE
%token HCE_INTERFACE
%token HCE_IDENTIFIER
%token HCE_OP
%token HCE_INTEGER_LITERAL
%token HCE_FLOATING_POINT_LITERAL
%token HCE_FALSE
%token HCE_TRUE
%token HCE_STRING_LITERAL
%token HCE_SCOPE_DELIMITER
%token HCE_CONST
%token BAD_CHAR

%%
start: definitions
;

// ----------------------------------------------------------------------
definitions
// ----------------------------------------------------------------------
: definition
{
}
';' definitions
| error ';'
{
    yyerrok;
}
definitions
| definition
{
    g_parse->error("`;' missing after definition");
}
|
{
}
;

// ----------------------------------------------------------------------
definition
// ----------------------------------------------------------------------
: namespace_def
{
    assert($1 == 0 || NamespacePtr::dynamicCast($1));
}
| interface_def
{
    assert($1 == 0 || InterfacePtr::dynamicCast($1));
}
| struct_def
{
    assert($1 == 0 || StructPtr::dynamicCast($1));
}
| const_def
{
    assert($1 == 0 || ConstPtr::dynamicCast($1));
}
;

// ----------------------------------------------------------------------
namespace_def
// ----------------------------------------------------------------------
: HCE_NAMESPACE HCE_IDENTIFIER
{
    StringTokPtr ident  = StringTokPtr::dynamicCast($2);
    ContainerPtr c      = g_parse->currentContainer();
    NamespacePtr n      = c->createNamespace(ident->v);
    if(n)
    {
	     g_parse->pushContainer(n);
	     $$ = GrammarBasePtr::dynamicCast(n);
    }
    else
    {
        $$ = 0;
    }
}
'{' definitions '}'
{
    if($3)
    {
	     g_parse->popContainer();
	     $$ = $3;
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
interface_def
// ----------------------------------------------------------------------
: interface_id
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);

    NamespacePtr c = NamespacePtr::dynamicCast(g_parse->currentContainer());

    InterfacePtr cl = c->createInterface(ident->v);
    if(cl)
    {
	     g_parse->pushContainer(cl);
	     $$ = GrammarBasePtr::dynamicCast(cl);
    }
    else
    {
        $$ = 0;
    }
}
'{' interface_exports '}'
{
    if($2)
    {
	    g_parse->popContainer();
	    $$ = GrammarBasePtr::dynamicCast($2);
    }
    else
    {
	    $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
interface_id
// ----------------------------------------------------------------------
: HCE_INTERFACE HCE_IDENTIFIER
{
    $$ = $2;
}
| HCE_INTERFACE keyword
{
    StringTokPtr ident = StringTokPtr::dynamicCast($2);
    g_parse->error("keyword `" + ident->v + "' cannot be used as interface name");
    $$ = $2;
}
;

// ----------------------------------------------------------------------
interface_exports
// ----------------------------------------------------------------------
: interface_export ';' interface_exports
{
}
| error ';' interface_exports
{
}
| interface_export
{
    g_parse->error("`;' missing after definition");
}
|
{
}
;

// ----------------------------------------------------------------------
interface_export
// ----------------------------------------------------------------------
: operation
;

// ----------------------------------------------------------------------
operation
// ----------------------------------------------------------------------
: operation_preamble parameters ')'
{
    if($1)
    {
	     g_parse->popContainer();
	     $$ = GrammarBasePtr::dynamicCast($1);
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
operation_preamble
// ----------------------------------------------------------------------
: return_type HCE_OP
{
    TypePtr returnType = TypePtr::dynamicCast($1);
    StringTokPtr ident = StringTokPtr::dynamicCast($2);
    string name        = ident->v;
    InterfacePtr cl    = InterfacePtr::dynamicCast(g_parse->currentContainer());
    if(cl)
    {
      	OperationPtr op = cl->createOperation(name, returnType);
      	if(op)
      	{
      	    g_parse->pushContainer(op);
      	    $$ = GrammarBasePtr::dynamicCast(op);
      	}
      	else
      	{
      	    $$ = 0;
      	}
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
return_type
// ----------------------------------------------------------------------
: type
| HCE_VOID
{
    $$ = 0;
}
;


// ----------------------------------------------------------------------
parameters
// ----------------------------------------------------------------------
: // empty
{
}
| type_id
{
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($1);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    if(op)
    {
   	  op->createParamDecl(tsp);
    }
}
| parameters ',' type_id
{
    TypeIdPtr  tsp         = TypeIdPtr::dynamicCast($3);

    OperationPtr op = OperationPtr::dynamicCast(g_parse->currentContainer());
    if(op)
    {
	     op->createParamDecl(tsp);
    }
}
;

// 结构定义--------------------------------------------------------------
struct_def
// ----------------------------------------------------------------------
: struct_id
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);
    NamespacePtr np = NamespacePtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
      	StructPtr sp = np->createStruct(ident->v);
      	if(sp)
      	{
      	    g_parse->pushContainer(sp);
      	    $$ = GrammarBasePtr::dynamicCast(sp);
      	}
      	else
      	{
      	    $$ = 0;
      	}
    }
    else
    {
       g_parse->error("struct '" + ident->v + "' must definition in namespace");
    }
}
'{' struct_exports '}'
{
    if($2)
    {
        g_parse->popContainer();
    }
    $$ = $2;

    //不能有空结构
    StructPtr st = StructPtr::dynamicCast($$);
    assert(st);
    if(st->getAllMemberPtr().size() == 0)
    {
        g_parse->error("struct `" + st->getSid() + "' must have at least one member");
    }
}
;

// 结构名称定义----------------------------------------------------------
struct_id
// ----------------------------------------------------------------------
: HCE_STRUCT HCE_IDENTIFIER
{
   $$ = $2;
}
| HCE_STRUCT keyword
{
    StringTokPtr ident = StringTokPtr::dynamicCast($2);

    g_parse->error("keyword `" + ident->v + "' cannot be used as struct name");
}
| HCE_STRUCT error
{
    g_parse->error("abstract declarator '<anonymous struct>' used as declaration");
}
;

// 结构成员变量部分------------------------------------------------------
struct_exports
// ----------------------------------------------------------------------
: data_member ';' struct_exports
{

}
| data_member
{
   g_parse->error("';' missing after definition");
}
|
{
}
;

// 数据成员--------------------------------------------------------------
data_member
// ----------------------------------------------------------------------
: type_id
{
    StructPtr np = StructPtr::dynamicCast(g_parse->currentContainer());
    if(np)
    {
        TypeIdPtr tPtr  = TypeIdPtr::dynamicCast($1);
        np->addTypeId(tPtr);
        $$ = GrammarBasePtr::dynamicCast($1);
    }
    else
    {
        $$ = 0;
    }
}
;

// ----------------------------------------------------------------------
const_initializer
// ----------------------------------------------------------------------
: HCE_INTEGER_LITERAL
{
    IntegerTokPtr intVal = IntegerTokPtr::dynamicCast($1);
    ostringstream sstr;
    sstr << intVal->v;
    ConstTokPtr c = new ConstTok();
    c->t = ConstTok::VALUE;
    c->v = sstr.str();
    $$ = c;
}
| HCE_FLOATING_POINT_LITERAL
{
    FloatTokPtr floatVal = FloatTokPtr::dynamicCast($1);
    ostringstream sstr;
    sstr << floatVal->v;
    ConstTokPtr c = new ConstTok();
    c->t = ConstTok::VALUE;
    c->v = sstr.str();
    $$ = c;
}
| HCE_STRING_LITERAL
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);
    ConstTokPtr c = new ConstTok();
    c->t = ConstTok::STRING;
    c->v = ident->v;
    $$ = c;
}
| HCE_FALSE
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);
    ConstTokPtr c = new ConstTok();
    c->t = ConstTok::BOOL;
    c->v = ident->v;
    $$ = c;
}
| HCE_TRUE
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);
    ConstTokPtr c = new ConstTok();
    c->t = ConstTok::BOOL;
    c->v = ident->v;
    $$ = c;
}
;

// 常量定义--------------------------------------------------------------
const_def
// ----------------------------------------------------------------------
: HCE_CONST type_id '=' const_initializer
{
    NamespacePtr np = NamespacePtr::dynamicCast(g_parse->currentContainer());
    if(!np)
    {
        g_parse->error("const type must define in namespace");
    }

    TypeIdPtr t   = TypeIdPtr::dynamicCast($2);
    ConstTokPtr c = ConstTokPtr::dynamicCast($4);
    ConstPtr cPtr = np->createConst(t, c);
    $$ = cPtr;
}
;

// 变量定义--------------------------------------------------------------
type_id
// ----------------------------------------------------------------------
: type HCE_IDENTIFIER
{
    TypePtr type = TypePtr::dynamicCast($1);
    StringTokPtr ident = StringTokPtr::dynamicCast($2);

    TypeIdPtr typeIdPtr = new TypeId(type, ident->v);

    $$ = GrammarBasePtr::dynamicCast(typeIdPtr);
}
| type keyword
{
    StringTokPtr ident = StringTokPtr::dynamicCast($2);
    g_parse->error("keyword `" + ident->v + "' cannot be used as data member name");
}
| type
{
    g_parse->error("missing data member name");
}
| error
{
    g_parse->error("unkown type");
}
;

// 类型------------------------------------------------------------------
type
// ----------------------------------------------------------------------
: HCE_BOOL
{
    $$ = g_parse->createBuiltin(Builtin::KindBool);
}
| HCE_BYTE
{
    $$ = g_parse->createBuiltin(Builtin::KindByte);
}
| HCE_SHORT
{
    $$ = g_parse->createBuiltin(Builtin::KindShort);
}
| HCE_INT
{
    $$ = g_parse->createBuiltin(Builtin::KindInt);
}
| HCE_LONG
{
    $$ = g_parse->createBuiltin(Builtin::KindLong);
}
| HCE_FLOAT
{
    $$ = g_parse->createBuiltin(Builtin::KindFloat);
}
| HCE_DOUBLE
{
    $$ = g_parse->createBuiltin(Builtin::KindDouble);
}
| HCE_STRING
{
    $$ = g_parse->createBuiltin(Builtin::KindString);
}
| HCE_BINARY
{
    $$ = g_parse->createBuiltin(Builtin::KindBinary);
}
| vector
{
   $$ = GrammarBasePtr::dynamicCast($1);
}
| map
{
   $$ = GrammarBasePtr::dynamicCast($1);
}
| scoped_name
{
    StringTokPtr ident = StringTokPtr::dynamicCast($1);
    TypePtr sp = g_parse->findUserType(ident->v);
    if(sp)
    {
        $$ = GrammarBasePtr::dynamicCast(sp);
    }
    else
    {
        g_parse->error("'" + ident->v + "' undefined!");
    }
}
;

// 关键字----------------------------------------------------------------
vector
// ----------------------------------------------------------------------
: HCE_VECTOR '<' type '>'
{
   $$ = GrammarBasePtr::dynamicCast(g_parse->createVector(TypePtr::dynamicCast($3)));
}
| HCE_VECTOR '<' error
{
   g_parse->error("vector error");
}
| HCE_VECTOR '<' type error
{
   g_parse->error("vector missing '>'");
}
| HCE_VECTOR error
{
   g_parse->error("vector missing type");
}
;

// 关键字----------------------------------------------------------------
map
// ----------------------------------------------------------------------
: HCE_MAP '<' type ',' type '>'
{
   $$ = GrammarBasePtr::dynamicCast(g_parse->createMap(TypePtr::dynamicCast($3), TypePtr::dynamicCast($5)));
}
| HCE_MAP '<' error
{
   g_parse->error("map error");
}
;

// ----------------------------------------------------------------------
scoped_name
// ----------------------------------------------------------------------
: HCE_IDENTIFIER
{
}
| HCE_SCOPE_DELIMITER HCE_IDENTIFIER
{
    StringTokPtr ident = StringTokPtr::dynamicCast($2);
    ident->v = "::" + ident->v;
    $$ = GrammarBasePtr::dynamicCast(ident);
}
| scoped_name HCE_SCOPE_DELIMITER HCE_IDENTIFIER
{
    StringTokPtr scoped = StringTokPtr::dynamicCast($1);
    StringTokPtr ident  = StringTokPtr::dynamicCast($3);
    scoped->v += "::";
    scoped->v += ident->v;
    $$ = GrammarBasePtr::dynamicCast(scoped);
}
;

// 关键字----------------------------------------------------------------
keyword
// ----------------------------------------------------------------------
: HCE_STRUCT
{
}
| HCE_VOID
{
}
| HCE_BOOL
{
}
| HCE_BYTE
{
}
| HCE_SHORT
{
}
| HCE_INT
{
}
| HCE_FLOAT
{
}
| HCE_DOUBLE
{
}
| HCE_STRING
{
}
| HCE_BINARY
{
}
| HCE_VECTOR
{
}
| HCE_MAP
{
}
| HCE_NAMESPACE
{
}
| HCE_INTERFACE
{
}
| HCE_INTEGER_LITERAL
{
}
| HCE_FLOATING_POINT_LITERAL
{
}
| HCE_FALSE
{
}
| HCE_TRUE
{
}
| HCE_STRING_LITERAL
{
}
| HCE_CONST
{
}
;

%%


