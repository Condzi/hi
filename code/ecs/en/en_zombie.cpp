#include "ecs/ecs_base.hpp"
ECS_Entity_ID
spawn_zombie() {
  ECS_Entity_ID id  = ecs_spawn();
  u16 const     idx = id.idx;

  gECS->name[idx] = "zombie"_s8;
  gECS->transform[idx] = {
      .scale = {.x = 3, .y = 3},
  };

  fvec2 const sz = {.x = 35, .y = 43};

  gECS->sprite[idx] = {
      .tex      = gGameMaster.characters,
      .tex_rect = {.x = 424, .y = 0, .w = (u16)sz.x, .h = (u16)sz.y},
      .layer    = {.category = GFX_Layer_Foreground},
  };

  PSX_Body_ID psx_body = psx_world_add(gGameMaster.psx_world,
                                       {
                                           .type           = PSX_BodyType_Dynamic,
                                           .pos            = {2, 2},
                                           .damping_linear = 1.0f,
                                       });

  psx_body_add_box_shape(
      psx_body,
      {
          .filter   = {.category = En_Type_Zombie, .mask = En_Type_Player | En_Type_Zombie},
          .friction = 1.0,
          .density  = 10,
          .enable_sensor_events  = true,
          .enable_contact_events = true,
      },
      sz * PSX_SCALE_INV * 3);

  gECS->physics_body[idx].body = psx_body;

  ecs_opt_in(Sys_Type_Physics, id);
  ecs_opt_in(Sys_Type_Rendering, id);

  return id;
}
