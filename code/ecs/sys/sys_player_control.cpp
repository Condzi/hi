internal void
player_control_init() {
  LogEng_Debug("player_control_init");
}

internal void
player_control_shutdown() {
  LogEng_Debug("player_control_shutdown");
}

internal void
player_control_update(f32 dt) {
  Unused(dt);

  fvec2 mov_dir = {};
  if (kb_state(KB_MoveUp).held) {
    mov_dir.y += 1;
  }
  if (kb_state(KB_MoveDown).held) {
    mov_dir.y -= 1;
  }
  if (kb_state(KB_MoveLeft).held) {
    mov_dir.x -= 1;
  }
  if (kb_state(KB_MoveRight).held) {
    mov_dir.x += 1;
  }

  fvec2 const force = mov_dir * 100.f;

  Bit_Array *system = gECS->systems[Sys_Type_PlayerControl];
  u64        idx    = ba_find_first_set(system);
  for (; idx != MAX_U64;) {
    ECS_Physics_Body_Component const &physics_body_component = gECS->physics_body[idx];

    psx_body_add_force(physics_body_component.body, force);

    idx = ba_find_first_set_from(system, idx + 1);
  }
}

must_use Sys_Logic
player_control_system() {
  return {
      .type     = Sys_Type_PlayerControl,
      .init     = player_control_init,
      .shutdown = player_control_shutdown,
      .update   = player_control_update,
  };
}
