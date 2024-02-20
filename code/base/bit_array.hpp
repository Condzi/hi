#pragma once

struct Bit_Array {
  u64  sz; // in bits
  u64 *v;
};

must_use global Bit_Array *
make_bit_array(Arena *arena, u64 num_bits);

must_use global bool
ba_test(Bit_Array *array, u64 bit);

global void
ba_set(Bit_Array *array, u64 bit);

global void
ba_unset(Bit_Array *array, u64 bit);

global void
ba_unset_all(Bit_Array *array);

must_use global bool
ba_is_any_set(Bit_Array *array);

must_use global bool
ba_is_any_unset(Bit_Array *array);

// Returns MAX_U64 if not found.
must_use global u64
ba_find_first_set(Bit_Array *array);

// Returns MAX_U64 if not found.
must_use global u64
ba_find_first_unset(Bit_Array *array);
