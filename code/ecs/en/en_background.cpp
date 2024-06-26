ECS_Entity_ID
spawn_background() {
  ECS_Entity_ID bg_id = ecs_spawn();
  u16 const     idx   = bg_id.idx;

  f32 const width  = (f32)os_gfx_surface_width();
  f32 const height = (f32)os_gfx_surface_height();

  fvec2 const bg_sz = fvec2 {width, height} * 10.f;


  gECS->name[idx] = "background"_s8;
  gECS->transform[idx] = {
      .pos   = {-bg_sz.x / 2, bg_sz.y / 2},
      .scale = {1, 1},
  };

  gECS->sprite[idx] = {
      .tex      = gGameMaster.bg,
      .tex_rect = {.x = 0, .y = 0, .w = (u16)bg_sz.x, .h = (u16)bg_sz.y},
      .layer    = {.category = GFX_Layer_Background},
  };

  ecs_opt_in(Sys_Type_Rendering, bg_id);

  return bg_id;
}
