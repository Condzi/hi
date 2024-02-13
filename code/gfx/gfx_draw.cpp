#pragma once
#include "all_inc.hpp"

// Internal declarations
//

internal void
gfx_renderer_init_reuseable_resources();

internal void
gfx_renderer_init_render_graph();

internal void
gfx_renderer_push_object(GFX_Object const &object);

// Public definitions
//

global void
gfx_renderer_init() {
  ErrorContext("SPRITE_BATCHES_COUNT=%u, RECT_BATCHES_COUNT=%u",
               GFX_RENDERER_SPRITE_BATCHES_COUNT,
               GFX_RENDERER_RECT_BATCHES_COUNT);

  gRen.rg = gfx_make_render_graph();

  // We render all batches to one render target, but sorterd by layers every frame.
  //
  GFX_Image *render_target = arena_alloc<GFX_Image>(gfx_arena);
  *render_target           = gfx_make_image(0, os_gfx_surface_width(), os_gfx_surface_height());
  // We always want to have render target that is the size of the framebuffer.
  //
  gfx_resize_image_with_framebuffer(render_target);
  gRen.batch_render_target = render_target;

  gfx_renderer_init_reuseable_resources();
  gfx_renderer_init_render_graph();
}

global void
gfx_renderer_begin_frame() {
  u64 const   obj_cap   = gRen.objects_in_frame.cap;
  GFX_Object *obj       = arena_alloc_array<GFX_Object>(gContext.frame_arena, obj_cap);
  gRen.objects_in_frame = {
      .v   = obj,
      .cap = obj_cap,
  };

  gRen.is_accepting_new_objects = true;
}

global void
gfx_renderer_end_frame() {
  ErrorContext("objets_in_frame.sz=%zu", gRen.objects_in_frame.sz);

  gRen.is_accepting_new_objects = false;
  GFX_Image final_image;

  if (gRen.objects_in_frame.sz) {
    // Sort, assign objects to batches and attach batches RG nodes. Then attach nodes
    // to each other and finally attach to render graph.
    //
  }

  // Render the image
  //
  final_image = gfx_rg_evaluate(gRen.rg);
  gfx_swap_buffers(final_image);

  // Reset the render graph nodes
  //
  gRen.node_before_batchers->children_count = 0;
  gRen.node_after_batchers->parents_count   = 0;

  // Walk through the used nodes and reset their parents/children.
  //
  for (GFX_RG_Node *node = gRen.used_nodes; node; node = node->next) {
    node->children_count = 0;
    node->parents_count  = 0;
    SLL_insert(gRen.free_nodes, node);
  }
  gRen.used_nodes = 0;

  // Same for batches.
  //
  for (GFX_Batch_Node *batch = gRen.used_batches; batch; batch = batch->next) {
    batch->batch->objects.sz = 0;
    SLL_insert(gRen.free_batches, batch);
  }
  gRen.used_batches = 0;
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
  // Alloc nodes
  //
  for (u32 i = 0; i < GFX_RENDERER_BATCHES_TOTAL; i++) {
    GFX_RG_Node *node = gfx_rg_make_node(gRen.rg);
    node->op          = {
                 .type = GFX_RG_OpType_Batch,
                 .out  = gRen.batch_render_target,
    };
    SLL_insert(gRen.free_nodes, node);
  }

  // Alloc batches
  //
  for (u32 i = 0; i < GFX_RENDERER_SPRITE_BATCHES_COUNT; i++) {
    GFX_Batch      *batch = gfx_make_batch(GFX_MaterialType_Sprite);
    GFX_Batch_Node *node  = arena_alloc<GFX_Batch_Node>(gfx_arena);
    node->batch           = batch;
    SLL_insert(gRen.free_batches, node);
  }

  for (u32 i = 0; i < GFX_RENDERER_RECT_BATCHES_COUNT; i++) {
    GFX_Batch      *batch = gfx_make_batch(GFX_MaterialType_Rect);
    GFX_Batch_Node *node  = arena_alloc<GFX_Batch_Node>(gfx_arena);
    node->batch           = batch;
    SLL_insert(gRen.free_batches, node);
  }
}

internal void
gfx_renderer_init_render_graph() {
  // Allocate common nodes
  //
  GFX_RG_Node *root     = gfx_rg_add_root(gRen.rg);
  GFX_RG_Node *vignette = gfx_rg_make_node(gRen.rg);

  // Connect common nodes
  // (we have very few nodes so nothing gets connected here ATM)
  //
  gRen.node_before_batchers = root;
  gRen.node_after_batchers  = vignette;

  // Set up common resources
  //
  GFX_Image *vignette_target = arena_alloc<GFX_Image>(gfx_arena);
  *vignette_target           = gfx_make_image(0, os_gfx_surface_width(), os_gfx_surface_height());
  gfx_resize_image_with_framebuffer(vignette_target);

  // Set up operations
  //

  // If nothing is rendered, only root node will be executed in the render graph.
  // In order to keep things working, we set the "out" image to be some valid render target,
  // in this case the batch render target, so when we copy the last output to the frame
  // buffer, we will have some valid image.
  //
  root->op = {
      .type = GFX_RG_OpType_ClearRenderTargets,
      .input =
          {
              .clear =
                  {
                      .targets     = {gRen.batch_render_target, vignette_target},
                      .num_targets = 2,
                  },
          },
      .out = gRen.batch_render_target,
  };

  vignette->op = {
      .type = GFX_RG_OpType_PostFx,
      .input =
          {
              .post_fx =
                  {
                      .src = gRen.batch_render_target,
                      .fx =
                          {
                              .type     = GFX_PostFXType_Vignette,
                              .strength = 1,
                          },
                  },
          },
      .out = vignette_target,
  };
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
