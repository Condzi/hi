
// Based on:
// https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/

must_use internal u32
hash_to_u32_alternative(u64 x) {
  u64 read_only internal a = 0x65d200ce55b19ad8L;
  u64 read_only internal b = 0x4f2162926e40c299L;
  u64 read_only internal c = 0x162dd799029970f8L;

  u32 const low  = (u32)x;
  u32 const high = (u32)(x >> 32);
  return (u32)((a * low + b * high + c ) >> 32);
}

must_use u32
hash_to_u32(u64 x) {
  u64 read_only internal a = 0x68b665e6872bd1f4L;
  u64 read_only internal b = 0xb6cfcf9d79b51db2L;
  u64 read_only internal c = 0x7a2b92ae912898c2L;

  u32 const low  = (u32)x;
  u32 const high = (u32)(x >> 32);
  u32 const res  = (u32)((a * low + b * high + c) >> 32);
  return res;
}

must_use u64
hash_to_u64(u64 x) {
  u64 const res_a = hash_to_u32(x);
  u64 const res_b = hash_to_u32_alternative(x);
  u64 const res   = (res_a >> 32) | ((res_b << 32) & 0xFFFFFFFF00000000L);
  return res;
}

must_use u64
hash_str8(Str8 string) {
  u64 read_only multiplier  = 0xff51afd7ed558ccdL;
  u64           hash_result = multiplier;

  // For performance, we try to hash 8 bytes / characters at a time.
  //
  u64 i = 0;
  for (; i + 7 < string.sz; i += 8) {
    u64 part = 0;
    MemoryCopy(&part, string.v + i, sizeof(part));
    hash_result ^= hash_to_u64(part);
    hash_result *= multiplier;
  }

  // Handle remaining bytes, if any.
  //
  if (string.sz % 8) {
    u64 tail_bytes = 0;
    MemoryCopy(&tail_bytes, string.v + i, string.sz % 8);
    hash_result ^= hash_to_u64(tail_bytes);
    hash_result *= multiplier;
  }

  return hash_result;
}
