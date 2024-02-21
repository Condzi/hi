#pragma once

// Constants
//
read_only global f32 PSX_STEP_HZ = 60;
read_only global f32 PSX_STEP    = 1 / PSX_STEP_HZ;

read_only global f32 PSX_SCALE     = 100.f;
read_only global f32 PSX_SCALE_INV = 1 / PSX_SCALE;

// Common types
//

union PSX_Body_ID {
  u32 v = 0;
  struct {
    u16 idx;        // Index in parent array.
    u16 generation; // Every time this object gets deleted, generation increments.
  };
};

union PSX_Shape_ID {
  u32 v = 0;
  struct {
    u16 idx;
    // When used in list context, this field disambiguates if idx=0 means index
    // 0 or NULL.
    u16 is_pointing : 1;
    u16 generation  : 15;
  };
};

enum PSX_Shape_Type : u8 {
  PSX_ShapeType_Polygon,
};

struct PSX_Polygon_Shape {
  fvec2 *v;
  u64    sz;
};

struct PSX_Shape {
  PSX_Shape_ID   id;
  PSX_Shape_ID   next;
  PSX_Shape_Type type = PSX_ShapeType_Polygon;
  union {
    PSX_Polygon_Shape polygon = {};
  };
};

struct PSX_Body {
  PSX_Body_ID  id;
  fvec2        pos;
  fvec2        vel;
  fvec2        force;
  f32          rot            = 0;
  f32          mass_inv       = 0;
  f32          linear_damping = 0;
  PSX_Shape_ID shapes;
};

struct PSX_Body_Array {
  PSX_Body *v;
  u64       sz;
};

struct PSX_Body_Opts {
  fvec2 pos;
  fvec2 sz;
  f32   mass           = 0;
  f32   rot            = 0;
  f32   linear_damping = 0;
};

struct PSX_Body_Opts_Array {
  PSX_Body_Opts *v;
  u64            sz;
  u64            cap;
};

struct PSX_World {
  Bit_Array     *bodies_status_lookup;
  PSX_Body_Array bodies;

  // Body removal is deferred to allow removing/adding of objects during collision callbacks
  // in physics steps.
  //
  Bit_Array *bodies_to_remove;
};

Arena *psx_arena;

// Functions
//

// Arena allocator, maybe space for some kind of performance counters...?
// Later -- multithreading or some job system setup.
//
global void
psx_init();

must_use global PSX_World *
psx_make_world(u64 num_objects);

global void
psx_world_simulate(PSX_World *world, f32 dt);

must_use global PSX_Body_ID
psx_world_add(PSX_World *w, PSX_Body_Opts const &opts);

global void
psx_world_remove(PSX_World *w, PSX_Body_ID id);

global void
psx_body_add_force(PSX_World *w, PSX_Body_ID id, fvec2 force);
