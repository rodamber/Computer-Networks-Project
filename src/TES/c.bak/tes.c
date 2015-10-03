#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tes.h"
#include "../utils.h"

#define IS_VALID_SID(X) 1

struct msg * parse_rqt_request(const char * const request) {
    struct msg * rqt = new_msg(request);

    if (rqt->n_parameters != 1) {
        return error_msg;
    }

    /* Check SID. */
    const char * SID = rqt->parameters[0];
    if (!IS_VALID_SID(SID)) {
        return error_msg;
    }

    return rqt;
}

struct msg * reply_rqs_request(const struct msg * const request) {
    static unsigned long QID = 0;

    /* Get quiz and its size. */
    const int  qnumber  = random_in_range(1, 5);
    const FILE *qstream = get_quiz(qnumber, "r");
    if(qstream == NULL) {
        return error_msg;
    }
    const int  qsize    = get_qsize(qstream);
    const char *qdata   = get_qdata(qstream);

    /* Get deadline. */
          time_t now      = time(null);
    const time_t deadline = now + QUIZ_SOLVING_INTERVAL * 3600;
    // Submission is due in QUIZ_SOLVING_INTERVAL days.

    /* Reply. */
    const char months[13][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                "JUL","AGO", "SEP", "OCT", "NOV", "DEC"};
    char * reply;
    asprintf(&reply, "%s %d %d%s%d_%d:%d:%d %d %s",
             "AWT",
             QID++,
             tm.tm_mday,
             months[tm.tm_mon],
             tm.tm_year + 1900,
             tm.tm_hour,
             tm.tm_min,
             tm.tm_sec);
    const struct msg * awt = new_msg(reply);
    free(reply);

    /* Save the transaction somewhere */
    save();

    return awt;

}

struct msg * parse_rqs_request(const char * const request) {
    struct msg * rqs = new_msg(request);
    return error_msg;
}


struct msg * parse_request(const char * const request) {
    if (strncmp(request, "RQT", REQUEST_TYPE_SIZE) == 0) {
        return parse_rqt_request(request);
    } else if (strncmp(request, "RQS", REQUEST_TYPE_SIZE) == 0) {
        return parse_rqs_request(request);
    } else {
        return error_msg;
    }
}

struct msg * reply_request(const struct msg * const request) {
    return error_msg;
}
