#pragma once

// Strings are assumed to point to the .TEXT section -- DO NOT
// MODIFY THEM DIRECTY, UB WILL OCCUR.
//

struct Str8 {
  u8 *v;
  u64 sz;
};

struct Str16 {
  u16 *v;
  u64  sz;
};

// String Constructors
//
#define str8_lit(S) str8((u8 *)S, sizeof(S) - 1)

must_use Str8
str8(u8 *v, u64 sz);

must_use Str8
str8_range(u8 *beg, u8 *end);

must_use Str8
str8_cstr(char const *cstr);

must_use Str16
str16(u16 *v, u64 sz);

must_use Str16
str16_range(u16 *beg, u16 *end);

// Unicode String Conversions
// For legacy reasons, results are NULL-terminated.
//

global Str8
str8_from_16(Arena *arena, Str16 in);

global Str16
str16_from_8(Arena *arena, Str8 in);
