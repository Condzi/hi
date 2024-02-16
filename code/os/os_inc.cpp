#pragma once
#include "all_inc.hpp"

#include "core/os_core.cpp"
#include "gfx/os_gfx_keys.cpp"

#if OS_WINDOWS
#include "core/win32/os_core_win32.cpp"
#include "gfx/win32/os_gfx_win32.cpp"
#include "gfx/win32/os_gfx_win32_keys.cpp"
#else
#error OS Layer not set up!
#endif
