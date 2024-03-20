#include "os_info_win32.cpp"

// Globals
//

global u64 w32_us_res = 0;

// Public interface definition
//

void
os_init() {
  // Microsecond counter resolution
  //
  LARGE_INTEGER large_int_resolution;
  AssertAlways(QueryPerformanceFrequency(&large_int_resolution));
  w32_us_res = (u64)large_int_resolution.QuadPart;

  w32_startup_time = os_now_us();

  // Parse launch options, if any.
  //
  /*
  if (argc > 1) {
    Str8 *opts_raw = arena_alloc_array<Str8>(gContext.frame_arena, (u64)(argc - 1));
    for (int i = 1; i < argc; i++) {
      opts_raw[i - 1] = str8_cstr(argv[i]);
    }
    parse_launch_opts(opts_raw, (u64)(argc - 1));
  }
  */
}

// Memory Allocation
//

must_use void *
os_alloc(u64 sz, u64 base_addr) {
  void *result = VirtualAlloc((PVOID)base_addr, sz, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  Assert(result);
  return result;
}

void
os_free(void *ptr, u64 sz) {
  // Win32 requires passing 0 as size.
  //
  Unused(sz);

  VirtualFree(ptr, 0, MEM_RELEASE);
}

// Time
//

must_use u64
os_now_us() {
  u64           res = 0;
  LARGE_INTEGER large_int_counter;
  if (QueryPerformanceCounter(&large_int_counter)) {
    res = (large_int_counter.QuadPart * (s64)Million(1)) / w32_us_res;
  }
  return res;
}

must_use u64
os_us_since_startup() {
  return os_now_us() - w32_startup_time;
}

// System Information
//

must_use u64
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
    Str16 out = str16_from_8(gContext.frame_arena, msg);
    OutputDebugStringW((LPCWSTR)out.v);
  }
}

must_use Str8
os_error_id_to_user_message(s64 error) {
  HRESULT hr = (HRESULT)error;

  if (SUCCEEDED(hr)) {
    return "Success"_s8;
  }

  if (hr == DXGI_ERROR_DEVICE_REMOVED) {
    hr = gD3d.device->GetDeviceRemovedReason();
  }

  LPWSTR buff = 0;
  DWORD  size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                  FORMAT_MESSAGE_IGNORE_INSERTS,
                              0,
                              (DWORD)hr,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)&buff,
                              0,
                              0);
  Defer { LocalFree(buff); };

  Str8 err8  = str8_from_16(gContext.frame_arena, str16((u16 *)buff, size));
  Str8 final = str8_sprintf(gContext.frame_arena, "0x%lX: %S", hr, err8);
  return final;
}
