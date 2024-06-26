#pragma once

global Arena *gfx_arena;

struct GFX_Camera {
  fvec2 center;
  f32 rot;
  f32 zoom;
};

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

must_use GFX_Image
gfx_make_empty_image();

must_use GFX_Image
gfx_make_image(u8 *data, u32 width, u32 height);

void
gfx_release_image(GFX_Image img);

must_use fvec2
gfx_image_size(GFX_Image img);

struct GFX_Buffer {
  u64 v[1] = {};
};

// Basic interface
//

struct GFX_Opts {
  u32  vp_width;
  u32  vp_height;
  bool vsync;
};

void
gfx_init(GFX_Opts const &opts);

must_use bool
gfx_is_vsync_enabled();

void
gfx_set_vsync(bool v);

void
gfx_resize(u32 new_width, u32 new_height);

void
gfx_swap_buffers(GFX_Image final_image);

// Layering.
// Layers are effective per-batch; there is no layering between batches!
//

enum GFX_Layer_Category {
  GFX_Layer_Background,
  GFX_Layer_Middle,
  GFX_Layer_Foreground,
};

// Higher priority means its closer to the foreground.
//
union GFX_Layer {
  u8 v = {};
  struct {
    u8 category : 2;
    u8 priority : 6;
  };
};

// Materials
//

enum GFX_Sampler_Type : u8 {
  GFX_SamplerType_Default,
  GFX_SamplerType_Linear = GFX_SamplerType_Default,
  GFX_SamplerType_PixelPerfect,

  GFX_SampelrType__count
};

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
      GFX_Image    tex;
      GFX_Tex_Rect tex_rect;
      GFX_Color    color;
    } sprite;
  };
  GFX_Sampler_Type sampler;
};

struct GFX_Object {
  fvec2        pos;
  fvec2        sz;
  f32          rot = 0;
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

u64 read_only global GFX_BATCH_MAX_INSTANCES = 64;

// Batches are specialized per-material type. They have hard limit on how many
// objects they can render, because we have to allocate some known amount of space
// on the gpu for the instances. The limit is set by GFX_BATCH_MAX_INSTANCES.
//
struct GFX_Batch {
  GFX_Buffer       instances;
  GFX_Object_Array objects;

  // What is the material of the objects in this batch?
  GFX_Material_Type type = {};
  GFX_Sampler_Type sampler = {};
  union {
    struct {
      GFX_Image texture;
    } sprite = {};
  } data;
};

struct GFX_Batch_Node {
  GFX_Batch_Node *next;
  GFX_Batch_Node *prev;
  GFX_Batch      *batch;
};

must_use GFX_Batch *
gfx_make_batch(GFX_Material_Type material);

void
gfx_batch_push(GFX_Batch *batch, GFX_Object const &object);

void
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
  GFX_Post_Fx_Type type     = {};
  f32              strength = 0;
};

void
gfx_combine_images(GFX_Image a, GFX_Image b, GFX_Image target);

must_use GFX_Image
gfx_checkerboard_image(u32 square_size, u32 width_squares, u32 height_squares);

void
gfx_apply_post_fx(GFX_Fx fx, GFX_Image src, GFX_Image target);

void
gfx_resize_image_with_framebuffer(GFX_Image *img);
