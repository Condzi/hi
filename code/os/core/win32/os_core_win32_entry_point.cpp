int
main() {
  bool run = gEntryPoint.startup();
  if (!run) {
    return -1;
  }

  f32 frame_begin = os_seconds_since_startup();
  f32 dt          = 0;
  while (run) {
    run = gEntryPoint.update(dt);
    gEntryPoint.render();

    f32 frame_end = os_seconds_since_startup();
    dt            = frame_end - frame_begin;
    frame_begin   = frame_end;
  }

  gEntryPoint.teardown();
}
