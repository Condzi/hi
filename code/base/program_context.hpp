#pragma once

struct Program_Context {
  Arena* frame_arena;
  Arena* misc_arena;
};

extern Program_Context gContext;
