#pragma once

// Constants
//
u64 read_only global ECS_LIMIT = 2'000;

// Common Types
//

struct ECS_Entity_ID {
  u16 idx;
  u16 is_set   : 1;
  u16 revision : 15;
};

#define ECS_IS_NULL(id) ((id).is_set == 0)
#define ECS_ID_EQUALS(a, b)                                                                        \
  (!ECS_IS_NULL(a) && !ECS_IS_NULL(b) && ((a).idx == (b).idx) && ((a).revision == (b).revision))

struct ECS_Transform_Component {
  bool  relative_to_psx_body;
  fvec2 pos;
  f32   rot = 0;
  fvec2 scale;
};

struct ECS_Sprite_Component {
  GFX_Image    tex;
  GFX_Tex_Rect tex_rect;
  GFX_Layer    layer;
};

struct ECS_Physics_Body_Component {
  PSX_Body_ID body;
};

struct ECS_World {
  Arena* arena;
  // Status
  //
  Bit_Array *alive;
  Bit_Array *to_remove;

  // Components
  //
#define X(type, name) type name[ECS_LIMIT];
#include "ecs_components.inl"
#undef X
} global *gECS;

void
ecs_init();

must_use ECS_Entity_ID
ecs_spawn();

void
ecs_kill(ECS_Entity_ID id);

void
ecs_kill_pass();
