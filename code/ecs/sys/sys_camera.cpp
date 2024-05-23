internal void
camera_system_init() {
  LogGame_Debug("camera_system_init");
}

internal void
camera_system_shutdown() {
  LogGame_Debug("camera_system_shutdown");
}

internal void
camera_system_update(f32 dt) {
  f32 constexpr static DIST = 100.f;
  Unused(dt);

  Bit_Array *system = gECS->systems[Sys_Type_Camera];
  u64        idx    = ba_find_first_set(system);
  if (idx == MAX_U64) {
    return;
  }

  fvec2 c        = gECS->transform[idx].pos;
  f32   distance = dist(c, gGameMaster.camera.center);
  if (distance > DIST) {
    fvec2 dir = normalized(c - gGameMaster.camera.center);
    gGameMaster.camera.center += dir * (distance - DIST);
  }
}

must_use Sys_Logic
camera_system() {
  return {
      .type     = Sys_Type_Camera,
      .init     = camera_system_init,
      .shutdown = camera_system_shutdown,
      .update   = camera_system_update,
  };
}
