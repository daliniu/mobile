#ifndef _LUA_REGISTRY_H_
#define _LUA_REGISTRY_H_

#include "luainclude.h"
#include "luavatartypes.h"
#include "luautil.h"
#include "luaexception.h"
#include "luavariable.h"
#include "luastate.h"


//C++Class Lua导出辅助类
template <typename T> class LuaRegister {
    typedef struct { T *pT; } userdataType;
public:
    typedef int (T::*mfp)(const Luavatar::LuaValueList& params, Luavatar::LuaValueList& values);
    typedef struct { const char *name; mfp mfunc; } RegType;

    //以lua原生api方式注册
    typedef int (T::*rawfp)(lua_State *L);
    typedef struct { const char *name; rawfp attr_func; } RawRegType;


    static void Register(lua_State *L,bool enable_new = true) 
    {
        lua_newtable(L);
        int methods = lua_gettop(L);

        luaL_newmetatable(L, T::luaClassName);
        int metatable = lua_gettop(L);

        // store method table in globals so that
        // scripts can add functions written in Lua.
        lua_pushvalue(L, methods);
        set(L, LUA_GLOBALSINDEX, T::luaClassName);

        // hide metatable from Lua getmetatable()
        lua_pushvalue(L, methods);
        set(L, metatable, "__metatable");

        lua_pushvalue(L, methods);
        set(L, metatable, "__index");

        lua_pushcfunction(L, tostring_T);
        set(L, metatable, "__tostring");

        lua_pushcfunction(L, gc_T);
        set(L, metatable, "__gc");

        if(enable_new)
        {
            lua_newtable(L);                // mt for method table
            lua_pushcfunction(L, new_T);
            lua_pushvalue(L, -1);           // dup new_T function
            set(L, methods, "new");         // add new_T to method table
            set(L, -3, "__call");           // mt.__call = new_T
            lua_setmetatable(L, methods);
        }

        // fill method table with methods from class T
        for (RegType *l = T::methods; l->name; l++)
        {
            lua_pushstring(L, l->name);
            lua_pushlightuserdata(L, (void*)l);
            lua_pushcclosure(L, thunk, 1);
            lua_settable(L, methods);
        }

        lua_pop(L, 2);  // drop metatable and method table
    }

    static void RegisterRaw(lua_State *L,bool enable_new = true) 
    {
        lua_newtable(L);
        int methods = lua_gettop(L);

        luaL_newmetatable(L, T::luaClassName);
        int metatable = lua_gettop(L);

        // store method table in globals so that
        // scripts can add functions written in Lua.
        lua_pushvalue(L, methods);
        set(L, LUA_GLOBALSINDEX, T::luaClassName);

        // hide metatable from Lua getmetatable()
        lua_pushvalue(L, methods);
        set(L, metatable, "__metatable");

        lua_pushvalue(L, methods);
        set(L, metatable, "__index");

        lua_pushcfunction(L, tostring_T);
        set(L, metatable, "__tostring");

        lua_pushcfunction(L, gc_T);
        set(L, metatable, "__gc");

        if(enable_new)
        {
            lua_newtable(L);                // mt for method table
            lua_pushcfunction(L, new_T);
            lua_pushvalue(L, -1);           // dup new_T function
            set(L, methods, "new");         // add new_T to method table
            set(L, -3, "__call");           // mt.__call = new_T
            lua_setmetatable(L, methods);
        }

        // fill method table with methods from class T
        for (RawRegType *l = T::methods; l->name; l++)
        {
            lua_pushstring(L, l->name);
            lua_pushlightuserdata(L, (void*)l);
            lua_pushcclosure(L, raw_thunk, 1);
            lua_settable(L, methods);
        }

        lua_pop(L, 2);  // drop metatable and method table
    }
  
    static void RegisterObj(Luavatar::LuaVariable lv, T *obj) 
    {
        lv.pushLastTable();
        Luavatar::LuaUtil::PushLuaValue (lv.getState(),lv.getKeys().back());
        push(lv.getState(), obj, false); // gc_T will not delete this object
        lua_settable (lv.getState(), -3);// userdata containing pointer to T object

        lua_pop (lv.getState(), 1);//将reference table 出栈
    }
  
    // call named lua method from userdata method table
    static int call(lua_State *L, const char *method,
                  int nargs=0, int nresults=LUA_MULTRET, int errfunc=0)
    {
        int base = lua_gettop(L) - nargs;  // userdata index
        if (!luaL_checkudata(L, base, T::luaClassName)) {
            lua_settop(L, base-1);           // drop userdata and args
        lua_pushfstring(L, "not a valid %s userdata", T::luaClassName);
        return -1;
        }

        lua_pushstring(L, method);         // method name
        lua_gettable(L, base);             // get method from userdata
        if (lua_isnil(L, -1)) {            // no method?
            lua_settop(L, base-1);           // drop userdata and args
            lua_pushfstring(L, "%s missing method '%s'", T::luaClassName, method);
            return -1;
        }
        lua_insert(L, base);               // put method under userdata, args

        int status = lua_pcall(L, 1+nargs, nresults, errfunc);  // call method
        if (status) {
            const char *msg = lua_tostring(L, -1);
            if (msg == NULL) msg = "(error with no message)";
            lua_pushfstring(L, "%s:%s status = %d\n%s",
                      T::luaClassName, method, status, msg);
            lua_remove(L, base);             // remove old message
            return -1;
        }
        return lua_gettop(L) - base + 1;   // number of results
    }

    // push onto the Lua stack a userdata containing a pointer to T object
    static int push(lua_State *L, T *obj, bool gc=false) {
        if (!obj) { lua_pushnil(L); return 0; }
        luaL_getmetatable(L, T::luaClassName);  // lookup metatable in Lua registry
        if (lua_isnil(L, -1)) luaL_error(L, "%s missing metatable", T::luaClassName);
        int mt = lua_gettop(L);
        subtable(L, mt, "userdata", "v");//table含有weak value
        userdataType *ud =
            static_cast<userdataType*>(pushuserdata(L, obj, sizeof(userdataType)));
        if (ud) {
            ud->pT = obj;  // store pointer to object in userdata
            lua_pushvalue(L, mt);
            lua_setmetatable(L, -2);
            if (gc == false) {
                lua_checkstack(L, 3);
                subtable(L, mt, "do not trash", "k");//table 含有weak key,当gc时会利用这个表确定是否执行gc动作 
                lua_pushvalue(L, -2);
                lua_pushboolean(L, 1);
                lua_settable(L, -3);
                lua_pop(L, 1);
            }
        }
        lua_replace(L, mt);
        lua_settop(L, mt);
        return mt;  // index of userdata containing pointer to T object
    }

    // get userdata from Lua stack and return pointer to T object
    static T *check(lua_State *L, int narg) {
        userdataType *ud =
            static_cast<userdataType*>(luaL_checkudata(L, narg, T::luaClassName));
        if(!ud) {
            luaL_typerror(L, narg, T::luaClassName);
            return NULL;
        }
        return ud->pT;  // pointer to T object
    }

private:
    LuaRegister();  // hide default constructor

    // member function dispatcher
    static int thunk(lua_State *L) {
        // stack has userdata, followed by method args
        T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
        lua_remove(L, 1);  // remove self so member function args start at index 1
        const int numParams = lua_gettop (L);
        try
        {
            Luavatar::LuaValueList params;
            Luavatar::LuaValue value;
            for (int i = 1; i <= numParams; ++i)
            {
                Luavatar::LuaUtil::ToLuaValue (L, i, value);
                params.push_back (value);
            }
            lua_pop (L, numParams);
        
            // get member function from upvalue
            RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));

            Luavatar::LuaValueList values;
            int iRet = (obj->*(l->mfunc))(params,values);// call member function
            /* Push the return values and return */
            typedef Luavatar::LuaValueList::const_iterator iter_t;
            for (iter_t p = values.begin(); p != values.end(); ++p)
            {
                Luavatar::LuaUtil::PushLuaValue (L, *p);
            }
        
            return iRet; 
        }
        catch (Luavatar::LuaError& e)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, e.what());
            return -1;
        }
        catch(...)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, "Unknown exception caught by wrapper.");
            return -1;
        }
    }

    static int raw_thunk(lua_State *L) {
        // stack has userdata, followed by method args
        T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
        lua_remove(L, 1);  // remove self so member function args start at index 1

        RawRegType *l = static_cast<RawRegType*>(lua_touserdata(L, lua_upvalueindex(1)));
        try
        {
            return (obj->*(l->attr_func))(L);  // call member function
        }
        catch (Luavatar::LuaError& e)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, e.what());
            return -1;
        }
        catch(...)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, "Unknown exception caught by wrapper.");
            return -1;
        }
    }

    // create a new T object and
    // push onto the Lua stack a userdata containing a pointer to T object
    static int new_T(lua_State *L) {
        lua_remove(L, 1);   // use luaClassName:new(), instead of luaClassName.new()
        const int numParams = lua_gettop (L);
        try
        {
            Luavatar::LuaValueList params;
            Luavatar::LuaValue value;
            for (int i = 1; i <= numParams; ++i)
            {
                Luavatar::LuaUtil::ToLuaValue (L, i, value);
                params.push_back (value);
            }
            lua_pop (L, numParams); 
        
            T *obj = new T(params);  // call constructor for T objects
            push(L, obj, true); // gc_T will delete this object
            return 1;           // userdata containing pointer to T object
        }
        catch (Luavatar::LuaError& e)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, e.what());
            return 0;
        }
        catch(...)
        {
            Luavatar::LuaUtil::ReportErrorFromCFunction (L, "Unknown exception caught by wrapper.");
            return 0;
        }
    }

    // garbage collection metamethod
    static int gc_T(lua_State *L) {
        if (luaL_getmetafield(L, 1, "do not trash")) {
            lua_pushvalue(L, 1);  // dup userdata
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) return 0;  // do not delete object
        }
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        if (obj) delete obj;  // call destructor for T objects
        return 0;
    }

    static int tostring_T (lua_State *L) {
        char buff[32];
        userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
        T *obj = ud->pT;
        sprintf(buff, "%p", (void*)obj);
        lua_pushfstring(L, "%s (%s)", T::luaClassName, buff);

        return 1;
    }

    static void set(lua_State *L, int table_index, const char *key) {
        lua_pushstring(L, key);
        lua_insert(L, -2);  // swap value and key
        lua_settable(L, table_index);
    }

    static void weaktable(lua_State *L, const char *mode) {
        lua_newtable(L);
        lua_pushvalue(L, -1);  // table is its own metatable
        lua_setmetatable(L, -2);
        lua_pushliteral(L, "__mode");
        lua_pushstring(L, mode);
        lua_settable(L, -3);   // metatable.__mode = mode
    }

    static void subtable(lua_State *L, int tindex, const char *name, const char *mode) {
        lua_pushstring(L, name);
        lua_gettable(L, tindex);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_checkstack(L, 3);// 检查statck是不是足够3个地址空间
            weaktable(L, mode);//在statck上生成一个weak table，weak属性由mode指定
            lua_pushstring(L, name);
            lua_pushvalue(L, -2);//将weaktable设置到metatable中，下次可以直接通过name找到
            lua_settable(L, tindex);
        }
    }

    static void *pushuserdata(lua_State *L, void *key, size_t sz) {
        void *ud = 0;
        lua_pushlightuserdata(L, key);
        lua_gettable(L, -2);     // lookup[key]
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);         // drop nil
            lua_checkstack(L, 3);
            ud = lua_newuserdata(L, sz);  // create new userdata
            lua_pushlightuserdata(L, key);
            lua_pushvalue(L, -2);  // dup userdata
            lua_settable(L, -4);   // lookup[key] = userdata
        }
        return ud;
    }
};

// LUA脚本类导出
// 声明部分
#define SCRIPT_FUNCTION(FunctionName) int FunctionName(const Luavatar::LuaValueList& params, Luavatar::LuaValueList& values);

#define DECLARE_SCRIPT_CLASS(ClassName)					\
	public:												\
	static const char luaClassName[];						\
	static LuaRegister<ClassName>::RegType methods[];

// 注册C函数
#define REGISTER_C_API( fnName, fnPoint ) lua_register(L , fnName, fnPoint);

// 实现部分
#define IMPLEMENT_SCRIPT_CLASS(ClassName)				\
	const char ClassName::luaClassName[] = #ClassName;

#define BEGIN_SCRIPT_METHOD(ClassName)					\
	LuaRegister<ClassName>::RegType ClassName::methods[] = {

#define LUNAR_DECLARE_METHOD(Class, Name) {#Name, &Class::Name}

#define END_SCRIPT_METHOD {NULL,NULL}};

// 应用部分
#define REGISTER_CLASS(ClassName,Ls)					    \
	LuaRegister<ClassName>::Register(Ls.getState());

//Lua脚本Struct导出
#define DECLARE_SCRIPT_STRUCT(ClassName)					\
	public:												\
	static const char luaClassName[];						\
	static LuaRegister<ClassName>::RawRegType methods[];

// 实现部分
#define IMPLEMENT_SCRIPT_STRUCT(ClassName)				\
	const char ClassName::luaClassName[] = #ClassName;

#define BEGIN_SCRIPT_ATTR(ClassName)					\
	LuaRegister<ClassName>::RawRegType ClassName::methods[] = {

#define LUNAR_DECLARE_ATTR(Class, Name) {#Name, &Class::Name}

#define END_SCRIPT_ATTR {NULL,NULL}};

#define REGISTER_STRUCT(StructName,Ls)					    \
	LuaRegister<StructName>::RegisterRaw(Ls.getState(),false);

//结构萃取类
template<typename T> 
struct LuaAttrHelper
{ 
	static int get(lua_State *L,const T& v) 
	{
        LuaRegister<T>::push(L, const_cast<T*>(&v), false);
		return 1;
	} 

    static int set(lua_State *L,T& v) 
	{
        void* addr = LuaRegister<T>::check(L,-1);
        if(addr)
        {
            v = *(reinterpret_cast<T*>(addr));
        }
        
        lua_pop(L, 1); 
        
        return 0;
	} 

    static void copy(const T& v,Luavatar::LuaVariable lv)
    {
        static_cast<T>(v).cs2table(lv);
    }
};

//偏特化版本
template<> 
struct LuaAttrHelper<bool>
{ 
    static int get(lua_State *L,const bool& v) 
    { 
        lua_pushboolean (L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,bool& v) 
	{
		v = lua_toboolean (L, -1) ? true : false;
        lua_pop(L, 1);  
        
        return 0;
	}
    
    static void copy(const bool& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<char>
{ 
    static int get(lua_State *L,const char& v) 
    { 
        lua_pushnumber(L, (short)v); 
        
        return 1;
    } 

    static int set(lua_State *L,char& v) 
	{
		v = (char)(static_cast<short>(lua_tonumber (L, -1)));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const char& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<short>
{ 
    static int get(lua_State *L,const short& v) 
    { 
        lua_pushnumber(L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,short& v) 
	{
		v = static_cast<short>(lua_tonumber (L, -1));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const short& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<float>
{ 
    static int get(lua_State *L,const float& v) 
    { 
        lua_pushnumber(L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,float& v) 
	{
		v = static_cast<float>(lua_tonumber (L, -1));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const float& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<int>
{ 
    static int get(lua_State *L,const int& v) 
    { 
        lua_pushnumber (L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,int& v) 
	{
		v = static_cast<int>(lua_tonumber (L, -1));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const int& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<double>
{ 
    static int get(lua_State *L,const double& v) 
    { 
        lua_pushnumber (L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,double& v) 
	{
		v = lua_tonumber (L, -1);
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const double& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<long long>
{ 
    static int get(lua_State *L,const long long& v) 
    { 
        lua_pushnumber(L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,long long& v) 
	{
		v = static_cast<long long>(lua_tonumber (L, -1));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const long long& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

template<> 
struct LuaAttrHelper<unsigned int>
{ 
    static int get(lua_State *L,const unsigned int& v) 
    { 
        lua_pushnumber(L, v); 
        
        return 1;
    } 

    static int set(lua_State *L,unsigned int& v) 
	{
		v = static_cast<unsigned int>(lua_tonumber (L, -1));
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const unsigned int& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};


template<> 
struct LuaAttrHelper<string>
{ 
    static int get(lua_State *L,const string& v) 
    { 
        lua_pushlstring (L, v.c_str(), v.length());
        
        return 1;
    } 

    static int set(lua_State *L,string& v) 
	{
        size_t size = lua_objlen(L, -1);
        v = string(lua_tostring(L, -1),size);
        lua_pop(L, 1);  
        
        return 0;
	}

    static void copy(const string& v,Luavatar::LuaVariable lv)
    {
        lv = Luavatar::LuaValue(v);
    }
};

//针对容器
template<typename T> 
struct LuaAttrHelper<std::vector<T> >
{ 
	static int get(lua_State *L,const std::vector<T>& v) 
	{ 
        //支持两种方式的获取，获取单个或获取全部
        //根据参数个数确定 
        const int param_num = lua_gettop (L);
        if(param_num == 1)//单个获取 
        {
            size_t index = static_cast<size_t>(lua_tonumber (L, -1));
            if(index < 1 || index > v.size())
            {
                //index 不合法    
                lua_pushnil(L); 
                lua_pop(L, 1);
                return 1;
            }
            lua_pop(L, 1);  
            
            return LuaAttrHelper<T>::get(L,v[index-1]);
        }
        else//全量获取 
        {
            lua_newtable(L);
            int tbl_idx = lua_gettop(L);
            
            size_t size = v.size();
            for(size_t i = 0;i<size;i++)
            {
                lua_pushnumber(L,i+1);
                LuaAttrHelper<T>::get(L,v[i]);
                lua_settable(L, tbl_idx);
            }

            return 1;
        }
	} 

    static int set(lua_State *L,std::vector<T>& v) 
	{ 
        //支持两种方式的设置，设置vector中的某一个下标
        //设置整个table，设置整个table，元素必须是string或number类型的
        const int param_num = lua_gettop (L);
        if(param_num == 2)//单个设置
        {
            size_t index = static_cast<size_t>(lua_tonumber (L, -2));
            if(index > v.size())
            {
                lua_pop(L,2);
                throw Luavatar::LuaRunTimeError("vector index exceed vector size.");
            }

            lua_remove(L,-2);
            LuaAttrHelper<T>::set(L,v[index-1]);
            
            return 0;
        }
        else//整表设置
        {
            int tbl_idx = lua_gettop(L);
            size_t tbl_size = lua_objlen(L, tbl_idx);
            v.resize(tbl_size);
            
            lua_pushnil (L);
            while (lua_next (L, tbl_idx) != 0)
            {
               LuaAttrHelper<T>::set(L,v[static_cast<int>(lua_tonumber (L, -2)) - 1]);
               //lua_pop (L, 1); //保留key，做下一次迭代
            }

            lua_pop(L,1); //将整个表pop出

            return 0;
        }
	}

    static void copy(const std::vector<T>& v,Luavatar::LuaVariable lv)
    {
        lv.emptyTable();
        for(size_t i = 0; i<v.size(); i++)
        {
            LuaAttrHelper<T>::copy(v[i],lv[i+1]);
        }
    }
};

//map支持两种方式，全量或索引，通过参数标识
//仅支持number及string类型的key 
template<typename T, typename U> 
struct LuaAttrHelper<std::map<T, U> >
{ 
	static int get(lua_State *L,const std::map<T, U>& v) 
	{ 
        //支持两种方式的获取，获取单个或获取全部
        //根据参数个数确定 
        typedef typename std::map<T,U>::const_iterator m_itr;
        m_itr mit;
        
        const int param_num = lua_gettop (L);
        if(param_num == 1)//单个获取 
        {
            T key = static_cast<T>(lua_tonumber (L, -1));
            lua_pop(L, 1); 
            
            mit = v.find(key);
            if(mit == v.end())
            {
                lua_pushnil(L); 
                return 1;
            }
            
            return LuaAttrHelper<U>::get(L,mit->second);;
        }
        else//全量获取 
        {
            lua_newtable(L);
            int tbl_idx = lua_gettop(L);

            mit = v.begin();
            while(mit != v.end())
            {
                LuaAttrHelper<T>::get(L,mit->first);
                LuaAttrHelper<U>::get(L,mit->second);
                lua_settable(L, tbl_idx);
                
                ++mit;
            }

            return 1;
        }
	}  

    static int set(lua_State *L,std::map<T, U>& v) 
	{ 
        //支持两种方式的设置，设置map中的某一个元素
        //整个map替换设置
        const int param_num = lua_gettop (L);
        if(param_num == 2)//单个设置
        {
            T key = static_cast<T>(lua_tonumber (L, -2));
            lua_remove(L,-2);
            
            LuaAttrHelper<U>::set(L,v[key]);
            
            return 0;
        }
        else//整表设置
        {
            int tbl_idx = lua_gettop(L);
            
            lua_pushnil (L);
            while (lua_next (L, tbl_idx) != 0)
            {
               LuaAttrHelper<U>::set(L,v[static_cast<T>(lua_tonumber (L, -2))]);
               //lua_pop (L, 1); //保留key，做下一次迭代
            }

            lua_pop(L,1); //将整个表pop出

            return 0;
        }
	}

    static void copy(const std::map<T, U>& v,Luavatar::LuaVariable lv)
    {
        lv.emptyTable();
        typedef typename std::map<T,U>::const_iterator m_itr;
        for(m_itr it = v.begin();it != v.end();++it)
        {
            LuaAttrHelper<U>::copy(it->second,lv[it->first]);
        }
    }
};

template<typename U> 
struct LuaAttrHelper<std::map<string, U> >
{ 
	static int get(lua_State *L,const std::map<string, U>& v) 
	{ 
        //支持两种方式的获取，获取单个或获取全部
        //根据参数个数确定 
        typedef typename std::map<string,U>::const_iterator m_itr;
        m_itr mit;
        
        const int param_num = lua_gettop (L);
        if(param_num == 1)//单个获取 
        {
            string key = string(lua_tostring(L, -1),lua_objlen(L, -1));
            lua_pop(L, 1); 
            
            mit = v.find(key);
            if(mit == v.end())
            {
                lua_pushnil(L); 
                return 1;
            }
            
            return LuaAttrHelper<U>::get(L,mit->second);;
        }
        else//全量获取 
        {
            lua_newtable(L);
            int tbl_idx = lua_gettop(L);

            mit = v.begin();
            while(mit != v.end())
            {
                LuaAttrHelper<string>::get(L,mit->first);
                LuaAttrHelper<U>::get(L,mit->second);
                lua_settable(L, tbl_idx);
                
                ++mit;
            }

            return 1;
        }
	}  

    static int set(lua_State *L,std::map<string, U>& v) 
	{ 
        //支持两种方式的设置，设置map中的某一个元素
        //整个map替换设置
        const int param_num = lua_gettop (L);
        if(param_num == 2)//单个设置
        {
            string key = string(lua_tostring(L, -2),lua_objlen(L, -2));
            lua_remove(L,-2);
            
            LuaAttrHelper<U>::set(L,v[key]);
            
            return 0;
        }
        else//整表设置
        {
            int tbl_idx = lua_gettop(L);
            
            lua_pushnil (L);
            while (lua_next (L, tbl_idx) != 0)
            {
               LuaAttrHelper<U>::set(L,v[string(lua_tostring(L, -2),lua_objlen(L, -2))]);
               //lua_pop (L, 1); //保留key，做下一次迭代
            }

            lua_pop(L,1); //将整个表pop出

            return 0;
        }
	}

    static void copy(const std::map<string, U>& v,Luavatar::LuaVariable lv)
    {
        lv.emptyTable();
        typedef typename std::map<string,U>::const_iterator m_itr;
        for(m_itr it = v.begin();it != v.end();++it)
        {
            LuaAttrHelper<U>::copy(it->second,lv[it->first]);
        }
    }
};

//calss,struct公共注册实例
#define REGISTER_OBJ(lv,TypeName,obj)					    \
	LuaRegister<TypeName>::RegisterObj(lv,obj);


#endif

