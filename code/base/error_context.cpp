#pragma once
#include "all_inc.hpp"

must_use Error_Context *
error_context_init(Arena *arena) {
  Error_Context *res = arena_alloc<Error_Context>(arena);
  *res               = {.arena = arena};
  return res;
}

void
error_context_push(Error_Context *context, Str8 file, Str8 func, Str8 desc) {
  Error_Context_Node *node = arena_alloc<Error_Context_Node>(context->arena);
  node->file               = file;
  node->function           = func;
  node->desc               = desc;

  if (context->last) {
    node->prev          = context->last;
    context->last->next = node;
    context->last       = node;
  } else {
    // It's the first entry in the context.
    //
    context->first = context->last = node;
  }
}

void
error_context_pop(Error_Context *context) {
  if (context->first == context->last) {
    context->first = context->last = 0;
  } else {
    context->last->prev->next = 0;
    context->last             = context->last->prev;
  }
}
