#pragma once

struct Debug_Console {
  bool is_open;
  u64  logs_max;
} global gDbgConsole;

struct Debug_Memory_Consumption {
  bool is_open;
} global gDbgMemoryConsumption;

void
tools_update();

void
tools_init();
