#ifndef _LUA_PROFILER_H
#define _LUA_PROFILER_H
#include "LuaTracker.h"
#include "stdafx.h"

#define MAX_FILE_PATH 1024

extern class LuaProfiler
{
public:
    LuaProfiler();
    ~LuaProfiler();

    BOOL            Init(int nTrackType, lua_State* L);
    BOOL            Uninit(lua_State* L);

    BOOL            SetHook(lua_State* L);
    BOOL            StopHook(lua_State* L);

    BOOL            GetTracker(LuaTracker*& pTracker);

    BOOL            AddCallNum();

private:
    BOOL            CreateDirAndFile();

private:
    LuaTracker*     m_pTracker;
    char            m_pszFilePath[MAX_FILE_PATH];
    BOOL            m_bIsHooking;
    int             m_nCallNums;
};
void Hook(lua_State* L, lua_Debug* pLua_Debug);

#endif
