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

  // Check collisions (ambarassingly slow and bad, but fuck it for now :))
  //
  for (u64 i = 0; i < participants.sz; i++) {
    PSX_Body const  &body_i  = world.bodies.v[participants.v[i]];
    PSX_Shape const &shape_i = world.shapes.v[body_i.shapes.idx];
    for (u64 j = i + 1; j < participants.sz; j++) {
      PSX_Body const  &body_j  = world.bodies.v[participants.v[j]];
      PSX_Shape const &shape_j = world.shapes.v[body_j.shapes.idx];

      // For now just assume we only have 1 shape per object because im lazy.
      // @Todo: support collision for multiple shapes

      if (psx_sat_test(shape_i.polygon, shape_j.polygon)) {
        os_debug_message("Collision!"_s8);
      }
    }
  }
  // Integrate velocities.
  //
  for (u64 i = 0; i < participants.sz; i++) {
    PSX_Body &body = world.bodies.v[participants.v[i]];

    body.pos += body.vel * dt;
    body.force = {};
  }
}

must_use internal bool
psx_sat_test(PSX_Polygon_Shape const &shape_a, PSX_Polygon_Shape const &shape_b) {
  fvec2 const *v_a = shape_a.v;
  fvec2 const *v_b = shape_b.v;
  u64          n   = shape_a.sz;
  u64          m   = shape_b.sz;

  for (u64 pass = 0; pass < 2; pass++) {
    for (u64 i = 0; i < n; i++) {
      fvec2 const current = v_a[i];
      fvec2 const next    = v_a[(i + 1) % n];
      fvec2 const edge    = next - current;
      fvec2 const axis    = {.x = -edge.y, .y = edge.x};

      f32 a_min = MAX_F32;
      f32 a_max = MIN_F32;

      for (u64 j = 0; j < n; j++) {
        f32 const proj = dot(axis, v_a[j]);
        a_min          = Min(a_min, proj);
        a_max          = Max(a_max, proj);
      }

      f32 b_min = MAX_F32;
      f32 b_max = MIN_F32;
      for (u64 j = 0; j < m; j++) {
        f32 const proj = dot(axis, v_b[j]);
        b_min          = Min(b_min, proj);
        b_max          = Max(b_max, proj);
      }

      if (a_max < b_min || a_min > b_max) {
        return false;
      }
    }

    Swap(v_a, v_b);
    Swap(n, m);
  }

  return true;
}
