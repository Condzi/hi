must_use global Arena *
make_arena(bool grow) {
  u64 mem = PtrToU64(os_alloc(ARENA_RESERVE_SIZE, 0));

  Arena *arena = (Arena *)U64ToPtr(mem);
  *arena       = {
            .base_pos = mem,
            .curr_pos = sizeof(Arena),
            .reserved = ARENA_RESERVE_SIZE,
            .grow     = grow,
  };

  AsanPoisonMemoryRegion(U64ToPtr(mem + arena->curr_pos),
                         arena->reserved - arena->curr_pos);

  return arena;
}

global void
unmake_arena(Arena &arena) {
  AssertAlways(arena.base_pos);

  // @ToDo: remove from linked list?

  os_free(U64ToPtr(arena.base_pos), arena.reserved);
  AsanPoisonMemoryRegion(U64ToPtr(arena.base_pos), arena.reserved);
}

must_use global void *
arena_alloc(Arena *arena, u64 size, u64 alignment) {
  AssertAlways(arena);
  AssertAlways(arena->base_pos);
  AssertAlways(IsPow2(alignment));

  Arena *it = arena;
  while (it) {
    // Add the safety zone, then align.
    //
    u64 new_pos = it->curr_pos + ARENA_SAFETY_SIZE;
    new_pos     = AlignPow2(new_pos, alignment);

    if (new_pos + size >= it->reserved) {
      it = it->next;
    } else {
      it->curr_pos = new_pos + size;

      void *res = U64ToPtr(it->base_pos + new_pos);
      AsanUnpoisonMemoryRegion(res, size);
      return res;
    }
  }

  if (!arena->grow) {
    InvalidPath;
  }

  // Arena is full - add a new one and retry.
  //
  Arena *new_arena = make_arena(true);
  DLL_insert(arena, new_arena);

  u64 new_pos = new_arena->curr_pos + ARENA_SAFETY_SIZE;
  new_pos     = AlignPow2(new_pos, alignment);

  if (new_pos + size >= new_arena->reserved) {
    // @ToDo: implement large arenas.
    //
    NotImplemented;
    return NULL;
  }


  void *res = U64ToPtr(new_arena->base_pos + new_pos);
  AsanUnpoisonMemoryRegion(res, size);
  return res;
}

global void
arena_rewind(Arena *arena, u64 where) {
  Assert(!arena->next);
  Assert(where < arena->curr_pos);
  Assert(where >= sizeof(Arena));

  // Unpoison the entire used region since we have the safety
  // bytes laying there.
  //
  void     *where_ptr     = U64ToPtr(arena->base_pos + where);
  u64 const bytes_to_free = arena->curr_pos - where;
  AsanUnpoisonMemoryRegion(where_ptr, bytes_to_free);

  MemorySet(where_ptr, 0, bytes_to_free);

  // Poision the memory again -- it is ready for reuse.
  //
  AsanPoisonMemoryRegion(where_ptr, bytes_to_free);
}

global void
arena_clear(Arena* arena) {
  arena_rewind(arena, sizeof(Arena));
}

must_use global Scratch_Buffer
scratch_begin(Arena *arena) {
  Assert(!arena->grow);
  return {
      .arena = arena,
      .mark  = arena->curr_pos,
  };
}

global void
scratch_end(Scratch_Buffer *buff) {
  arena_rewind(buff->arena, buff->mark);
}
