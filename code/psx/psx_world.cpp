#pragma once
#include "all_inc.hpp"

global void
psx_init() {
  psx_arena = make_arena(true);
}

must_use global PSX_World *
psx_make_world(u64 num_objects) {
  ErrorContext("num_objects=%zu", num_objects);
  ErrorIf(num_objects < 8, "Why so little objects?");

  PSX_World *w            = arena_alloc<PSX_World>(psx_arena);
  w->bodies_status_lookup = make_bit_array(psx_arena, num_objects);
  w->bodies_to_remove     = make_bit_array(psx_arena, num_objects);
  w->bodies.v             = arena_alloc_array<PSX_Body>(psx_arena, num_objects);
  w->bodies.sz            = num_objects;
  return w;
}

must_use global PSX_Body_ID
psx_world_add(PSX_World *w, PSX_Body_Opts const &opts) {
  ErrorContext("pos=(%g,%g), sz=(%g,%g), mass=%g, rot=%g, friction=%g",
               opts.pos.x,
               opts.pos.y,
               opts.sz.x,
               opts.sz.y,
               opts.mass,
               opts.rot,
               opts.friction);
  ErrorIf(opts.mass < 0, "Mass cannot be negative!");
  ErrorIf(opts.sz.width <= EPS_F32 || opts.sz.height <= EPS_F32, "Size must be positive!");
  ErrorIf(ba_is_any_unset(w->bodies_status_lookup), "No space for new objects.");

  u64 unset_idx = ba_find_first_unset(w->bodies_status_lookup);
  ba_set(w->bodies_status_lookup, unset_idx);

  PSX_Body         &body = w->bodies.v[unset_idx];
  PSX_Body_ID const id   = body.id;

  body = {
      .id       = id,
      .pos      = opts.pos,
      .sz       = opts.sz,
      .rot      = opts.rot,
      .friction = opts.friction,
  };

  // Set the mass only if not zero.
  //
  if (!f32_is_near(opts.mass, 0)) {
    body.mass_inv = 1 / opts.mass;
  }

  body.i_inv = 1.0f / (opts.mass * (opts.sz.x * opts.sz.x + opts.sz.y * opts.sz.y) / 12.f);

  return id;
}

global void
psx_world_remove(PSX_World *w, PSX_Body_ID id) {
  ErrorContext("idx=%u, generation=%u", id.idx, id.generation);
  ErrorIf(id.idx >= w->bodies.sz, "Index out of bounds!");

  PSX_Body &body = w->bodies.v[id.idx];
  ErrorIf(body.id.generation != id.generation,
          "Generation does not match! (current is %u)",
          body.id.generation);

  bool const is_or_will_be_dead =
      !ba_test(w->bodies_status_lookup, id.idx) || ba_test(w->bodies_to_remove, id.idx);
  if (is_or_will_be_dead) {
    return;
  }

  ba_set(w->bodies_to_remove, id.idx);

  // Should this happen here or after the actual removal of the body?
  //
  body.id.generation++;
}

global void
psx_body_add_force(PSX_World *w, PSX_Body_ID id, fvec2 force) {
  ErrorContext("idx=%u, generation=%u", id.idx, id.generation);
  ErrorIf(id.idx >= w->bodies.sz, "Index out of bounds!");

  PSX_Body &body = w->bodies.v[id.idx];
  ErrorIf(body.id.generation != id.generation,
          "Generation does not match! (current is %u)",
          body.id.generation);

  ErrorIf(!ba_test(w->bodies_status_lookup, id.idx), "Object is dead");

  body.force += force;
}
