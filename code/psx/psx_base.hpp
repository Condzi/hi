#pragma once

// Constants
//
read_only global f32 PSX_STEP_HZ = 60;
read_only global f32 PSX_STEP    = 1 / PSX_STEP_HZ;

read_only global f32 PSX_SCALE     = 100.f;
read_only global f32 PSX_SCALE_INV = 1 / PSX_SCALE;

union PSX_Body_ID {
  u64 v;
  struct {
    u32 idx;        // Index in parent array.
    u32 generation; // Every time this object gets deleted, generation increments.
  };
};

struct PSX_Shape {
  fvec2 *v;
  u64    sz;
};

struct PSX_Fixture {
  PSX_Body_ID parent;
  PSX_Shape   shape;
  f32         mass;
  bool        sensor;
  u8         *user_data;
};

struct PSX_Fixture_Array {
  PSX_Fixture *v;
  u64          sz;
};

struct PSX_Body {
  PSX_Body_ID       id;
  fvec2             pos;
  fvec2             imp;
  fvec2             vel;
  f32               friction;
  f32               rot;
  PSX_Fixture_Array fixtures;
  u8               *user_data;
};

struct PSX_Body_Array {
  PSX_Body *v;
  u64       sz;
};

struct PSX_Body_ID_Array {
  PSX_Body_ID *v;
  u64          sz;
};

struct PSX_Callbacks {
  void *on_collision_start;
  void *on_collision_end;
};

struct PSX_Callbacks_Array {
  PSX_Callbacks *v;
  u64            sz;
};

struct PSX_World {
  Bit_Array           bodies_status_lookup;
  PSX_Body_Array      bodies;
  PSX_Callbacks_Array callbacks;

  // Actions on bodies are deferred to allow removing/adding of objects during collision callbacks
  // in physics steps.
  PSX_Body_Array    new_bodies;
  PSX_Body_ID_Array bodies_to_remove;
};

// Arena allocator, maybe space for some kind of performance counters...?
// Later -- multithreading or some job system setup.
void
psx_init();

must_use global PSX_World *
psx_make_world(u64 num_objects);

global void
psx_simulate(PSX_World *world);

must_use global PSX_Body_ID
psx_world_add(...);

must_use global bool
psx_world_remove(PSX_Body_ID id);

// Object manipulation
//

global void
psx_apply_impulse(...);

global void
psx_rotate(...);

global void
psx_set_position(...);

// What about get/set?
// How does the collision data gets returned? What if we want to spawn new objects on collision?
