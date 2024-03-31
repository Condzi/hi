#pragma once

u32 read_only global GFX_RG_MAX_RENDER_TARGETS_TO_CLEAR = 4;

enum GFX_RG_Op_Type {
  GFX_RG_OpType_None,

  GFX_RG_OpType_SetCamera,

  // Clears the given render target.
  //
  GFX_RG_OpType_ClearRenderTargets,

  // Render a batch to an image.
  //
  GFX_RG_OpType_Batch,

  // PostFX process to aply to an image.
  //
  GFX_RG_OpType_PostFx,

  // Combine two images together. Example: render two batches to separate images,
  // then combine batch A with batch B images. B will be on top of A.
  //
  GFX_RG_OpType_CombineImages
};

struct GFX_RG_Operation {
  GFX_RG_Op_Type type = GFX_RG_OpType_None;
  union {
    GFX_Camera camera = {};

    struct {
      GFX_Image *targets[GFX_RG_MAX_RENDER_TARGETS_TO_CLEAR];
      u32        num_targets;
    } clear;

    struct {
      GFX_Batch *batch;
    } batch;

    struct {
      GFX_Image *src;
      GFX_Fx     fx;
    } post_fx;

    struct {
      // Assume a and b are the same size and type!
      //
      GFX_Image *a;
      GFX_Image *b;
    } combine_images;

    struct {
      GFX_Image *src;
    } framebuffer;
  } input;

  GFX_Image *out = 0;
};

// Random values for now. I think it should be enough.
// We have max 2 parents (combine_images depends on 2 images)
// and max 4 children (this is arbitrary, but for now I guess we don't need more).
//
u64 read_only global GFX_RG_MAX_PARENTS  = 2;
u64 read_only global GFX_RG_MAX_CHILDREN = 4;

struct GFX_RG_Node {
  // When used as a single-link list, we use this. Otherwise, we use parents and
  // children fields.
  //
  GFX_RG_Node* next = 0;

  GFX_RG_Operation op;
  GFX_RG_Node     *parents[GFX_RG_MAX_PARENTS]   = {};
  GFX_RG_Node     *children[GFX_RG_MAX_CHILDREN] = {};

  u32 parents_count  = 0;
  u32 children_count = 0;
  // We have some 8 bit counter incremented every time we
  // start to traverse the graph. If the visit_cookie != counter,
  // it means it was not visited. if visit_cookie == counter, it means
  // it was visited.
  //
  u8 visit_cookie = 0;
};

u64 read_only global GFX_RG_MAX_ROOTS = 2;
struct GFX_Render_Graph {
  GFX_RG_Node *roots[GFX_RG_MAX_ROOTS] = {};
  u32          roots_count             = 0;
  u16          operations_count        = 0;
  u8           visit_counter           = 0;
};

must_use GFX_Render_Graph *
gfx_make_render_graph();

must_use GFX_RG_Node *
gfx_rg_make_node(GFX_Render_Graph *rg);

void
gfx_rg_attach_node_to_parent(GFX_RG_Node *parent, GFX_RG_Node *child);

must_use GFX_RG_Node *
gfx_rg_add_root(GFX_Render_Graph *rg);

// Traverse the render graph, make the list of operations and execute them.
//
must_use internal GFX_Image
gfx_rg_evaluate(GFX_Render_Graph *rg);

// Implemented by the target backend.
//
must_use internal GFX_Image
gfx_rg_execute_operations(GFX_RG_Operation *operations, u32 count);
