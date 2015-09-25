#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"


int fail(const char * const msg) {
    perror(msg);
    return EXIT_FAILURE;
}

int read_bytes(const int fd, const unsigned nbytes, char * buf) {
    unsigned bytes_read = 0;
    while (bytes_read < nbytes) {
        const int res = read(fd, buf + bytes_read, nbytes - bytes_read);
        if (res < 1) return res;
        bytes_read += res;
    }
    return 0;
}

const struct msg error_reply = { .type = "ERR" };

char * msg_to_string(const struct msg * const m) {
    char * buffer = malloc(sizeof(char) * REQUEST_MAX_SIZE);
    buffer[0]     = '\0';

    // The length of the buffer is tracked to make the algorithm run in linear time.
    int len = 0;


    // Concatenate message type and then concatenate all its parameters, one by
    // one, space separated.
    strncat(buffer, m->type, REQUEST_MAX_SIZE);
    len += strlen(m->type);

    strncat(buffer, " ", REQUEST_MAX_SIZE - len);
    ++len;

    for (unsigned i = 0; i < m->n_parameters; ++i) {
        strncat(buffer, m->parameters[i], REQUEST_MAX_SIZE - len);
        len += strlen(m->parameters[i]);

        strncat(buffer, " ", REQUEST_MAX_SIZE - len);
        ++len;
    }
    buffer[len-1] = '\n'; // Convert last space to a new line.
    return buffer;
}
