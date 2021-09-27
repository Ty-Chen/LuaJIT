#ifndef _LUA_TRACKER_H
#define _LUA_TRACKER_H
#include "stdafx.h"
#include "CallInfoBuffer.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <map> 

#define TRACK_TIME_FLAG     1
#define TRACK_MEMORY_FLAG   2

#define MAX_CALL_STACK_NUM   100
#define MAX_SOURCE_PATH_LEN  256
#define STACK_INFO_LENGTH    1024 * 1024

typedef unsigned int lu_mem;
class LuaTracker
{
public:
    LuaTracker();
    ~LuaTracker();

    BOOL            Init(const char* pszPath, int nTrackType);
    BOOL            UnInit();

    void            StartTrack(lua_State* L, lua_Debug* pLua_Debug);
    void            EndTrack(int nTotalBytes = 0);

    int             GetTrackLevel();
    const char*     GetCallInfoBuffer();
    int             GetSize();
    void            GetTopText(const char* &pszOut);
    BOOL            AddCallInfoToBuffer(lua_Debug* pLua_Debug);

private:
    hook_time_t     Wclock();

private:
    int             m_nTop;
    int             m_nTrackFlag;

    CallInfoBuffer  m_CallInfoBuffer;

    hook_time_t     m_pTimeRecord[MAX_CALL_STACK_NUM];
    hook_time_t     m_pExpireTime[MAX_CALL_STACK_NUM];

    lu_mem          m_pAllocRecord[MAX_CALL_STACK_NUM];
    lu_mem          m_pExpireMemory[MAX_CALL_STACK_NUM];

    char            m_szCallPath[MAX_CALL_STACK_NUM][MAX_SOURCE_PATH_LEN];

    bool            m_bTopFunction;

    unsigned int    m_nWriteFileTime;

    LARGE_INTEGER   m_Freq;

    std::map<std::string, int> m_Map;
    int m_count;
};

#endif
