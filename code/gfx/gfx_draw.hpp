#pragma once

// Constants
//

read_only internal u32 GFX_RENDERER_SPRITE_BATCHES_COUNT = 8;
read_only internal u32 GFX_RENDERER_RECT_BATCHES_COUNT   = 2;
read_only internal u32 GFX_RENDERER_BATCHES_TOTAL =
    GFX_RENDERER_SPRITE_BATCHES_COUNT + GFX_RENDERER_RECT_BATCHES_COUNT;

struct {
  GFX_Render_Graph *rg = 0;

  GFX_Image *batch_render_target = 0;
  GFX_Image *ui_render_target    = 0;
  // Batches for reuse
  //
  GFX_Batch_Node   *free_batches = 0;
  GFX_Batch_Node   *used_batches = 0;

  // Nodes for reuse
  //
  GFX_RG_Node *free_nodes = 0;
  GFX_RG_Node *used_nodes = 0;

  GFX_Object_Array game_objects_in_frame;         // Uses frame arena as storage!
  GFX_Object_Array ui_objects_in_frame;           // Uses frame arena as storage!
  bool             is_accepting_new_objects = false; // true if between begin/end frame calls.

  // Batchers need to be added between these two nodes -- as children of "before" and the last one
  // as parent of "after".
  //
  GFX_RG_Node *node_before_batchers = 0;
  GFX_RG_Node *node_after_batchers  = 0;

  GFX_RG_Node *node_before_ui = 0;
  GFX_RG_Node *node_after_ui  = 0;

  GFX_RG_Node* batch_camera = 0;
} global gRen;

void
gfx_renderer_init();

void
gfx_renderer_begin_frame();

void
gfx_renderer_end_frame();

// Drawing frontend
//

struct GFX_Font;
struct GFX_Text_Opts {
  fvec2     pos;
  u16       height_px;
  GFX_Font *font;
  GFX_Layer layer;
  Str8      string;
};

void
gfx_draw_text(GFX_Text_Opts const &opts);

void
gfx_draw_text_color(GFX_Text_Opts const &opts, GFX_Color color);

struct GFX_Rich_Text_Opts {
  fvec2     pos;
  u16       height_px;
  GFX_Font *font;
  GFX_Layer layer;
  Str8      string;
};

void
gfx_draw_rich_text(GFX_Rich_Text_Opts const &opts);

void
gfx_draw_rich_text_color(GFX_Rich_Text_Opts const &opts, GFX_Color color);

struct GFX_Sprite_Opts {
  fvec2        pos;
  fvec2        sz;
  f32          rot = 0;
  GFX_Image    tex;
  GFX_Tex_Rect tex_rect;
  GFX_Layer    layer;
  GFX_Sampler_Type sampler = {};
};

void
gfx_draw_sprite(GFX_Sprite_Opts const &opts);

void
gfx_draw_sprite_color(GFX_Sprite_Opts const &opts, GFX_Color color);

struct GFX_Rect_Opts {
  fvec2 pos;
  fvec2 sz;
  f32   rot = 0;
  GFX_Layer layer;
};

void
gfx_draw_rect_color(GFX_Rect_Opts const &opts, GFX_Color color);

void
gfx_draw_rect_color_ui(GFX_Rect_Opts const &opts, GFX_Color color);

struct GFX_Camera {
  fvec2 center;
  f32 rot;
  f32 zoom;
};

void
gfx_set_camera_for_batches(GFX_Camera cam);
