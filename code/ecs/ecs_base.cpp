void
ecs_init() {
  ErrorContext("ECS_LIMIT=%zu", ECS_LIMIT);
  ErrorIf(gECS, "ECS layer already initialized");

  Arena     *arena     = make_arena(false);
  Bit_Array *alive     = make_bit_array(arena, ECS_LIMIT);
  Bit_Array *to_remove = make_bit_array(arena, ECS_LIMIT);
  Bit_Array *systems[Sys_Type__count] = {};
  for (int i = 0; i < Sys_Type__count; i++) {
    systems[i] = make_bit_array(arena, ECS_LIMIT);
  }

  gECS                 = arena_alloc<ECS_World>(arena);
  *gECS                = {
                     .arena     = arena,
                     .alive     = alive,
                     .to_remove = to_remove,
  };

  MemoryCopy(gECS->systems, systems, sizeof(systems));
}

must_use ECS_Entity_ID
ecs_spawn() {
  ErrorContext(
      "alive=%zu/%zu, to_remove=%zu", gECS->alive->num_set, ECS_LIMIT, gECS->to_remove->num_set);
  ErrorIf(gECS->alive->num_set == ECS_LIMIT, "Entity limit exceeded.");

  u16 const idx = (u16)ba_find_first_unset(gECS->alive);
  ba_set(gECS->alive, idx);
  gECS->id[idx].revision++;
  return gECS->id[idx];
}

void
ecs_kill(ECS_Entity_ID id) {
  ErrorContext("idx=%d, is_set=%d, revision=%d, alive=%zu/%zu, to_remove=%zu",
               (int)id.idx,
               (int)id.is_set,
               (int)id.revision,
               gECS->alive->num_set,
               ECS_LIMIT,
               gECS->to_remove->num_set);
  ErrorIf(gECS->to_remove->num_set == ECS_LIMIT, "Entity limit exceeded.");
  ErrorIf(ECS_IS_NULL(id), "Invalid ID");
  ErrorIf(!ECS_ID_EQUALS(id, gECS->id[id.idx]), "IDs do not match");

  ba_set(gECS->to_remove, id.idx);
  gECS->id[id.idx].revision++;
}

void
ecs_kill_pass() {
  u64 idx = ba_find_first_set_from(gECS->to_remove, 0);
  for (; idx != MAX_U64; idx = ba_find_first_set_from(gECS->to_remove, idx)) {
    ba_unset(gECS->alive, idx);
#define X(unused_, name) gECS->name[idx] = {};
#include "ecs_components.inl"
#undef X
    idx++;
  }

  ba_unset_all(gECS->to_remove);
}
