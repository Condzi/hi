#pragma once
#include "all_inc.hpp"

// Internal stuff
//

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

internal consteval Block_Size_Map
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

struct word_aligned Memory_Block {
  mem32 prev;
  mem32 next;
  u8    sz;
};

struct word_aligned Block_Allocator {
  u32 used_blocks;
  u32 free_blocks;

  mem64 base_pos;
  u32   curr_pos;
  u32   reserved;
};

must_use internal Memory_Block *
find_or_alloc_block(Block_Allocator *allocator, u8 sz);

// Public interface
//

must_use Block_Allocator *
make_block_allocator(Arena *arena, u64 sz) {
  Assert(sz > sizeof(Block_Allocator));
  Assert(sz < MAX_U32);

  void       *base_mem = arena_alloc(arena, sz, 8);
  mem64 const base     = PtrToU64(base_mem);

  Block_Allocator *result = (Block_Allocator *)base_mem;
  AsanUnpoisonMemoryRegion(U64ToPtr(base), sizeof(Block_Allocator));
  *result = {
      .base_pos = base,
      .curr_pos = sizeof(Block_Allocator),
      .reserved = (u32)sz,
  };
  return result;
}

must_use void *
alloc_block(Block_Allocator *allocator, u64 sz) {
  Assert(allocator && allocator->base_pos);
  Assert(sz > 0 && sz <= BLOCK_SIZE_MAX);

  u8 const      blk_sz = BLOCK_SIZE_MAP.values[sz];
  Memory_Block *block  = find_or_alloc_block(allocator, blk_sz);

  return U64ToPtr(PtrToU64(block) + sizeof(Memory_Block));
}

void
free_block(Block_Allocator *allocator, void *mem) {
  Assert(allocator && allocator->base_pos);
  Assert(mem);

  mem64 const mem_pos = PtrToU64(mem);
  Assert(mem_pos > allocator->base_pos);
  Assert(mem_pos < allocator->base_pos + allocator->curr_pos);
  Assert(Is8ByteAligned(mem_pos));

  // Assume the parent Memory_Block lives behind the `pos`.
  //
  mem32 const block_mem_pos = mem_pos - allocator->base_pos;
  mem32 const block_pos     = block_mem_pos - sizeof(Memory_Block);
  DLL_remove_relative<Memory_Block>(allocator->base_pos, allocator->used_blocks, block_pos);
  DLL_insert_at_end_relative<Memory_Block>(allocator->base_pos, allocator->free_blocks, block_pos);

  // Poison the underlaying memory region (but not the Memory_Block fields).
  //
  Memory_Block &block = *(Memory_Block *)U64ToPtr(allocator->base_pos + block_pos);
  AsanPoisonMemoryRegion((mem8 *)(&block) + sizeof(Memory_Block), BLOCK_SIZE_MAP.values[block.sz]);
}

// Internal stuff definition
//

must_use internal Memory_Block *
find_or_alloc_block(Block_Allocator *allocator, u8 sz) {
  Assert(sz >= 0 && sz <= BLOCK_SIZES_COUNT); // sic! we have <0, BLOCK_SIZES_COUNT> values.

  u32 block_pos = 0;
  if (allocator->free_blocks) {
    // Try to find a free block.
    //
    block_pos = allocator->free_blocks;
    while (block_pos) {
      Memory_Block &blk = *((Memory_Block *)U64ToPtr(allocator->base_pos + block_pos));
      if (blk.sz == sz) {
        DLL_remove_relative<Memory_Block>(allocator->base_pos, allocator->free_blocks, block_pos);
        break;
      }
      block_pos = blk.next;
    }
  }

  if (!block_pos) {
    // @Todo: might be worth trying to look in the free_blocks again, but this time with condition
    // blk.sz < sz instead of failing.
    ErrorIf(allocator->curr_pos + sizeof(Memory_Block) + BLOCK_SIZE_MAP.values[sz] >
                allocator->reserved,
            "Block allocator ran out of memory");
    // Need to allocate a new block.
    //
    block_pos = allocator->curr_pos;
    allocator->curr_pos += sizeof(Memory_Block) + BLOCK_SIZE_MAP.values[sz];
  }

  // Unpoison the memory.
  //
  mem64 const block_pos_abs = allocator->base_pos + block_pos;
  AsanUnpoisonMemoryRegion(U64ToPtr(block_pos_abs),
                           sizeof(Memory_Block) + BLOCK_SIZE_MAP.values[sz]);

  Memory_Block *blk = ((Memory_Block *)U64ToPtr(block_pos_abs));
  blk->sz           = sz;
  DLL_insert_at_end_relative<Memory_Block>(allocator->base_pos, allocator->used_blocks, block_pos);

  return blk;
}
