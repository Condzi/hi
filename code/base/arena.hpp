#pragma once

// Constants
//
read_only global u64 ARENA_RESERVE_SIZE = MB(64);

// Every allocation in the arena is pushed by at least 8 bytes of
// poisoned memory (+ alignment).
//
read_only global u64 ARENA_SAFETY_SIZE = 8;

struct Arena {
  Arena *prev;
  Arena *next;

  u64 base_pos;
  u64 curr_pos;
  u64 reserved;
};

must_use global Arena *
make_arena();

global void
unmake_arena(Arena &arena);

must_use global void *
arena_alloc(Arena *arena, u64 size, u64 alignment);
