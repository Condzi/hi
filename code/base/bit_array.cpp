#pragma once
#include "all_inc.hpp"

must_use global Bit_Array *
make_bit_array(Arena *arena, u64 num_bits) {
  u64        num_words = num_bits / 64 + 1;
  Bit_Array *array     = arena_alloc<Bit_Array>(arena);
  array->v             = arena_alloc_array<u64>(arena, num_words);
  array->sz            = num_bits;
  return array;
}

must_use global bool
ba_test(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  return array->v[idx] & (1ULL << bit_idx);
}

global void
ba_set(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  array->v[idx] |= (1ULL << bit_idx);
}

global void
ba_unset(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  array->v[idx] &= ~(1ULL << bit_idx);
}

global void
ba_unset_all(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    array->v[i] = 0;
  }
}

must_use global bool
ba_is_any_set(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i] != 0) {
      return true;
    }
  }
  return false;
}

must_use global bool
ba_is_any_unset(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i] != ~0ULL) {
      return true;
    }
  }
  return false;
}

must_use global u64
ba_find_first_set(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i]) {
      int bit_idx = __builtin_ctzll(array->v[i]);
      return i * 64 + bit_idx;
    }
  }
  return MAX_U64;
}

must_use global u64
ba_find_first_unset(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i] != ~0ULL) {
      u64 negated = ~(array->v[i]);
      int bit_idx = __builtin_ctzll(negated);
      return i * 64 + bit_idx;
    }
  }
  return MAX_U64;
}
