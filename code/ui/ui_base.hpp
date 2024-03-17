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
 // UI_WidgetFlag_Clickable        = bit1,
  //UI_WidgetFlag_ViewScroll       = bit2,
  UI_WidgetFlag_DrawText         = bit3,
  //UI_WidgetFlag_DrawBorder       = bit4,
  UI_WidgetFlag_DrawBackground   = bit5,
  UI_WidgetFlag_HorizontalLayout = bit6,
};

struct UI_Widget {
  // Tree links. Reassigned every frame because cache is vollatile etc.
  //
  UI_Widget *parent      = 0;
  UI_Widget *first_child = 0; // n-ary tree children

  // Links sibilings together.
  UI_Widget *next = 0;
  UI_Widget *prev = 0;

  // Key and generation info. Key is relevant to the cache.
  //
  UI_Key key;
  u64    last_frame_touched_idx = 0;

  // Per-frame info provided by Builders
  //
  UI_Widget_Flag flags  = {};
  Str8           string = {};
  UI_Size        semantic_size[Axis2__count];

  // Computed every frame. This is how big the widget wants to be, before sizing.
  //
  fvec2 pos_rel;
  fvec2 sz_px;

  // Actual position and size on the screen
  //
  fvec2 pos_final;
  fvec2 sz_final;
};

struct UI_Context {
  // Rebuilt every frame
  UI_Widget *widgets;
  // Lookup for widgets that lived in last frame
  Hash_Table *widgets_hash_table;
  // doubly-linked list of free widgets to use to avoid allocation
  UI_Widget *free_widgets;
  u64        widgets_cap;

  // Pop just replaces it with its parent.
  UI_Widget *widget_stack;

  u64 frame_id;

  // Style
  //
  GFX_Font *font;
  u16       text_height;
} internal gUI;

// Just hashes the string.
must_use UI_Key
ui_make_key(Str8 string);

must_use fvec2
ui_size_text(Str8 text);

void
ui_init(Arena *arena, u64 widgets_cap);

void
ui_begin();

struct UI_Widget_Opts {
  UI_Key         key;
  UI_Widget_Flag flags  = {};
  Str8           string = {};
  UI_Size        semantic_size[Axis2__count];
};

must_use UI_Widget *
ui_push_widget(UI_Widget_Opts const &opts);

void
ui_push_to_stack(UI_Widget *widget);
void
ui_pop_stack();

// Size the widgets. Render to render graph. Refresh hash table.
void
ui_end();
