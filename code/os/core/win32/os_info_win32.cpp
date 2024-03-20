
must_use OS_Memory_Stats
os_get_memory_stats() {
  HEAP_SUMMARY hs   = {.cb = sizeof(hs)};
  HANDLE       heap = GetProcessHeap();
  HeapSummary(heap, 0, &hs);

  return {
      .reserved = hs.cbReserved,
      .commited = hs.cbCommitted,
  };
}

must_use OS_Memory_Info
os_get_memory_info() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);

  return {
      .page_size = info.dwPageSize,
      .min_addr  = (mem64)info.lpMinimumApplicationAddress,
      .max_addr  = (mem64)info.lpMaximumApplicationAddress,
  };
}
