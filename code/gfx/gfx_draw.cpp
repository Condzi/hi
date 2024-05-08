
// Internal declarations
//

internal void
gfx_renderer_init_reuseable_resources();

internal void
gfx_renderer_init_render_graph();

internal void
gfx_renderer_push_game_object(GFX_Object const &object);

internal void
gfx_renderer_push_ui_object(GFX_Object const &object);

must_use internal GFX_Batch *
gfx_renderer_request_batch(GFX_Material_Type type);

must_use internal GFX_RG_Node *
gfx_renderer_request_node();

// Public definitions
//

void
gfx_renderer_init() {
  ErrorContext("SPRITE_BATCHES_COUNT=%u, RECT_BATCHES_COUNT=%u",
               GFX_RENDERER_SPRITE_BATCHES_COUNT,
               GFX_RENDERER_RECT_BATCHES_COUNT);

  gRen.rg = gfx_make_render_graph();

  // We render all batches to one render target, but sorterd by layers every frame.
  //
  GFX_Image *batch_render_target = arena_alloc<GFX_Image>(gfx_arena);
  *batch_render_target = gfx_make_image(0, os_gfx_surface_width(), os_gfx_surface_height());

  // We always want to have render target that is the size of the framebuffer.
  //
  gfx_resize_image_with_framebuffer(batch_render_target);

  gRen.batch_render_target = batch_render_target;

  gfx_renderer_init_reuseable_resources();
  gfx_renderer_init_render_graph();

  gRen.game_objects_in_frame.cap = 64;
  gRen.ui_objects_in_frame.cap   = 64;
}

void
gfx_renderer_begin_frame() {
  u64 const   game_obj_cap   = gRen.game_objects_in_frame.cap;
  GFX_Object *game_obj       = arena_alloc_array<GFX_Object>(gContext.frame_arena, game_obj_cap);
  gRen.game_objects_in_frame = {
      .v   = game_obj,
      .cap = game_obj_cap,
  };

  u64 const   ui_obj_cap   = gRen.ui_objects_in_frame.cap;
  GFX_Object *ui_obj       = arena_alloc_array<GFX_Object>(gContext.frame_arena, ui_obj_cap);
  gRen.ui_objects_in_frame = {
      .v   = ui_obj,
      .cap = ui_obj_cap,
  };

  gRen.is_accepting_new_objects = true;
}

void
gfx_renderer_end_frame() {
  gRen.is_accepting_new_objects = false;
  // Game objects
  //
  {
    // Aliases
    //
    GFX_Object *objects    = gRen.game_objects_in_frame.v;
    u64 const   objects_sz = gRen.game_objects_in_frame.sz;
    ErrorContext("game_objects_count=%zu", objects_sz);

    if (objects && objects_sz) {
      // @Performance Bubble sort the objects by layer.
      //
      {
        bool swapped = false;
        for (u64 i = 0; i < objects_sz - 1; i++) {
          swapped = false;
          for (u64 j = 0; j < objects_sz - i - 1; j++) {
            GFX_Object &obj_j   = objects[j];
            GFX_Object &obj_j_1 = objects[j + 1];
            if (obj_j.layer.v > obj_j_1.layer.v) {
              Swap(obj_j, obj_j_1);
              swapped = true;
            }
          }
          if (!swapped) {
            break;
          }
        }
      }

      // Look for subsequences of objects with compatible materials for batching.
      // Immedietaly assign them to correct batches.
      // Add these batches to "used" list. Every next batch will be added
      // to the end of this list, which maintains the order.
      // Then, add the batch to render node and connect the previous node with next node.
      //
      {
        GFX_Batch   *current_batch = 0;
        GFX_RG_Node *current_node  = 0;
        for (u64 i = 0; i < objects_sz; i++) {
          GFX_Object const   &obj = objects[i];
          GFX_Material const &mat = obj.material;

          if (!current_batch || current_batch->sampler != mat.sampler ||
              (current_batch->type != mat.type) ||
              (current_batch->type == mat.type && mat.type == GFX_MaterialType_Sprite &&
               current_batch->data.sprite.texture.v[0] != mat.sprite.tex.v[0])) {

            current_batch          = gfx_renderer_request_batch(mat.type);
            current_batch->sampler = mat.sampler;
            if (mat.type == GFX_MaterialType_Sprite) {
              current_batch->data.sprite.texture = mat.sprite.tex;
            }

            GFX_RG_Node *next_node = gfx_renderer_request_node();
            next_node->op.type     = GFX_RG_OpType_Batch;
            next_node->op.out      = gRen.node_before_batchers->op.out;
            if (current_node) {
              gfx_rg_attach_node_to_parent(current_node, next_node);
            } else {
              //  It's the first node in the batcher's list.
              //
              gfx_rg_attach_node_to_parent(gRen.node_before_batchers, next_node);
            }
            current_node                       = next_node;
            current_node->op.input.batch.batch = current_batch;
          }

          gfx_batch_push(current_batch, obj);
        }

        // Connect last batcher node in the list to correct node in RG.
        //
        gfx_rg_attach_node_to_parent(current_node, gRen.node_after_batchers);
      }
    }
  }

  // UI
  { // Aliases
    //
    GFX_Object *objects    = gRen.ui_objects_in_frame.v;
    u64 const   objects_sz = gRen.ui_objects_in_frame.sz;
    ErrorContext("ui_objects_count=%zu", objects_sz);

    if (objects && objects_sz) {
      // @Performance Bubble sort the objects by layer.
      //
      {
        bool swapped = false;
        for (u64 i = 0; i < objects_sz - 1; i++) {
          swapped = false;
          for (u64 j = 0; j < objects_sz - i - 1; j++) {
            GFX_Object &obj_j   = objects[j];
            GFX_Object &obj_j_1 = objects[j + 1];
            if (obj_j.layer.v > obj_j_1.layer.v) {
              Swap(obj_j, obj_j_1);
              swapped = true;
            }
          }
          if (!swapped) {
            break;
          }
        }
      }

      // Look for subsequences of objects with compatible materials for batching.
      // Immedietaly assign them to correct batches.
      // Add these batches to "used" list. Every next batch will be added
      // to the end of this list, which maintains the order.
      // Then, add the batch to render node and connect the previous node with next node.
      //
      {
        GFX_Batch   *current_batch = 0;
        GFX_RG_Node *current_node  = 0;
        for (u64 i = 0; i < objects_sz; i++) {
          GFX_Object const   &obj = objects[i];
          GFX_Material const &mat = obj.material;

          if (!current_batch || current_batch->objects.sz == current_batch->objects.cap ||
              current_batch->sampler != mat.sampler || (current_batch->type != mat.type) ||
              (current_batch->type == mat.type && mat.type == GFX_MaterialType_Sprite &&
               current_batch->data.sprite.texture.v[0] != mat.sprite.tex.v[0])) {
            current_batch          = gfx_renderer_request_batch(mat.type);
            current_batch->sampler = mat.sampler;
            if (mat.type == GFX_MaterialType_Sprite) {
              current_batch->data.sprite.texture = mat.sprite.tex;
            }

            GFX_RG_Node *next_node = gfx_renderer_request_node();
            next_node->op.type     = GFX_RG_OpType_Batch;
            next_node->op.out      = gRen.node_after_batchers->op.out;
            if (current_node) {
              gfx_rg_attach_node_to_parent(current_node, next_node);
            } else {
              //  It's the first node in the batcher's list.
              //
              gfx_rg_attach_node_to_parent(gRen.node_before_ui, next_node);
            }
            current_node                       = next_node;
            current_node->op.input.batch.batch = current_batch;
          }

          gfx_batch_push(current_batch, obj);
        }

        // Connect last batcher node in the list to correct node in RG.
        //
        if (gRen.node_after_ui) {
          gfx_rg_attach_node_to_parent(current_node, gRen.node_after_ui);
        }
      }
    }
  }

  // Render the image
  //
  GFX_Image final_image = gfx_rg_evaluate(gRen.rg);
  gfx_swap_buffers(final_image);

  // Reset the render graph nodes
  //
  gRen.node_before_batchers->children_count = 0;
  gRen.node_after_batchers->parents_count   = 0;

  gRen.node_before_ui->children_count = 0;
  if (gRen.node_after_ui) {
    gRen.node_after_ui->parents_count = 0;
  }

  // Walk through the used nodes and reset their parents/children.
  //
  for (GFX_RG_Node *node = gRen.used_nodes; node;) {
    node->children_count = 0;
    node->parents_count  = 0;
    node->op             = {};
    GFX_RG_Node *next    = node->next;
    node->next           = 0;
    SLL_insert_at_end(gRen.free_nodes, node);
    node = next;
  }
  gRen.used_nodes = 0;

  // Same for batches.
  //
  for (GFX_Batch_Node *batch = gRen.used_batches; batch;) {
    batch->batch->objects.sz = 0;
    GFX_Batch_Node *next     = batch->next;
    DLL_remove(gRen.used_batches, batch);
    DLL_insert_at_front(gRen.free_batches, batch);
    batch = next;
  }
  gRen.used_batches = 0;
}

void
gfx_draw_text(GFX_Text_Opts const &opts) {
  gfx_draw_text_color(opts, {.v = 0xFF'FF'FF'FF});
}

void
gfx_draw_text_color(GFX_Text_Opts const &opts, GFX_Color color) {
  fvec2 pen = opts.pos;

  for (u64 i = 0; i < opts.string.sz; i++) {
    GFX_Glyph glyph = gfx_get_glyph(opts.font, opts.string.v[i], opts.height_px);

    gfx_renderer_push_ui_object({
        .pos   = pen,
        .sz    = glyph.sz,
        .layer = opts.layer,
        .material =
            {
                .type = GFX_MaterialType_Sprite,
                .sprite =
                    {
                        .tex      = opts.font->image,
                        .tex_rect = glyph.rect,
                        .color    = color,
                    },
                .sampler = GFX_SamplerType_PixelPerfect,
            },
    });

    pen.x += glyph.advance_x;
  }
}

void
gfx_draw_rich_text(GFX_Rich_Text_Opts const &opts) {
  gfx_draw_rich_text_color(opts, {.v = 0xFF'FF'FF'FF});
}

void
gfx_draw_rich_text_color(GFX_Rich_Text_Opts const &opts, GFX_Color color) {
  Assert(opts.string.sz);
  u32 read_only local_persist       TAB_SIZE   = 4;
  GFX_Color read_only local_persist COLOR_ERR  = {.v = 0xD04848FF};
  GFX_Color read_only local_persist COLOR_WRN  = {.v = 0xF3B95FFF};
  GFX_Color read_only local_persist COLOR_DBG  = {.v = 0x6895D2FF};
  GFX_Color const                   color_base = color;

  GFX_Object obj = {
      .layer = opts.layer,
      .material =
          {
              .type = GFX_MaterialType_Sprite,
              .sprite =
                  {
                      .tex = opts.font->image,
                  },
              .sampler = GFX_SamplerType_PixelPerfect,
          },
  };

  fvec2 pen = opts.pos;
  for (u64 i = 0; i < opts.string.sz; i++) {
    u8 const  ch    = opts.string.v[i];
    GFX_Glyph glyph = gfx_get_glyph(opts.font, ch, opts.height_px);

    obj.material.sprite.color    = color;
    obj.material.sprite.tex_rect = glyph.rect;
    obj.sz                       = glyph.sz;
    obj.pos                      = pen;

    switch (ch) {
      default: {
        gfx_renderer_push_ui_object(obj);
        pen.x += glyph.advance_x;
      } break;

      case '\n': {
        pen.x = opts.pos.x;
        pen.y -= glyph.sz.height;
      } break;

      case '\t': {
        pen.x += glyph.advance_x * TAB_SIZE;
      } break;

      case '^': {
        if (i + 1 < opts.string.sz) {
          i++;
          u8 const modifier = opts.string.v[i];
          switch (modifier) {
            default: {
              i--; // the char does not set the color, so we render it.
              color = color_base;
            } break;

            case 'E': fallthrough;
            case 'e': {
              color = COLOR_ERR;
            } break;
            case 'W': fallthrough;
            case 'w': {
              color = COLOR_WRN;
            } break;
            case 'I': fallthrough;
            case 'i': {
              color = color_base;
            } break;
            case 'D': fallthrough;
            case 'd': {
              color = COLOR_DBG;
            } break;
          }
        }
      } break;
    }
  }
}

void
gfx_draw_sprite(GFX_Sprite_Opts const &opts) {
  gfx_draw_sprite_color(opts, {.v = 0xFF'FF'FF'FF});
}

void
gfx_draw_sprite_color(GFX_Sprite_Opts const &opts, GFX_Color color) {
  gfx_renderer_push_game_object({
      .pos   = opts.pos,
      .sz    = opts.sz,
      .rot   = opts.rot,
      .layer = opts.layer,
      .material =
          {
              .type = GFX_MaterialType_Sprite,
              .sprite =
                  {
                      .tex      = opts.tex,
                      .tex_rect = opts.tex_rect,
                      .color    = color,
                  },
              .sampler = opts.sampler,
          },
  });
}

void
gfx_draw_rect_color(GFX_Rect_Opts const &opts, GFX_Color color) {
  gfx_renderer_push_game_object({
      .pos   = opts.pos,
      .sz    = opts.sz,
      .rot   = opts.rot,
      .layer = opts.layer,
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

void
gfx_draw_rect_color_ui(GFX_Rect_Opts const &opts, GFX_Color color) {
  gfx_renderer_push_ui_object({
      .pos   = opts.pos,
      .sz    = opts.sz,
      .rot   = opts.rot,
      .layer = opts.layer,
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
    SLL_insert_at_end(gRen.free_nodes, node);
  }

  // Alloc batches
  //
  for (u32 i = 0; i < GFX_RENDERER_SPRITE_BATCHES_COUNT; i++) {
    GFX_Batch      *batch = gfx_make_batch(GFX_MaterialType_Sprite);
    GFX_Batch_Node *node  = arena_alloc<GFX_Batch_Node>(gfx_arena);
    node->batch           = batch;
    DLL_insert_at_front(gRen.free_batches, node);
  }

  for (u32 i = 0; i < GFX_RENDERER_RECT_BATCHES_COUNT; i++) {
    GFX_Batch      *batch = gfx_make_batch(GFX_MaterialType_Rect);
    GFX_Batch_Node *node  = arena_alloc<GFX_Batch_Node>(gfx_arena);
    node->batch           = batch;
    DLL_insert_at_front(gRen.free_batches, node);
  }
}

internal void
gfx_renderer_init_render_graph() {
  // Allocate common nodes
  //
  GFX_RG_Node *game_obj_root      = gfx_rg_add_root(gRen.rg);
  GFX_RG_Node *clear_game_targets = gfx_rg_make_node(gRen.rg);
  GFX_RG_Node *vignette           = gfx_rg_make_node(gRen.rg);

  // Connect common nodes
  // (we have very few nodes so nothing gets connected here ATM)
  //
  gRen.batch_camera = game_obj_root;
  gfx_rg_attach_node_to_parent(game_obj_root, clear_game_targets);
  gRen.node_before_batchers = clear_game_targets;
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
  game_obj_root->op = {
      .type = GFX_RG_OpType_SetCamera,
      .input =
          {
              .camera =
                  {
                      .center = {.x = 0, .y = 0},
                      .rot    = 0,
                      .zoom   = 1,
                  },
          },
      .out = gRen.batch_render_target,
  };

  clear_game_targets->op = {
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

  // Set up the UI part.
  //

  GFX_RG_Node *ui_obj_root = gfx_rg_make_node(gRen.rg);

  gfx_rg_attach_node_to_parent(vignette, ui_obj_root);
  gRen.node_before_ui = ui_obj_root;
  gRen.node_after_ui  = 0;

  ui_obj_root->op = {
      .type = GFX_RG_OpType_SetCamera,
      .input =
          {
              .camera =
                  {
                      .center = {os_gfx_surface_width() / 2.f, -(os_gfx_surface_height() / 2.f)},
                      .rot    = 0,
                      .zoom   = 1,
                  },
          },
      .out = vignette_target,
  };
}

internal void
gfx_renderer_push_game_object(GFX_Object const &object) {
  Assert(gRen.is_accepting_new_objects);

  if (gRen.game_objects_in_frame.sz == gRen.game_objects_in_frame.cap) {
    u64 const   old_cap     = gRen.game_objects_in_frame.cap;
    u64 const   new_cap     = old_cap * 2;
    GFX_Object *new_objects = arena_alloc_array<GFX_Object>(gContext.frame_arena, new_cap);
    MemoryCopy(new_objects, gRen.game_objects_in_frame.v, old_cap);
    gRen.game_objects_in_frame.v   = new_objects;
    gRen.game_objects_in_frame.cap = new_cap;
  }

  gRen.game_objects_in_frame.v[gRen.game_objects_in_frame.sz] = object;
  gRen.game_objects_in_frame.sz++;
}

internal void
gfx_renderer_push_ui_object(GFX_Object const &object) {
  Assert(gRen.is_accepting_new_objects);
  if (gRen.ui_objects_in_frame.sz == gRen.ui_objects_in_frame.cap) {
    u64 const   old_cap     = gRen.ui_objects_in_frame.cap;
    u64 const   new_cap     = old_cap * 2;
    GFX_Object *new_objects = arena_alloc_array<GFX_Object>(gContext.frame_arena, new_cap);
    MemoryCopy(new_objects, gRen.ui_objects_in_frame.v, old_cap);
    gRen.ui_objects_in_frame.v   = new_objects;
    gRen.ui_objects_in_frame.cap = new_cap;
  }

  gRen.ui_objects_in_frame.v[gRen.ui_objects_in_frame.sz] = object;
  gRen.ui_objects_in_frame.sz++;
}

must_use internal GFX_Batch *
gfx_renderer_request_batch(GFX_Material_Type type) {
  GFX_Batch_Node *node = 0;
  // First, check if there is a free batch available.
  //
  for (node = gRen.free_batches; node; node = node->next) {
    if (node->batch->type == type) {
      DLL_remove(gRen.free_batches, node);
      break;
    }
  }

  // If we don't have a free batch, allocate it.
  //
  if (!node) {
    node        = arena_alloc<GFX_Batch_Node>(gfx_arena);
    node->batch = gfx_make_batch(type);
  }

  DLL_insert_at_front(gRen.used_batches, node);
  return node->batch;
}

must_use internal GFX_RG_Node *
gfx_renderer_request_node() {
  GFX_RG_Node *node = 0;

  // Check if a free RG node is available.
  //
  if (gRen.free_nodes) {
    node            = gRen.free_nodes;
    gRen.free_nodes = node->next;
    node->next      = 0;
  }

  // Not available -- create a new one.
  //
  if (!node) {
    node = gfx_rg_make_node(gRen.rg);
  }

  node->op = {
      .type = GFX_RG_OpType_Batch,
      .out  = gRen.batch_render_target,
  };

  SLL_insert_at_end(gRen.used_nodes, node);
  return node;
}

void
gfx_set_camera_for_batches(GFX_Camera const &cam) {
  gRen.batch_camera->op.input.camera = cam;
}

must_use fmat4
gfx_calc_camera_matrix(GFX_Camera const &cam) {
  fvec2 const center = cam.center * -1.f;
  f32 const   rot    = cam.rot;
  f32 const   zoom   = cam.zoom;
  ErrorContext("center={%g,%g}, rot=%g, zoom=%g", center.x, center.y, rot, zoom);
  ErrorIf(zoom < 0.05f, "Tiny zoom, probably an error.");

  // @Todo: half_out_sz should be calculated using current image size, not the gfx surface size.
  //
  fvec2 const half_out_sz = {(f32)os_gfx_surface_width() / 2, (f32)os_gfx_surface_height() / 2};
  fmat4 const result      = calc_transform_matrix({
           .translation = center,
           .rot         = rot,
           .rot_center  = half_out_sz,
           .scale =
          {
                   .x = zoom,
                   .y = zoom,
          },
  });

  return result;
}

must_use fmat4
gfx_calc_inv_camera_matrix(GFX_Camera const &cam) {
  fvec2 const center = cam.center * -1.f;
  f32 const   rot    = cam.rot;
  f32 const   zoom   = cam.zoom;
  ErrorContext("center={%g,%g}, rot=%g, zoom=%g", center.x, center.y, rot, zoom);
  ErrorIf(zoom < 0.05f, "Tiny zoom, probably an error.");

  // @Todo: @Copypasta: half_out_sz should be calculated using current image size, not the gfx
  // surface size.
  //
  fvec2 const half_out_sz = {(f32)os_gfx_surface_width() / 2, (f32)os_gfx_surface_height() / 2};
  fmat4 const result      = calc_transform_matrix_inv({
           .translation = center,
           .rot         = rot,
           .rot_center  = half_out_sz,
           .scale =
          {
                   .x = zoom,
                   .y = zoom,
          },
  });
  return result;
}
