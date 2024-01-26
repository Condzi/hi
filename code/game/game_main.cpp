// HPP
//
#include "base/base_inc.hpp"
#include "os/os_inc.hpp"

// CPP
//
#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

int
main(int argc, char const *argv[]) {
  gContext.frame_arena = make_arena();
  gContext.misc_arena = make_arena();

  os_init(argc, argv);
  os_gfx_init();

  Arena *a = make_arena();

  u64 *u = (u64 *)arena_alloc(a, 80, 8);
  Unused(u);
  u = (u64 *)arena_alloc(a, 80, 16);
  // This should trigger asan!
  // u[11] = 123;

  Unused(arena_alloc(a, MB(62), 8));

  // This should be in a new arena
  //
  u = (u64 *)arena_alloc(a, MB(2), 8);

  // This should be in the old one
  //
  u    = (u64 *)arena_alloc(a, 8, 8);
  u[1] = 123;

  u64 now = os_now_us();
  Unused(now);

  os_debug_message(str8_lit("Cześć, Świecie! ąćźłóę ĄĆŹŁÓĘ\n"));

  os_gfx_open_window({
      .title      = str8_lit("Mój tytuł"),
      .width      = 1280,
      .height     = 720,
      .fullscreen = false,
  });

  while (true) {
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    MemorySet(
        U64ToPtr(gContext.frame_arena->base_pos), 0, gContext.frame_arena->curr_pos);
    gContext.frame_arena->curr_pos = 0;
  }

  return 0;
}
