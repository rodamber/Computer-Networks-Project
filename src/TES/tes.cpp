#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "tes_utils.h"
#include "../utils.h"



int main(int argc, char *argv[]) {
    int port = PORT; // Server port;

    /* Parse command line arguments */
    int option;
    while((option = getopt(argc, argv, "p:")) != -1) {
        if(option == 'p') {
            port = atoi(optarg);
        } else if (optopt == 'p'){
            fprintf(stderr, "Usage: %s [-p TESPORT]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* TEST: Command line arguments parse */
    printf("The port is %d.\n", port);

    /* Start TCP server */
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    MAY_FAIL(server_socket);

    const struct sockaddr_in client_addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)
    };

    MAY_FAIL(bind(server_socket, (struct sockaddr *) &client_addr, sizeof(client_addr)));
    MAY_FAIL(listen(server_socket, 5));

    while(1) {
        int addr_size = sizeof(client_addr);
        const int connection_socket =
            accept(server_socket, (struct sockaddr *) &client_addr, (unsigned *) &addr_size);
        MAY_FAIL(connection_socket);

        /* GET REQUEST */
        char request_buffer[REQUEST_MAX_SIZE];
        MAY_FAIL(read_bytes(connection_socket, REQUEST_MAX_SIZE, request_buffer)); // RODRIGO: FIXME: Does not close sockets.

        /* PROCESS REQUEST */
        printf("%s\n", request_buffer);
        const struct msg * const request = parse_request(request_buffer);
        const struct msg * const reply   = reply_request(&request);

        // FIXME: Don't forget to free things
        close(connection_socket);
    }

    close(server_socket);
    return EXIT_SUCCESS;
}


/* int bytes_read; */
/* char buffer[BUFFER_SIZE]; */

/* while((bytes_read = read(connection_socket, buffer, BUFFER_SIZE)) != 0) { */
/*     MAY_FAIL(bytes_read); */
/*     buffer[bytes_read] = 0; */

/*     printf("Client: %s\n", buffer); */
/*     strn_toupper(buffer, bytes_read); */

/*     char *ptr = buffer; */
/*     while(bytes_read > 0) { */
/*         int bytes_written; */
/*         MAY_FAIL(bytes_written = write(connection_socket, ptr, bytes_read)); */
/*         bytes_read -= bytes_written; */
/*         ptr        += bytes_written; */
/*     } */
/*     printf("Server: %s\n", buffer); */
/* } */
