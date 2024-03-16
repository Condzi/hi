#pragma once
#include "all_inc.hpp"

internal void
cleanup_pointers(UI_Widget *widget) {
  widget->next   = 0;
  widget->prev   = 0;
  widget->parent = 0;
}

internal void
calculate_standalone_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_Pixels) {
    sz_px_x = sz_x.value;
  } else if (sz_x.kind == UI_SizeKind_TextContent) {
    NotImplemented;
  }

  if (sz_y.kind == UI_SizeKind_Pixels) {
    sz_px_y = sz_y.value;
  } else if (sz_y.kind == UI_SizeKind_TextContent) {
    NotImplemented;
  }
}

internal void
calculate_upward_depend_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_PercentOfParent) {
    f32 const parent_w = widget->parent->sz_px.x;
    sz_px_x            = sz_x.value * parent_w;
  }

  if (sz_y.kind == UI_SizeKind_PercentOfParent) {
    f32 const parent_h = widget->parent->sz_px.y;
    sz_px_y            = sz_y.value * parent_h;
  }
}

internal void
calculate_downward_depend_size(UI_Widget *widget) {
  UI_Size const &sz_x    = widget->semantic_size[Axis2_X];
  UI_Size const &sz_y    = widget->semantic_size[Axis2_Y];
  f32           &sz_px_x = widget->sz_px.x;
  f32           &sz_px_y = widget->sz_px.y;

  if (sz_x.kind == UI_SizeKind_ChildrenSum) {
    f32        sum = 0;
    UI_Widget *it  = widget->next;
    while (it) {
      sum += it->sz_px.x;
      it = it->next;
    }
    sz_px_x = sum;
  }

  if (sz_y.kind == UI_SizeKind_ChildrenSum) {
    f32        sum = 0;
    UI_Widget *it  = widget->next;
    while (it) {
      sum += it->sz_px.y;
      it = it->next;
    }
    sz_px_y = sum;
  }
}

internal void
solve_size_violations(UI_Widget *widget) {
  Unused(widget);
  NotImplemented;
}

internal void
calculate_positions(UI_Widget *widget) {
  Unused(widget);
  NotImplemented;
}

internal void
render(UI_Widget *widget) {
  Unused(widget);
  NotImplemented;
}

must_use UI_Key
ui_make_key(Str8 string) {
  u64 result = hash_str8(string);
  return {.hash = result};
}

void
ui_init(Arena *arena, u64 widgets_cap) {
  gUI.widgets_cap        = widgets_cap;
  gUI.widgets_hash_table = make_hash_table(arena, widgets_cap);
  UI_Widget *widgets     = arena_alloc_array<UI_Widget>(arena, widgets_cap);
  for (u64 i = 0; i < widgets_cap; i++) {
    DLL_insert_at_front(gUI.free_widgets, &(widgets[i]));
  }
}

void
ui_begin() {
  // Reset widget tree.
  //
  Tree_post_order(gUI.widgets, cleanup_pointers);
  gUI.widgets = 0;
  gUI.frame_id++;
}

void
ui_end() {
  /**
  1. (Any order)  Calculate standalone sizes.
  2. (Pre-Order)  Calculate upward-depend sizes (Percent of parent)
  3. (Post-Order) Calculate downward-dependent sizes (ChildrenSum),
  4. (Pre-Order)  Solve size violations (using semantic size info). Ensure children do not extend
                  past the boundaries of a given parent (unless it is scrollable) to the best of
                  algorithm ability. In case of violation, take a proportion of *each* child
                  widget's size (on the given axis) proportional to both the size of the violation,
                  and `(1 - strictness)`.
  5. (Pre-Order)  Calculate relative size of each widget and final on-screen coordinates.
  */
  Tree_pre_order(gUI.widgets, calculate_standalone_size);
  Tree_pre_order(gUI.widgets, calculate_upward_depend_size);
  Tree_post_order(gUI.widgets, calculate_downward_depend_size);
  Tree_pre_order(gUI.widgets, solve_size_violations);
  Tree_pre_order(gUI.widgets, calculate_positions);

  // Render, back-to-front.
  //
  Tree_pre_order(gUI.widgets, render);

  // Free widgets which have not been touched by current frame from the cache.
  //
  Scratch_Buffer scratch = scratch_begin(gContext.frame_arena);
  Defer { scratch_end(&scratch); };

  u64 const   frame_id = gUI.frame_id;
  UI_Widget **widgets_to_free =
      arena_alloc_array<UI_Widget *>(gContext.frame_arena, gUI.widgets_cap);
  u64 widgets_to_free_sz = 0;

  // I'm not sure how efficient this is. Iterating over every bucket, then over a list...
  //
  for (u64 i = 0; i < gUI.widgets_hash_table->buckets_sz; i++) {
    HT_Entry *entry = gUI.widgets_hash_table->buckets[i].root;
    while (entry) {
      if (entry->value) {
        UI_Widget *val = (UI_Widget *)entry->value;
        if (val->last_frame_touched_idx != frame_id) {
          Assert(widgets_to_free_sz < gUI.widgets_cap);
          widgets_to_free[widgets_to_free_sz] = val;
          widgets_to_free_sz++;
        }
      }
      entry = entry->next;
    }
  }

  for (u64 i = 0; i < widgets_to_free_sz; i++) {
    ht_erase(gUI.widgets_hash_table, widgets_to_free[i]->key.hash);
  }
  for (u64 i = 0; i < widgets_to_free_sz; i++) {
    MemorySet(widgets_to_free[i], 0, sizeof(*(widgets_to_free[i])));
    DLL_insert_at_front(gUI.free_widgets, widgets_to_free[i]);
  }
}
