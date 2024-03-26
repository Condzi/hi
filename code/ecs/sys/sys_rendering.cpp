internal void
rendering_init() {
  LogEng_Debug("rendering_init");
  gfx_renderer_init();
}

internal void
rendering_shutdown() {
  LogEng_Debug("rendering_shutdown");
}

internal void
rendering_update(f32 dt) {
  Unused(dt);

  Bit_Array *system = gECS->systems[Sys_Type_Rendering];
  u64        idx    = ba_find_first_set(system);
  for (; idx != MAX_U64;) {
    ECS_Sprite_Component const    &sprite_component    = gECS->sprite[idx];
    ECS_Transform_Component const &transform_component = gECS->transform[idx];
    GFX_Sprite_Opts const          sprite              = {
                              .pos      = transform_component.pos,
                              .sz       = {.x = (f32)sprite_component.tex_rect.w * transform_component.scale.x,
                                           .y = (f32)sprite_component.tex_rect.h * transform_component.scale.y},
                              .rot      = transform_component.rot,
                              .tex      = sprite_component.tex,
                              .tex_rect = sprite_component.tex_rect,
                              .layer    = sprite_component.layer,
    };

    gfx_draw_sprite(sprite);
    idx = ba_find_first_set_from(system, idx + 1);
  }
}

must_use Sys_Logic
rendering_system() {
  return {
      .type     = Sys_Type_Rendering,
      .init     = rendering_init,
      .shutdown = rendering_shutdown,
      .update   = rendering_update,
  };
}
