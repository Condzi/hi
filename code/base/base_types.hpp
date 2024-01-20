#pragma once

// External dependencies on libc
//
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <cstdint>

// Language layer -- keywords
//
#define local_persist static
#define global static
#define internal static
#define read_only constexpr

// Language layer -- memory operations
//
#define MemoryCopy(dst, src, size) memmove((dst), (src), (size))
#define MemorySet(dst, byte, len) memset((dst), (byte), (len))
#define MemoryCompare(a, b, len) memcmp((a), (b), (len))

// Language layer -- asserts
//
#define StaticAssert(x) static_assert((x))

#define Unused(x) (void)(x)

#define Trap() __debugbreak();

#define AssertAlways(x)                                                              \
  do {                                                                               \
    if (!(x)) {                                                                      \
      Trap();                                                                        \
    }                                                                                \
  } while (0)

#if !defined(NDEBUG)
#define Assert(x) AssertAlways((x))
#else
#define Assert(x) Unused((x))
#endif

#define NotImplemented Assert(!"Not implemented!")
#define InvalidPath Assert(!"Invalid code path taken!")

// Language layer -- misc helper macros
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
#define AsanUnpoisonMemoryRegion(addr, size)                                         \
  __asan_unpoison_memory_region((addr), (size))
#else
#define AsanPoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))
#define AsanUnpoisonMemoryRegion(addr, size) ((void)(addr), (void)(size))

#endif

// Common math types
//

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using f32 = float;
using f64 = double;

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
