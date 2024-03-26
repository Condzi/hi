internal void
tools_system_init() {
  LogEng_Debug("tools_system_init");
  tools_init();
}

internal void
tools_system_shutdown() {
  LogEng_Debug("tools_system_shutdown");
}

internal void
tools_system_update(f32 dt) {
  Unused(dt);

  if (kb_state(KB_Debug1).pressed) {
    gDbgConsole.is_open = !gDbgConsole.is_open;
  }

  if (kb_state(KB_Debug2).pressed) {
    gDbgMemoryConsumption.is_open = !gDbgMemoryConsumption.is_open;
  }

  if (kb_state(KB_Debug3).pressed) {
    gfx_set_vsync(!gfx_is_vsync_enabled());
  }

  tools_update();
}

must_use Sys_Logic
tools_system() {
  return {
      .type     = Sys_Type_Tools,
      .init     = tools_system_init,
      .shutdown = tools_system_shutdown,
      .update   = tools_system_update,
  };
}
