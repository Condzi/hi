#pragma once

#include "gfx_base.hpp"
#include "gfx_render_graph.hpp"
#include "gfx_draw.hpp"
#include "gfx_text.hpp"

#if OS_WINDOWS
#include "win32/gfx_win32.hpp"
#include "win32/gfx_win32_hlsl.hpp"
#else
#error GFX layer not set up!
#endif
