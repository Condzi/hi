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

must_use Str8
operator""_s8(char const *str, size_t len);

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

must_use Str16
str16_cstr(wchar_t const *cstr);

// String Operations
//

must_use bool
str8_compare(Str8 a, Str8 b);

must_use bool
str8_has_prefix(Str8 str, Str8 prefix);

must_use Str8
str8_concat(Arena *arena, Str8 a, Str8 b);

#if defined(__has_attribute)
#if __has_attribute(format)
#define AttributeFormat(fmt, va) __attribute__((format(printf, fmt, va)))
#endif
#endif

#ifndef AttributeFormat
#define AttributeFormat(fmt, va)
#endif

template <typename... TArgs>
must_use Str8
str8_sprintf(Arena *arena, char const *format, TArgs... args) AttributeFormat(2, 3);

#undef AttributeFormat

// Unicode String Conversions
// For legacy reasons, results are NULL-terminated.
//

must_use Str8
str8_from_16(Arena *arena, Str16 in);

must_use Str16
str16_from_8(Arena *arena, Str8 in);

// Some custom functions
//

template <typename T>
must_use Str8
str8_dump_struct(T &s);
