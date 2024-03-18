#pragma once
#include "all_inc.hpp"

#include "game_main.hpp"

int
main(int argc, char const *argv[]) {
  gContext.frame_arena = make_arena(false);
  gContext.misc_arena  = make_arena(true);
  gContext.log         = make_log(make_arena(false));

  Arena *error_context_arena = make_arena(false);
  gContext.error_context     = error_context_init(error_context_arena);
  // Error context is put into scratch buffer, cleared every frame.
  Scratch_Buffer error_context_scratch = scratch_begin(error_context_arena);
  os_init(argc, argv);
  os_gfx_init();

  GFX_Layer l1 = {.category = GFX_Layer_Background};
  GFX_Layer l2 = {.category = GFX_Layer_Foreground};
  ErrorIf(l1.v > l2.v, "l1.v=0x%X, l2.v=0x%X", (u32)l1.v, (u32)l2.v);

  GFX_Layer l3 = {.category = GFX_Layer_Background, .priority = 0};
  GFX_Layer l4 = {.category = GFX_Layer_Background, .priority = 1};
  ErrorIf(l3.v > l4.v, "l3.v=0x%X, l4.v=0x%X", (u32)l3.v, (u32)l4.v);

  u64 const hash_1   = hash_str8("hash1"_s8);
  u64 const hash_2   = hash_str8("hash2"_s8);
  u64 const hash_1_2 = hash_str8("hash1"_s8);

  Assert(hash_1 != hash_2);
  Assert(hash_1 == hash_1_2);

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

  GFX_Image characters_img = d3d_load_png("W:/hi/run_tree/tex/characters.png"_s8);
  GFX_Image bg_img         = gfx_checkerboard_image(64, 16, 16);

  GFX_Sprite_Opts sprite_1 = {
      .pos      = {0, 0},
      .sz       = fvec2 {54.f, 43.f} * 3,
      .tex      = characters_img,
      .tex_rect = {.x = 263, .y = 132, .w = 49, .h = 43},
      .layer    = l2,
  };

  GFX_Rect_Opts rect_1 = {
      .pos   = {0, 0},
      .sz    = {50.f, 50.f},
      .layer = l2,
  };

  f32 const width  = (f32)os_gfx_surface_width();
  f32 const height = (f32)os_gfx_surface_height();

  fvec2 const     bg_sz = fvec2 {width, height} * 10.f;
  GFX_Sprite_Opts bg    = {
         .pos      = {-bg_sz.x / 2, bg_sz.y / 2},
         .sz       = bg_sz,
         .tex      = bg_img,
         .tex_rect = {.x = 0, .y = 0, .w = (u16)bg_sz.x, .h = (u16)bg_sz.y},
         .layer    = l1,
  };

  GFX_Camera cam = {
      .center = {200, 200},
      .zoom   = 0.8f,
  };

  GFX_Image font_img = d3d_load_png("W:/hi/run_tree/tex/pixel_font_basic_latin_ascii.png"_s8);
  GFX_Font  font     = gfx_make_font(font_img, 7, 9);

  psx_init();
  PSX_World_ID world = psx_make_world(64);
  PSX_Body_ID  body  = psx_world_add(
      world,
      {
            .pos            = sprite_1.pos * PSX_SCALE_INV,
            .center_of_mass = fvec2 {-sprite_1.sz.x, sprite_1.sz.y} * PSX_SCALE_INV * 0.5f,
            .mass           = 10,
            .linear_damping = 1.0f,
      });
  psx_body_add_box_shape(world, body, {0, 0}, sprite_1.sz * PSX_SCALE_INV);

  ui_init(gContext.misc_arena, 512);
  gUI.font        = &font;
  gUI.text_height = 12;

  LogEng_Info("Hello, World!");
  gDbgConsole.is_open = true;
  gDbgConsole.logs_max = 24;

  u64 frame       = 0;
  f32 psx_acc     = 0;
  f32 dt          = 0;
  f32 dt_min      = FLT_MAX;
  f32 dt_max      = 0;
  f32 frame_begin = os_seconds_since_startup();
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    // Frame start
    //
    ErrorContext("frame=%zu", frame);
    gfx_renderer_begin_frame();

    ui_begin();

    // Event handling
    //
    fvec2            mov_dir = {};
    OS_Window_Event *events  = os_gfx_event_pump(gContext.frame_arena);
    for (; events; events = events->next) {
      if (events->type == OS_EventType_ButtonPressed) {
        LogEng_Info("Event key: %S", game_input_to_str8((Game_Input)events->data.button));
        if (events->data.button == GameInput_LetterW) {
          mov_dir.y += 1;
        } else if (events->data.button == GameInput_LetterS) {
          mov_dir.y -= 1;
        } else if (events->data.button == GameInput_LetterA) {
          mov_dir.x -= 1;
        } else if (events->data.button == GameInput_LetterD) {
          mov_dir.x += 1;
        } else if (events->data.button == GameInput_LetterV) {
          gfx_set_vsync(!gfx_is_vsync_enabled());
        } else if (events->data.button == GameInput_LetterR) {
          dt_min = FLT_MAX;
          dt_max = 0;
        } else if (events->data.button == GameInput_F1) {
          gDbgConsole.is_open = !gDbgConsole.is_open;
        }
      }
    }
    psx_body_add_force(world, body, mov_dir * 1000.f);

    // Simulation update
    //

    psx_acc += dt;
    if (psx_acc >= PSX_STEP) {
      while (psx_acc > PSX_STEP) {
        psx_world_simulate(world, PSX_STEP);
        psx_acc -= PSX_STEP;
      }
      psx_world_simulate(world, psx_acc);
      psx_acc = 0;
    }

    // obj.pos.x += 0.5f;
    // obj2.pos.y += 0.5f;
    //  obj.material.rect.color.a += 1;
    //  obj2.material.rect.color.a += 1;
    sprite_1.pos = psx_body_get_position(world, body) * PSX_SCALE;
    rect_1.rot += 0.1f;

    // Draw stuff
    //
    cam.center.x += 0.05f;
    cam.center.y += 0.05f;

    gfx_set_camera_for_batches(cam);

    //OS_Memory_Info mem_info = os_get_memory_info();

    gfx_draw_sprite(bg);
    gfx_draw_sprite(sprite_1);

    gfx_draw_rect_color(rect_1, {.v = 0xFF'00'00'FF});
      UI_Widget *bg_widget = ui_push_widget({.key           = "background"_s8,
                                             .flags         = 0,
                                             .semantic_size = {
                                                 {
                                                     .kind       = UI_SizeKind_Pixels,
                                                     .value      = (f32)os_gfx_surface_width(),
                                                     .strictness = 1,
                                                 },
                                                 {
                                                     .kind       = UI_SizeKind_Pixels,
                                                     .value      = (f32)os_gfx_surface_height(),
                                                     .strictness = 1,
                                                 },
                                             }});
      Unused(bg_widget);

    /*
      Str8 dbg_text = str8_sprintf(gContext.frame_arena,
                                   "^wtime^=%.3fs\nf^dr^wa^eme^=%zu\n^wdt_min^=%0.3fms "
                                   "^wdt_max^=%0.3fms ^wdt^=%0.3fms, fps=%zu\n^Memory reserved: "
                                   "^d%_$$$zu^, Memory commited: ^d%_$$$zu^\n%S",
                                   os_seconds_since_startup(),
                                   frame,
                                   dt_min * 1000,
                                   dt_max * 1000,
                                   dt * 1000,
                                   u64(1.f / dt),
                                   os_get_memory_stats().reserved,
                                   os_get_memory_stats().commited,
                                   str8_dump_struct(mem_info));

      UI_Widget *txt_widget = ui_text("debug_text"_s8, dbg_text, 0);
      Unused(txt_widget);

      UI_Widget *txt_widget2 = ui_text("debug_text2"_s8, "This is one text"_s8, 0);
      Unused(txt_widget2);
      UI_Widget *txt_widget3 =
          ui_text("debug_text3"_s8, "This is second text"_s8, UI_WidgetFlag_HorizontalLayout);
      Unused(txt_widget3);
    */

    tools_update();
    ui_end();
    gfx_renderer_end_frame();

    // State cleanup
    //

    arena_clear(gContext.frame_arena);
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
    f32 frame_end = os_seconds_since_startup();
    dt            = frame_end - frame_begin;
    frame_begin   = frame_end;
    dt_min        = Min(dt, dt_min);
    dt_max        = Max(dt, dt_max);
  }
}
