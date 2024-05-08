#pragma once

// External dependencies on libc
//
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

// Language layer -- keywords
//
#define local_persist static
#define global        static
#define internal      static
#define read_only     constexpr
#define must_use      [[nodiscard]]
#define no_return     [[noreturn]]
#define fallthrough   [[fallthrough]]
#define word_aligned  alignas(8)

// Language layer -- memory operations
//
#define MemoryCopy(dst, src, size) memmove((dst), (src), (size))
#define MemorySet(dst, byte, len)  memset((dst), (byte), (len))
#define MemoryCompare(a, b, len)   memcmp((a), (b), (len))

// Language layer -- asserts
//
#define StaticAssert(x) static_assert((x))

#define Unused(x) (void)(x)

#define Trap() __debugbreak()

#define AssertAlways(x)                                                                            \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      Trap();                                                                                      \
    }                                                                                              \
  } while (0)

#if !defined(NDEBUG)
#define Assert(x) AssertAlways((x))
#else
#define Assert(x) Unused((x))
#endif

#define NotImplemented Assert(!"Not implemented!")
#define InvalidPath    Assert(!"Invalid code path taken!")

// Defer Macro
//
template <typename T>
struct Defer_Caller final {
  T const f;
  Defer_Caller(T f_) : f(f_) {}
  ~Defer_Caller() { f(); }

  void
  operator=(Defer_Caller) = delete;
};

struct Defer_Maker final {
  template <typename T>
  Defer_Caller<T>
  operator+(T f_) const {
    return Defer_Caller<T> {f_};
  }
};

#define Defer auto const Glue(_defer_caller_, __LINE__) = Defer_Maker {} + [&]

// Language layer -- misc helper macros
//

#define C_LINKAGE_BEGIN extern "C" {
#define C_LINKAGE_END   }
#define C_LINKAGE       extern "C"

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Stringify_(S) #S
#define Stringify(S)  Stringify_(S)

#define Glue_(A, B) A##B
#define Glue(A, B)  Glue_(A, B)

#define Min(A, B) (((A) < (B)) ? (A) : (B))
#define Max(A, B) (((A) > (B)) ? (A) : (B))

#define ClampTop(A, X) Min(A, X)
#define ClampBot(X, B) Max(X, B)
#define Clamp(A, X, B) (((X) < (A)) ? (A) : ((X) > (B)) ? (B) : (X))

#define Compose64Bit(a, b)  ((((U64)(a)) << 32) | ((U64)(b)));
#define AlignPow2(x, b)     (((x) + (b)-1) & (~((b)-1)))
#define AlignDownPow2(x, b) ((x) & (~((b)-1)))
#define AlignPadPow2(x, b)  ((0 - (x)) & ((b)-1))
#define IsPow2(x)           ((x) != 0 && ((x) & ((x)-1)) == 0)
#define IsPow2OrZero(x)     ((((x)-1) & (x)) == 0)
#define Is8ByteAligned(x)   (((x) & 0x07) == 0)

#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)
#define TB(n) (((u64)(n)) << 40)

#define Thousand(n) ((n) * 1'000ULL)
#define Million(n)  ((n) * 1'000'000ULL)
#define Billion(n)  ((n) * 1'000'000'000ULL)

#define Swap(a, b)                                                                                 \
  do {                                                                                             \
    auto t__ = (a);                                                                                \
    (a)      = (b);                                                                                \
    (b)      = t__;                                                                                \
  } while (0)

// Conversion macros
//
#define PtrToU64(p) (u64)((p))
#define U64ToPtr(u) (void *)((u64)(u))

// ASAN
//
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#define ASAN_ENABLED 1
#define NO_ASAN      __declspec(no_sanitize_address)
#else
#define NO_ASAN
#endif

#if defined(ASAN_ENABLED)

C_LINKAGE_BEGIN
void
__asan_poison_memory_region(void const volatile *addr, size_t size);
void
__asan_unpoison_memory_region(void const volatile *addr, size_t size);
C_LINKAGE_END

#define AsanPoisonMemoryRegion(addr, size)   __asan_poison_memory_region((addr), (size))
#define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#define AsanPoisonMemoryRegion(addr, size)   ((void)(addr), (void)(size))
#define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))

#endif

// Fixed-width types
//

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;

// Pointer types used in relative poitners arithmetic.
//
using mem8  = u8;
using mem16 = u16;
using mem32 = u32;
using mem64 = u64;

// Concepts
//
template <typename T>
concept UnsignedInteger = !static_cast<bool>(T(-1) < T(0));

template <typename TFrom, typename TTo>
concept ConvertibleTo = requires(TFrom from) { static_cast<TTo>(from); };

// Common math types
//

// Vector types
//

union fvec2 {
  struct {
    f32 x, y;
  };

  struct {
    f32 width, height;
  };

  struct {
    f32 min, max; // Extents
  };

  f32 v[2] = {};
};

union fvec3 {
  struct {
    f32 x, y, z;
  };

  struct {
    f32 r, g, b;
  };

  struct {
    fvec2 xy;
    f32   _unused0;
  };

  f32 v[3] = {};
};

union fvec4 {
  struct {
    f32 x, y, z, w;
  };

  struct {
    f32 r, g, b, a;
  };

  struct {
    fvec3 xyz;
    f32   _unused0;
  };

  struct {
    fvec2 xy;
    fvec2 _unused2;
  };

  struct {
    fvec2 _unused3;
    fvec2 zw;
  };

  struct {
    fvec2 min;
    fvec2 max;
  };

  f32 v[4] = {};
};

// Matrix types
//

struct fmat4 {
  f32 v[4][4];
};

// Common math constants
//

u64 read_only global MAX_U64 = UINT64_MAX;
u32 read_only global MAX_U32 = UINT32_MAX;
u16 read_only global MAX_U16 = UINT16_MAX;
u8 read_only global  MAX_U8  = UINT8_MAX;

s64 read_only global MAX_S64 = INT64_MAX;
s32 read_only global MAX_S32 = INT32_MAX;
s16 read_only global MAX_S16 = INT16_MAX;
s8 read_only global  MAX_S8  = INT8_MAX;

s64 read_only global MIN_S64 = INT64_MIN;
s32 read_only global MIN_S32 = INT32_MIN;
s16 read_only global MIN_S16 = INT16_MIN;
s8 read_only global  MIN_S8  = INT8_MIN;

f32 read_only global EPS_F32 = FLT_EPSILON;
f32 read_only global MIN_F32 = 1.175494351e-38f;
f32 read_only global MAX_F32 = 3.402823466e+38f;

f32 read_only global PI   = 3.1415927f;
f32 read_only global PI_2 = 1.5707963f;
f32 read_only global PI_6 = 0.5235988f;

// Bits
//

u32 read_only global bitmask1  = 0x00000001;
u32 read_only global bitmask2  = 0x00000003;
u32 read_only global bitmask3  = 0x00000007;
u32 read_only global bitmask4  = 0x0000000f;
u32 read_only global bitmask5  = 0x0000001f;
u32 read_only global bitmask6  = 0x0000003f;
u32 read_only global bitmask7  = 0x0000007f;
u32 read_only global bitmask8  = 0x000000ff;
u32 read_only global bitmask9  = 0x000001ff;
u32 read_only global bitmask10 = 0x000003ff;
u32 read_only global bitmask11 = 0x000007ff;
u32 read_only global bitmask12 = 0x00000fff;
u32 read_only global bitmask13 = 0x00001fff;
u32 read_only global bitmask14 = 0x00003fff;
u32 read_only global bitmask15 = 0x00007fff;
u32 read_only global bitmask16 = 0x0000ffff;
u32 read_only global bitmask17 = 0x0001ffff;
u32 read_only global bitmask18 = 0x0003ffff;
u32 read_only global bitmask19 = 0x0007ffff;
u32 read_only global bitmask20 = 0x000fffff;
u32 read_only global bitmask21 = 0x001fffff;
u32 read_only global bitmask22 = 0x003fffff;
u32 read_only global bitmask23 = 0x007fffff;
u32 read_only global bitmask24 = 0x00ffffff;
u32 read_only global bitmask25 = 0x01ffffff;
u32 read_only global bitmask26 = 0x03ffffff;
u32 read_only global bitmask27 = 0x07ffffff;
u32 read_only global bitmask28 = 0x0fffffff;
u32 read_only global bitmask29 = 0x1fffffff;
u32 read_only global bitmask30 = 0x3fffffff;
u32 read_only global bitmask31 = 0x7fffffff;
u32 read_only global bitmask32 = 0xffffffff;

u64 read_only global bitmask33 = 0x00000001ffffffffull;
u64 read_only global bitmask34 = 0x00000003ffffffffull;
u64 read_only global bitmask35 = 0x00000007ffffffffull;
u64 read_only global bitmask36 = 0x0000000fffffffffull;
u64 read_only global bitmask37 = 0x0000001fffffffffull;
u64 read_only global bitmask38 = 0x0000003fffffffffull;
u64 read_only global bitmask39 = 0x0000007fffffffffull;
u64 read_only global bitmask40 = 0x000000ffffffffffull;
u64 read_only global bitmask41 = 0x000001ffffffffffull;
u64 read_only global bitmask42 = 0x000003ffffffffffull;
u64 read_only global bitmask43 = 0x000007ffffffffffull;
u64 read_only global bitmask44 = 0x00000fffffffffffull;
u64 read_only global bitmask45 = 0x00001fffffffffffull;
u64 read_only global bitmask46 = 0x00003fffffffffffull;
u64 read_only global bitmask47 = 0x00007fffffffffffull;
u64 read_only global bitmask48 = 0x0000ffffffffffffull;
u64 read_only global bitmask49 = 0x0001ffffffffffffull;
u64 read_only global bitmask50 = 0x0003ffffffffffffull;
u64 read_only global bitmask51 = 0x0007ffffffffffffull;
u64 read_only global bitmask52 = 0x000fffffffffffffull;
u64 read_only global bitmask53 = 0x001fffffffffffffull;
u64 read_only global bitmask54 = 0x003fffffffffffffull;
u64 read_only global bitmask55 = 0x007fffffffffffffull;
u64 read_only global bitmask56 = 0x00ffffffffffffffull;
u64 read_only global bitmask57 = 0x01ffffffffffffffull;
u64 read_only global bitmask58 = 0x03ffffffffffffffull;
u64 read_only global bitmask59 = 0x07ffffffffffffffull;
u64 read_only global bitmask60 = 0x0fffffffffffffffull;
u64 read_only global bitmask61 = 0x1fffffffffffffffull;
u64 read_only global bitmask62 = 0x3fffffffffffffffull;
u64 read_only global bitmask63 = 0x7fffffffffffffffull;
u64 read_only global bitmask64 = 0xffffffffffffffffull;

u32 read_only global bit1  = (1 << 0);
u32 read_only global bit2  = (1 << 1);
u32 read_only global bit3  = (1 << 2);
u32 read_only global bit4  = (1 << 3);
u32 read_only global bit5  = (1 << 4);
u32 read_only global bit6  = (1 << 5);
u32 read_only global bit7  = (1 << 6);
u32 read_only global bit8  = (1 << 7);
u32 read_only global bit9  = (1 << 8);
u32 read_only global bit10 = (1 << 9);
u32 read_only global bit11 = (1 << 10);
u32 read_only global bit12 = (1 << 11);
u32 read_only global bit13 = (1 << 12);
u32 read_only global bit14 = (1 << 13);
u32 read_only global bit15 = (1 << 14);
u32 read_only global bit16 = (1 << 15);
u32 read_only global bit17 = (1 << 16);
u32 read_only global bit18 = (1 << 17);
u32 read_only global bit19 = (1 << 18);
u32 read_only global bit20 = (1 << 19);
u32 read_only global bit21 = (1 << 20);
u32 read_only global bit22 = (1 << 21);
u32 read_only global bit23 = (1 << 22);
u32 read_only global bit24 = (1 << 23);
u32 read_only global bit25 = (1 << 24);
u32 read_only global bit26 = (1 << 25);
u32 read_only global bit27 = (1 << 26);
u32 read_only global bit28 = (1 << 27);
u32 read_only global bit29 = (1 << 28);
u32 read_only global bit30 = (1 << 29);
u32 read_only global bit31 = (1 << 30);
u32 read_only global bit32 = (1u << 31);

u64 read_only global bit33 = (1ull << 32);
u64 read_only global bit34 = (1ull << 33);
u64 read_only global bit35 = (1ull << 34);
u64 read_only global bit36 = (1ull << 35);
u64 read_only global bit37 = (1ull << 36);
u64 read_only global bit38 = (1ull << 37);
u64 read_only global bit39 = (1ull << 38);
u64 read_only global bit40 = (1ull << 39);
u64 read_only global bit41 = (1ull << 40);
u64 read_only global bit42 = (1ull << 41);
u64 read_only global bit43 = (1ull << 42);
u64 read_only global bit44 = (1ull << 43);
u64 read_only global bit45 = (1ull << 44);
u64 read_only global bit46 = (1ull << 45);
u64 read_only global bit47 = (1ull << 46);
u64 read_only global bit48 = (1ull << 47);
u64 read_only global bit49 = (1ull << 48);
u64 read_only global bit50 = (1ull << 49);
u64 read_only global bit51 = (1ull << 50);
u64 read_only global bit52 = (1ull << 51);
u64 read_only global bit53 = (1ull << 52);
u64 read_only global bit54 = (1ull << 53);
u64 read_only global bit55 = (1ull << 54);
u64 read_only global bit56 = (1ull << 55);
u64 read_only global bit57 = (1ull << 56);
u64 read_only global bit58 = (1ull << 57);
u64 read_only global bit59 = (1ull << 58);
u64 read_only global bit60 = (1ull << 59);
u64 read_only global bit61 = (1ull << 60);
u64 read_only global bit62 = (1ull << 61);
u64 read_only global bit63 = (1ull << 62);
u64 read_only global bit64 = (1ull << 63);
