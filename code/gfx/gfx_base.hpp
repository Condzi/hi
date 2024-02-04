#pragma once

// Handles to resources
//

struct GFX_Texture {
  u64 v[1] = {};
};

struct GFX_Surface {
  u64 v[1] = {};
};

struct GFX_Buffer {
  u64 v[1] = {};
};

struct GFX_Shader {
  u64 v[1] = {};
};

// Misc types
//

enum GFX_Layer_Type {
  GFX_Layer_Background,
  GFX_Layer_Middle,
  GFX_Layer_Foreground,
};

union GFX_Layer {
  u8 v;
  struct {
    u8 type : 2     = 0;
    u8 sublayer : 6 = 0;
  };
};

struct GFX_Viewport {
  fvec2 pos;
  fvec2 sz;
  f32   zoom = {};
};

union GFX_Color {
  u32 v = 0;
  struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
  };
};

struct GFX_Tex_Coords {
  u16 x      = 0;
  u16 y      = 0;
  u16 width  = 0;
  u16 height = 0;
};

// Materials
//

enum GFX_Material_Type : u8 {
  GFX_MaterialType_Default = 0,
  GFX_MaterialType_Sprite  = GFX_MaterialType_Default,
  // BasicShape, AnimatedSprite, TiledTexture, Particles, Text?

  GFX_MaterialType__count,
};

struct GFX_Material {
  GFX_Material_Type type = {};
  union {
    struct {
      GFX_Tex_Coords tex_coords;
      GFX_Color      color;
    } sprite;
  };
};

struct GFX_Object {
  fvec2 pos;
  fvec2 sz;

  GFX_Material material;
};

// Batching
//

enum GFX_Batch_Type : u8 {
  GFX_BatchType_Sprite,

  GFX_BatchType__count
};

global read_only u64 GFX_BATCH_SIZE = 64;
struct GFX_Batch {
  GFX_Batch *next = 0;

  GFX_Buffer   instances;
  GFX_Viewport viewport;

  GFX_Batch_Type type = {};
  union {
    struct {
      GFX_Texture texture;
    } sprite;
  };

  // @ToDo: Sort by layer when inserting!!
  GFX_Object objects[GFX_BATCH_SIZE];
  u64        objects_count = 0;
};

struct GFX_Batch_List {
  GFX_Batch *first;
};

// Post processing types
//

enum GFX_Post_Fx_Type {
  GFX_PostFxType_None = 0,

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
  GFX_Fx *next = 0;

  GFX_Shader  vertex_shader;
  GFX_Shader  pixel_shader;
  GFX_Surface surface;

  GFX_Post_Fx_Type type = {};
  union {
    GFX_Post_Fx_Vignette_Opts vignette = {};
    GFX_Post_Fx_CRT_Opts      crt;
  };
};

struct GFX_Fx_List {
  GFX_Fx *first;
};

void
render(GFX_Batch_List *batchers, GFX_Surface surface);
void
render_post(GFX_Texture source, GFX_Fx_List effects, GFX_Surface target);
