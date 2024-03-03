#pragma once

struct OS_Memory_Stats {
  u64 reserved;
  u64 commited;
};

must_use OS_Memory_Stats
os_get_memory_stats();

struct OS_Memory_Info {
  u64 page_size;
  mem64 min_addr;
  mem64 max_addr;
};

must_use OS_Memory_Info 
os_get_memory_info();
