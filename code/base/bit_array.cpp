
must_use Bit_Array *
make_bit_array(Arena *arena, u64 num_bits) {
  u64        num_words = num_bits / 64 + 1;
  Bit_Array *array     = arena_alloc<Bit_Array>(arena);
  array->v             = arena_alloc_array<u64>(arena, num_words);
  array->sz            = num_bits;
  return array;
}

must_use bool
ba_test(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  return array->v[idx] & (1ULL << bit_idx);
}

void
ba_set(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  if (!ba_test(array, bit)) {
    array->num_set++;
  }
  array->v[idx] |= (1ULL << bit_idx);
}

void
ba_unset(Bit_Array *array, u64 bit) {
  u64 idx     = bit / 64;
  u64 bit_idx = bit % 64;
  if (ba_test(array, bit)) {
    array->num_set--;
  }
  array->v[idx] &= ~(1ULL << bit_idx);
}

void
ba_unset_all(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    array->v[i] = 0;
  }
  array->num_set = 0;
}

must_use bool
ba_is_any_set(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i] != 0) {
      return true;
    }
  }
  return false;
}

must_use bool
ba_is_any_unset(Bit_Array *array) {
  for (u64 i = 0; i < (array->sz + 63) / 64; ++i) {
    if (array->v[i] != ~0ULL) {
      return true;
    }
  }
  return false;
}

must_use u64
ba_find_first_set(Bit_Array *array) {
  return ba_find_first_set_from(array, 0);
}

must_use u64
ba_find_first_set_from(Bit_Array *array, u64 beg) {
for (u64 i = beg; i < array->sz; i++) {
    if (ba_test(array, i)) {
      return i;
    }
  }
  return MAX_U64;
}

must_use u64
ba_find_first_unset(Bit_Array *array) {
  return ba_find_first_unset_from(array, 0);
}

must_use u64
ba_find_first_unset_from(Bit_Array *array, u64 beg) {
  for (u64 i = beg; i < array->sz; i++) {
    if (!ba_test(array, i)) {
      return i;
    }
  }
  return MAX_U64;
}
