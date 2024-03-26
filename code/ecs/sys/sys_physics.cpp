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
