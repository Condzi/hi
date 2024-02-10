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

must_use global u64
os_us_since_startup();

must_use global f32
os_seconds_since_startup();

// System Information
//

must_use global u64
os_page_size();

// Misc
//

void
os_debug_message(Str8 msg);
