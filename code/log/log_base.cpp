
must_use Log *
make_log(Arena *arena) {
  Assert(arena);
  Log *log   = (Log *)arena_alloc(arena, sizeof(Log), 8);
  log->arena = arena;
  log->rewind_pos = arena->curr_pos;
  return log;
}

void
log_clear(Log *log) {
  Assert(log);
  arena_rewind(log->arena, log->rewind_pos);
  log->curr = 0;
}

must_use internal u32
log_make_timestamp() {
  u64 const microseconds     = os_us_since_startup();
  u64 const ten_milliseconds = 10'000; // 10ms = 10'000uc
  return (microseconds + ten_milliseconds / 2) / ten_milliseconds;
}

global f32
log_timestamp_to_seconds(u32 timestamp) {
  f32 const ts  = (f32)timestamp;
  f32 const sec = 100.f; // 1 sec = 100 tens of millieeconds = 1000 ms
  return ts / sec;
}

template <typename... TArgs>
internal void
log_add(Log *log, Log_Severity severity, Log_Category category, char const *fmt, TArgs... args) {
  Assert(log);
  if (log->curr == LOG_CAPACITY) {
    log_clear(log);
  }

  u64 const idx     = log->curr;
  log->headers[idx] = {
      .severity  = severity,
      .category  = category,
      .timestamp = log_make_timestamp(),
  };
  log->messages[idx] = str8_sprintf(log->arena, fmt, args...);

  // os_debug_message(str8_sprintf(gContext.frame_arena, "%.2f| %S\n", log_timestamp_to_seconds(log->headers[idx].timestamp), log->messages[idx]));

  log->curr++;
}
