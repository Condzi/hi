#pragma once

using Startup_proc  = bool (*)();
using Loop_proc     = bool (*)(f32);
using Teardown_proc = void (*)();

struct OS_Entry_Point_Setter {
  Startup_proc  startup;
  Loop_proc     loop;
  Teardown_proc teardown;
};

extern OS_Entry_Point_Setter gEntryPoint;

#define HI_ENTRY_POINT(s, l, t)                                                                    \
  OS_Entry_Point_Setter gEntryPoint {                                                              \
    .startup = (s), .loop = (l), .teardown = (t),                                 \
  }
