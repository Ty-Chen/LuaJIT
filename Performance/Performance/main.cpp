#include "lua.hpp"
#include "hook.hpp"

void TestLoadString(lua_State* L)
{
    char szBuff[256];
    int  nError;

    while (fgets(szBuff, sizeof(szBuff), stdin) != NULL)
    {
        nError = luaL_loadstring(L, szBuff) || lua_pcall(L, 0, 0, 0);
        if (nError)
        {
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            lua_pop(L, 1); /* pop error message from the stack */
        }
    }
}

void TestGetInfo(lua_State* L)
{
    luaL_loadfile(L, "t1.lua");
    lua_pcall(L, 0, 0, 0);
}

void TestLuaTrace(lua_State* L)
{
    luaL_loadfile(L, "t2.lua");
    lua_pcall(L, 0, 0, 0);
}

void TestCHookRegister(lua_State* L)
{
    luaL_loadfile(L, "t4.lua");
    luaopen_luatrace_c_hook(L);
    lua_pcall(L, 0, 0, 0);
}

void TestJitTrace(lua_State* L)
{
    luaL_loadfile(L, "testJit.lua");
    lua_pcall(L, 0, 0, 0);
}

void TestPackage(lua_State* L)
{
    luaL_loadfile(L, "module.lua");
    lua_pcall(L, 0, 0, 0);
}

int main()
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    //TestLoadString(L);
    //TestGetInfo(L);
    //TestLuaTrace(L);
    //TestCHookRegister(L);
    TestPackage(L);
    //TestJitTrace(L);

    lua_close(L);
    return 0;
}
