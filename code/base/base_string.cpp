#pragma once
#include "all_inc.hpp"

// Internal Functions Declarations
//

struct Unicode_Decode {
  u32 inc;
  u32 codepoint;
};

read_only global u8 utf8_class[32] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 5,
};

internal Unicode_Decode
utf8_decode(u8 *str, u64 max);

internal Unicode_Decode
utf16_decode(u16 *str, u64 max);

internal u32
utf8_encode(u8 *str, u32 codepoint);

internal u32
utf16_encode(u16 *str, u32 codepoint);

// String Constructors
//

must_use Str8
str8(u8 *v, u64 sz) {
  return {.v = v, .sz = sz};
}

must_use Str8
str8_range(u8 *beg, u8 *end) {
  return {.v = beg, .sz = PtrToU64(end) - PtrToU64(beg)};
}

must_use Str8
str8_cstr(char const *cstr) {
  u64 const len = strlen(cstr);
  return {.v = (u8 *)cstr, .sz = len};
}

must_use Str16
str16(u16 *v, u64 sz) {
  return {.v = v, .sz = sz};
}

must_use Str16
str16_range(u16 *beg, u16 *end) {
  return {.v = beg, .sz = PtrToU64(end) - PtrToU64(beg)};
}

// String Operations
//

must_use bool
str8_compare(Str8 a, Str8 b) {
  if (a.sz != b.sz) {
    return false;
  }

  if (a.v == b.v) {
    return true;
  }

  return MemoryCompare(a.v, b.v, a.sz) == 0;
}

must_use bool
str8_has_prefix(Str8 str, Str8 prefix) {
  AssertAlways(prefix.sz > 0);

  if (str.sz < prefix.sz) {
    return false;
  }

  return MemoryCompare(str.v, prefix.v, prefix.sz) == 0;
}

// Unicode String Conversions
//

global Str8
str8_from_16(Arena *arena, Str16 in) {
  u64            cap  = in.sz * 3;
  u8            *str  = arena_alloc_array<u8>(arena, cap + 1);
  u16           *ptr  = in.v;
  u16           *opl  = ptr + in.sz;
  u64            size = 0;
  Unicode_Decode consume;
  for (; ptr < opl; ptr += (u16)consume.inc) {
    consume = utf16_decode(ptr, (u64)(opl - ptr));
    size += utf8_encode(str + size, consume.codepoint);
  }
  str[size] = 0;
  return (str8(str, size));
}

global Str16
str16_from_8(Arena *arena, Str8 in) {
  u64            cap  = in.sz * 2;
  u16           *str  = arena_alloc_array<u16>(arena, cap + 1);
  u8            *ptr  = in.v;
  u8            *opl  = ptr + in.sz;
  u64            size = 0;
  Unicode_Decode consume;

  for (; ptr < opl; ptr += consume.inc) {
    consume = utf8_decode(ptr, PtrToU64(opl) - PtrToU64(ptr));
    size += utf16_encode(str + size, consume.codepoint);
  }

  str[size] = 0;
  return (str16(str, size));
}

// Internal functions definitions
//

////////////////////////////////
//~ rjf: UTF-8 & UTF-16 Decoding/Encoding

internal Unicode_Decode
utf8_decode(u8 *str, u64 max) {
  Unicode_Decode result     = {1, MAX_U32};
  u8             byte       = str[0];
  u8             byte_class = utf8_class[byte >> 3];
  switch (byte_class) {
    case 1: {
      result.codepoint = byte;
    } break;
    case 2: {
      if (1 < max) {
        u8 cont_byte = str[1];
        if (utf8_class[cont_byte >> 3] == 0) {
          result.codepoint = (u32)((byte & 0x0000001f) << 6);
          result.codepoint |= (cont_byte & 0x0000003f);
          result.inc = 2;
        }
      }
    } break;
    case 3: {
      if (2 < max) {
        u8 cont_byte[2] = {str[1], str[2]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0) {
          result.codepoint = (u32)((byte & 0x0000000f) << 12);
          result.codepoint |= ((cont_byte[0] & 0x0000003f) << 6);
          result.codepoint |= (cont_byte[1] & 0x0000003f);
          result.inc = 3;
        }
      }
    } break;
    case 4: {
      if (3 < max) {
        u8 cont_byte[3] = {str[1], str[2], str[3]};
        if (utf8_class[cont_byte[0] >> 3] == 0 &&
            utf8_class[cont_byte[1] >> 3] == 0 &&
            utf8_class[cont_byte[2] >> 3] == 0) {
          result.codepoint = (u32)((byte & 0x00000007) << 18);
          result.codepoint |= ((cont_byte[0] & 0x0000003f) << 12);
          result.codepoint |= ((cont_byte[1] & 0x0000003f) << 6);
          result.codepoint |= (cont_byte[2] & 0x0000003f);
          result.inc = 4;
        }
      }
    }
  }
  return (result);
}

internal Unicode_Decode
utf16_decode(u16 *str, u64 max) {
  Unicode_Decode result = {1, MAX_U32};
  result.codepoint      = str[0];
  result.inc            = 1;
  if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] &&
      str[1] < 0xE000) {
    result.codepoint = (u32)((str[0] - 0xD800) << 10) | (str[1] - 0xDC00);
    result.inc       = 2;
  }
  return (result);
}

internal u32
utf8_encode(u8 *str, u32 codepoint) {
  u32 inc = 0;
  if (codepoint <= 0x7F) {
    str[0] = (u8)codepoint;
    inc    = 1;
  } else if (codepoint <= 0x7FF) {
    str[0] = (0x00000003 << 6) | ((codepoint >> 6) & 0x0000001f);
    str[1] = (1 << 7) | (codepoint & 0x0000003f);
    inc    = 2;
  } else if (codepoint <= 0xFFFF) {
    str[0] = (0x00000007 << 5) | ((codepoint >> 12) & 0x0000000f);
    str[1] = (1 << 7) | ((codepoint >> 6) & 0x0000003f);
    str[2] = (1 << 7) | (codepoint & 0x0000003f);
    inc    = 3;
  } else if (codepoint <= 0x10FFFF) {
    str[0] = (0x0000000f << 3) | ((codepoint >> 18) & 0x00000007);
    str[1] = (1 << 7) | ((codepoint >> 12) & 0x0000003f);
    str[2] = (1 << 7) | ((codepoint >> 6) & 0x0000003f);
    str[3] = (1 << 7) | (codepoint & 0x0000003f);
    inc    = 4;
  } else {
    str[0] = '?';
    inc    = 1;
  }
  return (inc);
}

internal u32
utf16_encode(u16 *str, u32 codepoint) {
  u32 inc = 1;
  if (codepoint == MAX_U32) {
    str[0] = (u16)'?';
  } else if (codepoint < 0x10000) {
    str[0] = (u16)codepoint;
  } else {
    u32 v  = codepoint - 0x10000;
    str[0] = (u16)(0xD800 + (v >> 10));
    str[1] = (u16)(0xDC00 + (v & 0x000003ff));
    inc    = 2;
  }
  return (inc);
}
