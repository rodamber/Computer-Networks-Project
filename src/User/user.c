#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../utils.h"

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define DEBUG_PRINT(fmt, ...)                                       \
    do { if (DEBUG_TEST) fprintf(stderr, fmt, __VA_ARGS__); } while (0)




#define QID_SIZE 24
#define SID_SIZE 5

#define DEFAULT_UDP_PORT 58009
#define DEFAULT_TCP_PORT 59000

int valid_sid(const char * sid) {
    if (strlen(sid) != SID_SIZE) {
        return 0;
    }
    for (int i = 0; i < SID_SIZE; ++i) {
        if (sid[i] < '0' || '9' < sid[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {

    /* Parse command line arguments */
    int option;

    char * sid = argv[1];
    if (sid == NULL) {
        printf("Must enter SID\n");
        return -1;
    }

    /* strtok(sid, "\n"); // Strip newline. */

    if (!valid_sid(sid)) {
    DEBUG_PRINT("SID: %s\n", sid);

        printf("Invalid SID\n");
        return -1;
    }
    DEBUG_PRINT("SID: %s\n", sid);

    char * ECPname = "localhost";
    int    ECPport = DEFAULT_UDP_PORT;

    optind = 2;
    while((option = getopt(argc, argv, "n:p:")) != -1) {
        if(option == 'n') {
            ECPname = strdup(optarg);
        } else if (option == 'p') {
            ECPport = atoi(optarg);
        } else if (optopt == '?'){
            fprintf(stderr, "Usage: %s SID [-p ECPname] [-n ECPport]\n", argv[0]);
            return -1;
        }
    }

    DEBUG_PRINT("ECPname: %s\n", ECPname);
    DEBUG_PRINT("ECPport: %d\n", ECPport);

    if (ECPname != NULL) {
        /* free(ECPname); */

    }
    return 0;
}
