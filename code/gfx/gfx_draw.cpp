#pragma once
#include "all_inc.hpp"

// Internal declarations
//

internal void
gfx_renderer_init_reuseable_resources();

internal void
gfx_renderer_push_object(GFX_Object const &object);

// Public definitions
//

global void
gfx_renderer_init() {
  ErrorContext("");
  // Set up render graph and call gfx_renderer_init_reusable_resources
}

global void
gfx_renderer_begin_frame() {
  // prepare objects_in_frame (its using frame arena)
  // reset free nodes/batches
  // start accepting new objects again (Flag)
}

global void
gfx_renderer_end_frame() {
  // Sort, call render graph, swap buffers
}

global void
gfx_draw_sprite(GFX_Layer layer, GFX_Sprite_Opts const &opts) {
  gfx_draw_sprite_color(layer, opts, GFX_Color {.v = 0xFF'FF'FF'FF});
}

global void
gfx_draw_sprite_color(GFX_Layer layer, GFX_Sprite_Opts const &opts, GFX_Color color) {
  gfx_renderer_push_object({
      .pos   = opts.pos,
      .sz    = opts.sz,
      .rot   = opts.rot,
      .layer = layer,
      .material =
          {
              .type = GFX_MaterialType_Sprite,
              .sprite =
                  {
                      .tex      = opts.tex,
                      .tex_rect = opts.tex_rect,
                      .color    = color,
                  },
          },
  });
}

global void
gfx_draw_rect_color(GFX_Layer layer, GFX_Rect_Opts const &opts, GFX_Color color) {
  gfx_renderer_push_object({
      .pos   = opts.pos,
      .sz    = opts.sz,
      .rot   = opts.rot,
      .layer = layer,
      .material =
          {
              .type = GFX_MaterialType_Rect,
              .rect =
                  {
                      .color = color,
                  },
          },
  });
}

// Internal definitions
//

internal void
gfx_renderer_init_reuseable_resources() {
  InvalidPath;
}

internal void
gfx_renderer_push_object(GFX_Object const &object) {
  Assert(gRen.is_accepting_new_objects);
  
  if (gRen.objects_in_frame.sz == gRen.objects_in_frame.cap) {
    u64 const   old_cap     = gRen.objects_in_frame.cap;
    u64 const   new_cap     = old_cap * 2;
    GFX_Object *new_objects = arena_alloc_array<GFX_Object>(gContext.frame_arena, new_cap);
    MemoryCopy(new_objects, gRen.objects_in_frame.v, old_cap);
    gRen.objects_in_frame.v   = new_objects;
    gRen.objects_in_frame.cap = new_cap;
  }

  gRen.objects_in_frame.v[gRen.objects_in_frame.sz] = object;
  gRen.objects_in_frame.sz++;
}
