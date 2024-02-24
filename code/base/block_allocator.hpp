#pragma once

// Internally holds multiple lists of blocks. blocks have few common sizes: 16, 32, 64, 96...512.
// On allocation, we just take one of free blocks and return it.
// It's a more sophisticated version of Arena allocator, where you can release the allocation
// without the need of releasing the entire allocator.
//
struct Block_Allocator;

must_use global Block_Allocator *
make_block_allocator(Arena* arena, u64 sz);

must_use global void *
alloc_block(Block_Allocator *allocator, u64 sz);

global void
free_block(Block_Allocator *allocator, void *block);
