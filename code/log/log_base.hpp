#pragma once

enum Log_Severity : u32 {
  LogSeverity_Debug   = 0,
  LogSeverity_Info    = 1,
  LogSeverity_Warning = 2,
  LogSeverity_Error   = 3,

  LogSeverity__count,
};
StaticAssert(LogSeverity__count == 4);

enum Log_Category : u32 {
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

global read_only u64 LOG_CAPACITY = 4096;

// We want to store messages values separately from headers in order to avoid fragmentation
// of memory by strings. The overall structure looks like this:
//
//  [headers][    messages array    ][         messages values                ]
struct Log {
  Arena* arena;
  u64    rewind_pos;

  u64                             curr;
  aligned_to_word_size Log_Header headers[LOG_CAPACITY];
  aligned_to_word_size Str8       messages[LOG_CAPACITY];
};

must_use global Log*
make_log(Arena* arena);

global void
log_clear(Log* log);

must_use internal u32
log_make_timestamp();

must_use global f32
log_timestamp_to_seconds(u32 timestamp);

template <typename... TArgs>
internal void
log_add(Log *log, Log_Severity severity, Log_Category category, char const *fmt, TArgs... args);

//
// Engine Macros
//

#define LogEng_Debug(fmt, ...)                                                                     \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Debug, LogCategory_Engine, fmt, __VA_ARGS__)                 \
  } while (false)

#define LogEng_Info(fmt, ...)                                                                      \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Info, LogCategory_Engine, fmt, __VA_ARGS__)                  \
  } while (false)

#define LogEng_Warn(fmt, ...)                                                                      \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Warning, LogCategory_Engine, fmt, __VA_ARGS__)               \
  } while (false)

#define LogEng_Err(fmt, ...)                                                                       \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Error, LogCategory_Engine, fmt, __VA_ARGS__)                 \
  } while (false)

//
// Game Macros
//

#define LogGame_Debug(fmt, ...)                                                                    \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Debug, LogCategory_Game, fmt, __VA_ARGS__)                   \
  } while (false)

#define LogGame_Info(fmt, ...)                                                                     \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Info, LogCategory_Game, fmt, __VA_ARGS__)                    \
  } while (false)

#define LogGame_Warn(fmt, ...)                                                                     \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Warning, LogCategory_Game, fmt, __VA_ARGS__)                 \
  } while (false)

#define LogGame_Err(fmt, ...)                                                                      \
  do {                                                                                             \
    log_add(gContext.log, LogSeverity_Error, LogCategory_Game, fmt, __VA_ARGS__)                   \
  } while (false)
