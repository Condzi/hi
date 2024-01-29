#pragma once
#include "all_inc.hpp"

int
main(int argc, char const *argv[]) {
  gContext.frame_arena = make_arena(false);
  gContext.misc_arena  = make_arena(true);

  os_init(argc, argv);
  os_gfx_init();

  Arena *a = make_arena(true);

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
  // u[1] = 123;

  u64 now = os_now_us();
  Unused(now);

  os_debug_message("Cześć, Świecie! ąćźłóę ĄĆŹŁÓĘ\n"_s8);

  os_debug_message(
      str8_sprintf(a, "Test: %_$$d. 8 in binary is 0b%b"_s8, MB(128), 8));

  os_gfx_open_window({
      .title      = "Mój tytuł"_s8,
      .width      = 1280,
      .height     = 720,
      .fullscreen = false,
  });

  while (os_gfx_window_mode() != OS_WindowMode_Closed) {
    OS_Window_Event *events = os_gfx_event_pump(gContext.frame_arena);
    Unused(events);

    arena_clear(gContext.frame_arena);
  }

  return 0;
}
