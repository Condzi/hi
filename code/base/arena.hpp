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
  bool grow;
};

must_use global Arena *
make_arena(bool grow);

global void
unmake_arena(Arena &arena);

must_use global void *
arena_alloc(Arena *arena, u64 size, u64 alignment);

template <typename T>
must_use global T *
arena_alloc(Arena *arena) {
  return (T *)arena_alloc(arena, sizeof(T), alignof(T));
}

template <typename T>
must_use global T *
arena_alloc_array(Arena *arena, u64 count) {
  return (T *)arena_alloc(arena, sizeof(T) * count, alignof(T));
}

global void
arena_rewind(Arena *arena, u64 where);

global void
arena_clear(Arena* arena);

struct Scratch_Buffer {
  Arena *arena;
  u64    mark;
};

must_use global Scratch_Buffer
scratch_begin(Arena* arena);

global void 
scratch_end(Scratch_Buffer* buff);
