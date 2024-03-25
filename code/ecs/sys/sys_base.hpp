#pragma once

enum Sys_Type {
  Sys_Type_Physics,
  Sys_Type_Rendering,

  Sys_Type__count,
};

// @ToDo: replace void* with pointer to ECS instance 
//
using Sys_init_proc     = void (*)(void *);
using Sys_shutdown_proc = void (*)(void *);
using Sys_update_proc   = void (*)(void *, f32);

struct Sys_Logic {
  Sys_Logic *next;
  Sys_Logic *prev;

  Sys_Type type;

  Sys_init_proc     init;
  Sys_shutdown_proc shutdown;
  Sys_update_proc   update;
};

void
sys_init();

must_use Sys_Logic
sys_get(Sys_Type type);
