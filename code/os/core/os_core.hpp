#pragma once

#include "os_info.hpp"

// OS Layer initialization (implemented per-os)
//

void
os_init(int argc, char const *argv[]);

// Memory Allocation
//

must_use void *
os_alloc(u64 sz, u64 base_addr);

void
os_free(void *ptr, u64 sz);

// Time
//

must_use u64
os_now_us();

must_use u64
os_us_since_startup();

must_use f32
os_seconds_since_startup();

// System Information
//

must_use u64
os_page_size();

// Misc
//

void
os_debug_message(Str8 msg);

// @Note: s64 because HRESULT is a long
must_use Str8
os_error_id_to_user_message(s64 error);
