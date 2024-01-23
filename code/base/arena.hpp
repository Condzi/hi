#pragma once

// Constants
//
read_only global u64 ARENA_RESERVE_SIZE = MB(64);

struct Arena {
  Arena* prev;
  Arena* curr;

  u64 base_pos;
  u64 curr_pos;
  u64 res;
};

must_use global Arena*
arena_alloc();
