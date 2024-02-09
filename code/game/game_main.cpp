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

  u32 const width  = (u32)batch->viewport.sz.width;
  u32 const height = (u32)batch->viewport.sz.height;
  GFX_Image target = gfx_make_image(0, width, height);

  u64 frame = 0;
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    ErrorContext("frame=%zu"_s8, frame);
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    obj.pos.x += 0.0001f;
    obj2.pos.y += 0.0001f;
    obj.material.data.rect.color.v += 0xff;
    obj2.material.data.rect.color.v += 1;

    gfx_batch_push(batch, obj);
    gfx_batch_push(batch, obj2);

    gfx_batch_draw(batch, target);

    // Copy batch to frame buffer
    //
    gD3d.deferred_context->CopyResource(gD3d.framebuffer, (ID3D11Resource*)target.v[0]);

    gfx_swap_buffers();

    // Reset the batches
    //
    batch->objects.sz = 0;

    arena_clear(gContext.frame_arena);

    // Reset the error context so we don't leak the memory.
    //
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
  }
}
