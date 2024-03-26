must_use ECS_Entity_ID
spawn_player() {
  ECS_Entity_ID player_id = ecs_spawn();
  u16 const     idx       = player_id.idx;

  gECS->transform[idx] = {
      .scale                = {.x = 3, .y = 3},
  };

  fvec2 const sz = {.x = 49, .y = 43};

  gECS->sprite[idx] = {
      .tex      = gGameMaster.characters,
      .tex_rect = {.x = 263, .y = 132, .w = (u16)sz.x, .h = (u16)sz.y},
      .layer    = {.category = GFX_Layer_Foreground},
  };

  gECS->physics_body[idx] = {
      .body = psx_world_add(gGameMaster.psx_world,
                            {
                                .pos            = {},
                                .center_of_mass = fvec2 {-sz.x, sz.y} * 0.5f * PSX_SCALE_INV,
                                .mass           = 10,
                                .linear_damping = 1.0f,
                            }),
  };

  psx_body_add_box_shape(
      gGameMaster.psx_world, gECS->physics_body[idx].body, {0, 0}, sz * PSX_SCALE_INV);

  ecs_opt_in(Sys_Type_Physics, player_id);
  ecs_opt_in(Sys_Type_Rendering, player_id);
  ecs_opt_in(Sys_Type_PlayerControl, player_id);

  return player_id;
}
