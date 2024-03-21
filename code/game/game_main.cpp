#include "all_inc.hpp"

#include "game_main.hpp"

Scratch_Buffer error_context_scratch;

bool
startup() {
  gContext.frame_arena = make_arena(false);
  gContext.misc_arena  = make_arena(true);
  gContext.log         = make_log(make_arena(false));

  Arena *error_context_arena = make_arena(false);
  gContext.error_context     = error_context_init(error_context_arena);
  // Error context is put into scratch buffer, cleared every frame.
  error_context_scratch = scratch_begin(error_context_arena);
  os_init();
  os_gfx_init();

  os_gfx_open_window({
      .title      = GAME_TITLE_LITERAL ""_s8,
      .width      = 1280,
      .height     = 720,
      .fullscreen = false,
  });

  gfx_init({
      .vp_width  = os_gfx_surface_width(),
      .vp_height = os_gfx_surface_height(),
      .vsync     = true,
  });
  gfx_renderer_init();

  psx_init();

  ui_init(gContext.misc_arena, 512);
  tools_init();

  kb_set_key_bindings(make_key_bindings(gContext.misc_arena));

  return true;
}

f32 psx_acc;
bool
loop(f32 dt) {
  gfx_renderer_begin_frame();

  ui_begin(dt);

  // Event handling
  //
  OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
  kb_update(events);

  if (kb_state(KB_Debug1).pressed) {
    gDbgConsole.is_open = !gDbgConsole.is_open;
  }

  if (kb_state(KB_Debug2).pressed) {
    gDbgMemoryConsumption.is_open = !gDbgMemoryConsumption.is_open;
  }

  if (kb_state(KB_Debug3).pressed) {
    gfx_set_vsync(!gfx_is_vsync_enabled());
  }

  // Simulation update
  //

  psx_acc += dt;
  if (psx_acc >= PSX_STEP) {
    while (psx_acc > PSX_STEP) {
      // psx_world_simulate(world, PSX_STEP);
      psx_acc -= PSX_STEP;
    }
    // psx_world_simulate(world, psx_acc);
    psx_acc = 0;
  }

  ui_rect("background"_s8,
          0,
          {.kind = UI_SizeKind_Pixels, .value = (f32)os_gfx_surface_width(), .strictness = 1},
          {.kind = UI_SizeKind_Pixels, .value = (f32)os_gfx_surface_height(), .strictness = 1});

  tools_update();

  ui_end();

  gfx_renderer_end_frame();

  // State cleanup
  //

  arena_clear(gContext.frame_arena);
  scratch_end(&error_context_scratch);
  gContext.error_context->first = gContext.error_context->last = 0;

  return os_gfx_window_mode() != OS_WindowMode_Closed;
}

  void
  teardown() {
    LogGame_Info("Teardown");
  }

  HI_ENTRY_POINT(startup, loop, teardown);
