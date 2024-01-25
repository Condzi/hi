#pragma once

struct Thread_Context {
  Arena* frame_arena;
};

extern thread_local Thread_Context tcx;
