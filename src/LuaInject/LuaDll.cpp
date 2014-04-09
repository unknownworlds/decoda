/*

Decoda
Copyright (C) 2007-2013 Unknown Worlds Entertainment, Inc. 

This file is part of Decoda.

Decoda is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Decoda is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Decoda.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "LuaDll.h"
#include "Hook.h"
#include "DebugBackend.h"
#include "StdCall.h"
#include "CriticalSection.h"
#include "CriticalSectionLock.h"
#include "DebugHelp.h"

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <windows.h>
#include <malloc.h>
#include <assert.h>
#include <set>
#include <hash_map>
#include <hash_set>

// Macro for convenient pointer addition.
// Essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MAKE_PTR(cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

// When this is defined, additional information about what's going on will be
// output for debugging.
//#define VERBOSE
//#define LOG

typedef const char* (__stdcall *lua_Reader_stdcall) (lua_State*, void*, size_t*);
typedef void        (__stdcall *lua_Hook_stdcall)   (lua_State*, lua_Debug*);

typedef lua_State*      (*lua_open_cdecl_t)             (int stacksize);
typedef lua_State*      (*lua_open_500_cdecl_t)         ();
typedef lua_State*      (*lua_newstate_cdecl_t)         (lua_Alloc, void*);
typedef void            (*lua_close_cdecl_t)            (lua_State*);
typedef lua_State*      (*lua_newthread_cdecl_t)        (lua_State*);
typedef int             (*lua_error_cdecl_t)            (lua_State*);
typedef int             (*lua_absindex_cdecl_t)         (lua_State*, int);
typedef int             (*lua_sethook_cdecl_t)          (lua_State*, lua_Hook, int, int);
typedef int             (*lua_gethookmask_cdecl_t)      (lua_State*);
typedef int             (*lua_getinfo_cdecl_t)          (lua_State*, const char*, lua_Debug* ar);
typedef void            (*lua_remove_cdecl_t)           (lua_State*, int);
typedef void            (*lua_settable_cdecl_t)         (lua_State*, int);
typedef void            (*lua_gettable_cdecl_t)         (lua_State*, int);
typedef void            (*lua_setglobal_cdecl_t)        (lua_State*, const char*);
typedef void            (*lua_getglobal_cdecl_t)        (lua_State*, const char*);
typedef void            (*lua_rawget_cdecl_t)           (lua_State *L, int idx);
typedef void            (*lua_rawgeti_cdecl_t)          (lua_State *L, int idx, int n);
typedef void            (*lua_rawset_cdecl_t)           (lua_State *L, int idx);
typedef void            (*lua_pushstring_cdecl_t)       (lua_State*, const char*);
typedef void            (*lua_pushlstring_cdecl_t)      (lua_State*, const char*, size_t);
typedef int             (*lua_type_cdecl_t)             (lua_State*, int);
typedef const char*     (*lua_typename_cdecl_t)         (lua_State*, int);
typedef void            (*lua_settop_cdecl_t)           (lua_State*, int);
typedef const char*     (*lua_getlocal_cdecl_t)         (lua_State*, const lua_Debug*, int);
typedef const char*     (*lua_setlocal_cdecl_t)         (lua_State*, const lua_Debug*, int);
typedef int             (*lua_getstack_cdecl_t)         (lua_State*, int, lua_Debug*);
typedef void            (*lua_insert_cdecl_t)           (lua_State*, int);
typedef void            (*lua_pushnil_cdecl_t)          (lua_State*);
typedef void            (*lua_pushcclosure_cdecl_t)     (lua_State*, lua_CFunction, int);
typedef void            (*lua_pushvalue_cdecl_t)        (lua_State*, int);
typedef void            (*lua_pushinteger_cdecl_t)      (lua_State*, int);
typedef void            (*lua_pushnumber_cdecl_t)       (lua_State*, lua_Number);
typedef const char*     (*lua_tostring_cdecl_t)         (lua_State*, int);
typedef const char*     (*lua_tolstring_cdecl_t)        (lua_State*, int, size_t*);
typedef int             (*lua_toboolean_cdecl_t)        (lua_State*, int);
typedef int             (*lua_tointeger_cdecl_t)        (lua_State*, int);
typedef lua_Integer     (*lua_tointegerx_cdecl_t)       (lua_State*, int, int*);
typedef lua_CFunction   (*lua_tocfunction_cdecl_t)      (lua_State*, int);
typedef lua_Number      (*lua_tonumber_cdecl_t)         (lua_State*, int);
typedef lua_Number      (*lua_tonumberx_cdecl_t)        (lua_State*, int,  int*);
typedef void*           (*lua_touserdata_cdecl_t)       (lua_State*, int);
typedef int             (*lua_gettop_cdecl_t)           (lua_State*);
typedef int             (*lua_load_510_cdecl_t)         (lua_State*, lua_Reader, void*, const char *chunkname);
typedef int             (*lua_load_cdecl_t)             (lua_State*, lua_Reader, void*, const char *chunkname, const char *mode);
typedef void            (*lua_call_cdecl_t)             (lua_State*, int, int);
typedef void            (*lua_callk_cdecl_t)            (lua_State*, int, int, int, lua_CFunction);
typedef int             (*lua_pcall_cdecl_t)            (lua_State*, int, int, int);
typedef int             (*lua_pcallk_cdecl_t)           (lua_State*, int, int, int, int, lua_CFunction);
typedef void            (*lua_newtable_cdecl_t)         (lua_State*);
typedef void            (*lua_createtable_cdecl_t)      (lua_State*, int, int);
typedef int             (*lua_next_cdecl_t)             (lua_State*, int);
typedef int             (*lua_rawequal_cdecl_t)         (lua_State *L, int idx1, int idx2);
typedef int             (*lua_getmetatable_cdecl_t)     (lua_State*, int objindex);
typedef int             (*lua_setmetatable_cdecl_t)     (lua_State*, int objindex);
typedef int             (*luaL_ref_cdecl_t)             (lua_State *L, int t);
typedef void            (*luaL_unref_cdecl_t)           (lua_State *L, int t, int ref);
typedef int             (*luaL_newmetatable_cdecl_t)    (lua_State *L, const char *tname);
typedef int             (*luaL_loadbuffer_cdecl_t)      (lua_State *L, const char *buff, size_t sz, const char *name);
typedef int             (*luaL_loadbufferx_cdecl_t)     (lua_State *L, const char *buff, size_t sz, const char *name, const char* mode);
typedef int             (*luaL_loadfile_cdecl_t)        (lua_State *L, const char *fileName);
typedef int             (*luaL_loadfilex_cdecl_t)       (lua_State *L, const char *fileName, const char* mode);
typedef const lua_WChar* (*lua_towstring_cdecl_t)       (lua_State *L, int index);
typedef int             (*lua_iswstring_cdecl_t)        (lua_State *L, int index);
typedef const char*     (*lua_getupvalue_cdecl_t)       (lua_State *L, int funcindex, int n);
typedef const char*     (*lua_setupvalue_cdecl_t)       (lua_State *L, int funcindex, int n);
typedef void            (*lua_getfenv_cdecl_t)          (lua_State *L, int index);
typedef int             (*lua_setfenv_cdecl_t)          (lua_State *L, int index);
typedef void            (*lua_pushlightuserdata_cdecl_t)(lua_State *L, void *p);
typedef int             (*lua_pushthread_cdecl_t)       (lua_State *L);
typedef void *          (*lua_newuserdata_cdecl_t)      (lua_State *L, size_t size);
typedef lua_State*      (*luaL_newstate_cdecl_t)        ();
typedef int             (*lua_checkstack_cdecl_t)       (lua_State* L, int extra);

typedef lua_State*      (__stdcall *lua_open_stdcall_t)           (int stacksize);
typedef lua_State*      (__stdcall *lua_open_500_stdcall_t)       ();
typedef lua_State*      (__stdcall *lua_newstate_stdcall_t)       (lua_Alloc, void*);
typedef void            (__stdcall *lua_close_stdcall_t)          (lua_State*);
typedef lua_State*      (__stdcall *lua_newthread_stdcall_t)      (lua_State*);
typedef int             (__stdcall *lua_error_stdcall_t)          (lua_State*);
typedef int             (__stdcall *lua_absindex_stdcall_t)       (lua_State*, int);
typedef int             (__stdcall *lua_sethook_stdcall_t)        (lua_State*, lua_Hook_stdcall, int, int);
typedef int             (__stdcall *lua_gethookmaskstdcall_t)     (lua_State*);
typedef int             (__stdcall *lua_getinfo_stdcall_t)        (lua_State*, const char*, lua_Debug* ar);
typedef void            (__stdcall *lua_remove_stdcall_t)         (lua_State*, int);
typedef void            (__stdcall *lua_settable_stdcall_t)       (lua_State*, int);
typedef void            (__stdcall *lua_gettable_stdcall_t)       (lua_State*, int);
typedef void            (__stdcall *lua_setglobal_stdcall_t)      (lua_State*, const char*);
typedef void            (__stdcall *lua_getglobal_stdcall_t)      (lua_State*, const char*);
typedef void            (__stdcall *lua_rawget_stdcall_t)         (lua_State *L, int idx);
typedef void            (__stdcall *lua_rawgeti_stdcall_t)        (lua_State *L, int idx, int n);
typedef void            (__stdcall *lua_rawset_stdcall_t)         (lua_State *L, int idx);
typedef void            (__stdcall *lua_pushstring_stdcall_t)     (lua_State*, const char*);
typedef void            (__stdcall *lua_pushlstring_stdcall_t)    (lua_State*, const char*, size_t);
typedef int             (__stdcall *lua_type_stdcall_t)           (lua_State*, int);
typedef const char*     (__stdcall *lua_typename_stdcall_t)       (lua_State*, int);
typedef void            (__stdcall *lua_settop_stdcall_t)         (lua_State*, int);
typedef const char*     (__stdcall *lua_getlocal_stdcall_t)       (lua_State*, const lua_Debug*, int);
typedef const char*     (__stdcall *lua_setlocal_stdcall_t)       (lua_State*, const lua_Debug*, int);
typedef int             (__stdcall *lua_getstack_stdcall_t)       (lua_State*, int, lua_Debug*);
typedef void            (__stdcall *lua_insert_stdcall_t)         (lua_State*, int);
typedef void            (__stdcall *lua_pushnil_stdcall_t)        (lua_State*);
typedef void            (__stdcall *lua_pushcclosure_stdcall_t)   (lua_State*, lua_CFunction, int);
typedef void            (__stdcall *lua_pushvalue_stdcall_t)      (lua_State*, int);
typedef void            (__stdcall *lua_pushinteger_stdcall_t)    (lua_State*, int);
typedef void            (__stdcall *lua_pushnumber_stdcall_t)     (lua_State*, lua_Number);
typedef const char*     (__stdcall *lua_tostring_stdcall_t)       (lua_State*, int);
typedef const char*     (__stdcall *lua_tolstring_stdcall_t)      (lua_State*, int, size_t*);
typedef int             (__stdcall *lua_toboolean_stdcall_t)      (lua_State*, int);
typedef int             (__stdcall *lua_tointeger_stdcall_t)      (lua_State*, int);
typedef lua_Integer     (__stdcall *lua_tointegerx_stdcall_t)     (lua_State*, int, int*);
typedef lua_CFunction   (__stdcall *lua_tocfunction_stdcall_t)    (lua_State*, int);
typedef lua_Number      (__stdcall *lua_tonumber_stdcall_t)       (lua_State*, int);
typedef lua_Number      (__stdcall *lua_tonumberx_stdcall_t)      (lua_State*, int,  int*);
typedef void*           (__stdcall *lua_touserdata_stdcall_t)     (lua_State*, int);
typedef int             (__stdcall *lua_gettop_stdcall_t)         (lua_State*);
typedef int             (__stdcall *lua_load_510_stdcall_t)       (lua_State*, lua_Reader_stdcall, void*, const char *chunkname);
typedef int             (__stdcall *lua_load_stdcall_t)           (lua_State*, lua_Reader_stdcall, void*, const char *chunkname, const char *mode);
typedef void            (__stdcall *lua_call_stdcall_t)           (lua_State*, int, int);
typedef void            (__stdcall *lua_callk_stdcall_t)          (lua_State*, int, int, int, lua_CFunction);
typedef int             (__stdcall *lua_pcall_stdcall_t)          (lua_State*, int, int, int);
typedef int             (__stdcall *lua_pcallk_stdcall_t)         (lua_State*, int, int, int, int, lua_CFunction);
typedef void            (__stdcall *lua_newtable_stdcall_t)       (lua_State*);
typedef void            (__stdcall *lua_createtable_stdcall_t)    (lua_State*, int, int);
typedef int             (__stdcall *lua_next_stdcall_t)           (lua_State*, int);
typedef int             (__stdcall *lua_rawequal_stdcall_t)       (lua_State *L, int idx1, int idx2);
typedef int             (__stdcall *lua_getmetatable_stdcall_t)   (lua_State*, int objindex);
typedef int             (__stdcall *lua_setmetatable_stdcall_t)   (lua_State*, int objindex);
typedef int             (__stdcall *luaL_ref_stdcall_t)           (lua_State *L, int t);
typedef void            (__stdcall *luaL_unref_stdcall_t)         (lua_State *L, int t, int ref);
typedef int             (__stdcall *luaL_newmetatable_stdcall_t)  (lua_State *L, const char *tname);
typedef int             (__stdcall *luaL_loadbuffer_stdcall_t)    (lua_State *L, const char *buff, size_t sz, const char *name);
typedef int             (__stdcall *luaL_loadbufferx_stdcall_t)   (lua_State *L, const char *buff, size_t sz, const char *name, const char* mode);
typedef int             (__stdcall *luaL_loadfile_stdcall_t)      (lua_State *L, const char *fileName);
typedef int             (__stdcall *luaL_loadfilex_stdcall_t)     (lua_State *L, const char *fileName, const char* mode);
typedef const lua_WChar* (__stdcall *lua_towstring_stdcall_t)     (lua_State *L, int index);
typedef int             (__stdcall *lua_iswstring_stdcall_t)      (lua_State *L, int index);
typedef const char*     (__stdcall *lua_getupvalue_stdcall_t)     (lua_State *L, int funcindex, int n);
typedef const char*     (__stdcall *lua_setupvalue_stdcall_t)     (lua_State *L, int funcindex, int n);
typedef void            (__stdcall *lua_getfenv_stdcall_t)        (lua_State *L, int index);
typedef int             (__stdcall *lua_setfenv_stdcall_t)        (lua_State *L, int index);
typedef void            (__stdcall *lua_pushlightuserdata_stdcall_t)(lua_State *L, void *p);
typedef int             (__stdcall *lua_pushthread_stdcall_t)     (lua_State *L);
typedef void *          (__stdcall *lua_newuserdata_stdcall_t)    (lua_State *L, size_t size);
typedef lua_State*      (__stdcall *luaL_newstate_stdcall_t)      ();
typedef int             (__stdcall *lua_checkstack_stdcall_t)     (lua_State* L, int extra);

typedef HMODULE         (WINAPI *LoadLibraryExW_t)              (LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);
typedef ULONG           (WINAPI *LdrLockLoaderLock_t)           (ULONG flags, PULONG disposition, PULONG cookie);
typedef LONG            (WINAPI *LdrUnlockLoaderLock_t)         (ULONG flags, ULONG cookie);


/**
 * Structure that holds pointers to all of the Lua API functions.
 */
struct LuaInterface
{

    int                          version;   // One of 401, 500, 510, 520
    bool                         finishedLoading;

    bool                         stdcall;

    // Use these instead of the LUA_* constants in lua.h. The value of these
    // change depending on the version of Lua we're using.
    int                          registryIndex;
    int                          globalsIndex;
    int                          hookTailCall;
    int                          hookTailRet;

    // cdecl functions.
    lua_open_cdecl_t             lua_open_dll_cdecl;
    lua_open_500_cdecl_t         lua_open_500_dll_cdecl;
    lua_newstate_cdecl_t         lua_newstate_dll_cdecl;
    lua_close_cdecl_t            lua_close_dll_cdecl;
    lua_newthread_cdecl_t        lua_newthread_dll_cdecl;
    lua_error_cdecl_t            lua_error_dll_cdecl;
		lua_absindex_cdecl_t         lua_absindex_dll_cdecl;
    lua_gettop_cdecl_t           lua_gettop_dll_cdecl;
    lua_sethook_cdecl_t          lua_sethook_dll_cdecl;
    lua_gethookmask_cdecl_t      lua_gethookmask_dll_cdecl;
    lua_getinfo_cdecl_t          lua_getinfo_dll_cdecl;
    lua_remove_cdecl_t           lua_remove_dll_cdecl;
    lua_settable_cdecl_t         lua_settable_dll_cdecl;
    lua_gettable_cdecl_t         lua_gettable_dll_cdecl;
    lua_setglobal_cdecl_t        lua_setglobal_dll_cdecl;
    lua_getglobal_cdecl_t        lua_getglobal_dll_cdecl;
    lua_rawget_cdecl_t           lua_rawget_dll_cdecl;
    lua_rawgeti_cdecl_t          lua_rawgeti_dll_cdecl;
    lua_rawset_cdecl_t           lua_rawset_dll_cdecl;
    lua_pushstring_cdecl_t       lua_pushstring_dll_cdecl;
    lua_pushlstring_cdecl_t      lua_pushlstring_dll_cdecl;
    lua_type_cdecl_t             lua_type_dll_cdecl;
    lua_typename_cdecl_t         lua_typename_dll_cdecl;
    lua_settop_cdecl_t           lua_settop_dll_cdecl;
    lua_getlocal_cdecl_t         lua_getlocal_dll_cdecl;
    lua_setlocal_cdecl_t         lua_setlocal_dll_cdecl;
    lua_getstack_cdecl_t         lua_getstack_dll_cdecl;
    lua_insert_cdecl_t           lua_insert_dll_cdecl;
    lua_pushnil_cdecl_t          lua_pushnil_dll_cdecl;
    lua_pushvalue_cdecl_t        lua_pushvalue_dll_cdecl;
    lua_pushinteger_cdecl_t      lua_pushinteger_dll_cdecl;
    lua_pushnumber_cdecl_t       lua_pushnumber_dll_cdecl;
    lua_pushcclosure_cdecl_t     lua_pushcclosure_dll_cdecl;
    lua_tostring_cdecl_t         lua_tostring_dll_cdecl;
    lua_tolstring_cdecl_t        lua_tolstring_dll_cdecl;
    lua_toboolean_cdecl_t        lua_toboolean_dll_cdecl;
    lua_tointeger_cdecl_t        lua_tointeger_dll_cdecl;
    lua_tointegerx_cdecl_t       lua_tointegerx_dll_cdecl;
    lua_tocfunction_cdecl_t      lua_tocfunction_dll_cdecl;
    lua_tonumber_cdecl_t         lua_tonumber_dll_cdecl;
    lua_tonumberx_cdecl_t        lua_tonumberx_dll_cdecl;
    lua_touserdata_cdecl_t       lua_touserdata_dll_cdecl;
    lua_load_cdecl_t             lua_load_dll_cdecl;
    lua_load_510_cdecl_t         lua_load_510_dll_cdecl;
    lua_call_cdecl_t             lua_call_dll_cdecl;
    lua_callk_cdecl_t            lua_callk_dll_cdecl;
    lua_pcall_cdecl_t            lua_pcall_dll_cdecl;
    lua_pcallk_cdecl_t           lua_pcallk_dll_cdecl;
    lua_newtable_cdecl_t         lua_newtable_dll_cdecl;
    lua_createtable_cdecl_t      lua_createtable_dll_cdecl;
    lua_next_cdecl_t             lua_next_dll_cdecl;
    lua_rawequal_cdecl_t         lua_rawequal_dll_cdecl;
    lua_getmetatable_cdecl_t     lua_getmetatable_dll_cdecl;
    lua_setmetatable_cdecl_t     lua_setmetatable_dll_cdecl;
    luaL_ref_cdecl_t             luaL_ref_dll_cdecl;
    luaL_unref_cdecl_t           luaL_unref_dll_cdecl;
    luaL_newmetatable_cdecl_t    luaL_newmetatable_dll_cdecl;
    luaL_loadbuffer_cdecl_t      luaL_loadbuffer_dll_cdecl;
    luaL_loadbufferx_cdecl_t     luaL_loadbufferx_dll_cdecl;
    luaL_loadfile_cdecl_t        luaL_loadfile_dll_cdecl;
    luaL_loadfilex_cdecl_t       luaL_loadfilex_dll_cdecl;
    lua_towstring_cdecl_t        lua_towstring_dll_cdecl;
    lua_iswstring_cdecl_t        lua_iswstring_dll_cdecl;
    lua_getupvalue_cdecl_t       lua_getupvalue_dll_cdecl;
    lua_setupvalue_cdecl_t       lua_setupvalue_dll_cdecl;
    lua_getfenv_cdecl_t          lua_getfenv_dll_cdecl;
    lua_setfenv_cdecl_t          lua_setfenv_dll_cdecl;
    lua_pushlightuserdata_cdecl_t lua_pushlightuserdata_dll_cdecl;
    lua_pushthread_cdecl_t       lua_pushthread_dll_cdecl;
    lua_newuserdata_cdecl_t      lua_newuserdata_dll_cdecl;
    luaL_newstate_cdecl_t        luaL_newstate_dll_cdecl;
    lua_checkstack_cdecl_t       lua_checkstack_dll_cdecl;

    // stdcall functions.
    lua_open_stdcall_t           lua_open_dll_stdcall;
    lua_open_500_stdcall_t       lua_open_500_dll_stdcall;
    lua_newstate_stdcall_t       lua_newstate_dll_stdcall;
    lua_close_stdcall_t          lua_close_dll_stdcall;
    lua_newthread_stdcall_t      lua_newthread_dll_stdcall;
    lua_error_stdcall_t          lua_error_dll_stdcall;
    lua_absindex_stdcall_t       lua_absindex_dll_stdcall;
    lua_gettop_stdcall_t         lua_gettop_dll_stdcall;
    lua_sethook_stdcall_t        lua_sethook_dll_stdcall;
    lua_gethookmaskstdcall_t     lua_gethookmask_dll_stdcall;
    lua_getinfo_stdcall_t        lua_getinfo_dll_stdcall;
    lua_remove_stdcall_t         lua_remove_dll_stdcall;
    lua_settable_stdcall_t       lua_settable_dll_stdcall;
    lua_gettable_stdcall_t       lua_gettable_dll_stdcall;
    lua_setglobal_stdcall_t      lua_setglobal_dll_stdcall;
    lua_getglobal_stdcall_t      lua_getglobal_dll_stdcall;
    lua_rawget_stdcall_t         lua_rawget_dll_stdcall;
    lua_rawgeti_stdcall_t        lua_rawgeti_dll_stdcall;
    lua_rawset_stdcall_t         lua_rawset_dll_stdcall;
    lua_pushstring_stdcall_t     lua_pushstring_dll_stdcall;
    lua_pushlstring_stdcall_t    lua_pushlstring_dll_stdcall;
    lua_type_stdcall_t           lua_type_dll_stdcall;
    lua_typename_stdcall_t       lua_typename_dll_stdcall;
    lua_settop_stdcall_t         lua_settop_dll_stdcall;
    lua_getlocal_stdcall_t       lua_getlocal_dll_stdcall;
    lua_setlocal_stdcall_t       lua_setlocal_dll_stdcall;
    lua_getstack_stdcall_t       lua_getstack_dll_stdcall;
    lua_insert_stdcall_t         lua_insert_dll_stdcall;
    lua_pushnil_stdcall_t        lua_pushnil_dll_stdcall;
    lua_pushvalue_stdcall_t      lua_pushvalue_dll_stdcall;
    lua_pushinteger_stdcall_t    lua_pushinteger_dll_stdcall;
    lua_pushnumber_stdcall_t     lua_pushnumber_dll_stdcall;
    lua_pushcclosure_stdcall_t   lua_pushcclosure_dll_stdcall;
    lua_tostring_stdcall_t       lua_tostring_dll_stdcall;
    lua_tolstring_stdcall_t      lua_tolstring_dll_stdcall;
    lua_toboolean_stdcall_t      lua_toboolean_dll_stdcall;
    lua_tointeger_stdcall_t      lua_tointeger_dll_stdcall;
    lua_tointegerx_stdcall_t     lua_tointegerx_dll_stdcall;
    lua_tocfunction_stdcall_t    lua_tocfunction_dll_stdcall;
    lua_tonumber_stdcall_t       lua_tonumber_dll_stdcall;
    lua_tonumberx_stdcall_t      lua_tonumberx_dll_stdcall;
    lua_touserdata_stdcall_t     lua_touserdata_dll_stdcall;
    lua_load_stdcall_t           lua_load_dll_stdcall;
    lua_load_510_stdcall_t       lua_load_510_dll_stdcall;
    lua_call_stdcall_t           lua_call_dll_stdcall;
    lua_callk_stdcall_t          lua_callk_dll_stdcall;
    lua_pcall_stdcall_t          lua_pcall_dll_stdcall;
    lua_pcallk_stdcall_t         lua_pcallk_dll_stdcall;
    lua_newtable_stdcall_t       lua_newtable_dll_stdcall;
    lua_createtable_stdcall_t    lua_createtable_dll_stdcall;
    lua_next_stdcall_t           lua_next_dll_stdcall;
    lua_rawequal_stdcall_t       lua_rawequal_dll_stdcall;
    lua_getmetatable_stdcall_t   lua_getmetatable_dll_stdcall;
    lua_setmetatable_stdcall_t   lua_setmetatable_dll_stdcall;
    luaL_ref_stdcall_t           luaL_ref_dll_stdcall;
    luaL_unref_stdcall_t         luaL_unref_dll_stdcall;
    luaL_newmetatable_stdcall_t  luaL_newmetatable_dll_stdcall;
    luaL_loadbuffer_stdcall_t    luaL_loadbuffer_dll_stdcall;
    luaL_loadbufferx_stdcall_t   luaL_loadbufferx_dll_stdcall;
    luaL_loadfile_stdcall_t      luaL_loadfile_dll_stdcall;
    luaL_loadfilex_stdcall_t     luaL_loadfilex_dll_stdcall;
    lua_towstring_stdcall_t      lua_towstring_dll_stdcall;
    lua_iswstring_stdcall_t      lua_iswstring_dll_stdcall;
    lua_getupvalue_stdcall_t     lua_getupvalue_dll_stdcall;
    lua_setupvalue_stdcall_t     lua_setupvalue_dll_stdcall;
    lua_getfenv_stdcall_t        lua_getfenv_dll_stdcall;
    lua_setfenv_stdcall_t        lua_setfenv_dll_stdcall;
    lua_pushlightuserdata_stdcall_t lua_pushlightuserdata_dll_stdcall;
    lua_pushthread_stdcall_t     lua_pushthread_dll_stdcall;
    lua_newuserdata_stdcall_t    lua_newuserdata_dll_stdcall;
    luaL_newstate_stdcall_t      luaL_newstate_dll_stdcall;
    lua_checkstack_stdcall_t     lua_checkstack_dll_stdcall;

    lua_CFunction                DecodaOutput;
    lua_CFunction                CPCallHandler;
    lua_Hook                     HookHandler;

};

struct CPCallHandlerArgs
{
    lua_CFunction_dll           function;
    void*                       data;
};

/**
 * This macro outputs the prolog code for a naked intercept function. It
 * should be the first code in the function.
 */
#define INTERCEPT_PROLOG()                          \
	__asm                                           \
    {                                               \
        __asm push    ebp                           \
        __asm mov     ebp,            esp           \
        __asm sub     esp,            __LOCAL_SIZE  \
    }

/**
 * This macro outputs the epilog code for a naked intercept function. It
 * should be the last code in the function. argsSize is the number of 
 * bytes for the arguments to the function (not including the the api parameter).
 * The return from the function should be stored in the "result" variable, and
 * the "stdcall" bool variable determines if the function was called using the
 * stdcall or cdecl calling convention.
 */
#define INTERCEPT_EPILOG(argsSize)                  \
    __asm                                           \
    {                                               \
        __asm mov     eax,            result        \
        __asm cmp     stdcall,        0             \
        __asm mov     esp,            ebp           \
        __asm pop     ebp                           \
        __asm jne     stdcall_ret                   \
        __asm ret     4                             \
    __asm stdcall_ret:                              \
        __asm ret     (4 + argsSize)                \
    }

/**
 * This macro outputs the epilog code for a naked intercept function that doesn't
 * have a return value. It should be the last code in the function. argsSize is the
 * number of  bytes for the arguments to the function (not including the the api
 * parameter). The "stdcall" bool variable determines if the function was called using
 * the stdcall or cdecl calling convention.
 */
#define INTERCEPT_EPILOG_NO_RETURN(argsSize)        \
    __asm                                           \
    {                                               \
        __asm cmp     stdcall,        0             \
        __asm mov     esp,            ebp           \
        __asm pop     ebp                           \
        __asm jne     stdcall_ret                   \
        __asm ret     4                             \
    __asm stdcall_ret:                              \
        __asm ret     (4 + argsSize)                \
    }


LoadLibraryExW_t                LoadLibraryExW_dll      = NULL;
LdrLockLoaderLock_t             LdrLockLoaderLock_dll   = NULL;
LdrUnlockLoaderLock_t           LdrUnlockLoaderLock_dll = NULL;

bool                            g_loadedLuaFunctions = false;
std::set<std::string>           g_loadedModules;
CriticalSection                 g_loadedModulesCriticalSection;

std::vector<LuaInterface>       g_interfaces;
stdext::hash_map<void*, void*>  g_hookedFunctionMap;

stdext::hash_set<std::string>   g_warnedAboutLua;   // Indivates that we've warned the module contains Lua functions but none were loaded.
stdext::hash_set<std::string>   g_warnedAboutPdb;   // Indicates that we've warned about a module having a mismatched PDB.
bool                            g_warnedAboutThreads = false;
bool                            g_warnedAboutJit     = false;

std::string                     g_symbolsDirectory;
static DWORD                    g_disableInterceptIndex = 0;
bool                            g_initializedDebugHelp = false; 

/**
 * Function called after a library has been loaded by the host application.
 * We use this to check for the Lua dll.
 */
void PostLoadLibrary(HMODULE hModule);

/**
 * Data structure passed into the MemoryReader function.
 */
struct Memory
{
    const char* buffer;
    size_t      size;
};

/**
 * lua_Reader function used to read from a memory buffer.
 */
const char* MemoryReader_cdecl(lua_State* L, void* data, size_t* size)
{
    
    Memory* memory = static_cast<Memory*>(data);
    
    if (memory->size > 0)
    {
        *size = memory->size;
        memory->size = 0;
        return memory->buffer;
    }
    else
    {
        return NULL;
    }

}

/**
 * lua_Reader function used to read from a memory buffer.
 */
const char* __stdcall MemoryReader_stdcall(lua_State* L, void* data, size_t* size)
{
    
    Memory* memory = static_cast<Memory*>(data);
    
    if (memory->size > 0)
    {
        *size = memory->size;
        memory->size = 0;
        return memory->buffer;
    }
    else
    {
        return NULL;
    }

}

#pragma auto_inline(off)
int DecodaOutputWorker(unsigned long api, lua_State* L, bool& stdcall)
{

    stdcall = g_interfaces[api].stdcall;

    const char* message = lua_tostring_dll(api, L, 1);
    DebugBackend::Get().Message(message);
    
    return 0;

}
#pragma auto_inline()

__declspec(naked) int DecodaOutput(unsigned long api, lua_State* L)
{

    int result;
    bool stdcall;

    INTERCEPT_PROLOG()

    result = DecodaOutputWorker(api, L, stdcall);

    INTERCEPT_EPILOG(4)

}

#pragma auto_inline(off)
int CPCallHandlerWorker(unsigned long api, lua_State* L, bool& stdcall)
{
    
    stdcall = g_interfaces[api].stdcall;
    CPCallHandlerArgs args = *static_cast<CPCallHandlerArgs*>(lua_touserdata_dll(api, L, 1));

    // Remove the old args and put the new one on the stack.
    lua_pop_dll(api, L, 1);
    lua_pushlightuserdata_dll(api, L, args.data);

    return args.function(api, L);

}
#pragma auto_inline()

__declspec(naked) int CPCallHandler(unsigned long api, lua_State* L)
{

    int result;
    bool stdcall;
        
    INTERCEPT_PROLOG()
    
    result = CPCallHandlerWorker(api, L, stdcall);

    INTERCEPT_EPILOG(4)

}

int lua_cpcall_dll(unsigned long api, lua_State *L, lua_CFunction_dll func, void *udn)
{

    CPCallHandlerArgs args;

    args.function   = func;
    args.data       = udn;

    // lua_cpcall is deprecated in Lua 5.2, so do as if it didn't exist
    lua_pushcfunction_dll(api, L, g_interfaces[api].CPCallHandler);
    lua_pushlightuserdata_dll( api, L, &args);
    return lua_pcall_dll(api, L, 1, 0, 0);
}

#pragma auto_inline(off)
void HookHandlerWorker(unsigned long api, lua_State* L, lua_Debug* ar,  bool& stdcall)
{
    stdcall = g_interfaces[api].stdcall;
    return DebugBackend::Get().HookCallback(api, L, ar);
}
#pragma auto_inline()

__declspec(naked) void HookHandler(unsigned long api, lua_State* L, lua_Debug* ar)
{

    bool stdcall;

    INTERCEPT_PROLOG()
    
    HookHandlerWorker(api, L, ar, stdcall);

    INTERCEPT_EPILOG_NO_RETURN(8)

}

void SetHookMode(unsigned long api, lua_State* L, HookMode mode)
{

  if(mode == HookMode_None)
  {
      lua_sethook_dll(api, L, NULL, 0, 0);
  }
  else
  {
      int mask;

      switch (mode)
      {
      case HookMode_CallsOnly:
        mask = LUA_MASKCALL;
        break;
      case HookMode_CallsAndReturns:
        mask = LUA_MASKCALL|LUA_MASKRET;
        break;
      case HookMode_Full:
        mask = LUA_MASKCALL|LUA_MASKRET|LUA_MASKLINE;
        break;
      }

      lua_sethook_dll(api, L, g_interfaces[api].HookHandler, mask, 0);
  }

  
}

int lua_gethookmask(unsigned long api, lua_State *L)
{
    if (g_interfaces[api].lua_gethookmask_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_gethookmask_dll_cdecl(L);
    }
    else
    {
        return g_interfaces[api].lua_gethookmask_dll_stdcall(L);
    }
}

HookMode GetHookMode(unsigned long api, lua_State* L)
{

  int mask = lua_gethookmask(api, L);

  if(mask == 0)
  {
    return HookMode_None;
  }
  else if(mask == (LUA_MASKCALL))
  {
    return HookMode_CallsOnly;
  }
  else if(mask == (LUA_MASKCALL|LUA_MASKRET))
  {
    return HookMode_CallsAndReturns;
  }
  else
  {
    return HookMode_Full;
  }  
}


bool GetIsHookEventRet( unsigned long api, int event)
{
    return event == LUA_HOOKRET || event == g_interfaces[api].hookTailRet;
}

bool GetIsHookEventCall( unsigned long api, int event)
{
    return event == LUA_HOOKCALL || event == g_interfaces[api].hookTailCall;
}

int GetEvent(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.event;
        default: return ar->ld51.event;
    }
}

int GetNups(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.nups;
        default: return ar->ld51.nups;
    }
}

int GetCurrentLine(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.currentline;
        default: return ar->ld51.currentline;
    }
}

int GetLineDefined(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.linedefined;
        default: return ar->ld51.linedefined;
    }
}

int GetLastLineDefined(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.lastlinedefined;
        default: return ar->ld51.lastlinedefined;
    }
}

const char* GetSource(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.source;
        default: return ar->ld51.source;
    }
}

const char* GetWhat(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: ar->ld52.what;
        default: return ar->ld51.what;
    }
}

const char* GetName(unsigned long api, const lua_Debug* ar)
{
    switch( g_interfaces[api].version)
    {
        case 520: return ar->ld52.name;
        default: return ar->ld51.name;
    }
}

const char* GetHookEventName(unsigned long api, const lua_Debug* ar)
{
    int event = GetEvent( api, ar);
    const char* eventType = "Unknown";

    if (event == LUA_HOOKLINE)
    {
        eventType = "LUA_HOOKLINE";
    }
    else if (event == LUA_HOOKRET)
    {
        eventType = "LUA_HOOKRET";
    }
    else if (event == g_interfaces[api].hookTailRet)
    {
      eventType = "LUA_HOOKTAILRET";
    }
    else if (event == g_interfaces[api].hookTailCall)
    {
        eventType = "LUA_HOOKTAILCALL";
    }
    else if (event == LUA_HOOKCALL)
    {
        eventType = "LUA_HOOKCALL";
    }
    return eventType;
}

bool lua_pushthread_dll(unsigned long api, lua_State *L)
{

    // These structures are taken out of the Lua 5.0 source code.

    union lua_Value_500
    {
        void*         gc;
        void*         p;
        lua_Number    n;
        int           b;
    };

    struct lua_TObject_500
    {
        int             tt;
        lua_Value_500   value;
    };

    struct lua_State_500
    {
        void*               next;
        unsigned char       tt;
        unsigned char       marked;
        lua_TObject_500*    top;
    };

    #pragma pack(1)

    union lua_Value_500_pack1
    {
        void*         gc;
        void*         p;
        lua_Number    n;
        int           b;
    };

    struct lua_TObject_500_pack1
    {
        int                         tt;
        lua_Value_500_pack1         value;
    };

    struct lua_State_500_pack1
    {
        void*                       next;
        unsigned char               tt;
        unsigned char               marked;
        lua_TObject_500_pack1*      top;
    };

    #pragma pack()

    if (g_interfaces[api].lua_pushthread_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushthread_dll_cdecl(L);
        return true;
    }
    else if (g_interfaces[api].lua_pushthread_dll_stdcall != NULL)
    {
        g_interfaces[api].lua_pushthread_dll_stdcall(L);
        return true;
    }
    else
    {

        // The actual push thread function doesn't exist (probably Lua 5.0), so
        // emulate it. The lua_pushthread function just pushes the state onto the
        // stack and sets the type to LUA_TTHREAD. We use the pushlightuserdata
        // function which basically does the same thing, except we need to modify the
        // type of the object on the top of the stack.

        lua_pushlightuserdata_dll(api, L, L);

        // Check that the thing we think is pointing to the top of the stack actually
        // is so that we don't overwrite something in memory.

        bool success = false;

        // If the structures are laid out differently in the implementation of Lua
        // we might get crashes, so we wrap the access in a try block.

        __try
        {
            lua_State_500* S = reinterpret_cast<lua_State_500*>(L);
            lua_TObject_500* top = S->top - 1;
            if (top->tt == LUA_TLIGHTUSERDATA && top->value.p == L)
            {
                top->tt = LUA_TTHREAD;
                top->value.gc = L;
                success = true;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        if (!success)
        {
            // The unpacked version didn't work out right, so try the version with no alignment.
            __try
            {
                lua_State_500_pack1* S = reinterpret_cast<lua_State_500_pack1*>(L);
                lua_TObject_500_pack1* top = S->top - 1;
                if (top->tt == LUA_TLIGHTUSERDATA && top->value.p == L)
                {
                    top->tt = LUA_TTHREAD;
                    top->value.gc = L;
                    success = true;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }
        }

        if (!success)
        {
            lua_pop_dll(api, L, 1);
            if (!g_warnedAboutThreads)
            {
                DebugBackend::Get().Message("Warning 1006: lua_pushthread could not be emulated due to modifications to Lua. Coroutines may be unstable", MessageType_Warning);
                g_warnedAboutThreads = true;
            }
        }

        return success;

    }

}

void* lua_newuserdata_dll(unsigned long api, lua_State *L, size_t size)
{
    if (g_interfaces[api].lua_newuserdata_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_newuserdata_dll_cdecl(L, size);
    }
    else
    {
        return g_interfaces[api].lua_newuserdata_dll_stdcall(L, size);
    }
}

void EnableIntercepts(bool enableIntercepts)
{
    int value = reinterpret_cast<int>(TlsGetValue(g_disableInterceptIndex));
    if (enableIntercepts)
    {
        --value;
    }
    else
    {
        ++value;
    }
    TlsSetValue(g_disableInterceptIndex, reinterpret_cast<LPVOID>(value));
}

bool GetAreInterceptsEnabled()
{
    int value = reinterpret_cast<int>(TlsGetValue(g_disableInterceptIndex));
    return value <= 0;
}

void RegisterDebugLibrary(unsigned long api, lua_State* L)
{
    lua_register_dll(api, L, "decoda_output", g_interfaces[api].DecodaOutput);
}

int GetGlobalsIndex(unsigned long api)
{
    assert( g_interfaces[api].version >= 500 && g_interfaces[api].version < 520);
    return g_interfaces[api].globalsIndex;
}

int GetRegistryIndex(unsigned long api)
{
    return g_interfaces[api].registryIndex;
}

int lua_absindex_dll(unsigned long api, lua_State* L, int i)
{
    if (g_interfaces[api].lua_absindex_dll_cdecl != NULL)
{
        return g_interfaces[api].lua_absindex_dll_cdecl(L, i);
    }
    else if (g_interfaces[api].lua_absindex_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_absindex_dll_stdcall(L, i);
    }

    // Older version of Lua without lua_absindex API, emulate the macro
    if (i > 0 || i <= GetRegistryIndex(api))
    {
        return i;
    }
    else
    {
        return lua_gettop_dll(api, L) + i + 1;
    }
}

int lua_upvalueindex_dll(unsigned long api, int i)
{
   if( g_interfaces[api].version >= 520)
    {
        return GetRegistryIndex(api) - i;
    }
    else 
    {
    return GetGlobalsIndex(api) - i;
}
}

void lua_setglobal_dll(unsigned long api, lua_State* L, const char* s)
{
    if (g_interfaces[api].lua_setglobal_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_setglobal_dll_cdecl(L, s);
    }
    else if (g_interfaces[api].lua_setglobal_dll_stdcall != NULL)
    {
        g_interfaces[api].lua_setglobal_dll_stdcall( L, s);
    }
    else
    {
    lua_setfield_dll(api, L, GetGlobalsIndex(api), s);
}
}

void lua_getglobal_dll(unsigned long api, lua_State* L, const char* s)
{
    if (g_interfaces[api].lua_getglobal_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_getglobal_dll_cdecl(L, s);
    }
    else if (g_interfaces[api].lua_getglobal_dll_stdcall != NULL)
    {
        g_interfaces[api].lua_getglobal_dll_stdcall( L, s);
    }
    else
    {
    lua_getfield_dll(api, L, GetGlobalsIndex(api), s);
}
}

void lua_rawgetglobal_dll(unsigned long api, lua_State* L, const char* s)
{
    lua_pushglobaltable_dll(api, L);
    int glb = lua_gettop_dll(api, L);
    lua_pushstring_dll(api, L, s);
    lua_rawget_dll(api, L, glb);
    lua_remove_dll(api, L, glb);
}

lua_State* lua_newstate_dll(unsigned long api, lua_Alloc f, void* ud)
{
    if (g_interfaces[api].lua_newstate_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_newstate_dll_cdecl(f, ud);
    }
    else if (g_interfaces[api].lua_newstate_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_newstate_dll_stdcall(f, ud);
    }
    
    // This is an older version of Lua that doesn't support lua_newstate, so emulate it
    // with lua_open.

    if (g_interfaces[api].lua_open_500_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_open_500_dll_cdecl();
    }
    else if (g_interfaces[api].lua_open_500_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_open_500_dll_stdcall();
    }
    else if (g_interfaces[api].lua_open_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_open_dll_cdecl(0);
    }
    else if (g_interfaces[api].lua_open_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_open_dll_stdcall(0);
    }

    assert(0);
    return NULL;

}

void lua_close_dll(unsigned long api, lua_State* L)
{
    if (g_interfaces[api].lua_close_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_close_dll_cdecl(L);
    }
    else
    {
        g_interfaces[api].lua_close_dll_stdcall(L);
    }
}

lua_State* lua_newthread_dll(unsigned long api, lua_State* L)
{
    if (g_interfaces[api].lua_newthread_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_newthread_dll_cdecl(L);
    }
    else
    {
        return g_interfaces[api].lua_newthread_dll_stdcall(L);
    }
}

int lua_error_dll(unsigned long api, lua_State* L)
{
    if (g_interfaces[api].lua_error_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_error_dll_cdecl(L);
    }
    else
    {
        return g_interfaces[api].lua_error_dll_stdcall(L);
    }
}

int lua_sethook_dll(unsigned long api, lua_State* L, lua_Hook f, int mask, int count)
{
    if (g_interfaces[api].lua_sethook_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_sethook_dll_cdecl(L, f, mask, count);
    }
    else
    {
        return g_interfaces[api].lua_sethook_dll_stdcall(L, (lua_Hook_stdcall)f, mask, count);
    }
}

int lua_getinfo_dll(unsigned long api, lua_State* L, const char* what, lua_Debug* ar)
{
    if (g_interfaces[api].lua_getinfo_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_getinfo_dll_cdecl(L, what, ar);
    }
    else
    {
        return g_interfaces[api].lua_getinfo_dll_stdcall(L, what, ar);
    }
}

void lua_remove_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_remove_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_remove_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_remove_dll_stdcall(L, index);
    }
}

void lua_settable_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_settable_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_settable_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_settable_dll_stdcall(L, index);
    }
}

void lua_gettable_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_gettable_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_gettable_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_gettable_dll_stdcall(L, index);
    }
}

void lua_rawget_dll(unsigned long api, lua_State* L, int idx)
{
    if (g_interfaces[api].lua_rawget_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_rawget_dll_cdecl(L, idx);
    }
    else
    {
        g_interfaces[api].lua_rawget_dll_stdcall(L, idx);
    }
}

void lua_rawgeti_dll(unsigned long api, lua_State *L, int idx, int n)
{
    if (g_interfaces[api].lua_rawgeti_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_rawgeti_dll_cdecl(L, idx, n);
    }
    else
    {
        g_interfaces[api].lua_rawgeti_dll_stdcall(L, idx, n);
    }
}

void lua_rawset_dll(unsigned long api, lua_State* L, int idx)
{
    if (g_interfaces[api].lua_rawset_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_rawset_dll_cdecl(L, idx);
    }
    else
    {
        g_interfaces[api].lua_rawset_dll_stdcall(L, idx);
    }
}

void lua_pushstring_dll(unsigned long api, lua_State* L, const char* s)
{
    if (g_interfaces[api].lua_pushstring_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushstring_dll_cdecl(L, s);
    }
    else
    {
        g_interfaces[api].lua_pushstring_dll_stdcall(L, s);
    }
}

void lua_pushlstring_dll(unsigned long api, lua_State* L, const char* s, size_t len)
{
    if (g_interfaces[api].lua_pushlstring_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushlstring_dll_cdecl(L, s, len);
    }
    else
    {
        g_interfaces[api].lua_pushlstring_dll_stdcall(L, s, len);
    }
}

int lua_type_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_type_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_type_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_type_dll_stdcall(L, index);
    }
}

const char* lua_typename_dll(unsigned long api, lua_State* L, int type)
{
    if (g_interfaces[api].lua_typename_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_typename_dll_cdecl(L, type);
    }
    else
    {
        return g_interfaces[api].lua_typename_dll_stdcall(L, type);
    }
}

int lua_checkstack_dll(unsigned long api, lua_State* L, int extra)
{
    if (g_interfaces[api].lua_checkstack_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_checkstack_dll_cdecl(L, extra);
    }
    else
    {
        return g_interfaces[api].lua_checkstack_dll_stdcall(L, extra);
    }
}

void lua_getfield_dll(unsigned long api, lua_State* L, int index, const char* k)
{

    // Since Lua 4.0 doesn't include lua_getfield, we just emulate its
    // behavior for simplicity.

    index = lua_absindex_dll(api, L, index);

    lua_pushstring_dll(api, L, k);
    lua_gettable_dll(api, L, index);

}

void lua_setfield_dll(unsigned long api, lua_State* L, int index, const char* k)
{

    // Since Lua 4.0 doesn't include lua_setfield, we just emulate its
    // behavior for simplicity.

    index = lua_absindex_dll(api, L, index);

    lua_pushstring_dll(api, L, k);
    lua_insert_dll(api, L, -2);
    lua_settable_dll(api, L, index);

}

void lua_settop_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_settop_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_settop_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_settop_dll_stdcall(L, index);
    }
}

const char* lua_getlocal_dll(unsigned long api, lua_State* L, const lua_Debug* ar, int n)
{
    if (g_interfaces[api].lua_getlocal_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_getlocal_dll_cdecl(L, ar, n);
    }
    else
    {
        return g_interfaces[api].lua_getlocal_dll_stdcall(L, ar, n);
    }
}

const char* lua_setlocal_dll(unsigned long api, lua_State* L, const lua_Debug* ar, int n)
{
    if (g_interfaces[api].lua_setlocal_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_setlocal_dll_cdecl(L, ar, n);
    }
    else
    {
        return g_interfaces[api].lua_setlocal_dll_stdcall(L, ar, n);
    }
}

int lua_getstack_dll(unsigned long api, lua_State* L, int level, lua_Debug* ar)
{
    if (g_interfaces[api].lua_getstack_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_getstack_dll_cdecl(L, level, ar);
    }
    else
    {
        return g_interfaces[api].lua_getstack_dll_stdcall(L, level, ar);
    }
}

void lua_insert_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_insert_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_insert_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_insert_dll_stdcall(L, index);
    }
}

void lua_pushnil_dll(unsigned long api, lua_State* L)
{
    if (g_interfaces[api].lua_pushnil_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushnil_dll_cdecl(L);
    }
    else
    {
        g_interfaces[api].lua_pushnil_dll_stdcall(L);
    }
}

void lua_pushcclosure_dll(unsigned long api, lua_State* L, lua_CFunction fn, int n)
{
    if (g_interfaces[api].lua_pushcclosure_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushcclosure_dll_cdecl(L, fn, n);
    }
    else
    {
        g_interfaces[api].lua_pushcclosure_dll_stdcall(L, fn, n);
    }
}

void lua_pushvalue_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_pushvalue_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushvalue_dll_cdecl(L, index);
    }
    else
    {
        g_interfaces[api].lua_pushvalue_dll_stdcall(L, index);
    }
}

void lua_pushnumber_dll(unsigned long api, lua_State* L, lua_Number value)
{
    if (g_interfaces[api].lua_pushnumber_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushnumber_dll_cdecl(L, value);
    }
    else
    {
        g_interfaces[api].lua_pushnumber_dll_stdcall(L, value);
    }
}

void lua_pushinteger_dll(unsigned long api, lua_State* L, int value)
{
    if (g_interfaces[api].lua_pushinteger_dll_cdecl != NULL ||
        g_interfaces[api].lua_pushinteger_dll_stdcall != NULL)
    {
        // Lua 5.0 version.
        if (g_interfaces[api].lua_pushinteger_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_pushinteger_dll_cdecl(L, value);
        }
        else
        {
            return g_interfaces[api].lua_pushinteger_dll_stdcall(L, value);
        }
    }
    else
    {
        // Fallback to lua_pushnumber on Lua 4.0.
        lua_pushnumber_dll(api, L, static_cast<lua_Number>(value));
    }
}

void lua_pushlightuserdata_dll(unsigned long api, lua_State* L, void* p)
{
    if (g_interfaces[api].lua_pushlightuserdata_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_pushlightuserdata_dll_cdecl(L, p);
    }
    else
    {
        g_interfaces[api].lua_pushlightuserdata_dll_stdcall(L, p);
    }
}

void lua_pushglobaltable_dll(unsigned long api, lua_State* L)
{
    if( g_interfaces[api].version >= 520)
    {
        lua_rawgeti_dll( api, L, GetRegistryIndex(api), g_interfaces[api].globalsIndex);
    }
    else
    {
        lua_pushvalue_dll( api, L, GetGlobalsIndex(api));
    }
}

const char* lua_tostring_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_tostring_dll_cdecl != NULL ||
        g_interfaces[api].lua_tostring_dll_stdcall != NULL)
    {
        // Lua 4.0 implementation.
        if (g_interfaces[api].lua_tostring_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tostring_dll_cdecl(L, index);
        }
        else
        {
            return g_interfaces[api].lua_tostring_dll_stdcall(L, index);
        }
    }
    else
    {
        // Lua 5.0 version.
        if (g_interfaces[api].lua_tolstring_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tolstring_dll_cdecl(L, index, NULL);
        }
        else
        {
            return g_interfaces[api].lua_tolstring_dll_stdcall(L, index, NULL);
        }
    }
}

const char* lua_tolstring_dll(unsigned long api, lua_State* L, int index, size_t* len)
{
    if (g_interfaces[api].lua_tolstring_dll_cdecl != NULL ||
        g_interfaces[api].lua_tolstring_dll_stdcall != NULL)
    {
        // Lua 5.0 version.
        if (g_interfaces[api].lua_tolstring_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tolstring_dll_cdecl(L, index, len);
        }
        else
        {
            return g_interfaces[api].lua_tolstring_dll_stdcall(L, index, len);
        }
    }
    else
    {
        // Lua 4.0 implementation. lua_tolstring doesn't exist, so we just use lua_tostring
        // and compute the length ourself. This means strings with embedded zeros doesn't work
        // in Lua 4.0.

        const char* string = NULL;

        if (g_interfaces[api].lua_tostring_dll_cdecl != NULL)
        {
            string = g_interfaces[api].lua_tostring_dll_cdecl(L, index);
        }
        else
        {
            string = g_interfaces[api].lua_tostring_dll_stdcall(L, index);
        }

        if (len)
        {
            if (string)
            {
                *len = strlen(string);
            }
            else
            {
                *len = 0;
            }
        }

        return string;

    }
}

int lua_toboolean_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_toboolean_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_toboolean_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_toboolean_dll_stdcall(L, index);
    }
}

int lua_tointeger_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_tointegerx_dll_cdecl != NULL ||
        g_interfaces[api].lua_tointegerx_dll_stdcall != NULL)
    {
        // Lua 5.2 implementation.
        if (g_interfaces[api].lua_tointegerx_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tointegerx_dll_cdecl(L, index, NULL);
        }
        else
        {
            return g_interfaces[api].lua_tointegerx_dll_stdcall(L, index, NULL);
        }
    }
    if (g_interfaces[api].lua_tointeger_dll_cdecl != NULL ||
        g_interfaces[api].lua_tointeger_dll_stdcall != NULL)
    {
        // Lua 5.0 implementation.
        if (g_interfaces[api].lua_tointeger_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tointeger_dll_cdecl(L, index);
        }
        else
        {
            return g_interfaces[api].lua_tointeger_dll_stdcall(L, index);
        }
    }
    else
    {
        // On Lua 4.0 fallback to lua_tonumber.
        return static_cast<int>(lua_tonumber_dll(api, L, index));
    }
}

lua_CFunction lua_tocfunction_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_tocfunction_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_tocfunction_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_tocfunction_dll_stdcall(L, index);
    }
}

lua_Number lua_tonumber_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_tonumberx_dll_cdecl != NULL ||
        g_interfaces[api].lua_tonumberx_dll_stdcall != NULL)
    {
        // Lua 5.2 implementation.
        if (g_interfaces[api].lua_tonumberx_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_tonumberx_dll_cdecl(L, index, NULL);
        }
        else
        {
            return g_interfaces[api].lua_tonumberx_dll_stdcall(L, index, NULL);
        }
    }
    // Lua 5.0 and earlier.
    if (g_interfaces[api].lua_tonumber_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_tonumber_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_tonumber_dll_stdcall(L, index);
    }
}

void* lua_touserdata_dll(unsigned long api, lua_State *L, int index)
{
    if (g_interfaces[api].lua_touserdata_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_touserdata_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_touserdata_dll_stdcall(L, index);
    }
}

int lua_gettop_dll(unsigned long api, lua_State* L)
{
    if (g_interfaces[api].lua_gettop_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_gettop_dll_cdecl(L);
    }
    else
    {
        return g_interfaces[api].lua_gettop_dll_stdcall(L);
    }
}

int lua_loadbuffer_dll(unsigned long api, lua_State* L, const char* buffer, size_t size, const char* chunkname, const char* mode)
{

    Memory memory;

    memory.buffer   = buffer;
    memory.size     = size;

    if (g_interfaces[api].lua_load_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_load_dll_cdecl(L, MemoryReader_cdecl, &memory, chunkname, mode);
    }
    else if (g_interfaces[api].lua_load_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_load_dll_stdcall(L, MemoryReader_stdcall, &memory, chunkname, mode);
    }
    else if (g_interfaces[api].lua_load_510_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_load_510_dll_cdecl(L, MemoryReader_cdecl, &memory, chunkname);
    }
    else if (g_interfaces[api].lua_load_510_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_load_510_dll_stdcall(L, MemoryReader_stdcall, &memory, chunkname);
    }

    assert(0);
    return 0;
}

void lua_call_dll(unsigned long api, lua_State* L, int nargs, int nresults)
{
    if (g_interfaces[api].lua_call_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_call_dll_cdecl(L, nargs, nresults);
    }
    else
    {
        return g_interfaces[api].lua_call_dll_stdcall(L, nargs, nresults);
    }
}

int lua_pcallk_dll(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k)
{
    if (g_interfaces[api].lua_pcallk_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_pcallk_dll_cdecl(L, nargs, nresults, errfunc, ctx, k);
    }
    else
    {
        return g_interfaces[api].lua_pcallk_dll_stdcall(L, nargs, nresults, errfunc, ctx, k);
    }
}

int lua_pcall_dll(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc)
{
    // Lua 5.2.
    if (g_interfaces[api].lua_pcallk_dll_cdecl != NULL ||
        g_interfaces[api].lua_pcallk_dll_stdcall != NULL)
    {
        return lua_pcallk_dll(api, L, nargs, nresults, errfunc, 0, NULL);
    }
    // Lua 5.1 and earlier.
    if (g_interfaces[api].lua_pcall_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_pcall_dll_cdecl(L, nargs, nresults, errfunc);
    }
    else
    {
        return g_interfaces[api].lua_pcall_dll_stdcall(L, nargs, nresults, errfunc);
    }
}

void lua_newtable_dll(unsigned long api, lua_State* L)
{

    if (g_interfaces[api].lua_newtable_dll_cdecl   != NULL ||
        g_interfaces[api].lua_newtable_dll_stdcall != NULL)
    {
        // Lua 4.0 implementation.
        if (g_interfaces[api].lua_newtable_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_newtable_dll_cdecl(L);
        }
        else
        {
            return g_interfaces[api].lua_newtable_dll_stdcall(L);
        }
    }
    else
    {
        // Lua 5.0 version.
        if (g_interfaces[api].lua_createtable_dll_cdecl != NULL)
        {
            g_interfaces[api].lua_createtable_dll_cdecl(L, 0, 0);
        }
        else
        {
            g_interfaces[api].lua_createtable_dll_stdcall(L, 0, 0);
        }
    }

}

int lua_next_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_next_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_next_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_next_dll_stdcall(L, index);
    }
}

int lua_rawequal_dll(unsigned long api, lua_State *L, int idx1, int idx2)
{
    if (g_interfaces[api].lua_rawequal_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_rawequal_dll_cdecl(L, idx1, idx2);
    }
    else
    {
        return g_interfaces[api].lua_rawequal_dll_stdcall(L, idx1, idx2);
    }
}

int lua_getmetatable_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_getmetatable_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_getmetatable_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_getmetatable_dll_stdcall(L, index);
    }
}

int lua_setmetatable_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_setmetatable_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_setmetatable_dll_cdecl(L, index);
    }
    else
    {
        return g_interfaces[api].lua_setmetatable_dll_stdcall(L, index);
    }
}

int luaL_ref_dll(unsigned long api, lua_State *L, int t)
{
    if (g_interfaces[api].luaL_ref_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_ref_dll_cdecl(L, t);
    }
    else if (g_interfaces[api].luaL_ref_dll_stdcall != NULL)
    {
        return g_interfaces[api].luaL_ref_dll_stdcall(L, t);
    }
    // We don't require that luaL_ref be present, so provide a suitable
    // implementation if it's not.
    return LUA_NOREF;
}

void luaL_unref_dll(unsigned long api, lua_State *L, int t, int ref)
{
    if (g_interfaces[api].luaL_unref_dll_cdecl != NULL)
    {
        g_interfaces[api].luaL_unref_dll_cdecl(L, t, ref);
    }
    else if (g_interfaces[api].luaL_ref_dll_stdcall != NULL)
    {
        g_interfaces[api].luaL_unref_dll_stdcall(L, t, ref);
    }
}

int luaL_newmetatable_dll(unsigned long api, lua_State *L, const char *tname)
{
    if (g_interfaces[api].luaL_newmetatable_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_newmetatable_dll_cdecl(L, tname);
    }
    else
    {
        return g_interfaces[api].luaL_newmetatable_dll_stdcall(L, tname);
    }
}

int luaL_loadbuffer_dll(unsigned long api, lua_State *L, const char *buff, size_t sz, const char *name)
{
    if (g_interfaces[api].luaL_loadbuffer_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_loadbuffer_dll_cdecl(L, buff, sz, name);
    }
    else
    {
        return g_interfaces[api].luaL_loadbuffer_dll_stdcall(L, buff, sz, name);
    }
}

int luaL_loadbufferx_dll(unsigned long api, lua_State *L, const char *buff, size_t sz, const char *name, const char* mode)
{
    if (g_interfaces[api].luaL_loadbufferx_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_loadbufferx_dll_cdecl(L, buff, sz, name, mode);
    }
    else
    {
        return g_interfaces[api].luaL_loadbufferx_dll_stdcall(L, buff, sz, name, mode);
    }
}

int luaL_loadfile_dll(unsigned long api, lua_State* L, const char* fileName)
{
    if (g_interfaces[api].luaL_loadfile_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_loadfile_dll_cdecl(L, fileName);
    }
    else
    {
        return g_interfaces[api].luaL_loadfile_dll_stdcall(L, fileName);
    }
}

int luaL_loadfilex_dll(unsigned long api, lua_State* L, const char* fileName, const char* mode)
{
    if (g_interfaces[api].luaL_loadfilex_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_loadfilex_dll_cdecl(L, fileName, mode);
    }
    else
    {
        return g_interfaces[api].luaL_loadfilex_dll_stdcall(L, fileName, mode);
    }
}

lua_State* luaL_newstate_dll(unsigned long api)
{
    if (g_interfaces[api].luaL_newstate_dll_cdecl != NULL)
    {
        return g_interfaces[api].luaL_newstate_dll_cdecl();
    }
    else
    {
        return g_interfaces[api].luaL_newstate_dll_stdcall();
    }
}

const lua_WChar* lua_towstring_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_towstring_dll_cdecl != NULL ||
        g_interfaces[api].lua_towstring_dll_stdcall != NULL)
    {
        if (g_interfaces[api].lua_towstring_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_towstring_dll_cdecl(L, index);
        }
        else
        {
            return g_interfaces[api].lua_towstring_dll_stdcall(L, index);
        }
    }
    else
    {
        // The application is not using LuaPlus, so just return NULL.
        return NULL;
    }
}

int lua_iswstring_dll(unsigned long api, lua_State* L, int index)
{
    if (g_interfaces[api].lua_iswstring_dll_cdecl != NULL ||
        g_interfaces[api].lua_iswstring_dll_stdcall != NULL)
    {
        if (g_interfaces[api].lua_iswstring_dll_cdecl != NULL)
        {
            return g_interfaces[api].lua_iswstring_dll_cdecl(L, index);
        }
        else
        {
            return g_interfaces[api].lua_iswstring_dll_stdcall(L, index);
        }
    }
    else
    {
        // The application is not using LuaPlus, so just return 0.
        return 0;
    }
}

const char* lua_getupvalue_dll(unsigned long api, lua_State *L, int funcindex, int n)
{
    if (g_interfaces[api].lua_getupvalue_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_getupvalue_dll_cdecl(L, funcindex, n);
    }
    else
    {
        return g_interfaces[api].lua_getupvalue_dll_stdcall(L, funcindex, n);
    }
}

const char* lua_setupvalue_dll(unsigned long api, lua_State *L, int funcindex, int n)
{
    if (g_interfaces[api].lua_setupvalue_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_setupvalue_dll_cdecl(L, funcindex, n);
    }
    else
    {
        return g_interfaces[api].lua_setupvalue_dll_stdcall(L, funcindex, n);
    }
}

void lua_getfenv_dll(unsigned long api, lua_State *L, int index)
{
    if (g_interfaces[api].lua_getfenv_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_getfenv_dll_cdecl(L, index);
    }
    else if (g_interfaces[api].lua_getfenv_dll_stdcall != NULL)
    {
        g_interfaces[api].lua_getfenv_dll_stdcall(L, index);
    }
    else // no lua_setfenv: Lua 5.2+ uses an upvalue named _ENV instead
    {
        index = lua_absindex_dll( api, L, index);
        // look for it
        int upidx = 1;
        const char* upname = NULL;
        while ((upname = lua_getupvalue_dll( api, L, index, upidx)) != NULL && strcmp( upname, "_ENV") != 0)
        {
            lua_pop_dll( api, L, 1);
            ++ upidx;
        }
        // _ENV is left on the stack (or not)
        if (upname == NULL)
        {
            lua_pushnil_dll( api, L);
        }
    }
}

int lua_setfenv_dll(unsigned long api, lua_State *L, int index)
{
    if (g_interfaces[api].lua_setfenv_dll_cdecl != NULL)
    {
        return g_interfaces[api].lua_setfenv_dll_cdecl(L, index);
    }
    else if (g_interfaces[api].lua_setfenv_dll_stdcall != NULL)
    {
        return g_interfaces[api].lua_setfenv_dll_stdcall(L, index);
    }
    else // no lua_setfenv: Lua 5.2+ uses an upvalue named _ENV instead
{
        index = lua_absindex_dll( api, L, index);
        // look for it
        int upidx = 1;
        const char* upname = NULL;
        while( (upname = lua_getupvalue_dll( api, L, index, upidx)) != NULL && strcmp( upname, "_ENV") != 0)
    {
            lua_pop_dll( api, L, 1);
            ++ upidx;
    }
        if (upname != NULL)
    {
            lua_pop_dll( api, L, 1); // pop the actual value, we are only interested in its index
            lua_setupvalue_dll( api, L, index, upidx);
            return 1;
        }
        // "If the value at the given index is neither a function nor a thread nor a userdata, lua_setfenv returns 0."
        lua_pop_dll( api, L, 1);
        return 0;
    }
}

HMODULE WINAPI LoadLibraryExW_intercept(LPCWSTR fileName, HANDLE hFile, DWORD dwFlags)
{

    // We have to call the loader lock (if it is available) so that we don't get deadlocks
    // in the case where Dll initialization acquires the loader lock and calls LoadLibrary
    // while another thread is inside PostLoadLibrary.

    ULONG cookie;

    if (LdrLockLoaderLock_dll   != NULL &&
        LdrUnlockLoaderLock_dll != NULL)
    {
        LdrLockLoaderLock_dll(0, 0, &cookie);
    }

    HMODULE hModule = LoadLibraryExW_dll(fileName, hFile, dwFlags);

    if (hModule != NULL)
    {
        PostLoadLibrary(hModule);
    }

    if (LdrLockLoaderLock_dll   != NULL &&
        LdrUnlockLoaderLock_dll != NULL)
    {
        LdrUnlockLoaderLock_dll(0, cookie);
    }

    return hModule;

}

void FinishLoadingLua(unsigned long api, bool stdcall)
{

    #define SET_STDCALL(function)                                                                                                               \
        if ( g_interfaces[api].function##_dll_cdecl != NULL) {                                                                                  \
             g_interfaces[api].function##_dll_stdcall = reinterpret_cast<function##_stdcall_t>(g_interfaces[api].function##_dll_cdecl);         \
             g_interfaces[api].function##_dll_cdecl   = NULL;                                                                                   \
        }

    if (g_interfaces[api].finishedLoading)
    {
        return;
    }

    g_interfaces[api].stdcall = stdcall;

    if (stdcall)
    {
        SET_STDCALL(lua_newstate);
        SET_STDCALL(lua_open);
        SET_STDCALL(lua_open_500);
        SET_STDCALL(lua_newstate);
        SET_STDCALL(lua_newthread);
        SET_STDCALL(lua_close);
        SET_STDCALL(lua_error);
        SET_STDCALL(lua_absindex);
        SET_STDCALL(lua_sethook);
        SET_STDCALL(lua_getinfo);
        SET_STDCALL(lua_remove);
        SET_STDCALL(lua_settable);
        SET_STDCALL(lua_gettable);
        SET_STDCALL(lua_setglobal);
        SET_STDCALL(lua_getglobal);
        SET_STDCALL(lua_rawget);
        SET_STDCALL(lua_rawgeti);
        SET_STDCALL(lua_rawset);
        SET_STDCALL(lua_pushstring);
        SET_STDCALL(lua_pushlstring);
        SET_STDCALL(lua_type);
        SET_STDCALL(lua_typename);
        SET_STDCALL(lua_settop);
        SET_STDCALL(lua_gettop);
        SET_STDCALL(lua_getlocal);
        SET_STDCALL(lua_setlocal);
        SET_STDCALL(lua_getstack);
        SET_STDCALL(lua_insert);
        SET_STDCALL(lua_pushnil);
        SET_STDCALL(lua_pushvalue);
        SET_STDCALL(lua_pushinteger);
        SET_STDCALL(lua_pushnumber);
        SET_STDCALL(lua_pushcclosure);
        SET_STDCALL(lua_pushlightuserdata);
        SET_STDCALL(lua_tostring);
        SET_STDCALL(lua_tolstring);
        SET_STDCALL(lua_toboolean);
        SET_STDCALL(lua_tointeger);
        SET_STDCALL(lua_tointegerx);
        SET_STDCALL(lua_tocfunction);
        SET_STDCALL(lua_tonumber);
        SET_STDCALL(lua_tonumberx);
        SET_STDCALL(lua_touserdata);
        SET_STDCALL(lua_call);
        SET_STDCALL(lua_callk);
        SET_STDCALL(lua_pcall);
        SET_STDCALL(lua_pcallk);
        SET_STDCALL(lua_newtable);
        SET_STDCALL(lua_createtable);
        SET_STDCALL(lua_load);
        SET_STDCALL(lua_load_510);
        SET_STDCALL(lua_next);
        SET_STDCALL(lua_rawequal);
        SET_STDCALL(lua_getmetatable);
        SET_STDCALL(lua_setmetatable);
        SET_STDCALL(luaL_ref);
        SET_STDCALL(luaL_unref);
        SET_STDCALL(luaL_newmetatable);
        SET_STDCALL(luaL_loadbuffer);
        SET_STDCALL(luaL_loadbufferx);
        SET_STDCALL(luaL_loadfile);
        SET_STDCALL(luaL_loadfilex);
        SET_STDCALL(lua_getupvalue);
        SET_STDCALL(lua_setupvalue);
        SET_STDCALL(lua_getfenv);
        SET_STDCALL(lua_setfenv);
        SET_STDCALL(lua_pushthread);
        SET_STDCALL(lua_newuserdata);
        SET_STDCALL(lua_pushthread);
        SET_STDCALL(lua_checkstack);
    }
        
    g_interfaces[api].finishedLoading = true;

    DebugBackend::Get().CreateApi(api);

}

#pragma auto_inline(off)
void lua_call_worker(unsigned long api, lua_State* L, int nargs, int nresults, bool& stdcall)
{

    if (!g_interfaces[api].finishedLoading)
    {
        
        int result;
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_call_dll_cdecl, (void*)L, (void*)nargs, (void*)nresults, (void**)&result);

        FinishLoadingLua(api, stdcall);
        DebugBackend::Get().AttachState(api, L);
    
    }
    else
    {

        DebugBackend::Get().AttachState(api, L);

        if (g_interfaces[api].lua_call_dll_cdecl != NULL)
        {
            stdcall = false;
        }
        else if (g_interfaces[api].lua_call_dll_stdcall != NULL)
        {
            stdcall = true;
        }

        if (lua_gettop_dll(api, L) < nargs + 1)
        {
            DebugBackend::Get().Message("Warning 1005: lua_call called with too few arguments on the stack", MessageType_Warning);
        }

        if (DebugBackend::Get().Call(api, L, nargs, nresults, 0))
        {
            lua_error_dll(api, L);
        }

    }

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) void lua_call_intercept(unsigned long api, lua_State* L, int nargs, int nresults)
{
    
    bool stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    lua_call_worker(api, L, nargs, nresults, stdcall);

    INTERCEPT_EPILOG_NO_RETURN(12)

}

#pragma auto_inline(off)
void lua_callk_worker(unsigned long api, lua_State* L, int nargs, int nresults, int ctk, lua_CFunction k, bool& stdcall)
{

    if (!g_interfaces[api].finishedLoading)
    {
        
        int result;
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_callk_dll_cdecl, (void*)L, (void*)nargs, (void*)nresults, (void*)ctk, (void*)k, (void**)&result);

        FinishLoadingLua(api, stdcall);
        DebugBackend::Get().AttachState(api, L);
    
    }
    else
    {

        DebugBackend::Get().AttachState(api, L);

        if (g_interfaces[api].lua_callk_dll_cdecl != NULL)
        {
            stdcall = false;
        }
        else if (g_interfaces[api].lua_callk_dll_stdcall != NULL)
        {
            stdcall = true;
        }

        if (lua_gettop_dll(api, L) < nargs + 1)
        {
            DebugBackend::Get().Message("Warning 1005: lua_call called with too few arguments on the stack", MessageType_Warning);
        }

        if (DebugBackend::Get().Call(api, L, nargs, nresults, 0))
        {
            lua_error_dll(api, L);
        }

    }

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) void lua_callk_intercept(unsigned long api, lua_State* L, int nargs, int nresults, int ctx, lua_CFunction k)
{
    
    bool stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    lua_callk_worker(api, L, nargs, nresults, ctx, k, stdcall);

    INTERCEPT_EPILOG_NO_RETURN(20)

}


#pragma auto_inline(off)
int lua_pcall_worker(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc, bool& stdcall)
{

    int result;

    if (!g_interfaces[api].finishedLoading)
    {
        
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_pcall_dll_cdecl, (void*)L, (void*)nargs, (void*)nresults, (void*)errfunc, (void**)&result);

        FinishLoadingLua(api, stdcall);
        DebugBackend::Get().AttachState(api, L);
    
    }
    else
    {

        DebugBackend::Get().AttachState(api, L);

        if (g_interfaces[api].lua_pcall_dll_cdecl != NULL)
        {
            stdcall = false;
        }
        else if (g_interfaces[api].lua_pcall_dll_stdcall != NULL)
        {
            stdcall = true;
        }

        if (lua_gettop_dll(api, L) < nargs + 1)
        {
            DebugBackend::Get().Message("Warning 1005: lua_pcall called with too few arguments on the stack", MessageType_Warning);
        }

        if (GetAreInterceptsEnabled())
        {
            result = DebugBackend::Get().Call(api, L, nargs, nresults, errfunc);
        }
        else
        {
            result = lua_pcall_dll(api, L, nargs, nresults, errfunc);
        }

    }

    return result;    

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int lua_pcall_intercept(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_pcall_worker(api, L, nargs, nresults, errfunc, stdcall);

    INTERCEPT_EPILOG(16)

}

#pragma auto_inline(off)
int lua_pcallk_worker(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k, bool& stdcall)
{

    int result;

    if (!g_interfaces[api].finishedLoading)
    {
        
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_pcall_dll_cdecl, (void*)L, (void*)nargs,
            (void*)nresults, (void*)errfunc, (void*)ctx, (void*)k, (void**)&result);

        FinishLoadingLua(api, stdcall);
        DebugBackend::Get().AttachState(api, L);
    
    }
    else
    {

        DebugBackend::Get().AttachState(api, L);

        if (g_interfaces[api].lua_pcallk_dll_cdecl != NULL)
        {
            stdcall = false;
        }
        else if (g_interfaces[api].lua_pcallk_dll_stdcall != NULL)
        {
            stdcall = true;
        }

        if (lua_gettop_dll(api, L) < nargs + 1)
        {
            DebugBackend::Get().Message("Warning 1005: lua_pcallk called with too few arguments on the stack", MessageType_Warning);
        }

        if (GetAreInterceptsEnabled())
        {
            result = DebugBackend::Get().Call(api, L, nargs, nresults, errfunc);
        }
        else
        {
            result = lua_pcallk_dll(api, L, nargs, nresults, errfunc, ctx, k);
        }

    }

    return result;    

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int lua_pcallk_intercept(unsigned long api, lua_State* L, int nargs, int nresults, int errfunc, int ctx, lua_CFunction k)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_pcallk_worker(api, L, nargs, nresults, errfunc, ctx, k, stdcall);

    INTERCEPT_EPILOG(24)

}

#pragma auto_inline(off)
lua_State* lua_newstate_worker(unsigned long api, lua_Alloc f, void* ud, bool& stdcall)
{

    lua_State* result = NULL;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_newstate_dll_cdecl, (void*)f, ud, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].lua_newstate_dll_cdecl != NULL)
    {
        result = g_interfaces[api].lua_newstate_dll_cdecl(f, ud);
        stdcall = false;
    }
    else if (g_interfaces[api].lua_newstate_dll_stdcall != NULL)
    {
        result = g_interfaces[api].lua_newstate_dll_stdcall(f, ud);
        stdcall = true;
    }
    
    if (result != NULL)
    {
        DebugBackend::Get().AttachState(api, result);
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) lua_State* lua_newstate_intercept(unsigned long api, lua_Alloc f, void* ud)
{

    lua_State*      result;
    bool            stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_newstate_worker(api, f, ud, stdcall);

    INTERCEPT_EPILOG(8)

}

#pragma auto_inline(off)
lua_State* lua_newthread_worker(unsigned long api, lua_State* L, bool& stdcall)
{
    
    lua_State* result = NULL;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_newthread_dll_cdecl, L, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].lua_newthread_dll_cdecl != NULL)
    {
        result = g_interfaces[api].lua_newthread_dll_cdecl(L);
        stdcall = false;
    }
    else if (g_interfaces[api].lua_newthread_dll_stdcall != NULL)
    {
        result = g_interfaces[api].lua_newthread_dll_stdcall(L);
        stdcall = true;
    }
    
    if (result != NULL)
    {
        DebugBackend::Get().AttachState(api, result);
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) lua_State* lua_newthread_intercept(unsigned long api, lua_State* L)
{

    lua_State*      result;
    bool            stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_newthread_worker(api, L, stdcall);

    INTERCEPT_EPILOG(4)

}

#pragma auto_inline(off)
lua_State* lua_open_worker(unsigned long api, int stacksize, bool& stdcall)
{

    lua_State* result = NULL;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_open_dll_cdecl, (void*)stacksize, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].lua_open_dll_cdecl != NULL)
    {
        result = g_interfaces[api].lua_open_dll_cdecl(stacksize);
        stdcall = false;
    }
    else if (g_interfaces[api].lua_open_dll_stdcall != NULL)
    {
        result = g_interfaces[api].lua_open_dll_stdcall(stacksize);
        stdcall = true;
    }
    
    if (result != NULL)
    {
        DebugBackend::Get().AttachState(api, result);
    }

    return result;

}
#pragma auto_inline()

#pragma auto_inline(off)
lua_State* lua_open_500_worker(unsigned long api, bool& stdcall)
{

    lua_State* result = NULL;

    if (!g_interfaces[api].finishedLoading)
    {

        // We can't test stdcall with the Lua 5.0 lua_open function since it doesn't
        // take any arguments. To do the test, we create a dummy state and destroy it
        // using the lua_close function to do the test.
        
        lua_State* L = g_interfaces[api].lua_open_500_dll_cdecl();
        stdcall = GetIsStdCallConvention( g_interfaces[api].lua_close_dll_cdecl, (void*)L, (void**)&result);
        FinishLoadingLua(api, stdcall);

    }

    if (g_interfaces[api].lua_open_500_dll_cdecl != NULL)
    {
        result = g_interfaces[api].lua_open_500_dll_cdecl();
        stdcall = false;
    }
    else if (g_interfaces[api].lua_open_500_dll_stdcall != NULL)
    {
        result = g_interfaces[api].lua_open_500_dll_stdcall();
        stdcall = true;
    }
    
    if (result != NULL)
    {
        DebugBackend::Get().AttachState(api, result);
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) lua_State* lua_open_intercept(unsigned long api, int stacksize)
{

    lua_State*      result;
    bool            stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_open_worker(api, stacksize, stdcall);

    INTERCEPT_EPILOG(4)

}

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) lua_State* lua_open_500_intercept(unsigned long api)
{

    lua_State*      result;
    bool            stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_open_500_worker(api, stdcall);

    INTERCEPT_EPILOG(0)

}

#pragma auto_inline(off)
int lua_load_worker(unsigned long api, lua_State* L, lua_Reader reader, void* data, const char* name, const char* mode, bool& stdcall)
{

    // If we haven't finished loading yet this will be wrong, but we'll fix it up
    // when we access the reader function.
    stdcall = (g_interfaces[api].lua_load_dll_stdcall != NULL);

    // Read all of the data out of the reader and into a big buffer.

    std::vector<char> buffer;

    const char* chunk;
    size_t chunkSize;

    do
    {

        if (!g_interfaces[api].finishedLoading)
        {
            // In this case we must have attached the debugger so we're intercepting a lua_load
            // function before we've initialized.
            stdcall = GetIsStdCallConvention(reader, L, data, &chunkSize, (void**)&chunk);
            FinishLoadingLua(api, stdcall);
        }
        else if (stdcall)
        {
            // We assume that since the lua_load function is stdcall the reader function is as well.
            chunk = reinterpret_cast<lua_Reader_stdcall>(reader)(L, data, &chunkSize);
        }
        else
        {
            // We assume that since the lua_load function is cdecl the reader function is as well.
            chunk = reader(L, data, &chunkSize);
        }

        // We allow the reader to return 0 for the chunk size since Lua supports
        // that, although according to the manual it should return NULL to signal
        // the end of the data.

        if (chunk != NULL && chunkSize > 0)
        {
            buffer.insert(buffer.end(), chunk, chunk + chunkSize);
        }

    }
    while (chunk != NULL && chunkSize > 0);

    const char* source = NULL;
    
    if (!buffer.empty())
    {
        source = &buffer[0];
    }

    // Make sure the debugger knows about this state. This is necessary since we might have
    // attached the debugger after the state was created.
    DebugBackend::Get().AttachState(api, L);

    // Disables JIT compilation if LuaJIT is being used. Otherwise we won't get hooks for
    // this chunk.
    if (DebugBackend::Get().EnableJit(api, L, false))
    {
        if (!g_warnedAboutJit)
        {
            DebugBackend::Get().Message("Warning 1007: Just-in-time compilation of Lua code disabled to allow debugging", MessageType_Warning);
            g_warnedAboutJit = true;
        }
    }

    int result = lua_loadbuffer_dll(api, L, source, buffer.size(), name, mode);

    if (!buffer.empty())
    {
        result = DebugBackend::Get().PostLoadScript(api, result, L, source, buffer.size(), name);
    }

    return result;
   
}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int lua_load_510_intercept(unsigned long api, lua_State* L, lua_Reader reader, void* data, const char* name)
{
    
    bool    stdcall;
    int     result;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_load_worker(api, L, reader, data, name, NULL, stdcall);

    INTERCEPT_EPILOG(16)

}

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int lua_load_intercept(unsigned long api, lua_State* L, lua_Reader reader, void* data, const char* name, const char* mode)
{
    
    bool    stdcall;
    int     result;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_load_worker(api, L, reader, data, name, mode, stdcall);

    INTERCEPT_EPILOG(20)

}

#pragma auto_inline(off)
void lua_close_worker(unsigned long api, lua_State* L, bool& stdcall)
{
    
    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].lua_close_dll_cdecl, L, NULL);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].lua_close_dll_cdecl != NULL)
    {
        g_interfaces[api].lua_close_dll_cdecl(L);
        stdcall = false;
    }
    else if (g_interfaces[api].lua_close_dll_stdcall != NULL)
    {
        g_interfaces[api].lua_close_dll_stdcall(L);
        stdcall = true;
    }

    DebugBackend::Get().DetachState(api, L);
    
}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) void lua_close_intercept(unsigned long api, lua_State* L)
{
    
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    lua_close_worker(api, L, stdcall);

    INTERCEPT_EPILOG_NO_RETURN(4)

}

#pragma auto_inline(off)
int luaL_newmetatable_worker(unsigned long api, lua_State *L, const char* tname, bool& stdcall)
{

    int result;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].luaL_newmetatable_dll_cdecl, L, (void*)tname, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].luaL_newmetatable_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_newmetatable_dll_cdecl(L, tname);
        stdcall = false;
    }
    else if (g_interfaces[api].luaL_newmetatable_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_newmetatable_dll_stdcall(L, tname);
        stdcall = true;
    }

    if (result != 0)
    {
        // Only register if we haven't seen this name before.
        DebugBackend::Get().RegisterClassName(api, L, tname, lua_gettop_dll(api, L));
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int luaL_newmetatable_intercept(unsigned long api, lua_State* L, const char* tname)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = luaL_newmetatable_worker(api, L, tname, stdcall);

    INTERCEPT_EPILOG(8)

}

#pragma auto_inline(off)
int lua_sethook_worker(unsigned long api, lua_State *L, lua_Hook f, int mask, int count, bool& stdcall)
{
    
    // Currently we're using the hook and can't let anyone else use it.
    // What we should do is implement the lua hook on top of our existing hook.
        
    int result = 0;
 
    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].lua_sethook_dll_cdecl, L, f, (void*)mask, (void*)count, (void**)&result);
        FinishLoadingLua(api, stdcall);
        DebugBackend::Get().AttachState(api, L);
    }
    else
    {
        if (g_interfaces[api].luaL_newmetatable_dll_cdecl != NULL)
        {
            stdcall = false;
        }
        else if (g_interfaces[api].luaL_newmetatable_dll_stdcall != NULL)
        {
            stdcall = true;
        }
        // Note, the lua_hook call is currently bypassed.
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int lua_sethook_intercept(unsigned long api, lua_State *L, lua_Hook f, int mask, int count)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = lua_sethook_worker(api, L, f, mask, count, stdcall);

    INTERCEPT_EPILOG(16)

}

#pragma auto_inline(off)
int luaL_loadbufferx_worker(unsigned long api, lua_State *L, const char *buff, size_t sz, const char *name, const char* mode, bool& stdcall)
{

    int result = 0;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].luaL_loadbuffer_dll_cdecl, L, (void*)buff, (void*)sz, (void*)name, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].luaL_loadbufferx_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_loadbufferx_dll_cdecl(L, buff, sz, name, mode);
        stdcall = false;
    }
    else if (g_interfaces[api].luaL_loadbufferx_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_loadbufferx_dll_stdcall(L, buff, sz, name, mode);
        stdcall = true;
    }
    else if (g_interfaces[api].luaL_loadbuffer_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_loadbuffer_dll_cdecl(L, buff, sz, name);
        stdcall = false;
    }
    else if (g_interfaces[api].luaL_loadbuffer_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_loadbuffer_dll_stdcall(L, buff, sz, name);
        stdcall = true;
    }

    // Make sure the debugger knows about this state. This is necessary since we might have
    // attached the debugger after the state was created.
    DebugBackend::Get().AttachState(api, L);

    return DebugBackend::Get().PostLoadScript(api, result, L, buff, sz, name);

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int luaL_loadbuffer_intercept(unsigned long api, lua_State *L, const char *buff, size_t sz, const char *name)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interfering with the inline assembly and other strange
    // aspects of this function.
    result = luaL_loadbufferx_worker(api, L, buff, sz, name, NULL, stdcall);

    INTERCEPT_EPILOG(16)

}

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int luaL_loadbufferx_intercept(unsigned long api, lua_State *L, const char *buff, size_t sz, const char *name, const char* mode)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interfering with the inline assembly and other strange
    // aspects of this function.
    result = luaL_loadbufferx_worker(api, L, buff, sz, name, mode, stdcall);

    INTERCEPT_EPILOG(20)

}

#pragma auto_inline(off)
int luaL_loadfilex_worker(unsigned long api, lua_State *L, const char *fileName, const char* mode, bool& stdcall)
{

    int result = 0;

    if (!g_interfaces[api].finishedLoading)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].luaL_loadfile_dll_cdecl, L, (void*)fileName, (void**)&result);
        FinishLoadingLua(api, stdcall);
    }
    else if (g_interfaces[api].luaL_loadfilex_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_loadfilex_dll_cdecl(L, fileName, mode);
        stdcall = false;
    }
    else if (g_interfaces[api].luaL_loadfilex_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_loadfilex_dll_stdcall(L, fileName, mode);
        stdcall = true;
    }
    else if (g_interfaces[api].luaL_loadfile_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_loadfile_dll_cdecl(L, fileName);
        stdcall = false;
    }
    else if (g_interfaces[api].luaL_loadfile_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_loadfile_dll_stdcall(L, fileName);
        stdcall = true;
    }

    // Make sure the debugger knows about this state. This is necessary since we might have
    // attached the debugger after the state was created.
    DebugBackend::Get().AttachState(api, L);

    // Load the file.

    FILE* file = fopen(fileName, "rb");

    if (file != NULL)
    {

        std::string name = "@";
        name += fileName;

        fseek(file, 0, SEEK_END);
        unsigned int length = ftell(file);

        char* buffer = new char[length];
        fseek(file, 0, SEEK_SET);
        fread(buffer, 1, length, file);

        fclose(file);

        result = DebugBackend::Get().PostLoadScript(api, result, L, buffer, length, name.c_str());

        delete [] buffer;
    
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int luaL_loadfile_intercept(unsigned long api, lua_State *L, const char *fileName)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = luaL_loadfilex_worker(api, L, fileName, NULL, stdcall);

    INTERCEPT_EPILOG(8)

}

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) int luaL_loadfilex_intercept(unsigned long api, lua_State *L, const char *fileName, const char* mode)
{

    int     result;
    bool    stdcall;

    INTERCEPT_PROLOG()

    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = luaL_loadfilex_worker(api, L, fileName, mode, stdcall);

    INTERCEPT_EPILOG(12)

}

#pragma auto_inline(off)
lua_State* luaL_newstate_worker(unsigned long api, bool& stdcall)
{

    lua_State* result = NULL;

    if (g_interfaces[api].luaL_newstate_dll_cdecl != NULL)
    {
        result = g_interfaces[api].luaL_newstate_dll_cdecl();
    }
    else if (g_interfaces[api].luaL_newstate_dll_stdcall != NULL)
    {
        result = g_interfaces[api].luaL_newstate_dll_stdcall();
    }

    // Since we couldn't test if luaL_newstate was stdcall or cdecl (since it
    // doesn't have any arguments), call another function. lua_gettop is a good
    // choice since it has no side effects.

    if (!g_interfaces[api].finishedLoading && result != NULL)
    {
        stdcall = GetIsStdCallConvention(g_interfaces[api].lua_gettop_dll_cdecl, result, NULL);
        FinishLoadingLua(api, stdcall);
    }
    
    if (result != NULL)
    {
        DebugBackend::Get().AttachState(api, result);
    }

    return result;

}
#pragma auto_inline()

// This function cannot be called like a normal function. It changes its
// calling convention at run-time and removes and extra argument from the stack.
__declspec(naked) lua_State* luaL_newstate_intercept(unsigned long api)
{

    lua_State*      result;
    bool            stdcall;

    INTERCEPT_PROLOG()
    
    // We push the actual functionality of this function into a separate, "normal"
    // function so avoid interferring with the inline assembly and other strange
    // aspects of this function.
    result = luaL_newstate_worker(api, stdcall);

    INTERCEPT_EPILOG(0)

}

std::string GetEnvironmentVariable(const std::string& name)
{

    DWORD size = ::GetEnvironmentVariable(name.c_str(), NULL, 0);

    std::string result;

    if (size > 0)
    {
    
        char* buffer = new char[size];
        buffer[0] = 0;

        GetEnvironmentVariable(name.c_str(), buffer, size);

        result = buffer;
        delete [] buffer;

    }

    return result;

}

std::string GetApplicationDirectory()
{

    char fileName[_MAX_PATH];
    GetModuleFileNameEx(GetCurrentProcess(), NULL, fileName, _MAX_PATH);

    char* term = strrchr(fileName, '\\'); 

    if (term != NULL)
    {
        *term = 0;
    }

    return fileName;

}

bool LoadLuaFunctions(const stdext::hash_map<std::string, DWORD64>& symbols, HANDLE hProcess)
{

    #define GET_FUNCTION_OPTIONAL(function)                                                                                     \
        {                                                                                                                       \
            stdext::hash_map<std::string, DWORD64>::const_iterator iterator = symbols.find(#function);                          \
            if (iterator != symbols.end())                                                                                      \
            {                                                                                                                   \
                luaInterface.function##_dll_cdecl = reinterpret_cast<function##_cdecl_t>(iterator->second);                     \
            }                                                                                                                   \
        }

    #define GET_FUNCTION(function)                                                                                              \
        GET_FUNCTION_OPTIONAL(function)                                                                                         \
        if (luaInterface.function##_dll_cdecl == NULL)                                                                          \
        {                                                                                                                       \
            if (report)                                                                                                         \
            {                                                                                                                   \
                DebugBackend::Get().Message("Warning 1004: Couldn't hook Lua function '" #function "'", MessageType_Warning);   \
            }                                                                                                                   \
            return false;                                                                                                       \
        }

    #define HOOK_FUNCTION(function)                                                                                             \
        if (luaInterface.function##_dll_cdecl != NULL)                                                                          \
        {                                                                                                                       \
            void* original = luaInterface.function##_dll_cdecl;                                                                 \
            luaInterface.function##_dll_cdecl = (function##_cdecl_t)(HookFunction(original, function##_intercept, api));        \
        }

    LuaInterface luaInterface = { 0 };
    luaInterface.finishedLoading = false;
    luaInterface.stdcall         = false;

    unsigned long api = g_interfaces.size();

    bool report = false;

    // Check if the lua_tag function exists. This function is only in Lua 4.0 and not in Lua 5.0.
    // This helps us differentiate between those two versions.

    luaInterface.registryIndex  = 0;
    luaInterface.globalsIndex   = 0;
    luaInterface.hookTailCall = -1;
    luaInterface.hookTailRet = -1;

    if (symbols.find("lua_tag") != symbols.end())
    {
        luaInterface.version = 401;
    }
    else
    {
        if (symbols.find("lua_open") != symbols.end())
        {
            luaInterface.version        = 500;
            luaInterface.registryIndex  = -10000;
            luaInterface.globalsIndex   = -10001;
        }
        else if (symbols.find("lua_callk") != symbols.end())
        {
            luaInterface.version        = 520;
            // LUA_REGISTRYINDEX == LUAI_FIRSTPSEUDOIDX with LUAI_FIRSTPSEUDOIDX == (-LUAI_MAXSTACK - 1000) with LUAI_MAXSTACK == 15000 (for 32 bits build...)
            luaInterface.registryIndex  = -1001000;
            // starting with Lua 5.2, there is no longer a LUA_GLOBALSINDEX pseudo-index. Instead the global table is stored in the registry at LUA_RIDX_GLOBALS
            luaInterface.globalsIndex   = 2;
            luaInterface.hookTailCall = LUA_HOOKTAILCALL; // Lua5.2 has LUA_HOOKTAILCALL, but no LUA_HOOKTAILRET
        }
        else if ( symbols.find("lua_gettop") != symbols.end()) // should be ok for any version
        {
            luaInterface.version        = 510;
            luaInterface.registryIndex  = -10000;
            luaInterface.globalsIndex   = -10002;
            luaInterface.hookTailRet = LUA_HOOKTAILRET; // // Lua5.1 has LUA_HOOKTAILRET, but no LUA_HOOKTAILCALL
        }
        else // if we get here, this means the module isn't related to Lua at all
        {
            return false;
        }
    }
    
    // Only present in Lua 4.0 and Lua 5.0 (not 5.1)
    GET_FUNCTION_OPTIONAL(lua_open);
    if (luaInterface.lua_open_dll_cdecl == NULL)
    {
        GET_FUNCTION(lua_newstate);
    }

    // Start reporting errors about functions we couldn't hook.
    report = true;
    
    GET_FUNCTION(lua_newthread);
    GET_FUNCTION(lua_close);
    GET_FUNCTION(lua_error);
    GET_FUNCTION_OPTIONAL(lua_absindex); // Only present in Lua 5.2+
    GET_FUNCTION(lua_sethook);
    GET_FUNCTION(lua_getinfo);
    GET_FUNCTION(lua_remove);
    GET_FUNCTION(lua_settable);
    GET_FUNCTION(lua_gettable);
    GET_FUNCTION(lua_rawget);
    GET_FUNCTION(lua_rawgeti);
    GET_FUNCTION(lua_rawset);
    GET_FUNCTION(lua_pushstring);
    GET_FUNCTION(lua_pushlstring);
    GET_FUNCTION(lua_type);
    GET_FUNCTION(lua_typename);
    GET_FUNCTION(lua_settop);
    GET_FUNCTION(lua_gettop);
    GET_FUNCTION(lua_getlocal);
    GET_FUNCTION(lua_setlocal);
    GET_FUNCTION(lua_getstack);
    GET_FUNCTION(lua_insert);
    GET_FUNCTION(lua_pushnil);
    GET_FUNCTION(lua_pushvalue);
    GET_FUNCTION(lua_pushcclosure);
    GET_FUNCTION(lua_pushnumber);
    GET_FUNCTION(lua_pushlightuserdata);
    GET_FUNCTION(lua_checkstack);
    GET_FUNCTION(lua_gethookmask);

    // Only present in Lua 5.2+
    GET_FUNCTION_OPTIONAL(lua_getglobal);
    GET_FUNCTION_OPTIONAL(lua_setglobal);

    // Only present in Lua 5.1 (*number funtions used in Lua 4.0)
    GET_FUNCTION_OPTIONAL(lua_pushinteger);
    GET_FUNCTION_OPTIONAL(lua_tointeger);
    GET_FUNCTION_OPTIONAL(lua_tointegerx);

    // Only present in Lua 4.0 and 5.0 (exists as a macro in Lua 5.1)
    GET_FUNCTION_OPTIONAL(lua_tostring);

    if (luaInterface.lua_tostring_dll_cdecl == NULL)
    {
        GET_FUNCTION(lua_tolstring);
    }

    GET_FUNCTION_OPTIONAL(lua_tonumberx);

    if (luaInterface.lua_tonumberx_dll_cdecl == NULL)
    {
        // If the Lua 5.2 tonumber isn't present, require the previous version.
        GET_FUNCTION(lua_tonumber);
    }

    GET_FUNCTION(lua_toboolean);
    GET_FUNCTION(lua_tocfunction);
    GET_FUNCTION(lua_touserdata);
    
    // Exists as a macro in Lua 5.2
    GET_FUNCTION_OPTIONAL(lua_callk);
    if (luaInterface.lua_callk_dll_cdecl == NULL)
    {
        GET_FUNCTION(lua_call);
    }
    
    // Exists as a macro in Lua 5.2
    GET_FUNCTION_OPTIONAL(lua_pcallk);
    if (luaInterface.lua_pcallk_dll_cdecl == NULL)
    {
        GET_FUNCTION(lua_pcall);
    }
    
    // Only present in Lua 4.0 and 5.0 (exists as a macro in Lua 5.1)
    GET_FUNCTION_OPTIONAL(lua_newtable);
    if (luaInterface.lua_newtable_dll_cdecl == NULL)
    {
        GET_FUNCTION(lua_createtable);
    }

    GET_FUNCTION(lua_load);
    GET_FUNCTION(lua_next);
    GET_FUNCTION(lua_rawequal);
    GET_FUNCTION(lua_getmetatable);
    GET_FUNCTION(lua_setmetatable);
    GET_FUNCTION_OPTIONAL(luaL_ref);
    GET_FUNCTION_OPTIONAL(luaL_unref);
    GET_FUNCTION(luaL_newmetatable);
    GET_FUNCTION(lua_getupvalue);
    GET_FUNCTION(lua_setupvalue);

    // Only available in Lua 5.0/5.1
    GET_FUNCTION_OPTIONAL(lua_getfenv);
    GET_FUNCTION_OPTIONAL(lua_setfenv);

    if (luaInterface.version >= 510)
    {
        GET_FUNCTION(lua_pushthread);
    }
    else
    {
        // This function doesn't exist in Lua 5.0, so make it optional.
        GET_FUNCTION_OPTIONAL(lua_pushthread);
    }

    GET_FUNCTION(lua_newuserdata);

    // This function isn't strictly necessary. We only hook it
    // in case the base function was inlined.
    GET_FUNCTION_OPTIONAL(luaL_newstate);
    GET_FUNCTION_OPTIONAL(luaL_loadbuffer);
    GET_FUNCTION_OPTIONAL(luaL_loadfile);
    GET_FUNCTION_OPTIONAL(luaL_loadfilex);

    // These functions only exists in LuaPlus.
    GET_FUNCTION_OPTIONAL(lua_towstring);
    GET_FUNCTION_OPTIONAL(lua_iswstring);

    // Hook the functions we need to intercept calls to.

    if (luaInterface.version == 500)
    {
        luaInterface.lua_open_500_dll_cdecl = reinterpret_cast<lua_open_500_cdecl_t>(luaInterface.lua_load_dll_cdecl);
        luaInterface.lua_open_dll_cdecl = NULL;
    }

    if (luaInterface.version == 510)
    {
        luaInterface.lua_load_510_dll_cdecl = reinterpret_cast<lua_load_510_cdecl_t>(luaInterface.lua_load_dll_cdecl);
        luaInterface.lua_load_dll_cdecl = NULL;
    }

    HOOK_FUNCTION(lua_open);
    HOOK_FUNCTION(lua_open_500);
    HOOK_FUNCTION(lua_newstate);
    HOOK_FUNCTION(lua_close);
    HOOK_FUNCTION(lua_newthread);
    HOOK_FUNCTION(lua_pcall);
    HOOK_FUNCTION(lua_pcallk);
    HOOK_FUNCTION(lua_call);
    HOOK_FUNCTION(lua_callk);
    HOOK_FUNCTION(lua_load);
    HOOK_FUNCTION(lua_load_510);
    HOOK_FUNCTION(luaL_newmetatable);
    HOOK_FUNCTION(lua_sethook);

    HOOK_FUNCTION(luaL_loadbuffer);
    HOOK_FUNCTION(luaL_loadfile);
    HOOK_FUNCTION(luaL_loadfilex);
    HOOK_FUNCTION(luaL_newstate);

#ifdef VERBOSE
    DebugBackend::Get().Message("Found all necessary Lua functions");
#endif
    
    // Setup our API.

    luaInterface.DecodaOutput  = (lua_CFunction)InstanceFunction(DecodaOutput, api);
    luaInterface.CPCallHandler = (lua_CFunction)InstanceFunction(CPCallHandler, api);
    luaInterface.HookHandler   = (lua_Hook)InstanceFunction(HookHandler, api);

    g_interfaces.push_back( luaInterface );

    if (!g_loadedLuaFunctions)
    {
        DebugBackend::Get().Message("Debugger attached to process");
        g_loadedLuaFunctions = true;
    }

    return true;

}

static PIMAGE_NT_HEADERS PEHeaderFromHModule(HMODULE hModule)
{
    PIMAGE_NT_HEADERS pNTHeader = 0;
    
    __try
    {
        if ( PIMAGE_DOS_HEADER(hModule)->e_magic != IMAGE_DOS_SIGNATURE )
            __leave;

        pNTHeader = PIMAGE_NT_HEADERS(PBYTE(hModule)
                    + PIMAGE_DOS_HEADER(hModule)->e_lfanew);
        
        if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
            pNTHeader = 0;
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {       
    }

    return pNTHeader;
}

/**
 * Gets a list of the files that are imported by a module.
 */
bool GetModuleImports(HANDLE hProcess, HMODULE hModule, std::vector<std::string>& imports)
{

    PIMAGE_NT_HEADERS pExeNTHdr = PEHeaderFromHModule( hModule );
    
    if ( !pExeNTHdr )
    {
        return false;
    }

    DWORD importRVA = pExeNTHdr->OptionalHeader.DataDirectory
                        [IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if ( !importRVA )
    {
        return false;
    }

    // Convert imports RVA to a usable pointer
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MAKE_PTR( PIMAGE_IMPORT_DESCRIPTOR,
                                                    hModule, importRVA );

    // Iterate through each import descriptor, and redirect if appropriate
    while ( pImportDesc->FirstThunk )
    {
        PSTR pszImportModuleName = MAKE_PTR( PSTR, hModule, pImportDesc->Name);
        imports.push_back(pszImportModuleName);
        pImportDesc++;  // Advance to next import descriptor
    }

    return true;
}

bool GetFileExists(const char* fileName)
{
    return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
}

void ReplaceExtension(char fileName[_MAX_PATH], const char* extension)
{

    char* start = strrchr(fileName, '.');

    if (start == NULL)
    {
        strcat(fileName, extension);
    }
    else
    {
        strcpy(start + 1, extension);
    }

}

void GetFileTitle(const char* fileName, char fileTitle[_MAX_PATH])
{

    const char* slash1 = strrchr(fileName, '\\');
    const char* slash2 = strrchr(fileName, '/');

    const char* pathEnd = max(slash1, slash2);

    if (pathEnd == NULL)
    {
        // There's no path so the whole thing is the file title.
        strcpy(fileTitle, fileName);
    }
    else
    {
        strcpy(fileTitle, pathEnd + 1);
    }

}

void GetFilePath(const char* fileName, char path[_MAX_PATH])
{

    const char* slash1 = strrchr(fileName, '\\');
    const char* slash2 = strrchr(fileName, '/');

    const char* pathEnd = max(slash1, slash2);

    if (pathEnd == NULL)
    {
        // There's no path on the file name.
        path[0] = 0;
    }
    else
    {
        size_t length = pathEnd - fileName + 1;
        memcpy(path, fileName, length);
        path[length] = 0;
    }

}

bool LocateSymbolFile(const IMAGEHLP_MODULE64& moduleInfo, char fileName[_MAX_PATH])
{

    // The search order for symbol files is described here:
    // http://msdn2.microsoft.com/en-us/library/ms680689.aspx

    // This function doesn't currently support the full spec.

    const char* imageFileName = moduleInfo.LoadedImageName;
    
    // First check the absolute path specified in the CodeView data.
    if (GetFileExists(moduleInfo.CVData))
    {
        strncpy(fileName, moduleInfo.CVData, _MAX_PATH);
        return true;
    }

    char symbolTitle[_MAX_PATH];
    GetFileTitle(moduleInfo.CVData, symbolTitle);

    // Now check in the same directory as the image.
    
    char imagePath[_MAX_PATH];
    GetFilePath(imageFileName, imagePath);

    strcat(imagePath, symbolTitle);
    
    if (GetFileExists(imagePath))
    {
        strncpy(fileName, imagePath, _MAX_PATH);
        return true;
    }

    return false;

}

BOOL CALLBACK GatherSymbolsCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    
    stdext::hash_map<std::string, DWORD64>* symbols = reinterpret_cast<stdext::hash_map<std::string, DWORD64>*>(UserContext);

    if (pSymInfo != NULL && pSymInfo->Name != NULL)
    {
        symbols->insert(std::make_pair(pSymInfo->Name, pSymInfo->Address));
    }
    
    return TRUE;

}

BOOL CALLBACK FindSymbolsCallback(PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
{
    bool* found = reinterpret_cast<bool*>(UserContext);
    *found = true;
    return FALSE;
}

bool ScanForSignature(DWORD64 start, DWORD64 length, const char* signature)
{

    unsigned int signatureLength = strlen(signature);

    for (DWORD64 i = start; i < start + length - signatureLength; ++i)
    {

        void* p = reinterpret_cast<void*>(i);

        // Check that we have read access to the data. For some reason under Windows
        // Vista part of the DLL is not accessible (possibly some sort of new delay
        // loading mechanism for DLLs?)
        if (IsBadReadPtr(reinterpret_cast<LPCSTR>(p), signatureLength))
        {
            break;
        }

        if (memcmp(p, signature, signatureLength) == 0)
        {
            return true;
        }

    }

    return false;

}

void LoadSymbolsRecursively(std::set<std::string>& loadedModules, stdext::hash_map<std::string, DWORD64>& symbols, HANDLE hProcess, HMODULE hModule)
{

    assert(hModule != NULL);

    char moduleName[_MAX_PATH];
    GetModuleBaseName(hProcess, hModule, moduleName, _MAX_PATH);

    if (loadedModules.find(moduleName) == loadedModules.end())
    {

        // Record that we've loaded this module so that we don't
        // try to load it again.
        loadedModules.insert(moduleName);

        MODULEINFO moduleInfo = { 0 };
        GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo));

        char moduleFileName[_MAX_PATH];
        GetModuleFileNameEx(hProcess, hModule, moduleFileName, _MAX_PATH);
        
        DWORD64 base = SymLoadModule64_dll(hProcess, NULL, moduleFileName, moduleName, (DWORD64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);

        #ifdef VERBOSE
            char message[1024];
            _snprintf(message, 1024, "Examining '%s' %s\n", moduleName, base ? "(symbols loaded)" : "");
            DebugBackend::Get().Log(message);
        #endif

        // Check to see if there was a symbol file we failed to load (usually
        // becase it didn't match the version of the module).
            
        IMAGEHLP_MODULE64 module;
        memset(&module, 0, sizeof(module));
        module.SizeOfStruct = sizeof(module);

        BOOL result = SymGetModuleInfo64_dll(hProcess, base, &module);

        if (result && module.SymType == SymNone)
        {

            // No symbols were found. Check to see if the module file name + ".pdb"
            // exists, since the symbol file and/or module names may have been renamed.
            
            char pdbFileName[_MAX_PATH];
            strcpy(pdbFileName, moduleFileName);
            ReplaceExtension(pdbFileName, "pdb");
            
            if (GetFileExists(pdbFileName))
            {
                
                SymUnloadModule64_dll(hProcess, base);
                base = SymLoadModule64_dll(hProcess, NULL, pdbFileName, moduleName, (DWORD64)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);

                if (base != 0)
                {
                    result = SymGetModuleInfo64_dll(hProcess, base, &module);
                }
                else
                {
                    result = FALSE;
                }

            }

        }

        if (result)
        {

            // Check to see if we've already warned about this module.
            if (g_warnedAboutPdb.find(moduleFileName) == g_warnedAboutPdb.end())
            {
                if (strlen(module.CVData) > 0 && (module.SymType == SymExport || module.SymType == SymNone))
                {

                    char symbolFileName[_MAX_PATH];

                    if (LocateSymbolFile(module, symbolFileName))
                    {
                        char message[1024];
                        _snprintf(message, 1024, "Warning 1002: Symbol file '%s' located but it does not match module '%s'", symbolFileName, moduleFileName);
                        DebugBackend::Get().Message(message, MessageType_Warning);
                    }

                    // Remember that we've checked on this file, so no need to check again.
                    g_warnedAboutPdb.insert(moduleFileName);

                }
            }

        }
           
        if (base != 0)
        {
            // SymFromName is really slow, so we gather up our own list of the symbols that we
            // can index much faster.
            SymEnumSymbols_dll(hProcess, base, "lua*", GatherSymbolsCallback, reinterpret_cast<PVOID>(&symbols));
        }

        // Check to see if the module contains the Lua signature but we didn't find any Lua functions.

        if (g_warnedAboutLua.find(moduleFileName) == g_warnedAboutLua.end())
        {
            
            // Check to see if this module contains any Lua functions loaded from the symbols.

            bool foundLuaFunctions = false;

            if (base != 0)
            {
                SymEnumSymbols_dll(hProcess, base, "lua_*", FindSymbolsCallback, &foundLuaFunctions);
            }

            if (!foundLuaFunctions)
            {

                // Check to see if this module contains a string from the Lua source code. If it's there, it probably
                // means this module has Lua compiled into it.

                bool luaFile = ScanForSignature((DWORD64)hModule, moduleInfo.SizeOfImage, "$Lua:");

                if (luaFile)
                {
                    char message[1024];
                    _snprintf(message, 1024, "Warning 1001: '%s' appears to contain Lua functions however no Lua functions could located with the symbolic information", moduleFileName);
                    DebugBackend::Get().Message(message, MessageType_Warning);
                }

            }

            // Remember that we've checked on this file, so no need to check again.
            g_warnedAboutLua.insert(moduleFileName);

        }

        // Get the imports for the module. These are loaded before we're able to hook
        // LoadLibrary for the module.

        std::vector<std::string> imports;
        GetModuleImports(hProcess, hModule, imports);

        for (unsigned int i = 0; i < imports.size(); ++i)
        {

            HMODULE hImportModule = GetModuleHandle(imports[i].c_str());
            
            // Sometimes the import module comes back NULL, which means that for some reason
            // it wasn't loaded. Perhaps these are delay loaded and we'll catch them later?
            if (hImportModule != NULL)
            {
                LoadSymbolsRecursively(loadedModules, symbols, hProcess, hImportModule);
            }

        }

    }

}

BOOL CALLBACK SymbolCallbackFunction(HANDLE hProcess, ULONG code, ULONG64 data, ULONG64 UserContext)
{

    if (code == CBA_DEBUG_INFO)
    {
        DebugBackend::Get().Message(reinterpret_cast<char*>(data));
    }

    return TRUE;

}

void PostLoadLibrary(HMODULE hModule)
{

    extern HINSTANCE g_hInstance;

    if (hModule == g_hInstance)
    {
        // Don't investigate ourself.
        return;
    }

    HANDLE hProcess = GetCurrentProcess();

    char moduleName[_MAX_PATH];
    GetModuleBaseName(hProcess, hModule, moduleName, _MAX_PATH);

    CriticalSectionLock lock(g_loadedModulesCriticalSection);

    if (g_loadedModules.find(moduleName) == g_loadedModules.end())
    {

        // Record that we've loaded this module so that we don't
        // try to load it again.
        g_loadedModules.insert(moduleName);

        if (!g_initializedDebugHelp)
        {
            if (!SymInitialize_dll(hProcess, g_symbolsDirectory.c_str(), FALSE))
            {
                return;
            }
            g_initializedDebugHelp = true;
        }

        //SymSetOptions(SYMOPT_DEBUG);

        std::set<std::string> loadedModules;
        stdext::hash_map<std::string, DWORD64> symbols;

        LoadSymbolsRecursively(loadedModules, symbols, hProcess, hModule);

        LoadLuaFunctions(symbols, hProcess);

        //SymCleanup_dll(hProcess);
        //hProcess = NULL;
    
    }

}

void HookLoadLibrary()
{

    HMODULE hModuleKernel = GetModuleHandle("kernel32.dll");

    if (hModuleKernel != NULL)
    {
        // LoadLibraryExW is called by the other LoadLibrary functions, so we
        // only need to hook it.
        LoadLibraryExW_dll = (LoadLibraryExW_t) HookFunction( GetProcAddress(hModuleKernel, "LoadLibraryExW"), LoadLibraryExW_intercept);
    }

    // These NTDLL functions are undocumented and don't exist in Windows 2000.

    HMODULE hModuleNt = GetModuleHandle("ntdll.dll");

    if (hModuleNt != NULL)
    {
        LdrLockLoaderLock_dll   = (LdrLockLoaderLock_t)   GetProcAddress(hModuleNt, "LdrLockLoaderLock");
        LdrUnlockLoaderLock_dll = (LdrUnlockLoaderLock_t) GetProcAddress(hModuleNt, "LdrUnlockLoaderLock");
    }

}

bool InstallLuaHooker(HINSTANCE hInstance, const char* symbolsDirectory)
{

    // Load the dbghelp functions. We have to do this dynamically since the
    // older version of dbghelp that ships with Windows doesn't successfully
    // load the symbols from PDBs. We can't simply include our new DLL since
    // it needs to be in the directory for the application we're *debugging*
    // since this DLL is injected.
    if (!LoadDebugHelp(hInstance))
    {
        return false;
    }

    g_symbolsDirectory = symbolsDirectory;

    // Add the "standard" stuff to the symbols directory search path.
    g_symbolsDirectory += ";" + GetApplicationDirectory();
    g_symbolsDirectory += ";" + GetEnvironmentVariable("_NT_SYMBOL_PATH");
    g_symbolsDirectory += ";" + GetEnvironmentVariable("_NT_ALTERNATE_SYMBOL_PATH");

    // Hook LoadLibrary* functions so that we can intercept those calls and search
    // for Lua functions.
    HookLoadLibrary();

    // Avoid deadlock if a new DLL is loaded during this function.
    ULONG cookie;

    if (LdrLockLoaderLock_dll   != NULL &&
        LdrUnlockLoaderLock_dll != NULL)
    {
        LdrLockLoaderLock_dll(0, 0, &cookie);
    }

    // Process all of the loaded modules.

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

    if (hSnapshot == NULL)
    {

        // If for some reason we couldn't take a snapshot, just load the
        // main module. This shouldn't ever happen, but we do it just in
        // case.
        HMODULE hModule = GetModuleHandle(NULL);
        PostLoadLibrary(hModule);

        if (LdrLockLoaderLock_dll   != NULL &&
            LdrUnlockLoaderLock_dll != NULL)
        {
            LdrUnlockLoaderLock_dll(0, cookie);
        }

        return true;

    }

    MODULEENTRY32 module;
    module.dwSize = sizeof(MODULEENTRY32);

    BOOL moreModules = Module32First(hSnapshot, &module);

    while (moreModules)
    {
        PostLoadLibrary(module.hModule);
        moreModules = Module32Next(hSnapshot, &module);
    }

    CloseHandle(hSnapshot);
    hSnapshot = NULL;

    if (LdrLockLoaderLock_dll   != NULL &&
        LdrUnlockLoaderLock_dll != NULL)
    {
        LdrUnlockLoaderLock_dll(0, cookie);
    }

    return true;

}

bool GetIsLuaLoaded()
{
    return g_loadedLuaFunctions;
}

bool GetIsStdCall(unsigned long api)
{
    return g_interfaces[api].stdcall;
}

struct CFunctionArgs
{
    unsigned long       api;
    lua_CFunction_dll   function;
};

#pragma auto_inline(off)
int CFunctionHandlerWorker(CFunctionArgs* args, lua_State* L, bool& stdcall)
{
    stdcall = g_interfaces[args->api].stdcall;
    return args->function(args->api, L);
}
#pragma auto_inline()

__declspec(naked) int CFunctionHandler(CFunctionArgs* args, lua_State* L)
{

    int result;
    bool stdcall;
        
    INTERCEPT_PROLOG()
    
    stdcall = false;
    result = CFunctionHandlerWorker(args, L, stdcall);

    INTERCEPT_EPILOG(4)

}

lua_CFunction CreateCFunction(unsigned long api, lua_CFunction_dll function)
{
    
    // This is never deallocated, but it doesn't really matter since we never
    // destroy these functions.
    CFunctionArgs* args = new CFunctionArgs;
    args->api       = api;
    args->function  = function;

    return (lua_CFunction)InstanceFunction(CFunctionHandler, reinterpret_cast<unsigned long>(args));
    
}