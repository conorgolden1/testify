#include "../testify/test_assert.h"
#include "lib/lib.h"
#include <stdint.h>

int main() {
   return 0;
}

void test_func() {
  assert(1 == does_something());
  assert(2 == does_something());
}
