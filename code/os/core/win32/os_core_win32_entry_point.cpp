int
main() {
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

    t1 = os_seconds_since_startup();
    dt = t1 - t0;
    t0 = t1;
  }

  gEntryPoint.teardown();
  return 0;
}
