#include <stdio.h>

#define TEST "Testing: "
#define PASS "Passed: "

void test_read_bytes();

int main() {
    puts(TEST "read_bytes");
    test_read_bytes();
    puts(PASS "read_bytes");

    return 0;
}
