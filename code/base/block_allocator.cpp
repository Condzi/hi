#pragma once
#include "all_inc.hpp"

// Original code by Erin Catto, Box2c. I just removed dynamic allocation in favor of static
// arena-like buffer.

internal read_only u64 BLOCK_SIZES[] = {
    16,  // 0
    32,  // 1
    64,  // 2
    96,  // 3
    128, // 4
    160, // 5
    192, // 6
    224, // 7
    256, // 8
    320, // 9
    384, // 10
    448, // 11
    512, // 12
    640, // 13
};

internal read_only u64 BLOCK_SIZES_COUNT = ArrayCount(BLOCK_SIZES);
internal read_only u64 BLOCK_SIZE_MAX    = BLOCK_SIZES[BLOCK_SIZES_COUNT - 1];

struct Block_Size_Map {
  u8 values[BLOCK_SIZE_MAX + 1];
};

consteval Block_Size_Map
generate_block_size_map() {
  Block_Size_Map map = {};
  map.values[0]      = 0;
  s32 j              = 0;
  for (int32_t i = 1; i <= BLOCK_SIZE_MAX; ++i) {
    if (i <= BLOCK_SIZES[j]) {
      map.values[i] = (uint8_t)j;
    } else {
      ++j;
      map.values[i] = (uint8_t)j;
    }
  }
  return map;
}

internal read_only Block_Size_Map BLOCK_SIZE_MAP = generate_block_size_map();

struct Memory_Block {
  Memory_Block *next;
  Memory_Block *prev;
  u64           base_pos;
};

struct Memory_Chunk {
  Memory_Block *next;
  u64           sz;
  Memory_Block *blocks;
};

struct Block_Allocator {
  Memory_Chunk *used;
  Memory_Chunk *free;

  u64 base_pos;
  u64 curr_pos;
  u64 reserved;
};
