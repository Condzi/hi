// Globals
//

global u64 w32_us_res = 0;

// Public interface definition
//

global void
os_init(int argc, char const *argv[]) {
  // Microsecond counter resolution
  //
  LARGE_INTEGER large_int_resolution;
  AssertAlways(QueryPerformanceFrequency(&large_int_resolution));
  w32_us_res = (u64)large_int_resolution.QuadPart;

  // Parse launch options, if any.
  //
  if (argc > 1) {
    Str8 *opts_raw = arena_alloc_array<Str8>(tcx.frame_arena, (u64)(argc - 1));
    for (int i = 1; i < argc; i++) {
      opts_raw[i - 1] = str8_cstr(argv[i]);
    }
    parse_launch_opts(opts_raw, (u64)(argc - 1));
  }
}

// Memory Allocation
//

must_use global void *
os_alloc(u64 sz, u64 base_addr) {
  void *result = VirtualAllocEx(GetCurrentProcess(),
                                (PVOID)base_addr,
                                sz,
                                MEM_RESERVE | MEM_COMMIT,
                                PAGE_READWRITE);

  return result;
}

global void
os_free(void *ptr, u64 sz) {
  // Win32 requires passing 0 as size.
  //
  Unused(sz);

  VirtualFree(ptr, 0, MEM_RELEASE);
}

// Time
//

must_use global u64
os_now_us() {
  u64           res = 0;
  LARGE_INTEGER large_int_counter;
  if (QueryPerformanceCounter(&large_int_counter)) {
    res = (large_int_counter.QuadPart * Million(1)) / w32_us_res;
  }
  return res;
}

// System Information
//

must_use global u64
os_page_size() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);

  return info.dwPageSize;
}

// Misc
//

void
os_debug_message(Str8 msg) {
  if (IsDebuggerPresent()) {
    Str16 out = str16_from_8(tcx.frame_arena, msg);
    OutputDebugStringW((LPCWSTR)out.v);
  }
}
