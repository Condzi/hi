#pragma once

struct Program_Context {
  Arena         *frame_arena;
  Arena         *misc_arena;
  Error_Context *error_context;
};

global Program_Context gContext;
