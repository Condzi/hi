must_use global Arena *
make_arena() {
  u64 mem = PtrToU64(os_alloc(ARENA_RESERVE_SIZE, 0));

  Arena *arena = (Arena *)U64ToPtr(mem);
  *arena       = {
            .base_pos = mem,
            .curr_pos = sizeof(Arena),
            .reserved = ARENA_RESERVE_SIZE,
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

  // Arena is full - add a new one and retry.
  //
  Arena *new_arena = make_arena();
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
