#include <stdio.h>

#define TEST "[Testing] "
#define PASS "[Passed]  "

void test_read_bytes();
void test_msg_to_string();

int main() {
    puts(TEST "read_bytes");
    test_read_bytes();
    puts(PASS "read_bytes");

    puts(TEST "msg_to_string");
    test_msg_to_string();
    puts(PASS "msg_to_string");

    return 0;
}
