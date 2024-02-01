#pragma once

struct GFX_Cmd_List {
  u64 v[1];
};

struct GFX_Opts {
  u32  vp_width;
  u32  vp_height;
  bool vsync;
};

// pass initial settings
void
gfx_init(GFX_Opts const& opts);

must_use bool
gfx_is_vsync_enabled();

void
gfx_set_vsync(bool v);

// Resource creation
//

must_use int
compile_pixel_shader(...);

must_use int
compile_vertex_shader(...);

must_use int
create_texture(...);

// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-render-multi-thread-command-list
must_use GFX_Cmd_List
create_command_list();

must_use int
create_pipeline();

must_use int
create_render_target();

void
gfx_resize(u32 new_width, u32 new_height);

must_use GFX_Cmd_List
gfx_start_cmd_list();

void
gfx_finish_cmd_list(GFX_Cmd_List queue);

void
gfx_execute_cmd_list(GFX_Cmd_List queue);

void
gfx_swap_buffers();

#if OS_WINDOWS
#include "win32/gfx_win32.hpp"
#else
#error GFX layer not set up!
#endif
