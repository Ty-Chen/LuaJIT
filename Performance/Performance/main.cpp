#include "lua.hpp"

void testT1(lua_State* L)
{
    luaL_loadfile(L, "t1.lua"); /*调入Lua脚本文件，注意路径*/
    lua_pcall(L, 0, 0, 0); /*执行Lua脚本*/
}

void testT2(lua_State* L)
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

int main()
{
    lua_State *L = luaL_newstate(); /*创建一个解释器句柄*/
    luaL_openlibs(L); /*打开所有的Lua库*/

    testT1(L);
    testT2(L);

    lua_close(L); /*关闭句柄*/
    return 0;
}
