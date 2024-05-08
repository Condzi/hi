#include "all_inc.hpp"
#include "all_inc.cpp"

bool
startup() {
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

  ecs_init();
  physics_system().init();
  rendering_system().init();
  player_control_system().init();
  tools_system().init();

  ui_init(gContext.misc_arena, 512);

  kb_set_key_bindings(make_key_bindings(gContext.misc_arena));

  // @Assets
  gGameMaster.characters = d3d_load_png("W:/hi/run_tree/tex/characters.png"_s8);
  gGameMaster.bg         = gfx_checkerboard_image(64, 16, 16);

  GFX_Image font_img = d3d_load_png("W:/hi/run_tree/tex/pixel_font_basic_latin_ascii.png"_s8);
  gGameMaster.font   = gfx_make_font(font_img, 7, 9);
  gUI.font           = &(gGameMaster.font);

  gGameMaster.camera = {
      .center = {200, 200},
      .zoom   = 0.8f,
  };

  spawn_background();
  spawn_player();
  spawn_zombie();


  return true;
}

bool
loop(f32 dt) {
  // Event handling
  //
  OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
  kb_update(events);

  gfx_renderer_begin_frame();
  gfx_set_camera_for_batches(gGameMaster.camera);

  ui_begin(dt);
  ui_rect("background"_s8,
          0,
          {.kind = UI_SizeKind_Pixels, .value = (f32)os_gfx_surface_width(), .strictness = 1},
          {.kind = UI_SizeKind_Pixels, .value = (f32)os_gfx_surface_height(), .strictness = 1});

  player_control_system().update(dt);
  physics_system().update(dt);
  tools_system().update(dt);
  rendering_system().update(dt);

  ecs_kill_pass();

  ui_end();
  gfx_renderer_end_frame();

  return os_gfx_window_mode() != OS_WindowMode_Closed;
}

void
teardown() {
  LogGame_Info("Teardown");
}

HI_ENTRY_POINT(startup, loop, teardown);
