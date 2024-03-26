internal void
physics_init() {
  LogEng_Debug("physics_init");
  psx_init();
  gGameMaster.psx_world = psx_make_world(64);
}

internal void
physics_shutdown() {
  LogEng_Debug("physics_shutdown");
}

internal void
physics_update(f32 dt) {
  f32 &psx_acc = gGameMaster.psx_accumulator;

  psx_acc += dt;
  if (psx_acc >= PSX_STEP) {
    while (psx_acc > PSX_STEP) {
      psx_world_simulate(gGameMaster.psx_world, PSX_STEP);
      psx_acc -= PSX_STEP;
    }
    psx_world_simulate(gGameMaster.psx_world, psx_acc);
    psx_acc = 0;
  }

  PSX_World_ID const world_id = gGameMaster.psx_world;
  Bit_Array         *system   = gECS->systems[Sys_Type_Physics];
  u64                idx      = ba_find_first_set(system);
  for (; idx != MAX_U64;) {
    ECS_Transform_Component          &transform_component = gECS->transform[idx];
    ECS_Physics_Body_Component const &physics_component   = gECS->physics_body[idx];
    PSX_Body_ID const                 body_id             = physics_component.body;

    transform_component.pos = psx_body_get_position(world_id, body_id);
    transform_component.rot = psx_body_get_rotation(world_id, body_id);
    idx                     = ba_find_first_set_from(system, idx + 1);
  }
}

must_use Sys_Logic
physics_system() {
  return {
      .type     = Sys_Type_Physics,
      .init     = physics_init,
      .shutdown = physics_shutdown,
      .update   = physics_update,
  };
}
