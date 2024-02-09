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
    u8 r;
    u8 g;
    u8 b;
    u8 a;
  };
};

struct GFX_Tex_Coords {
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

must_use GFX_Image
gfx_make_empty_image();

must_use GFX_Image
gfx_make_image(u8* data, u64 sz);

void
gfX_release_image(GFX_Image img);

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
      GFX_Tex_Coords tex_coords;
      GFX_Color      color;
    } sprite;
  } data;
};

struct GFX_Object {
  fvec2        pos;
  fvec2        sz;
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

must_use GFX_Batch*
gfx_make_batch(GFX_Material_Type material);

void
gfx_release_batch(GFX_Batch* batch);

void
gfx_batch_push(GFX_Batch *batch, GFX_Object object);

void
gfx_batch_draw(GFX_Batch* batch, GFX_Image target);

// Post processing types
//

enum GFX_Post_Fx_Type {
  GFX_PostFxType_None,

  GFX_PostFxType_Vignette,
  GFX_PostFxType_CRT,

  GFX_PostFxType__count,
};

struct GFX_Post_Fx_Vignette_Opts {
  u32 strength;
};

struct GFX_Post_Fx_CRT_Opts {
  u32 strength;
};

struct GFX_Fx {
  GFX_Post_Fx_Type type = {};
  union {
    GFX_Post_Fx_Vignette_Opts vignette = {};
    GFX_Post_Fx_CRT_Opts      crt;
  } data;
};

// Render Graph
//

enum GFX_RG_Node_Type {
  GFX_RG_NodeType_None,

  // Batch of objects to render.
  //
  GFX_RG_NodeType_Batch,

  // PostFX process to aply to some image.
  //
  GFX_RG_NodeType_PostFx,

  // Combine two images together. Example: render two batches to separate images,
  // then combine batch A with batch B images. B will be on top of A.
  //
  GFX_RG_NodeType_CombineImages,
};

struct GFX_RG_Node {
  GFX_Image out_img;

  GFX_RG_Node_Type type = GFX_RG_NodeType_None;
  union {
    struct {
      GFX_Batch batch;
    } batch = {};

    struct {
      GFX_Image img;
      GFX_Fx    fx;
    } post_fx;

    struct {
      GFX_Image a;
      GFX_Image b;
    } combine_images;
  } input;
};
