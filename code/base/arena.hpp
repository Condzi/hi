#pragma once

// Constants
//
u64 read_only global ARENA_RESERVE_SIZE = MB(64);

// Every allocation in the arena is pushed by at least 8 bytes of
// poisoned memory (+ alignment).
//
u64 read_only global ARENA_SAFETY_SIZE = 8;

struct Arena {
  Arena *prev;
  Arena *next;

  mem64 base_pos;
  u64 curr_pos;
  u64 reserved;
  bool grow;
};

must_use Arena *
make_arena(bool grow);

void
unmake_arena(Arena &arena);

must_use void *
arena_alloc(Arena *arena, u64 size, u64 alignment);

template <typename T>
must_use T *
arena_alloc(Arena *arena) {
  return (T *)arena_alloc(arena, sizeof(T), alignof(T));
}

template <typename T>
must_use T *
arena_alloc_array(Arena *arena, u64 count) {
  return (T *)arena_alloc(arena, sizeof(T) * count, alignof(T));
}

void
arena_rewind(Arena *arena, u64 where);

void
arena_clear(Arena *arena);

struct Scratch_Buffer {
  Arena *arena;
  u64    mark;
};

must_use Scratch_Buffer
scratch_begin(Arena *arena);

void
scratch_end(Scratch_Buffer *buff);
