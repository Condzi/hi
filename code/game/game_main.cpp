#pragma once
#include "all_inc.hpp"

#include "game_main.hpp"

int
main(int argc, char const *argv[]) {
  gContext.frame_arena = make_arena(false);
  gContext.misc_arena  = make_arena(true);

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


  os_gfx_open_window({
      .title      = GAME_TITLE_LITERAL ""_s8,
      .width      = 1280,
      .height     = 720,
      .fullscreen = false,
  });

  gfx_init({.vp_width = os_gfx_surface_width(), .vp_height = os_gfx_surface_height()});
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
    .zoom = 0.8f,
  };

  GFX_Image font_img = d3d_load_png("W:/hi/run_tree/tex/pixel_font_basic_latin_ascii.png"_s8);
  GFX_Font  font     = gfx_make_font(font_img, 7, 9);

  u64 frame = 0;
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    // Frame start
    //
    ErrorContext("frame=%zu", frame);
    gfx_renderer_begin_frame();

    // Event handling
    //
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    for (;events; events = events->next) {
      os_debug_message(str8_sprintf(gContext.frame_arena, "%S\n", str8_dump_struct(*events)));
    }

    // Simulation update
    //

    // obj.pos.x += 0.5f;
    // obj2.pos.y += 0.5f;
    //  obj.material.rect.color.a += 1;
    //  obj2.material.rect.color.a += 1;
    sprite_1.rot -= 0.01f;
    rect_1.rot += 0.1f;


    // Draw stuff
    //
    cam.center.x += 0.05f;
    cam.center.y += 0.05f;

    gfx_set_camera_for_batches(cam);

    gfx_draw_sprite(bg);
    gfx_draw_sprite(sprite_1);
    gfx_draw_rect_color(rect_1, {.v = 0xFF'00'00'FF});
    gfx_draw_text_color(
        {
            .pos       = {0, 0},
            .height_px = 32,
            .font      = &font,
            .layer     = l4,
            .string    = str8_sprintf(gContext.frame_arena, "frame=%zu", frame),
        },
        {.v = 0xD04848FF});

    gfx_renderer_end_frame();

    // State cleanup
    //

    arena_clear(gContext.frame_arena);
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
  }
}
