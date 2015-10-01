#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tes.h"
#include "../utils.h"



int main(int argc, char *argv[]) {
    int    TESport = 59000;
    char * ECPname = "localhost";
    int    ECPport = 58009;

    /* Parse command line arguments */
    int option;
    while((option = getopt(argc, argv, "p:")) != -1) {
        if(option == 'p') {
            TESport = atoi(optarg);
        } else if (option == 'n') {
            ECPname = strdup(optarg);
        } else if (option == 'e') {
            ECPport = atoi(optarg);
        } else if (optopt == '?'){
            fprintf(stderr, "Usage: %s [-p TESport] [-n ECPname] [-e ECPport]\n", argv[0]);
            return -1;
        }
    }

    /* Start TCP server */
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        fprintf(stderr, "%s: Could not open server socket on TESport %d",
                strerror(errno), TESport);
        return -1;
    }

    printf("Started TCP server on TESport %d\n", TESport);

    const struct sockaddr_in client_addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(TESport),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    MAY_FAIL(bind(server_socket, (struct sockaddr *) &client_addr, sizeof(client_addr)));
    MAY_FAIL(listen(server_socket, 10));

    while(1) {
        int addr_size = sizeof(client_addr);
        const int connection_socket =
            accept(server_socket, (struct sockaddr *) &client_addr, (unsigned *) &addr_size);

        if (connection_socket == -1) {
            perror("Failed to accept connection");
            continue;
        }

        printf("Accepted client with IP %s on TESport %d\n",
               inet_ntoa(client_addr.sin_addr), TESport);

        const pid_t pid = fork();
        if (pid == -1) {
            perror("Failed to fork");
        } else if (pid == 0) { // Child process.
            char request[REQUEST_MAX_SIZE];
            const int bytes_read =
                read_bytes(connection_socket, REQUEST_MAX_SIZE, request);

            if (bytes_read == -1) {
                perror("Failed to read from client");
            } else {
                request[bytes_read] = '\0';
                printf("%s", request);

                const struct msg  * const reply_msg = reply_request(request);

                char * reply = msg_to_string(reply);
                free(reply_msg);

                const bytes_written = write_bytes(connection_socket, strlen(reply), reply);
                free(reply);

                if (bytes_written == -1) {
                    perror("Failed to write to client");
                }

                close(connection_socket);
            }
        }
        // Parent process.
    }

    // FIXME: Must finish cleanly and handle SIGPIPE and SIGINT!
    close(server_socket);
    if (strcmp(ECPname, "localhost") != 0) {
        free(ECPname);
    }
    return 0;
}
