#pragma once
#include "all_inc.hpp"

must_use global Log *
make_log(Arena *arena) {
  Assert(arena);
  Log *log   = (Log *)arena_alloc(arena, sizeof(Log), 8);
  log->arena = arena;
  return log;
}
