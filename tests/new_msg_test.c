#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils.h"
#include "test_utils.h"

void test_new_msg() {
    struct msg * rqt = new_msg("RQT 78942\n");

    assert__(strcmp(rqt->type, "RQT") == 0) {
        fprintf(stderr, "rqt->type == %s\n", rqt->type);
    }
    assert__(strcmp(rqt->parameters[0], "78942") == 0) {
        fprintf(stderr, "rqt->parameters[0] == %s\n", rqt->parameters[0]);
    }

    struct msg * rqs = new_msg("RQS 78942 09OUT2015_20:00:00 A B C D E\n");

    assert__(strcmp(rqs->type, "RQS") == 0) {
        fprintf(stderr, "rqs->type == %s\n", rqs->type);
    }
    assert__(strcmp(rqs->parameters[0], "78942") == 0) { }
    assert__(strcmp(rqs->parameters[1], "09OUT2015_20:00:00") == 0) { }
    assert__(strcmp(rqs->parameters[2], "A") == 0) { }
    assert__(strcmp(rqs->parameters[3], "B") == 0) { }
    assert__(strcmp(rqs->parameters[4], "C") == 0) { }
    assert__(strcmp(rqs->parameters[5], "D") == 0) { }
    assert__(strcmp(rqs->parameters[6], "E") == 0) {
        fprintf(stderr, "rqs->parameters[6] == %s\n", rqs->parameters[6]);
    }

    free_msg(rqt);
    free_msg(rqs);
}
