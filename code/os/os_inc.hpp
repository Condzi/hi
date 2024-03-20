#pragma once

#include "core/os_core.hpp"
#include "core/os_info.hpp"
#include "core/os_entry_point.hpp"
#include "gfx/os_gfx.hpp"
#include "gfx/os_gfx_keys.hpp"

#if OS_WINDOWS
#include "core/win32/os_core_win32.hpp"
#include "gfx/win32/os_gfx_win32.hpp"
#include "gfx/win32/os_gfx_win32_keys.hpp"
#else
#error OS Layer not set up!
#endif
