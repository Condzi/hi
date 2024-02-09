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

  Arena *a = make_arena(true);

  u64 *u = (u64 *)arena_alloc(a, 80, 8);
  Unused(u);
  u = (u64 *)arena_alloc(a, 80, 16);
  // This should trigger asan!
  // u[11] = 123;

  Unused(arena_alloc(a, MB(62), 8));
  // This should be in a new arena
  //
  u = (u64 *)arena_alloc(a, MB(2), 8);

  // This should be in the old one
  //
  u = (u64 *)arena_alloc(a, 8, 8);
  // u[1] = 123;

  u64 now = os_now_us();
  Unused(now);

  os_debug_message("Cześć, Świecie! ąćźłóę ĄĆŹŁÓĘ\n"_s8);

  os_debug_message(str8_sprintf(a, "Test: %_$$d. 8 in binary is 0b%b"_s8, MB(128), 8));

  os_gfx_open_window({
      .title      = GAME_TITLE_LITERAL ""_s8,
      .width      = 1280,
      .height     = 720,
      .fullscreen = false,
  });

  gfx_init({.vp_width = 1280, .vp_height = 720});
  GFX_Batch *batch = gfx_make_batch(GFX_MaterialType_Rect);

  GFX_Object obj = {
      .pos = {0, 0},
      .sz  = {0.25f, 0.25f},
      .material =
          {
              .type = GFX_MaterialType_Rect,
              .data = {.rect = {.color = {.v = 0xFF0000FF}}},
          },
  };

  GFX_Object obj2 = obj;
  obj2.pos.x += 0.1f;
  obj2.pos.y += 0.1f;

  GFX_Image target;
  target.v[0] = PtrToU64(gD3d.framebuffer);

  u64 frame = 0;
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    ErrorContext("frame=%zu"_s8, frame);
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    obj.pos.x += 0.0001f;
    obj2.pos.y += 0.0001f;
    obj.material.data.rect.color.g += 1;
    obj2.material.data.rect.color.b += 1;

    gfx_batch_push(batch, obj);
    gfx_batch_push(batch, obj2);

    gfx_batch_draw(batch, target);
    batch->objects.sz = 0;

    gfx_swap_buffers();

    arena_clear(gContext.frame_arena);

    // Reset the error context so we don't leak the memory.
    //
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
  }
}
