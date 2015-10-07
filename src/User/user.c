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

#define ABORT do { perror("Error: "); return -1; } while (0)

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


struct msg * getlist(int udp_socket, struct sockaddr * udp_address,
                     socklen_t addrlen) {
    if (sendto(udp_socket, "TQR\n", 4, 0,  udp_address, addrlen) == -1) {
        perror("Error: ");
        return msgdup(&error_msg);
    }

#define BUF_SZ 256
    char buf[BUF_SZ];
    if (recvfrom(udp_socket, buf, BUF_SZ, 0, udp_address, &addrlen) == -1) {
        printf("ERR\n");
        perror("Error: ");
        return msgdup(&error_msg);
    }

    DEBUG_PRINT("AWT command: %s\n", buf);

    return new_msg(buf);

#undef BUF_SZ
}


int main(int argc, char *argv[]) {

    // -----------------------------------------------------------------------------
    // Parse command line arguments

    char * sid = argv[1];

    if (sid == NULL) {
        printf("Must enter SID\n");
        return -1;
    } else if (!valid_sid(sid)) {
        DEBUG_PRINT("SID: %s\n", sid);
        printf("Invalid SID\n");
        return -1;
    }
    DEBUG_PRINT("SID: %s\n", sid);

    char * ECPname = "localhost";
    int    ECPport = DEFAULT_UDP_PORT;

    int option;
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
    DEBUG_PRINT("ECPport: %d\n\n", ECPport);

    // -----------------------------------------------------------------------------
    // Socket initialization.

    const int ecp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(ecp_socket == -1) ABORT;

    DEBUG_PRINT("ECP socket file descriptor: %d\n", ecp_socket);

    const struct hostent * ecp = gethostbyname(ECPname);
    if (ecp == NULL) ABORT;

    DEBUG_PRINT("ecp: %s\n\n", ecp->h_name);

    struct sockaddr_in ecp_address = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = ((struct in_addr *) (ecp->h_addr_list[0]))->s_addr,
        .sin_port        = htons((unsigned short) ECPport)
    };
    socklen_t ecp_addrlen = sizeof(ecp_address);


    // -----------------------------------------------------------------------------
    // User interaction.

    while (1) {
        printf("\nPlease, enter a command\n");
        printf("> ");

        char * command  = calloc(sizeof(char), 1);
        int command_cap = 1;
        int command_size;

        if ((command_size = getline(&command, (size_t *) &command_cap, stdin)) == -1) {
            perror("Error: ");
            return -1;
        }
        command[command_size] = '\0';
        DEBUG_PRINT("\nCommand entered: %sCommand size: %d\n", command, command_size);

        struct msg * command_msg = new_msg(command);
        free(command);


        #ifdef DEBUG
        char * command_str_debug = msg_to_string(command_msg);
        DEBUG_PRINT("Command parsed: %s\n", command_str_debug);
        free(command_str_debug);
        #endif


        if (strncmp(command_msg->type, "list", 4) == 0) { // List command.
            struct msg * list_msg = getlist(ecp_socket,
                                            (struct sockaddr*) &ecp_address,
                                            ecp_addrlen);


            /* #ifdef DEBUG */
            /* char * list_str_debug = msg_to_string(list_msg); */
            /* DEBUG_PRINT("Command parsed: %s\n", list_str_debug); */
            /* free(list_str_debug); */
            /* #endif */


            // Print numbered list.

            /* const int topics_number = atoi(list_msg->parameters[0]); */
            /* for (int i = 1; i <= topics_number; ++i) { */
            /*     printf("%d - %s\n", i, list_msg->parameters[i]); */
            /* } */

            free_msg(list_msg);
        } else if (strncmp(command_msg->type, "exit", 4) == 0) { // Exit command.
            free_msg(command_msg);
            break;
        } else {
            printf("Unknown command\n");
        }
    }


    // -----------------------------------------------------------------------------
    // Cleanup.

    if (ECPname != NULL) {
        /* free(ECPname); */

    }
    return 0;
}
