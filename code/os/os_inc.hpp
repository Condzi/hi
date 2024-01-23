#pragma once

#include "core/os_core.hpp"

#if OS_WINDOWS
#include "core/win32/os_core_win32.hpp"
#else
#error OS Layer not set up!
#endif
