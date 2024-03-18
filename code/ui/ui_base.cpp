#pragma once
#include "all_inc.hpp"

internal void
cleanup_pointers(UI_Widget *widget) {
  widget->parent      = 0;
  widget->first_child = 0;
  widget->next        = 0;
  widget->prev        = 0;
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
        .layer =
            {
                .category = GFX_Layer_Background,
                .priority = 1,
            },
        .string = widget->string,
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

  gUI.anim_speed = 16.f; // HZ
}

void
ui_begin(f32 dt) {
  ErrorContext("frame_id=%zu", gUI.frame_id);

  // Reset widget tree.
  //
  Tree_post_order(gUI.widgets, cleanup_pointers);
  gUI.widgets      = 0;
  gUI.widget_stack = 0;
  gUI.frame_id++;
  gUI.frame_dt = dt;
}

must_use UI_Widget *
ui_push_widget(UI_Widget_Opts const &opts) {
  ErrorContext("frame_id=%zu, key=%S", gUI.frame_id, opts.key);
  ErrorIf(!opts.key.sz, "Widget without key.");
  if (opts.flags & UI_WidgetFlag_DrawText) {
    ErrorIf(!opts.string.sz, "Widget wants to draw text, but no text given.");
  }

  UI_Key     key    = ui_make_key(opts.key);
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
      .hot_anim               = ClampTop(widget->hot_anim + gUI.frame_dt * gUI.anim_speed, 1.0f),
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

must_use UI_Widget *
ui_text(Str8 key, Str8 text, u32 flags) {
  UI_Widget_Opts const opts = {.key           = key,
                               .flags         = flags | UI_WidgetFlag_DrawText,
                               .string        = text,
                               .semantic_size = {
                                   {.kind = UI_SizeKind_TextContent, .strictness = 0},
                                   {.kind = UI_SizeKind_TextContent, .strictness = 1},
                               }};

  UI_Widget *result = ui_push_widget(opts);
  return result;
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
