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
  os_init(argc, argv);

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

  tcx.frame_arena = make_arena();
  os_debug_message(str8_lit("Cześć, Świecie!"));
  
  return 0;
}
