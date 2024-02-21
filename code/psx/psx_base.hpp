#pragma once

// Constants
//
read_only global f32 PSX_STEP_HZ = 60;
read_only global f32 PSX_STEP    = 1 / PSX_STEP_HZ;

read_only global f32 PSX_SCALE     = 100.f;
read_only global f32 PSX_SCALE_INV = 1 / PSX_SCALE;

// Common types
//

struct PSX_World_ID {
  u16 idx;
  u16 is_set   : 1;
  u16 revision : 15;
};

struct PSX_Body_ID {
  u16 idx;
  u16 world;
  u16 is_set   : 1;
  u16 revision : 15;
};

struct PSX_Shape_ID {
  u16 idx;
  u16 world;
  u16 is_set   : 1;
  u16 revision : 15;
};

#define PSX_IS_NULL(id)  ((id).is_set == 0)
#define PSX_NON_NULL(id) ((id).is_set != 0)
#define PSX_ID_EQUALS(a, b)                                                                        \
  (((a).idx == (b).idx) && ((a).world == (b).world) && ((a).revision == (b).revision))

#define PSX_ID_NOT_EQUALS(a, b)                                                                        \
  (((a).idx != (b).idx) || ((a).world != (b).world) || ((a).revision != (b).revision))

enum PSX_Shape_Type : u8 {
  PSX_ShapeType_Polygon,
};

struct PSX_Polygon_Shape {
  fvec2 *v;
  u64    sz;
};

struct PSX_Shape {
  PSX_Shape_ID   id   = {};
  PSX_Shape_ID   next = {};
  PSX_Shape_Type type = PSX_ShapeType_Polygon;
  union {
    PSX_Polygon_Shape polygon = {};
  };
};

struct PSX_Body {
  PSX_Body_ID  id = {};
  fvec2        pos;
  fvec2        vel;
  fvec2        force;
  f32          rot            = 0;
  f32          mass_inv       = 0;
  f32          linear_damping = 0;
  PSX_Shape_ID shapes         = {};
};

struct PSX_Body_Array {
  PSX_Body *v;
  u64       sz;
};

struct PSX_Body_Opts {
  fvec2 pos;
  f32   mass           = 0;
  f32   rot            = 0;
  f32   linear_damping = 0;
};

struct PSX_World {
  PSX_World_ID   id;
  Bit_Array     *bodies_status_lookup;
  PSX_Body_Array bodies;

  // Body removal is deferred to allow removing/adding of objects during collision callbacks
  // in physics steps.
  //
  Bit_Array *bodies_to_remove;
  PSX_World *next;
};

Arena *psx_arena;

// Functions
//

// Arena allocator, maybe space for some kind of performance counters...?
// Later -- multithreading or some job system setup.
//
global void
psx_init();

must_use global PSX_World_ID
psx_make_world(u64 num_objects);

global void
psx_world_simulate(PSX_World_ID w, f32 dt);

must_use global PSX_Body_ID
psx_world_add(PSX_World_ID world, PSX_Body_Opts const &opts);

global void
psx_world_remove(PSX_World_ID world, PSX_Body_ID id);

global void
psx_body_add_force(PSX_World_ID world, PSX_Body_ID id, fvec2 force);

must_use global fvec2 
psx_body_get_position(PSX_World_ID world, PSX_Body_ID body);
