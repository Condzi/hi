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

/*
  I guess the idea is that we can have some pool of preallocated Widget nodes which we
  can reuse, similarly to nodes in render graph.
*/

#pragma once

enum Axis2 {
  Axis2_X,
  Axis2_Y,
  Axis2__count,
};

enum UI_Size_Kind {
  UI_SizeKind_None,
  UI_SizeKind_Pixels,
  UI_SizeKind_TextContent,
  UI_SizeKind_PercentOfParent,
  UI_SizeKind_ChildrenSum,
};

struct UI_Size {
  UI_Size_Kind kind       = {};
  f32          value      = {};
  f32          strictness = {}; // 1 - no compromises, 0 - compromise entire size
};

struct UI_Key {
  u64 hash = 0;
};

enum UI_Widget_Flag : u32 {
  UI_WidgetFlag_Clickable      = bit1,
  UI_WidgetFlag_ViewScroll     = bit2,
  UI_WidgetFlag_DrawText       = bit3,
  UI_WidgetFlag_DrawBorder     = bit4,
  UI_WidgetFlag_DrawBackground = bit5,
};

struct UI_Widget {
  // Tree links
  //
  UI_Widget *next   = 0;
  UI_Widget *prev   = 0;
  UI_Widget *parent = 0;

  // Key and generation info
  //
  UI_Key key;
  u64    last_frame_touched_idx = 0;

  // Per-frame info provided by Builders
  //
  UI_Widget_Flag flags = {};
  Str8           string;
  UI_Size        semantic_size[Axis2__count];

  // Computed every frame
  //
  fvec2 pos_rel;
  fvec2 sz_px;

  // Actual position and size on the screen
  //
  fvec2 pos_final;
  fvec2 sz_final;
};

void
ui_init();

// @Todo: We only care about mouse clicks at the moment, so we just filter them out.
//        Remember that we can have multiple mouse click events.
//
void
ui_begin(OS_Window_Event *events);

struct UI_Widget_Opts {
  UI_Key         key;
  UI_Widget_Flag flags = {};
  Str8           string;
  UI_Size        semantic_size[Axis2__count];
};

must_use UI_Widget *
ui_push_widget(UI_Widget_Opts const &opts);

// Render to render graph.
void
ui_end();
