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
      .sz  = fvec2 {54.f, 43.f} * 3,
      .material =
          {
              .type = GFX_MaterialType_Sprite,
              .sprite =
                  {
                      .tex_rect = {.x = 263, .y = 132, .w = 49, .h = 43},
                      .color    = {.v = 0xFFFFFFFF},
                  },
          },
  };

  GFX_Object obj2 = {
      .pos = {600, 200},
      .sz  = {50.f, 50.f},
      .material =
          {
              .type = GFX_MaterialType_Rect,
              .rect = {.color = {.v = 0xFF0000FF}},
          },
  };

  GFX_Image characters_img = d3d_load_png("W:/hi/run_tree/tex/characters.png"_s8);

  GFX_Batch *batch_a           = gfx_make_batch(GFX_MaterialType_Sprite);
  batch_a->data.sprite.texture = characters_img;

  GFX_Batch *batch_b = gfx_make_batch(GFX_MaterialType_Rect);
  u32 const  width   = os_gfx_surface_width();
  u32 const  height  = os_gfx_surface_height();

  GFX_Image target_a     = gfx_make_image(0, width, height);
  GFX_Image target_a_blurred = gfx_make_image(0, width, height);
  GFX_Image target_b     = gfx_make_image(0, width, height);
  GFX_Image target_ab    = gfx_make_image(0, width, height);

  gfx_resize_image_with_framebuffer(&target_a);
  gfx_resize_image_with_framebuffer(&target_a_blurred);
  gfx_resize_image_with_framebuffer(&target_b);
  gfx_resize_image_with_framebuffer(&target_ab);

  GFX_Render_Graph *rg            = gfx_make_render_graph();
  GFX_RG_Node      *root_1        = gfx_rg_add_root(rg);
  GFX_RG_Node      *draw_a        = gfx_rg_make_node(rg);
  GFX_RG_Node      *blur_a        = gfx_rg_make_node(rg);
  GFX_RG_Node      *draw_b        = gfx_rg_make_node(rg);
  GFX_RG_Node      *combine_ab    = gfx_rg_make_node(rg);

  gfx_rg_attach_node_to_parent(root_1, draw_a);
  gfx_rg_attach_node_to_parent(root_1, draw_b);
  gfx_rg_attach_node_to_parent(draw_a, blur_a);
  gfx_rg_attach_node_to_parent(blur_a, combine_ab);
  gfx_rg_attach_node_to_parent(draw_b, combine_ab);

  root_1->op.type                    = GFX_RG_OpType_ClearRenderTargets;
  root_1->op.input.clear.num_targets = 4;
  root_1->op.input.clear.targets[0]  = &target_a;
  root_1->op.input.clear.targets[1]  = &target_b;
  root_1->op.input.clear.targets[2]  = &target_ab;
  root_1->op.input.clear.targets[3]  = &target_a_blurred;

  draw_a->op.type              = GFX_RG_OpType_Batch;
  draw_a->op.input.batch.batch = batch_a;
  draw_a->op.out               = &target_a;

  blur_a->op.type                  = GFX_RG_OpType_PostFx;
  blur_a->op.input.post_fx.fx.type = GFX_PostFXType_Blur;
  blur_a->op.input.post_fx.src     = &target_a;
  blur_a->op.out                   = &target_a_blurred;

  draw_b->op.type              = GFX_RG_OpType_Batch;
  draw_b->op.input.batch.batch = batch_b;
  draw_b->op.out               = &target_b;

  combine_ab->op.type                   = GFX_RG_OpType_CombineImages;
  combine_ab->op.input.combine_images.a = &target_b;
  combine_ab->op.input.combine_images.b = &target_a_blurred;
  combine_ab->op.out                    = &target_ab;

  GFX_Object bg_obj = {
      .pos = {0, (f32)height},
      .sz  = {(f32)width, (f32)height},
      .material =
          {
              .type = GFX_MaterialType_Sprite,
              .sprite =
                  {
                      .tex_rect = {.x = 0, .y = 0, .w = (u16)width, .h = (u16)height},
                      .color    = {.v = 0xFFFFFFFF},
                  },
          },
  };

  GFX_Image bg_img       = gfx_checkerboard_image(64, 16, 16);
  GFX_Image target_bg    = gfx_make_image(0, width, height);
  GFX_Image target_ab_bg = gfx_make_image(0, width, height);
  GFX_Image target_vignette = gfx_make_image(0, width, height);

  gfx_resize_image_with_framebuffer(&target_bg);
  gfx_resize_image_with_framebuffer(&target_ab_bg);
  gfx_resize_image_with_framebuffer(&target_vignette);


  GFX_Batch *batch_bg           = gfx_make_batch(GFX_MaterialType_Sprite);
  batch_bg->data.sprite.texture = bg_img;

  GFX_RG_Node *root_2        = gfx_rg_add_root(rg);
  GFX_RG_Node *draw_bg       = gfx_rg_make_node(rg);
  GFX_RG_Node *combine_ab_bg = gfx_rg_make_node(rg);
  GFX_RG_Node *vignette      = gfx_rg_make_node(rg);

  gfx_rg_attach_node_to_parent(root_2, draw_bg);
  gfx_rg_attach_node_to_parent(draw_bg, combine_ab_bg);
  gfx_rg_attach_node_to_parent(combine_ab, combine_ab_bg);
  gfx_rg_attach_node_to_parent(combine_ab_bg, vignette);

  root_2->op.type                    = GFX_RG_OpType_ClearRenderTargets;
  root_2->op.input.clear.num_targets = 3;
  root_2->op.input.clear.targets[0]  = &target_bg;
  root_2->op.input.clear.targets[1]  = &target_ab_bg;
  root_2->op.input.clear.targets[2]  = &target_vignette;

  draw_bg->op.type              = GFX_RG_OpType_Batch;
  draw_bg->op.input.batch.batch = batch_bg;
  draw_bg->op.out               = &target_bg;

  combine_ab_bg->op.type                   = GFX_RG_OpType_CombineImages;
  combine_ab_bg->op.input.combine_images.a = &target_bg;
  combine_ab_bg->op.input.combine_images.b = &target_ab;
  combine_ab_bg->op.out                    = &target_ab_bg;

  vignette->op.type                  = GFX_RG_OpType_PostFx;
  vignette->op.input.post_fx.fx.type = GFX_PostFXType_Vignette;
  vignette->op.input.post_fx.src     = &target_ab_bg;
  vignette->op.out                   = &target_vignette;

  u64 frame = 0;
  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    ErrorContext("frame=%zu", frame);
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    // obj.pos.x += 0.5f;
    // obj2.pos.y += 0.5f;
    //  obj.material.rect.color.a += 1;
    //  obj2.material.rect.color.a += 1;
    obj.rot -= 0.01f;
    obj2.rot += 0.1f;

    gfx_batch_push(batch_a, obj);
    gfx_batch_push(batch_b, obj2);
    gfx_batch_push(batch_bg, bg_obj);

    GFX_Image graph_result = gfx_rg_evaluate(rg);
    gD3d.deferred_context->CopyResource(gD3d.framebuffer, (ID3D11Resource *)(graph_result.v[0]));

    gfx_swap_buffers();

    // Reset the batches
    //
    batch_a->objects.sz = 0;
    batch_b->objects.sz = 0;
    batch_bg->objects.sz = 0;

    arena_clear(gContext.frame_arena);

    // Reset the error context so we don't leak the memory.
    //
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;
    frame++;
  }
}
