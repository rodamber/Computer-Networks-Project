#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // open

#include "../src/utils.h"

void test_read_bytes() {
    const int file = open("./read_bytes_test.txt", O_RDONLY);
    assert(file != -1);

    const int bytes = 10;
    char *buffer = malloc(sizeof(char) * 256);

    assert(read_bytes(file, -1, buffer) == -1);

    assert(read_bytes(file, bytes, buffer) == 0);
    buffer[bytes] = 0;
    assert(strcmp(buffer, "\"It's no u") == 0);

    assert(read_bytes(file, bytes, buffer + bytes) == 0);
    buffer[2 * bytes] = 0;
    assert(strcmp(buffer, "\"It's no use going b") == 0);

    free(buffer);
}
