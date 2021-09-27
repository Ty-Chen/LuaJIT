#include "stdafx.h"
#include "LuaProfiler.h"

static LuaProfiler* g_LuaProfiler = NULL;

LuaProfiler::LuaProfiler()
{
    m_nCallNums      = 0;
    m_pTracker       = NULL;
    m_pszFilePath[0] = '\0';
    m_bIsHooking     = false;
    g_LuaProfiler    = this;
}

LuaProfiler::~LuaProfiler()
{
    if (m_pTracker)
    {
        delete m_pTracker;
    }

    m_pTracker    = NULL;
    g_LuaProfiler = NULL;
}

BOOL LuaProfiler::Init(int nTrackType, lua_State* L)
{
    BOOL  bResult  = false;
    BOOL  bRetCode = false;

    if (m_bIsHooking)
    {
        printf("Already in hook");
        bResult = true;
        goto Exit0;
    }

    if (strlen(m_pszFilePath) == 0)
    {
        CreateDirAndFile();
    }

    m_nCallNums = 0;

    m_pTracker = new LuaTracker();
    ASSERT_ERROR(m_pTracker);

    bRetCode = m_pTracker->Init(m_pszFilePath, nTrackType);
    ASSERT_ERROR(bRetCode);

    bRetCode = SetHook(L);
    ASSERT_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL LuaProfiler::Uninit(lua_State* L)
{
    BOOL bResult = false;
    BOOL bRetCode = false;

    if (m_bIsHooking)
    {
        bRetCode = StopHook(L);
        ASSERT_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL LuaProfiler::StopHook(lua_State* L)
{
    BOOL bResult = false;
    BOOL bRetCode = false;

    lua_sethook(L, NULL, 0, 0);

    bRetCode = m_pTracker->UnInit();
    ASSERT_ERROR(bRetCode);

    m_bIsHooking = false;

    printf("Hook %d function calls", m_nCallNums);

    bResult = true;
Exit0:
    return bResult;
}

BOOL LuaProfiler::SetHook(lua_State* L)
{
    BOOL bResult = false;
    BOOL bRetCode = false;
    int  nMask = 0;

    ASSERT_ERROR(L && m_pTracker);

    bRetCode = lua_sethook(L, Hook, LUA_MASKCALL | LUA_MASKRET, 0);
    m_bIsHooking = bRetCode;

    bResult = true;
Exit0:
    return bResult;
}

BOOL LuaProfiler::GetTracker(LuaTracker*& pTracker)
{
    pTracker = m_pTracker;
    return true;
}

BOOL LuaProfiler::AddCallNum()
{
    m_nCallNums++;
    return true;
}

BOOL LuaProfiler::CreateDirAndFile()
{
    BOOL  bResult = false;
    int   nLen = 0;
    int   nLeftSize = MAX_FILE_PATH;
    char* pszPos = NULL;
    char  szPath[MAX_FILE_PATH];

    pszPos = szPath;
    nLen = GetCurrentDirectory(sizeof(szPath), (LPWSTR)szPath);

    nLeftSize -= nLen;
    pszPos += nLen;

    nLen = snprintf(pszPos, nLeftSize, "\\logs\\LuaProfiler");
    ASSERT_ERROR(nLen);

    nLeftSize -= nLen;
    pszPos += nLen;

    CreateDirectory((LPWSTR)szPath, NULL);

    nLen = snprintf(pszPos, nLeftSize, "\\LuaProfilerOutput.txt");
    ASSERT_ERROR(nLen);

    nLen = strlen(szPath);
    strncpy(m_pszFilePath, szPath, nLen);
    m_pszFilePath[nLen] = '\0';

    bResult = true;
Exit0:
    return bResult;
}

void Hook(lua_State* L, lua_Debug* pLua_Debug)
{
    int          nBYtes   = 0;
    LuaTracker*  pTracker = NULL;

    g_LuaProfiler->GetTracker(pTracker);
    ASSERT_ERROR(pTracker);

    if (pLua_Debug->event == LUA_HOOKCALL)
    {
        g_LuaProfiler->AddCallNum();
        lua_getinfo(L, "nSl", pLua_Debug);
        pTracker->StartTrack(L, pLua_Debug);
    }
    else if (pLua_Debug->event == LUA_HOOKRET || pLua_Debug->event == LUA_HOOKTAILRET)
    {
        nBYtes = Lua_GetTotalBytes(L);
        pTracker->EndTrack(nBYtes);
    }

Exit0:
    return;
}
