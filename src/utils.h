#ifndef __UTILS_H__
#define __UTILS_H__


#include <string.h>


#define FAIL        return fail("error")
#define MAY_FAIL(X) if ((X) == -1) FAIL

#define PORT         59000 + GROUP_NUMBER
#define GROUP_NUMBER 9

#define REQUEST_MAX_SIZE  10 * 1024
#define REQUEST_TYPE_SIZE 3


// Helper function to indicate failure. Beware that this may not be a clean a
// way to exit the program, e.g., if you need to free resources.
int fail(const char * const msg) {
    perror(msg);
    return EXIT_FAILURE;
}

// Attempts to read nbytes bytes of data from the object referenced by the
// descriptor fd into the buffer pointed to by buf. If successful, returns 0.
// Otherwise, returns -1 and the global variable errno is set to indicate the
// error.
int read_bytes(const int fd, const unsigned nbytes, char * buf) {
    unsigned bytes_read = 0;
    while (bytes_read < nbytes) {
        const int res = read(fd, buf + bytes_read, nbytes - bytes_read);
        if (res < 1) return res;
        bytes_read += res;
    }
    return 0;
}

// Basic representation of a client-server request/reply.
struct msg {
    char * type;
    char ** parameters;
    unsigned n_parameters; // Number of parameters.
};

const struct msg error_reply = { .type = "ERR" };

// Converts a struct msg to a string according to the project protocol.
// So if msg has type t and parameters p1, p2, ..., pn, the resulting string
// will be "t p1 p2 ... pn".
// The returning string will need to be freed.
char * msg_to_string(const struct msg * const m) {
    char * buffer = malloc(sizeof(char) * REQUEST_MAX_SIZE);
    buffer[0] = '\0';

    strncat(buffer, m->type, REQUEST_MAX_SIZE);
    for (unsigned i = 0; i < m->n_parameters; ++i) {
        strncat(buffer, m->parameters[i], REQUEST_MAX_SIZE - strlen(buffer));
    }
    return buffer;
}


#endif // __UTILS_H__
