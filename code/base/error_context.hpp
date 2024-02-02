#pragma once

// Error Context is designed to provide additional information for errors
// that occur in higher layers (i.e. layers above `base`, such as `gfx` and `game`).
//

struct Error_Context_Node {
  Error_Context_Node *next;
  Error_Context_Node *prev;
  Str8                file;
  Str8                function;
  Str8                desc;
};

struct Error_Context {
  Arena              *arena;
  Error_Context_Node *first;
  Error_Context_Node *last;
};

must_use global Error_Context *
error_context_init(Arena *arena);

global void
error_context_push(Error_Context *context, Str8 file, Str8 func, Str8 desc);

global void
error_context_pop(Error_Context *context);

#define ErrorContext(fmt, ...)                                                       \
  Defer { error_context_pop(gContext.error_context); };                              \
  do {                                                                               \
    Str8 file__ = __FILE__ ":" Stringify(__LINE__) ""_s8;                            \
    Str8 func__ = str8_cstr(__FUNCTION__);                                           \
    Str8 desc__ = str8_sprintf(gContext.error_context->arena, fmt, __VA_ARGS__);     \
    error_context_push(gContext.error_context, file__, func__, desc__);              \
  } while (0)

#define ErrorIf(cnd)                                                                 \
  do {                                                                               \
    if ((cnd)) {                                                                     \
      Str8 file__ = __FILE__ ":" Stringify(__LINE__) ""_s8;                          \
      Str8 func__ = str8_cstr(__FUNCTION__);                                         \
      Str8 cnd__  = Stringify(cnd) ""_s8;                                            \
      os_hard_fail(file__, func__, cnd__);                                           \
    }                                                                                \
  } while (0)
