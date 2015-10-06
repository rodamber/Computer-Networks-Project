#include <ctype.h>
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

        if (res == -1) return -1;
        if (res ==  0) return bytes_read;

        bytes_read += res;
    }
    return bytes_read;
}

char *strupr(char *str) {
    char c; int i;
    for (c = str[0], i = 0; c; c = str[++i]) {
        str[i] = toupper(str[i]);
    }
    return str;
}

const struct msg error_msg = { .type = "ERR\n" };

struct msg * new_msg(const char * const str) {
    struct msg * m = msgdup(&error_msg);
    free(m->type);

    char *string = strdup(str);
    char *token;

    if ((token = strsep(&string, " \n")) != NULL) {
        m->type = strdup(token);
    }

    int i = 0;
    while ((token = strsep(&string, " \n")) != NULL && strcmp(token, "") != 0) {
        m->parameters[i++] = strdup(token);
    }
    m->n_parameters = i;

    free(string);
    return m;
}

struct msg * msgdup(const struct msg * const m) {
    struct msg * dup  = malloc(sizeof(struct msg));
    dup->type         = strdup(m->type);
    dup->parameters   = malloc(sizeof(char *) * m->n_parameters);
    dup->n_parameters = m->n_parameters;

    for (unsigned i = 0; i < m->n_parameters; ++i) {
        dup->parameters[i] = strdup(m->parameters[i]);
    }
    return dup;
}

void free_msg(struct msg * tofree) {
    free(tofree->type);
    for (unsigned i = 0; i < tofree->n_parameters; ++i) {
        free(tofree->parameters[i]);
    }
    free(tofree->parameters);
    free(tofree);
}

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
