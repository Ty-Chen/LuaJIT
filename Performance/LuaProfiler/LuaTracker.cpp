#include "stdafx.h"
#include "LuaTracker.h"

LuaTracker::LuaTracker()
{
    m_nTop = 0;
    m_nTrackFlag = 0;
    m_nWriteFileTime = 0;
    m_count = 0;
    QueryPerformanceFrequency(&m_Freq);
}

BOOL LuaTracker::UnInit()
{
    while (m_nTop > 0)
    {
        EndTrack();
    }

    m_CallInfoBuffer.UnInit();
    m_nTrackFlag = 0;
    m_nWriteFileTime = 0;
    return true;
}

LuaTracker::~LuaTracker()
{
    UnInit();
}

hook_time_t LuaTracker::Wclock()
{
    LARGE_INTEGER Tick;

    QueryPerformanceCounter(&Tick);

    return Tick.QuadPart;
}

void LuaTracker::GetTopText(const char*& pszOut)
{
    m_CallInfoBuffer.GetTopText(pszOut);
}

BOOL LuaTracker::Init(const char* pszPath, int nTrackType)
{
    BOOL bResult = false;
    BOOL bRetCode = 0;

    m_nTrackFlag |= nTrackType;

    ASSERT_ERROR(pszPath);
    bRetCode = m_CallInfoBuffer.Init(pszPath, STACK_INFO_LENGTH);
    ASSERT_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        UnInit();
    }

    return bResult;
}

int LuaTracker::GetTrackLevel()
{
    return m_nTop;
}

const char* LuaTracker::GetCallInfoBuffer()
{
    return m_CallInfoBuffer.GetBuffer();
}

int LuaTracker::GetSize()
{
    return m_nTop;
}

void LuaTracker::StartTrack(lua_State* L, lua_Debug* pLua_Debug)
{
    BOOL bRetCode = 0;
    int nCallInfoSize = 0;

    if (m_nTop >= MAX_CALL_STACK_NUM)
    {
        printf("Lua Profiler Full\n");
    }

    lua_getinfo(L, "nSl", pLua_Debug);

    bRetCode = AddCallInfoToBuffer(pLua_Debug);
    ASSERT_ERROR(bRetCode);
    m_bTopFunction = true;

    if (m_nTrackFlag & TRACK_TIME_FLAG)
    {
        hook_time_t nTimeNow = Wclock();
        m_pTimeRecord[m_nTop] = nTimeNow;
        m_pExpireTime[m_nTop] = 0;
    }

    if (m_nTrackFlag & TRACK_MEMORY_FLAG)
    {
        m_pAllocRecord[m_nTop] = Lua_GetTotalBytes(L);
        m_pExpireMemory[m_nTop] = 0;
    }

    m_nTop++;
Exit0:
    return;
}

void LuaTracker::EndTrack(int nTotalBytes)
{
    BOOL bRetCode = 0;
    hook_time_t nTimeCost = 0;
    lu_mem      nMemoryAlloc = 0;

    ASSERT_ERROR(m_nTop > 0);

    m_nTop--;

    if (m_nTrackFlag & TRACK_TIME_FLAG)
    {
        hook_time_t nTimeNow = Wclock();
        hook_time_t nTimeInterval = (nTimeNow - m_pTimeRecord[m_nTop]) * 1000000 / m_Freq.QuadPart;
        hook_time_t nExpire = m_pExpireTime[m_nTop];
        nTimeCost = nTimeInterval - nExpire;
        if (m_nTop >= 1)
        {
            m_pExpireTime[m_nTop - 1] += nTimeInterval;
        }
    }

    if (nTotalBytes)
    {
        if (m_nTrackFlag & TRACK_MEMORY_FLAG)
        {
            nMemoryAlloc = nTotalBytes - m_pAllocRecord[m_nTop];
            if (m_nTop >= 1)
            {
                m_pExpireMemory[m_nTop - 1] += nMemoryAlloc;
            }
        }
    }

    if (nTimeCost <= 0 && m_bTopFunction)
    {
        m_CallInfoBuffer.PopLastLine();
    }
    else
    {
        char szCostText[50];
        sprintf(szCostText, "#%lld#%u\0", nTimeCost, nMemoryAlloc);
        bRetCode = m_CallInfoBuffer.Add(szCostText, strlen(szCostText), true);
        m_nWriteFileTime++;
    }
    m_bTopFunction = false;
Exit0:
    return;
}

BOOL LuaTracker::AddCallInfoToBuffer(lua_Debug* pLua_Debug)
{
    BOOL bResult = false;
    BOOL bRetCode = 0;
    int nLength = 0;
    int nFileNamePos = 0;

    bool bNewLine = !pLua_Debug->currentline && !pLua_Debug->name;
    if (pLua_Debug->source)
    {
        ASSERT_ERROR(pLua_Debug->source[0] == '=' || pLua_Debug->source[0] == '@');
        nLength = strlen(pLua_Debug->source);
        if (nLength < MAX_SOURCE_PATH_LEN - 1)
        {
            if (0)
            {
                nFileNamePos = nLength - 5;
                while (nFileNamePos > 0 && pLua_Debug->source[--nFileNamePos] != '\\');
                nFileNamePos++;
                nLength = nLength - nFileNamePos;
            }

            if (m_nTop == 0 || strncmp(m_szCallPath[m_nTop - 1], pLua_Debug->source + nFileNamePos, nLength) != 0)
            {
                bRetCode = m_CallInfoBuffer.Add(pLua_Debug->source + nFileNamePos, nLength, bNewLine);
                ASSERT_ERROR(bRetCode);
            }
            strncpy(m_szCallPath[m_nTop], pLua_Debug->source + nFileNamePos, nLength);
            m_szCallPath[m_nTop][nLength] = '\0';
        }
    }

    bNewLine = !pLua_Debug->name;
    if (pLua_Debug->currentline)
    {
        char pszCurrentLine[10];
        _itoa(pLua_Debug->currentline, pszCurrentLine, 10);
        nLength = strlen(pszCurrentLine);
        bRetCode = m_CallInfoBuffer.Add(pszCurrentLine, nLength, bNewLine);
        ASSERT_ERROR(bRetCode);
    }

    if (pLua_Debug->name)
    {
        nLength = strlen(pLua_Debug->name);
        bRetCode = m_CallInfoBuffer.Add(pLua_Debug->name, nLength, true);
        ASSERT_ERROR(bRetCode);
    }
    //pair<map<string, int>::iterator, bool> Insert_Pair;
    //Insert_Pair = m_Map.insert(pair<string, int>(s, 0));
    //if (Insert_Pair.second == true)
    //{
    //    m_count++;
    //}
    bResult = true;
Exit0:
    return bResult;
}
