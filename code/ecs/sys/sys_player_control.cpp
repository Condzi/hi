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

  bool const shoot = kb_state(KB_PrimaryWeapon).pressed;

  fvec2 const force = mov_dir * 100.f;
  fvec2 const cursor_pos = gfx_get_mouse_pos_in_world();

  Bit_Array *system = gECS->systems[Sys_Type_PlayerControl];
  u64        idx    = ba_find_first_set(system);
  for (; idx != MAX_U64;) {
    ECS_Physics_Body_Component const &physics_body_component = gECS->physics_body[idx];
    ECS_Transform_Component const    &transform_component    = gECS->transform[idx];
    PSX_Body_ID const                 body                   = physics_body_component.body;

    fvec2 const en_pos = transform_component.pos;
    fvec2 const dir    = normalized(cursor_pos - en_pos);
    f32 const   rot    = fatan2(dir.y, dir.x);
    // @ToDo(kkubacki): replace with https://gamedev.stackexchange.com/a/72459
    //

    psx_body_set_rotation(body, rot);

    // @Todo(kkubacki): set linear velocity instead of applying force.
    //
    psx_body_add_force(body, force);

    if (shoot) {
      PSX_Shape_ID res = psx_raycast_nearest(gGameMaster.psx_world,
                                             en_pos * PSX_SCALE_INV,
                                             dir * 100.f,
                                             {.category = En_Type_Player, .mask = En_Type_Zombie});
      if (PSX_NON_NULL(res)) {
        LogGame_Info("Zombie hit!");
        PSX_Body_ID   res_body = psx_shape_id_to_body_id(res);
        ECS_Entity_ID en_id {.v = PtrToU64(psx_body_get_user_data(res_body))};
        ecs_kill(en_id);
      }
    }

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
