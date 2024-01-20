#include "base/base_types.hpp"

int main() {
  char *c = (char *)malloc(10);
  std::puts("Hello, World!");
  //  Trigger ASAN
  c[10] = 123;
  return 0;
}
