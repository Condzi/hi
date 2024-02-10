#pragma once
#include "all_inc.hpp"

#include "core/os_core.cpp"

#if OS_WINDOWS
#include "core/win32/os_core_win32.cpp"
#include "gfx/win32/os_gfx_win32.cpp"
#else
#error OS Layer not set up!
#endif
