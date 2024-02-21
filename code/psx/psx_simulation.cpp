#pragma once
#include "all_inc.hpp"

must_use internal PSX_Participants_Array
psx_get_simulation_participants(PSX_World &w) {
  PSX_Participants_Array res = {.v = arena_alloc_array<u32>(gContext.frame_arena, w.bodies.sz)};

  for (u64 i = 0; i < w.bodies_status_lookup->sz; i++) {
    if (ba_test(w.bodies_status_lookup, i)) {
      res.v[res.sz] = (u32)i;
      res.sz++;
    }
  }

  return res;
}

global void
psx_world_simulate(PSX_World_ID w, f32 dt) {
  PSX_World &world = psx_world_from_id(w);
  // Remove entities first.
  //
  while (ba_is_any_set(world.bodies_to_remove)) {
    u64 idx = ba_find_first_set(world.bodies_to_remove);
    ba_unset(world.bodies_status_lookup, idx);
    ba_unset(world.bodies_to_remove, idx);
    PSX_Shape_ID shape = world.bodies.v[idx].shapes;
    while (shape.is_set) {
      ba_unset(world.shapes_status_lookup, shape.idx);
      shape = world.shapes.v[shape.idx].next;
    }
  }

  PSX_Participants_Array participants = psx_get_simulation_participants(world);

  // Integrate forces.
  //
  for (u64 i = 0; i < participants.sz; i++) {
    PSX_Body &body = world.bodies.v[participants.v[i]];

    body.vel += body.force * body.mass_inv * dt;

    f32 damping_factor = 1.0f - body.linear_damping * dt;
    damping_factor     = Max(0, damping_factor);
    body.vel *= damping_factor;
  }

  //
  // <collision code will go here>
  //

  // Integrate velocities.
  //
  for (u64 i = 0; i < participants.sz; i++) {
    PSX_Body &body = world.bodies.v[participants.v[i]];

    body.pos += body.vel * dt;
    body.force = {};
  }
}
