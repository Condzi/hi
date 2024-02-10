#pragma once

// Helper types
//

struct GFX_Viewport {
  fvec2 pos;
  fvec2 sz;
  f32   zoom = {};
};

union GFX_Color {
  u32 v = {};
  struct {
    u8 a;
    u8 b;
    u8 g;
    u8 r;
  };
};

struct GFX_Tex_Rect {
  u16 x = {};
  u16 y = {};
  u16 w = {};
  u16 h = {};
};

// Handles to resources
//

struct GFX_Image {
  u64 v[1] = {};
};

must_use global GFX_Image
gfx_make_empty_image();

must_use global GFX_Image
gfx_make_image(u8 *data, u32 width, u32 height);

global void
gfx_release_image(GFX_Image img);

struct GFX_Buffer {
  u64 v[1] = {};
};

// Layering.
// Layers are effective per-batch; there is no layering between batches!
//

enum GFX_Layer_Category {
  GFX_Layer_Background,
  GFX_Layer_Middle,
  GFX_Layer_Foreground,
};

union GFX_Layer {
  u8 v = {};
  struct {
    u8 category : 2;
    u8 priority : 6;
  };
};

// Materials
//

enum GFX_Material_Type : u8 {
  GFX_MaterialType_Default,
  GFX_MaterialType_Rect = GFX_MaterialType_Default,
  GFX_MaterialType_Sprite,
  // AnimatedSprite, TiledTexture, Particles, Text?

  GFX_MaterialType__count,
};

struct GFX_Material {
  GFX_Material_Type type = {};
  union {
    struct {
      GFX_Color color;
    } rect = {};
    struct {
      GFX_Tex_Rect tex_rect;
      GFX_Color    color;
    } sprite;
  };
};

struct GFX_Object {
  fvec2        pos;
  fvec2        sz;
  f32          rot;
  GFX_Layer    layer;
  GFX_Material material;
};

struct GFX_Object_Array {
  GFX_Object *v   = {};
  u64         sz  = {};
  u64         cap = {};
};

// Batching
//

struct GFX_Batch {
  GFX_Buffer       instances;
  GFX_Viewport     viewport; // At what exacly are we looking?
  GFX_Object_Array objects;

  // What is the material of the objects in this batch?
  GFX_Material_Type type = {};
  union {
    struct {
      GFX_Image texture;
    } sprite = {};
  } data;
};

must_use global GFX_Batch *
gfx_make_batch(GFX_Material_Type material);

global void
gfx_release_batch(GFX_Batch *batch);

global void
gfx_batch_push(GFX_Batch *batch, GFX_Object object);

global void
gfx_batch_draw(GFX_Batch *batch, GFX_Image target);

// Post processing types
//

enum GFX_Post_Fx_Type {
  GFX_PostFxType_None,

  GFX_PostFXType_Blur,
  GFX_PostFXType_Vignette,

  GFX_PostFxType__count,
};

struct GFX_Fx {
  GFX_Post_Fx_Type type = {};
  f32              strength = 0;
};

global void
gfx_combine_images(GFX_Image a, GFX_Image b, GFX_Image target);

must_use global GFX_Image
gfx_checkerboard_image(u32 square_size, u32 width_squares, u32 height_squares);

global void
gfx_apply_post_fx(GFX_Fx fx, GFX_Image src, GFX_Image target);

global void
gfx_resize_image_with_framebuffer(GFX_Image* img);
