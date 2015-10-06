#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils.h"
#include "test_utils.h"

void test_template(char *type, char **parameters, int n_parameters, char *str_to_match) {
    const struct msg test_msg = {
        .type = type,
        .parameters = parameters,
        .n_parameters = n_parameters
    };

    char * msg_str = msg_to_string(&test_msg);

    assert__(msg_str[strlen(msg_str)-1] == '\n') {
        printf("msg_str[strlen(msg_str)-1] = %c\n", msg_str[strlen(msg_str)-1]);
    }
    assert__(strcmp(msg_str, str_to_match) == 0) {
        printf("msg_str: %s\n", msg_str);
    }
    free(msg_str);
}

void test_msg_to_string() {
    test_template("RQT", (char *[]){"SID"}, 1, "RQT SID\n");
    test_template("RQS", (char *[]){"SID", "QID", "V1", "V2", "V3", "V4", "V5"}, 7,
                  "RQS SID QID V1 V2 V3 V4 V5\n");
}
