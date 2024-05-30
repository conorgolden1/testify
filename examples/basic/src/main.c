#include "../testify/test_assert.h"
#include <stdio.h>

int main() {
    printf("DO NOT PRINT\n");
}

void testFunc() {
    assert(1 == 1);
    assert(1 == 2);
}
