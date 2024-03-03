#pragma once

struct Program_Context {
  Arena         *frame_arena;
  Arena         *misc_arena;
  Error_Context *error_context;
  struct Log    *log;
};

global Program_Context gContext;
