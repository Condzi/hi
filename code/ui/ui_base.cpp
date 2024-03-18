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
    f32 const w = ui_size_text(widget->string).x;
    sz_px_x     = w;
  }

  if (sz_y.kind == UI_SizeKind_Pixels) {
    sz_px_y = sz_y.value;
  } else if (sz_y.kind == UI_SizeKind_TextContent) {
    f32 const h = ui_size_text(widget->string).y;
    sz_px_y     = h;
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
    UI_Widget *it  = widget->first_child;
    while (it) {
      if (it->flags & UI_WidgetFlag_HorizontalLayout) {
        sum += it->sz_px.x;
      } else if (it->sz_px.x > sum) {
        sum = it->sz_px.x;
      }
      it = it->next;
    }
    sz_px_x = sum;
  }

  if (sz_y.kind == UI_SizeKind_ChildrenSum) {
    f32        sum = 0;
    UI_Widget *it  = widget->first_child;
    while (it) {
      if (it->flags & UI_WidgetFlag_HorizontalLayout) {
        if (it->sz_px.y > sum) {
          sum = it->sz_px.y;
        }
      } else {
        sum += it->sz_px.y;
      }
      it = it->next;
    }
    sz_px_y = sum;
  }
}

internal void
solve_size_violations(UI_Widget *widget) {
  widget->sz_final = widget->sz_px;
  if (!widget->parent) {
    // It's the root node.
    return;
  }
  fvec2 const parent_sz = widget->parent->sz_final;

  // How big children want to be.
  fvec2      wanted = {};
  UI_Widget *it     = widget->first_child;
  while (it) {
    if (it->flags & UI_WidgetFlag_HorizontalLayout) {
      wanted.x += it->sz_px.x;
    } else {
      wanted.y += it->sz_px.y;
    }
    if (it->sz_px.x > wanted.x) {
      wanted.x = it->sz_px.x;
    }
    it = it->next;
  }

  if (wanted.x > parent_sz.x) {
    f32 const violation = wanted.x - parent_sz.x;
    it                  = widget->first_child;
    while (it) {
      f32 const multiplier = 1 - it->semantic_size[Axis2_X].strictness;
      f32 const adjustment = violation * multiplier;
      it->sz_final.x -= adjustment;
      it = it->next;
    }
  }
  if (wanted.y > parent_sz.y) {
    f32 const violation = wanted.y - parent_sz.y;
    it                  = widget->first_child;
    while (it) {
      f32 const multiplier = 1 - it->semantic_size[Axis2_Y].strictness;
      f32 const adjustment = violation * multiplier;
      it->sz_final.y -= adjustment;
      it = it->next;
    }
  }
}

internal void
calculate_relative_positions(UI_Widget *widget) {
  fvec2      pen     = {};
  UI_Widget *it      = widget->first_child;
  fvec2      prev_sz = {};
  while (it) {
    if (it->flags & UI_WidgetFlag_HorizontalLayout) {
      pen.x += prev_sz.x;
      prev_sz.y = Max(it->sz_final.y, prev_sz.y);
    } else {
      pen.x = 0;
      pen.y += prev_sz.y;
      prev_sz = it->sz_final;
    }
    it->pos_rel = pen;
    it          = it->next;
  }
}

internal void
calculate_final_positions(UI_Widget *widget) {
  if (!widget->parent) {
    // Root
    widget->pos_final = {}; // Top - left
    return;
  }

  // @Note: We need to remember that we're using Cartesian coordinate system (X-> Y^) in renderer,
  // but X-> Y\/ in UI.
  fvec2 const parent_pos = widget->parent->pos_final;
  fvec2 const rel_pos    = widget->pos_rel;
  fvec2       final_pos  = {};
  final_pos.x            = parent_pos.x + rel_pos.x;
  final_pos.y            = parent_pos.y - rel_pos.y;
  widget->pos_final      = final_pos;
}

internal void
render(UI_Widget *widget) {
  u32 const   flags = widget->flags;
  fvec2 const pos   = widget->pos_final;
  fvec2 const sz    = widget->sz_final;

  if (flags & UI_WidgetFlag_DrawBackground) {
    gfx_draw_rect_color_ui(
        {
            .pos = pos,
            .sz  = sz,
            .layer =
                {
                    .category = GFX_Layer_Background,
                    .priority = 1,
                },
        },
        {.v = 0x00'00'00'55});
  }

  if (flags & UI_WidgetFlag_DrawText) {
    gfx_draw_rich_text({
        .pos       = pos,
        .height_px = gUI.text_height,
        .font      = gUI.font,
        .string    = widget->string,
    });
  }
}

must_use UI_Key
ui_make_key(Str8 string) {
  u64 result = hash_str8(string);
  Assert(result);
  return {.hash = result};
}

must_use fvec2
ui_size_text(Str8 text) {
  GFX_Rich_Text_Opts const opts = {
      .height_px = gUI.text_height,
      .font      = gUI.font,
      .string    = text,
  };
  fvec2 const sz = gfx_size_rich_text(opts);
  return {sz.x, sz.y * -1};
}

void
ui_init(Arena *arena, u64 widgets_cap) {
  gUI.widgets_cap        = widgets_cap;
  gUI.widgets_hash_table = make_hash_table(arena, widgets_cap);
  gUI.text_height        = 12;
  UI_Widget *widgets     = arena_alloc_array<UI_Widget>(arena, widgets_cap);
  for (u64 i = 0; i < widgets_cap; i++) {
    DLL_insert_at_front(gUI.free_widgets, &(widgets[i]));
  }
}

void
ui_begin() {
  ErrorContext("frame_id=%zu", gUI.frame_id);

  // Reset widget tree.
  //
  Tree_post_order(gUI.widgets, cleanup_pointers);
  gUI.widgets      = 0;
  gUI.widget_stack = 0;
  gUI.frame_id++;
}

must_use UI_Widget *
ui_push_widget(UI_Widget_Opts const &opts) {
  ErrorContext("frame_id=%zu, key=%S", gUI.frame_id, opts.key);
  ErrorIf(!opts.key.sz, "Widget without key.");
  if (opts.flags & UI_WidgetFlag_DrawText) {
    ErrorIf(!opts.string.sz, "Widget wants to draw text, but no text given.");
  }

  UI_Key key = ui_make_key(opts.key);
  UI_Widget *widget = (UI_Widget *)ht_find(gUI.widgets_hash_table, key.hash);
  if (!widget) {
    ErrorIf(!gUI.free_widgets, "No free widgets available. Increase the cache size!");
    widget           = gUI.free_widgets;
    gUI.free_widgets = widget->next;
    if (gUI.free_widgets) {
      gUI.free_widgets->prev = 0;
    }
    ht_insert(gUI.widgets_hash_table, key.hash, widget);
  }

  *widget = {
      .key                    = key,
      .last_frame_touched_idx = gUI.frame_id,
      .flags                  = (UI_Widget_Flag)opts.flags,
      .string                 = opts.string,
      .semantic_size          = {opts.semantic_size[0], opts.semantic_size[1]},
  };

  if (gUI.widget_stack) {
    widget->parent = gUI.widget_stack;
    DLL_insert_at_end(gUI.widget_stack->first_child, widget);
  } else {
    // It's the first widget in this pass, so assume it's the root.
    Assert(!gUI.widgets);
    gUI.widgets = widget;
    ui_push_to_stack(widget);
  }

  return widget;
}

void
ui_push_to_stack(UI_Widget *widget) {
  Assert(widget);
  gUI.widget_stack = widget;
}

void
ui_pop_stack() {
  ErrorIf(!gUI.widget_stack, "Mismatched call to ui_pop_stack");
  gUI.widget_stack = gUI.widget_stack->parent;
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
  Tree_pre_order(gUI.widgets, calculate_relative_positions);
  Tree_pre_order(gUI.widgets, calculate_final_positions);

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
