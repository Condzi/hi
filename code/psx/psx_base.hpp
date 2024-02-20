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
  u64 v = 0;
  struct {
    u32 idx;        // Index in parent array.
    u32 generation; // Every time this object gets deleted, generation increments.
  };
};

struct PSX_Body {
  PSX_Body_ID id;
  fvec2       pos;
  fvec2       vel;
  fvec2       force;
  fvec2       sz;
  f32         rot         = 0;
  f32         friction    = 0;
  f32         mass_inv    = 0;
  f32         torque      = 0;
  f32         angular_vel = 0;
  f32         i_inv       = 0; // inertia
};

struct PSX_Body_Array {
  PSX_Body *v;
  u64       sz;
};

struct PSX_Body_Opts {
  fvec2 pos;
  fvec2 sz;
  f32   mass     = 0;
  f32   rot      = 0;
  f32   friction = 0;
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
