#pragma once 
#include "all_inc.hpp"

#if OS_WINDOWS
#include "win32/gfx_win32.cpp"
#else
#error GFX layer not set up!
#endif
