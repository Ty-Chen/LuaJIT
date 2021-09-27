#pragma once
#ifndef _STDAFX_H_
#define _STDAFX_H_

#include "lua.hpp"
#include "lauxlib.h"
#include <assert.h>

#ifdef __linux__
#include <time.h>
typedef long hook_time_t;
#define CLOCK_FUNCTION lclock
#ifdef CLOCK_MONOTONIC_RAW
#define LINUX_CLOCK CLOCK_MONOTONIC_RAW
#else
#define LINUX_CLOCK CLOCK_MONOTONIC
#endif
#elif _WIN32
#include <windows.h>
typedef long long hook_time_t;
#endif

#define ASSERT_ERROR assert

int Lua_GetTotalBytes(lua_State* L) { return 0;}

#endif
