#pragma once
#include "all_inc.hpp"

// @ToDo: support for more physics worlds.
PSX_World *psx_world;

must_use internal PSX_World &
psx_world_from_id(PSX_World_ID id) {
  ErrorIf(PSX_IS_NULL(id), "World ID is not set!");
  ErrorIf((id.idx != psx_world->id.idx || id.revision != psx_world->id.revision),
          "IDs do not match!");
  return *psx_world;
}

must_use internal PSX_Body &
psx_body_from_id(PSX_World &w, PSX_Body_ID id) {
  ErrorIf(PSX_IS_NULL(id), "Body ID is not set!");
  ErrorIf(id.idx >= w.bodies.sz, "Body index out of bounds");

  PSX_Body &body = w.bodies.v[id.idx];
  ErrorIf(PSX_ID_NOT_EQUALS(body.id, id), "Body ID does not match!");
  return body;
}

global void
psx_init() {
  psx_arena = make_arena(true);
}

must_use global PSX_World_ID
psx_make_world(u64 num_objects) {
  ErrorContext("num_objects=%zu", num_objects);
  ErrorIf(num_objects < 8, "Why so little objects?");
  ErrorIf(psx_world, "Physics world already exists!");

  psx_world                       = arena_alloc<PSX_World>(psx_arena);
  psx_world->bodies_status_lookup = make_bit_array(psx_arena, num_objects);
  psx_world->bodies_to_remove     = make_bit_array(psx_arena, num_objects);
  psx_world->bodies.v             = arena_alloc_array<PSX_Body>(psx_arena, num_objects);
  psx_world->bodies.sz            = num_objects;

  psx_world->id = {
      .idx      = 0,
      .is_set   = 1,
      .revision = 0,
  };
  return psx_world->id;
}

must_use global PSX_Body_ID
psx_world_add(PSX_World_ID world, PSX_Body_Opts const &opts) {
  ErrorContext("pos=(%g,%g), mass=%g, rot=%g, linear_damping=%g",
               opts.pos.x,
               opts.pos.y,
               opts.mass,
               opts.rot,
               opts.linear_damping);
  ErrorIf(PSX_IS_NULL(world), "World ID is NULL!");
  ErrorIf(opts.mass < 0, "Mass cannot be negative!");
  ErrorIf(opts.linear_damping < 0, "Linear damping factor cannot be be negative!");

  PSX_World &w = psx_world_from_id(world);
  ErrorIf(!ba_is_any_unset(w.bodies_status_lookup), "No space for new objects.");

  u64 unset_idx = ba_find_first_unset(w.bodies_status_lookup);
  ba_set(w.bodies_status_lookup, unset_idx);

  PSX_Body   &body = w.bodies.v[unset_idx];
  PSX_Body_ID id   = {
        .idx      = (u16)unset_idx,
        .world    = world.idx,
        .is_set   = 1,
        .revision = ++body.id.revision,
  };

  body = {
      .id             = id,
      .pos            = opts.pos,
      .rot            = opts.rot,
      .linear_damping = opts.linear_damping,
  };

  // Set the mass only if not zero.
  //
  if (!f32_is_near(opts.mass, 0)) {
    body.mass_inv = 1 / opts.mass;
  }

  return id;
}

global void
psx_world_remove(PSX_World_ID world, PSX_Body_ID id) {
  ErrorContext("idx=%u, world=%u, revision=%u", id.idx, id.world, id.revision);
  ErrorIf(PSX_IS_NULL(id), "Body ID is NULL!");
  ErrorIf(PSX_IS_NULL(world), "World ID is NULL!");

  PSX_World &w    = psx_world_from_id(world);
  PSX_Body  &body = psx_body_from_id(w, id);

  bool const is_or_will_be_dead =
      !ba_test(w.bodies_status_lookup, id.idx) || ba_test(w.bodies_to_remove, id.idx);
  if (is_or_will_be_dead) {
    return;
  }

  ba_set(w.bodies_to_remove, id.idx);

  // Should this happen here or after the actual removal of the body?
  //
  body.id.revision++;
}

global void
psx_body_add_force(PSX_World_ID world, PSX_Body_ID id, fvec2 force) {
  ErrorContext("idx=%u, world=%u, revision=%u", id.idx, id.world, id.revision);
  PSX_World &w    = psx_world_from_id(world);
  PSX_Body  &body = psx_body_from_id(w, id);

  ErrorIf(!ba_test(w.bodies_status_lookup, id.idx), "Object is dead");

  body.force += force;
}

must_use global fvec2
psx_body_get_position(PSX_World_ID world, PSX_Body_ID body) {
  ErrorContext("idx=%u, world=%u, revision=%u", body.idx, body.world, body.revision);
  PSX_World &w = psx_world_from_id(world);
  PSX_Body  &b = psx_body_from_id(w, body);
  return b.pos;
}
