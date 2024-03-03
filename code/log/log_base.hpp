#pragma once

enum Log_Severity {
  LogSeverity_Debug   = 0,
  LogSeverity_Info    = 1,
  LogSeverity_Warning = 2,
  LogSeverity_Error   = 3,

  LogSeverity__count,
};
StaticAssert(LogSeverity__count == 4);

enum Log_Category {
  LogCategory_Engine,
  LogCategory_Game,

  LogCategory__count,
};
StaticAssert(LogCategory__count == 2);

// @Note: Keeping it small in order to make filtering fast.
//
struct Log_Header {
  u32 severity  : 2;
  u32 category  : 1;
  u32 timestamp : 24; // 10^-2s (or tens of milliseconds), max 46.5h
};

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbitfield-constant-conversion"
#pragma clang diagnostic ignored "-Wc++11-narrowing"
#endif
// Check if the timestamp is able to hold at least 24h values.
// We do x100 because the timestamp holds the time in tens of milliseconds.
//
StaticAssert(Log_Header {.timestamp = -1}.timestamp > (60 * 60 * 24) * 100);

#ifdef __clang__
#pragma clang diagnostic pop
#endif

global read_only u64 LOG_MAX_MESSAGE_COUNT = 4096;

// We want to store messages values separately from headers in order to avoid fragmentation
// of memory by strings. The overall structure looks like this:
//
//  [headers][    messages array    ][         messages values                ]
struct Log {
  Arena* arena;

  struct {
    Log_Header *v;
    u64         sz;
  } headers;

  struct {
    Str8 *v;
    u64   sz;
  } messages;
};

must_use global Log*
make_log(Arena* arena);

