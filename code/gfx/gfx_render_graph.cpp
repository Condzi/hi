
must_use GFX_Render_Graph *
gfx_make_render_graph() {
  GFX_Render_Graph *rg = arena_alloc<GFX_Render_Graph>(gfx_arena);
  return rg;
}

must_use GFX_RG_Node *
gfx_rg_make_node(GFX_Render_Graph *rg) {
  Assert(rg);

  rg->operations_count++;
  GFX_RG_Node *node = arena_alloc<GFX_RG_Node>(gfx_arena);
  return node;
}

void
gfx_rg_attach_node_to_parent(GFX_RG_Node *parent, GFX_RG_Node *child) {
  Assert(parent);
  Assert(child);
  Assert(parent->children_count != GFX_RG_MAX_CHILDREN);
  Assert(child->parents_count != GFX_RG_MAX_PARENTS);

  parent->children[parent->children_count] = child;
  child->parents[child->parents_count]     = parent;
  parent->children_count++;
  child->parents_count++;
}

must_use GFX_RG_Node *
gfx_rg_add_root(GFX_Render_Graph *rg) {
  Assert(rg);
  Assert(rg->roots_count != GFX_RG_MAX_ROOTS);

  GFX_RG_Node *root          = gfx_rg_make_node(rg);
  rg->roots[rg->roots_count] = root;
  rg->roots_count++;
  return root;
}

internal void
gfx_dsf(GFX_RG_Node *node, u8 visit_cookie, GFX_RG_Operation *op_arr, u32 &op_arr_idx) {
  if (!node) {
    return;
  }
  if (node->visit_cookie == visit_cookie) {
    // Already visited.
    //
    return;
  }
  node->visit_cookie = visit_cookie;

  // Make sure the parents are completed first.
  //
  for (u32 i = 0; i < node->parents_count; i++) {
    gfx_dsf(node->parents[i], visit_cookie, op_arr, op_arr_idx);
  }

  op_arr[op_arr_idx] = node->op;
  op_arr_idx++;

  for (u32 i = 0; i < node->children_count; i++) {
    gfx_dsf(node->children[i], visit_cookie, op_arr, op_arr_idx);
  }
}

must_use GFX_Image
gfx_rg_evaluate(GFX_Render_Graph *rg) {
  Assert(rg);
  ErrorContext("roots=%d, operations=%d (MAX), visit_counter=%d",
               (int)rg->roots_count,
               (int)rg->operations_count,
               (int)rg->visit_counter);

  // Prepare the space for operations.
  //
  Scratch_Buffer scratch = scratch_begin(gContext.frame_arena);
  Defer { scratch_end(&scratch); };

  GFX_RG_Operation *op_arr =
      arena_alloc_array<GFX_RG_Operation>(gContext.frame_arena, rg->operations_count);

  rg->visit_counter++;
  // The actual number of operations is equal or less than rg->operations_count.
  //
  u32 op_arr_idx = 0;
  for (u32 i = 0; i < rg->roots_count; i++) {
    gfx_dsf(rg->roots[i], rg->visit_counter, op_arr, op_arr_idx);
  }

  GFX_Image result = gfx_rg_execute_operations(op_arr, op_arr_idx);
  return result;
}
