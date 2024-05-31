#include "../testify/test_assert.h"
#include <stdio.h>

int main() {
  printf("Hello World\n");
  assert(1 == 1);
  assert(1 == 2);
}

void test_basic_assert() {
  assert(1 == 1);
  assert(1 == 2);
}
