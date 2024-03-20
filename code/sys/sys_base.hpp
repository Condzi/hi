#pragma once

enum Sys_Entity_Type : u8 {
  EntityType_None,

  EntityType_Player,
  EntityType_Gun,

  EntityType_Zombie,
  EntityType_Trap,

  EntityType_LevelStatic,
  EntityType_LevelDynamic,

  EntityType__count,
};

union Entity_Handle {
  u32 v;
  struct {
    u16 idx;
    u16 is_set   : 1;
    u16 revision : 15;
  };
};

#define SYS_HANDLE_IS_NULL(handle) ((handle).is_set == 0)
#define SYS_HANDLE_EQ(a, b)        ((a).v == (b).v)

struct Sys_Sprite {
  Sys_Sprite     *next;
  GFX_Sprite_Opts v;
};

struct Sys_Psx {
  Sys_Psx    *next;
  PSX_Body_ID id;
};

// Base object for all entities in game.
// Entities own lists of components. Common ones, like sprites and physics.
//
struct Entity {
  u16 revision;
  u8  type;
};

u64 read_only global ENT_COUNT = 512;

struct Entity_System {
  Bit_Array *alive;
  Bit_Array *dying;

  Entity     v[ENT_COUNT];
  Str8       names[ENT_COUNT];
  Sys_Sprite sprites[ENT_COUNT];
  Sys_Psx    psx[ENT_COUNT];
};

struct Sys_Zombie {
  u64 dummy;
};

u64 read_only global ZOMBIE_COUNT = 512;

struct Zombie_System {
  Entity_Handle h[ZOMBIE_COUNT];
  Sys_Zombie    v[ZOMBIE_COUNT];
};

struct Sys_Trap {
  u64 dummy;
};

u64 read_only global TRAP_COUNT = 512;

struct Trap_System {
  Entity_Handle h[TRAP_COUNT];
  Sys_Trap      v[TRAP_COUNT];
};

// ... and similar structures for other entity types, such as level geometry and guns (although with
// smaller COUNTs).
