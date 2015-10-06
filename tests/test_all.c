#include <stdio.h>

#define TEST "[Testing] "
#define PASS "[Passed]  "

void test_read_bytes();
void test_msg_to_string();
void test_new_msg();
void test_msg_dup();

int main() {
    puts(TEST "read_bytes");
    test_read_bytes();
    puts(PASS "read_bytes");

    puts(TEST "msg_to_string");
    test_msg_to_string();
    puts(PASS "msg_to_string");

    puts(TEST "new_msg");
    test_new_msg();
    puts(PASS "new_msg");

    puts(TEST "msg_dup");
    test_msg_dup();
    puts(PASS "msg_dup");

    return 0;
}
