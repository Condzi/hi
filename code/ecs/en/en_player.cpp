#include "psx/psx_base.hpp"
ECS_Entity_ID
spawn_player() {
  ECS_Entity_ID player_id = ecs_spawn();
  u16 const     idx       = player_id.idx;

  gECS->name[idx] = "player"_s8;
  gECS->transform[idx] = {
      .scale                = {.x = 3, .y = 3},
  };

  fvec2 const sz = {.x = 49, .y = 43};
  
  gECS->sprite[idx] = {
      .tex      = gGameMaster.characters,
      .tex_rect = {.x = 263, .y = 132, .w = (u16)sz.x, .h = (u16)sz.y},
      .layer    = {.category = GFX_Layer_Foreground},
  };

  PSX_Body_ID psx_body = psx_world_add(gGameMaster.psx_world,
                                       {
                                           .type           = PSX_BodyType_Dynamic,
                                           .pos            = {},
                                           .fixed_rot      = true,
                                           .damping_linear = 5.0f,
                                       });

  psx_body_add_box_shape(psx_body,
                         {
                             .filter                = {.category = En_Type_Player, .mask = En_Type_Zombie},
                             .friction              = 1.0,
                             .density               = 10,
                             .enable_sensor_events  = true,
                             .enable_contact_events = true,
                         },
                         sz * PSX_SCALE_INV * 3.f);

  psx_body_fix_rotation(psx_body);

  gECS->physics_body[idx].body = psx_body;

  ecs_opt_in(Sys_Type_Physics, player_id);
  ecs_opt_in(Sys_Type_Rendering, player_id);
  ecs_opt_in(Sys_Type_PlayerControl, player_id);

  return player_id;
}
