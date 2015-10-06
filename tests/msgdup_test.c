#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils.h"
#include "test_utils.h"

void test_msg_dup() {
    struct msg * rqt = new_msg("RQT 78942\n");
    struct msg * dup = msgdup(rqt);

    char * rqt_str = msg_to_string(rqt);
    char * dup_str = msg_to_string(dup);

    assert__(strcmp(rqt_str, dup_str) == 0) {
    }
    assert__(strcmp(dup_str, "RQT 78942\n") == 0) {
    }

    free_msg(rqt);
    free_msg(dup);
    free(dup_str);
}
