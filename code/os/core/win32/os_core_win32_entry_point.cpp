int
main() {
  // Lowest level of app initialization.
  //
  gContext.frame_arena = make_arena(false);
  gContext.misc_arena  = make_arena(true);
  gContext.log         = make_log(make_arena(false));

  Arena *error_context_arena = make_arena(false);
  gContext.error_context     = error_context_init(error_context_arena);
  // Error context is put into scratch buffer, cleared every frame.
  Scratch_Buffer error_context_scratch = scratch_begin(error_context_arena);

  // Higher level app initialization
  //
  bool run = gEntryPoint.startup();
  if (!run) {
    // @ToDo: Save logs or something?
    //
    return 0;
  }

  f32 t0 = os_seconds_since_startup();
  f32 t1 = t0;
  f32 dt = 0;
  while (run) {
    run = gEntryPoint.loop(dt);

    // Frame cleanup
    //
    arena_clear(gContext.frame_arena);
    scratch_end(&error_context_scratch);
    gContext.error_context->first = gContext.error_context->last = 0;

    t1 = os_seconds_since_startup();
    dt = t1 - t0;
    t0 = t1;
  }

  gEntryPoint.teardown();
  return 0;
}
