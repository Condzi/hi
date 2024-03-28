ECS_Entity_ID
spawn_zombie() {
  ECS_Entity_ID id  = ecs_spawn();
  u16 const     idx = id.idx;

  gECS->transform[idx] = {
      .scale = {.x = 3, .y = 3},
  };

  fvec2 const sz = {.x = 35, .y = 43};

  gECS->sprite[idx] = {
      .tex      = gGameMaster.characters,
      .tex_rect = {.x = 424, .y = 0, .w = (u16)sz.x, .h = (u16)sz.y},
      .layer    = {.category = GFX_Layer_Foreground},
  };

  gECS->physics_body[idx] = {
      .body = psx_world_add(gGameMaster.psx_world,
                            {
                                .pos            = {2, 2},
                                .center_of_mass = fvec2 {-sz.x, sz.y} * 0.5f * PSX_SCALE_INV,
                                .mass           = 10,
                                .linear_damping = 1.0f,
                            }),
  };

  psx_body_add_box_shape(
      gGameMaster.psx_world, gECS->physics_body[idx].body, {0, 0}, sz * PSX_SCALE_INV);

  ecs_opt_in(Sys_Type_Physics, id);
  ecs_opt_in(Sys_Type_Rendering, id);

  return id;
}
