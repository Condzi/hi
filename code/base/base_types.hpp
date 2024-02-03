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
#define global static
#define internal static
#define read_only constexpr
#define must_use [[nodiscard]]
#define no_return [[noreturn]]

// Language layer -- memory operations
//
#define MemoryCopy(dst, src, size) memmove((dst), (src), (size))
#define MemorySet(dst, byte, len) memset((dst), (byte), (len))
#define MemoryCompare(a, b, len) memcmp((a), (b), (len))

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
#define InvalidPath Assert(!"Invalid code path taken!")

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
#define C_LINKAGE_END }
#define C_LINKAGE extern "C"

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)

#define Glue_(A, B) A##B
#define Glue(A, B) Glue_(A, B)

#define Min(A, B) (((A) < (B)) ? (A) : (B))
#define Max(A, B) (((A) > (B)) ? (A) : (B))

#define ClampTop(A, X) Min(A, X)
#define ClampBot(X, B) Max(X, B)
#define Clamp(A, X, B) (((X) < (A)) ? (A) : ((X) > (B)) ? (B) : (X))

#define Compose64Bit(a, b) ((((U64)a) << 32) | ((U64)b));
#define AlignPow2(x, b) (((x) + (b)-1) & (~((b)-1)))
#define AlignDownPow2(x, b) ((x) & (~((b)-1)))
#define AlignPadPow2(x, b) ((0 - (x)) & ((b)-1))
#define IsPow2(x) ((x) != 0 && ((x) & ((x)-1)) == 0)
#define IsPow2OrZero(x) ((((x)-1) & (x)) == 0)

#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)
#define TB(n) (((u64)(n)) << 40)

#define Thousand(n) ((n) * 1000)
#define Million(n) ((n) * 1000000)
#define Billion(n) ((n) * 1000000000)

#define Swap(a, b)                                                                                 \
  do {                                                                                             \
    decltype(a) t__ = a;                                                                           \
    a               = b;                                                                           \
    b               = t__;                                                                         \
  } while (0)

// Conversion macros
//
#define PtrToU64(p) (u64)((p))
#define U64ToPtr(u) (void *)(u)

// ASAN
//
#if defined(__SANITIZE_ADDRESS__)
#define ASAN_ENABLED 1
#define NO_ASAN __declspec(no_sanitize_address)
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

#define AsanPoisonMemoryRegion(addr, size) __asan_poison_memory_region((addr), (size))
#define AsanUnpoisonMemoryRegion(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
#define AsanPoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
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

read_only global u64 MAX_U64 = UINT64_MAX;
read_only global u32 MAX_U32 = UINT32_MAX;
read_only global u16 MAX_U16 = UINT16_MAX;
read_only global u8  MAX_U8  = UINT8_MAX;

read_only global s64 MAX_S64 = INT64_MAX;
read_only global s32 MAX_S32 = INT32_MAX;
read_only global s16 MAX_S16 = INT16_MAX;
read_only global s8  MAX_S8  = INT8_MAX;

read_only global s64 MIN_S64 = INT64_MIN;
read_only global s32 MIN_S32 = INT32_MIN;
read_only global s16 MIN_S16 = INT16_MIN;
read_only global s8  MIN_S8  = INT8_MIN;

read_only global f32 EPS_F32 = FLT_EPSILON;

read_only global f32 PI   = 3.141592653589793f;
read_only global f32 PI_2 = 1.57079632679489661923f; // PI/2
read_only global f32 PI_6 = 0.52359877559f;          // PI/6

// Bits
//

read_only global u32 bitmask1  = 0x00000001;
read_only global u32 bitmask2  = 0x00000003;
read_only global u32 bitmask3  = 0x00000007;
read_only global u32 bitmask4  = 0x0000000f;
read_only global u32 bitmask5  = 0x0000001f;
read_only global u32 bitmask6  = 0x0000003f;
read_only global u32 bitmask7  = 0x0000007f;
read_only global u32 bitmask8  = 0x000000ff;
read_only global u32 bitmask9  = 0x000001ff;
read_only global u32 bitmask10 = 0x000003ff;
read_only global u32 bitmask11 = 0x000007ff;
read_only global u32 bitmask12 = 0x00000fff;
read_only global u32 bitmask13 = 0x00001fff;
read_only global u32 bitmask14 = 0x00003fff;
read_only global u32 bitmask15 = 0x00007fff;
read_only global u32 bitmask16 = 0x0000ffff;
read_only global u32 bitmask17 = 0x0001ffff;
read_only global u32 bitmask18 = 0x0003ffff;
read_only global u32 bitmask19 = 0x0007ffff;
read_only global u32 bitmask20 = 0x000fffff;
read_only global u32 bitmask21 = 0x001fffff;
read_only global u32 bitmask22 = 0x003fffff;
read_only global u32 bitmask23 = 0x007fffff;
read_only global u32 bitmask24 = 0x00ffffff;
read_only global u32 bitmask25 = 0x01ffffff;
read_only global u32 bitmask26 = 0x03ffffff;
read_only global u32 bitmask27 = 0x07ffffff;
read_only global u32 bitmask28 = 0x0fffffff;
read_only global u32 bitmask29 = 0x1fffffff;
read_only global u32 bitmask30 = 0x3fffffff;
read_only global u32 bitmask31 = 0x7fffffff;
read_only global u32 bitmask32 = 0xffffffff;

read_only global u64 bitmask33 = 0x00000001ffffffffull;
read_only global u64 bitmask34 = 0x00000003ffffffffull;
read_only global u64 bitmask35 = 0x00000007ffffffffull;
read_only global u64 bitmask36 = 0x0000000fffffffffull;
read_only global u64 bitmask37 = 0x0000001fffffffffull;
read_only global u64 bitmask38 = 0x0000003fffffffffull;
read_only global u64 bitmask39 = 0x0000007fffffffffull;
read_only global u64 bitmask40 = 0x000000ffffffffffull;
read_only global u64 bitmask41 = 0x000001ffffffffffull;
read_only global u64 bitmask42 = 0x000003ffffffffffull;
read_only global u64 bitmask43 = 0x000007ffffffffffull;
read_only global u64 bitmask44 = 0x00000fffffffffffull;
read_only global u64 bitmask45 = 0x00001fffffffffffull;
read_only global u64 bitmask46 = 0x00003fffffffffffull;
read_only global u64 bitmask47 = 0x00007fffffffffffull;
read_only global u64 bitmask48 = 0x0000ffffffffffffull;
read_only global u64 bitmask49 = 0x0001ffffffffffffull;
read_only global u64 bitmask50 = 0x0003ffffffffffffull;
read_only global u64 bitmask51 = 0x0007ffffffffffffull;
read_only global u64 bitmask52 = 0x000fffffffffffffull;
read_only global u64 bitmask53 = 0x001fffffffffffffull;
read_only global u64 bitmask54 = 0x003fffffffffffffull;
read_only global u64 bitmask55 = 0x007fffffffffffffull;
read_only global u64 bitmask56 = 0x00ffffffffffffffull;
read_only global u64 bitmask57 = 0x01ffffffffffffffull;
read_only global u64 bitmask58 = 0x03ffffffffffffffull;
read_only global u64 bitmask59 = 0x07ffffffffffffffull;
read_only global u64 bitmask60 = 0x0fffffffffffffffull;
read_only global u64 bitmask61 = 0x1fffffffffffffffull;
read_only global u64 bitmask62 = 0x3fffffffffffffffull;
read_only global u64 bitmask63 = 0x7fffffffffffffffull;
read_only global u64 bitmask64 = 0xffffffffffffffffull;

read_only global u32 bit1  = (1 << 0);
read_only global u32 bit2  = (1 << 1);
read_only global u32 bit3  = (1 << 2);
read_only global u32 bit4  = (1 << 3);
read_only global u32 bit5  = (1 << 4);
read_only global u32 bit6  = (1 << 5);
read_only global u32 bit7  = (1 << 6);
read_only global u32 bit8  = (1 << 7);
read_only global u32 bit9  = (1 << 8);
read_only global u32 bit10 = (1 << 9);
read_only global u32 bit11 = (1 << 10);
read_only global u32 bit12 = (1 << 11);
read_only global u32 bit13 = (1 << 12);
read_only global u32 bit14 = (1 << 13);
read_only global u32 bit15 = (1 << 14);
read_only global u32 bit16 = (1 << 15);
read_only global u32 bit17 = (1 << 16);
read_only global u32 bit18 = (1 << 17);
read_only global u32 bit19 = (1 << 18);
read_only global u32 bit20 = (1 << 19);
read_only global u32 bit21 = (1 << 20);
read_only global u32 bit22 = (1 << 21);
read_only global u32 bit23 = (1 << 22);
read_only global u32 bit24 = (1 << 23);
read_only global u32 bit25 = (1 << 24);
read_only global u32 bit26 = (1 << 25);
read_only global u32 bit27 = (1 << 26);
read_only global u32 bit28 = (1 << 27);
read_only global u32 bit29 = (1 << 28);
read_only global u32 bit30 = (1 << 29);
read_only global u32 bit31 = (1 << 30);
read_only global u32 bit32 = (1u << 31);

read_only global u64 bit33 = (1ull << 32);
read_only global u64 bit34 = (1ull << 33);
read_only global u64 bit35 = (1ull << 34);
read_only global u64 bit36 = (1ull << 35);
read_only global u64 bit37 = (1ull << 36);
read_only global u64 bit38 = (1ull << 37);
read_only global u64 bit39 = (1ull << 38);
read_only global u64 bit40 = (1ull << 39);
read_only global u64 bit41 = (1ull << 40);
read_only global u64 bit42 = (1ull << 41);
read_only global u64 bit43 = (1ull << 42);
read_only global u64 bit44 = (1ull << 43);
read_only global u64 bit45 = (1ull << 44);
read_only global u64 bit46 = (1ull << 45);
read_only global u64 bit47 = (1ull << 46);
read_only global u64 bit48 = (1ull << 47);
read_only global u64 bit49 = (1ull << 48);
read_only global u64 bit50 = (1ull << 49);
read_only global u64 bit51 = (1ull << 50);
read_only global u64 bit52 = (1ull << 51);
read_only global u64 bit53 = (1ull << 52);
read_only global u64 bit54 = (1ull << 53);
read_only global u64 bit55 = (1ull << 54);
read_only global u64 bit56 = (1ull << 55);
read_only global u64 bit57 = (1ull << 56);
read_only global u64 bit58 = (1ull << 57);
read_only global u64 bit59 = (1ull << 58);
read_only global u64 bit60 = (1ull << 59);
read_only global u64 bit61 = (1ull << 60);
read_only global u64 bit62 = (1ull << 61);
read_only global u64 bit63 = (1ull << 62);
read_only global u64 bit64 = (1ull << 63);
