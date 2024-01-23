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

  // @ToDo: parse CMD
  //
  Unused(argc);
  Unused(argv);
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
