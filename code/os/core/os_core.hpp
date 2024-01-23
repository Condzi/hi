#pragma once

// OS Layer initialization (implemented per-os)
//

global void
os_init(int argc, char const *argv[]);

// Memory Allocation
//

must_use global void *
os_alloc(u64 sz, u64 base_addr);

global void
os_free(void *ptr, u64 sz);

// Time
//

must_use global u64
os_now_us();
