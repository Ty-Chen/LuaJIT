#include "lua.hpp"
int main()
{
    lua_State *L = luaL_newstate(); /*创建一个解释器句柄*/
    luaL_openlibs(L); /*打开所有的Lua库*/
    luaL_loadfile(L, "../LuaJIT-Debug/t1.lua"); /*调入Lua脚本文件，注意路径*/
    lua_pcall(L, 0, 0, 0); /*执行Lua脚本*/
    lua_close(L); /*关闭句柄*/
    return 0;
}
