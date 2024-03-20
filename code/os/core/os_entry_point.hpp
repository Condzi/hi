#pragma once

using Startup_proc  = bool (*)();
using Update_proc   = bool (*)(f32);
using Render_proc   = void (*)();
using Teardown_proc = void (*)();

struct OS_Entry_Point_Setter {
  Startup_proc  startup;
  Update_proc   update;
  Render_proc   render;
  Teardown_proc teardown;
};
extern OS_Entry_Point_Setter gEntryPoint;

#define HI_ENTRY_POINT(s, u, r, t)                                                                 \
  OS_Entry_Point_Setter gEntryPoint {                                                              \
    .startup = (s), .update = (u), .render = (r), .teardown = (t),                                 \
  }
