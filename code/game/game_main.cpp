// HPP
//
#include "base/base_inc.hpp"
#include "os/os_inc.hpp"

// CPP
//
#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

int
main(int argc, char const *argv[]) {
  os_init(argc, argv);

  void *mem = os_alloc(100, TB(2));
  os_free(mem, 100);

  u64 now = os_now_us();
  Unused(now);
  
  return 0;
}
