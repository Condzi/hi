#pragma once
#include "all_inc.hpp"

must_use global f32
os_seconds_since_startup() {
  return ((f32)os_us_since_startup() / 1.0e6f);
}
