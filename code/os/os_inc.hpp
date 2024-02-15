#pragma once

#include "core/os_core.hpp"
#include "gfx/os_gfx.hpp"
#include "gfx/os_gfx_keys.hpp"

#if OS_WINDOWS
#include "core/win32/os_core_win32.hpp"
#include "gfx/win32/os_gfx_win32.hpp"
#else
#error OS Layer not set up!
#endif
