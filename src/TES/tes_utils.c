#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tes_utils.h"
#include "../utils.h"

struct msg parse_rqt_request(const char * const request) {
    (void) request;
    // Check if SID is valid.

    // Get a quiz and its size

    // Get the deadline

    // Get the QID

    // Save the transaction somewhere

    return error_reply;
}

struct msg parse_request(const char * const request) {
    char * type = malloc(sizeof(char) * REQUEST_TYPE_SIZE);
    sscanf(request, "%" STR(REQUEST_TYPE_SIZE) "[^ ]", type);

    if (strncmp(type, "RQT", REQUEST_TYPE_SIZE) == 0) {
        return parse_rqt_request(request + REQUEST_TYPE_SIZE + 1);
    } else if (strncmp(type, "RQS", REQUEST_TYPE_SIZE) == 0) {
        return parse_rqs_request(request + REQUEST_TYPE_SIZE + 1);
    } else {
        return error_reply;
    }
}
