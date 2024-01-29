#pragma once

// Error Context is designed to provide additional information for errors
// that occur in higher layers (i.e. layers above `base`, such as `gfx` and `game`).
//

struct Error_Context_Node {
  Error_Context_Node *next;
  Error_Context_Node* prev;
  Str8                info;
};

struct Error_Context {
  Arena              *arena;
  Error_Context_Node *first;
  Error_Context_Node *last;
};

must_use global Error_Context *
error_context_init(Arena *arena);

global void
error_context_push(Error_Context *context, Str8 entry);

global void
error_context_pop(Error_Context *context);

#define ErrorContext(fmt, ...)                                                       \
  Defer { error_context_pop(gContext.error_context); };                              \
  do {                                                                               \
    Str8 entry__ = str8_sprintf(                                                     \
        gContext.error_context->arena,                                               \
        __FILE__ ":" Stringify(__LINE__) " -- " __FUNCTION__ " -- " fmt,             \
        __VA_ARGS__);                                                                \
    error_context_push(gContext.error_context, entry__);                             \
  } while (0)
