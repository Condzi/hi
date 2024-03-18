#pragma once

struct Debug_Console {
  bool is_open;
  u64  logs_max;
} global gDbgConsole;

void tools_update();
