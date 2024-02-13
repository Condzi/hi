#pragma once

#include "gfx_base.hpp"
#include "gfx_render_graph.hpp"
#include "gfx_draw.hpp"

global Arena *gfx_arena;

struct GFX_Opts {
  u32  vp_width;
  u32  vp_height;
  bool vsync;
};

global void
gfx_init(GFX_Opts const& opts);

must_use global bool
gfx_is_vsync_enabled();

global void
gfx_set_vsync(bool v);

global void
gfx_resize(u32 new_width, u32 new_height);

global void
gfx_swap_buffers(GFX_Image final_image);


#if OS_WINDOWS
#include "win32/gfx_win32.hpp"
#include "win32/gfx_win32_hlsl.hpp"
#else
#error GFX layer not set up!
#endif
