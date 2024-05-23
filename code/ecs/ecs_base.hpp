#pragma once

// Constants
//
u64 read_only global ECS_LIMIT = 2'000;

enum En_Type {
  En_Type_Player = Bit(0),
  En_Type_Zombie = Bit(1),
};

enum Sys_Type {
  Sys_Type_Physics,
  Sys_Type_Rendering,

  Sys_Type_PlayerControl,
  Sys_Type_Tools,

  Sys_Type_Camera,

  Sys_Type__count,
};

// Common Types
//

union ECS_Entity_ID {
  struct {
    u16 idx;
    u16 is_set   : 1;
    u16 revision : 15;
  };
  u64 v;
};

#define ECS_IS_NULL(id) ((id).is_set == 0)
#define ECS_ID_EQUALS(a, b)                                                                        \
  (!ECS_IS_NULL(a) && !ECS_IS_NULL(b) && ((a).idx == (b).idx) && ((a).revision == (b).revision))

using Sys_init_proc     = void (*)();
using Sys_shutdown_proc = void (*)();
using Sys_update_proc   = void (*)(f32);

struct Sys_Logic {
  Sys_Logic *next;
  Sys_Logic *prev;

  Sys_Type type;

  Sys_init_proc     init;
  Sys_shutdown_proc shutdown;
  Sys_update_proc   update;
};

struct ECS_World {
  Arena* arena;
  // Status
  //
  Bit_Array *alive;
  Bit_Array *to_remove;
  Bit_Array *systems[Sys_Type__count];

  // Components
  //
#define X(type, name) type name[ECS_LIMIT];
#include "ecs_components_x.inl"
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

void
ecs_opt_in(Sys_Type system, ECS_Entity_ID id);
