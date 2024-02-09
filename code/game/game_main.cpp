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

  gfx_init({.vp_width = os_gfx_surface_width(), .vp_height = os_gfx_surface_height()});

  GFX_Object obj = {
      .pos = {200, 200},
      .sz  = {50.f, 50.f},
      .material =
          {
              .type = GFX_MaterialType_Rect,
              .rect = {.color = {.v = 0xFF0000FF}},
          },
  };

  GFX_Object obj2 = obj;
  obj2.material.rect.color.v = 0x00FF00FF;

  GFX_Batch *batch_a = gfx_make_batch(GFX_MaterialType_Rect);
  GFX_Batch *batch_b = gfx_make_batch(GFX_MaterialType_Rect);
  u32 const  width   = (u32)batch_a->viewport.sz.width;
  u32 const  height  = (u32)batch_a->viewport.sz.height;

  GFX_Image background   = gfx_make_image(gfx_checkerboard(8, 16, 16), 16 * 8, 16 * 8);
  GFX_Image target_a     = gfx_make_image(0, width, height);
  GFX_Image target_b     = gfx_make_image(0, width, height);
  GFX_Image target_ab    = gfx_make_image(0, width, height);
  GFX_Image target_final = gfx_make_image(0, width, height);

  GFX_Render_Graph *rg            = gfx_make_render_graph();
  GFX_RG_Node      *root          = gfx_rg_add_root(rg);
  GFX_RG_Node      *draw_a        = gfx_rg_make_node(rg);
  GFX_RG_Node      *draw_b        = gfx_rg_make_node(rg);
  GFX_RG_Node      *combine_ab    = gfx_rg_make_node(rg);
  GFX_RG_Node      *combine_ab_bg = gfx_rg_make_node(rg);

  gfx_rg_attach_node_to_parent(root, draw_a);
  gfx_rg_attach_node_to_parent(root, draw_b);
  gfx_rg_attach_node_to_parent(draw_a, combine_ab);
  gfx_rg_attach_node_to_parent(draw_b, combine_ab);
  gfx_rg_attach_node_to_parent(combine_ab, combine_ab_bg);

  root->op.type = GFX_RG_OpType_ClearRenderTargets;
  root->op.input.clear.num_targets = 4;
  root->op.input.clear.targets[0]  = &target_a;
  root->op.input.clear.targets[1]  = &target_b;
  root->op.input.clear.targets[2]  = &target_ab;
  root->op.input.clear.targets[3]  = &target_final;

  draw_a->op.type              = GFX_RG_OpType_Batch;
  draw_a->op.input.batch.batch = batch_a;
  draw_a->op.out               = target_a;

  draw_b->op.type              = GFX_RG_OpType_Batch;
  draw_b->op.input.batch.batch = batch_b;
  draw_b->op.out               = target_b;

  combine_ab->op.type                   = GFX_RG_OpType_CombineImages;
  combine_ab->op.input.combine_images.a = target_b;
  combine_ab->op.input.combine_images.b = target_a;
  combine_ab->op.out                    = target_ab;

  combine_ab_bg->op.type                   = GFX_RG_OpType_CombineImages;
  combine_ab_bg->op.input.combine_images.a = background;
  combine_ab_bg->op.input.combine_images.b = target_ab;
  combine_ab_bg->op.out                    = target_final;

  u64 frame = 0;
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    ErrorContext("frame=%zu"_s8, frame);
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    obj.pos.x += 0.5f;
    obj2.pos.y += 0.5f;
    //obj.material.rect.color.a += 1;
    //obj2.material.rect.color.a += 1;
    obj.rot -= 0.1f;
    obj2.rot += 0.1f;

    gfx_batch_push(batch_a, obj);
    gfx_batch_push(batch_b, obj2);

    GFX_Image graph_result = gfx_rg_evaluate(rg);
    gD3d.deferred_context->CopyResource(gD3d.framebuffer, (ID3D11Resource *)(graph_result.v[0]));

    gfx_swap_buffers();

    // Reset the batches
    //
    batch_a->objects.sz = 0;
    batch_b->objects.sz = 0;

    arena_clear(gContext.frame_arena);

    // Reset the error context so we don't leak the memory.
    //
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
  }
}
