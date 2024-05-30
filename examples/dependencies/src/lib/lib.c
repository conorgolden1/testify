#include "../../testify/test_assert.h"
#include <stdint.h>

int does_something() {
    return 1;
}


void test_does_something() {
    assert(does_something() == does_something());
}
