#pragma once 
#include "all_inc.hpp"

#include "gfx_base.cpp"
#include "gfx_render_graph.cpp"
#include "gfx_draw.cpp"
#include "gfx_text.cpp"

#if OS_WINDOWS
#include "win32/gfx_win32.cpp"
#else
#error GFX layer not set up!
#endif

