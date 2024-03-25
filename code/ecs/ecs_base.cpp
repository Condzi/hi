void
ecs_init() {
  ErrorContext("ECS_LIMIT=%zu", ECS_LIMIT);
  ErrorIf(gECS, "ECS layer already initialized");

  Arena     *arena     = make_arena(false);
  Bit_Array *alive     = make_bit_array(arena, ECS_LIMIT);
  Bit_Array *to_remove = make_bit_array(arena, ECS_LIMIT);
  gECS                 = arena_alloc<ECS_World>(arena);
  *gECS                = {
                     .arena     = arena,
                     .alive     = alive,
                     .to_remove = to_remove,
  };
}

ECS_Entity_ID
ecs_spawn() {
  ErrorContext(
      "alive=%zu/%zu, to_remove=%zu", gECS->alive->num_set, ECS_LIMIT, gECS->to_remove->num_set);
  ErrorIf(gECS->alive->num_set == ECS_LIMIT, "Entity limit exceeded.");

  u16 const idx = (u16)ba_find_first_unset(gECS->alive);
  ba_set(gECS->alive, idx);
  gECS->id[idx].revision++;
  return gECS->id[idx];
}
