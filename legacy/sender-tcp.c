#include "sender-tcp.h"

int sender_tcp(char *target, int port, char *msg) {
    int sockfd;              // socket file descriptor
    struct sockaddr_in dest; // address structure of the target

    // socket init
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // address family
    dest.sin_family = AF_INET;

    // IPv4 dest address
    inet_aton(target, &(dest.sin_addr));

    // dest port
    dest.sin_port = htons(port);

    debug(1, "Connecting to %s:%d\n", target, port);

    // connection
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) == -1) {
        panic(1, "connect failure");
    }

    info(1, "Connected to %s:%d\n", target, port);

    // Authentication
    if (send(sockfd, msg, strlen(msg), 0) == -1) {
        panic(1, "send failure");
    }
    debug(1, "Connected as: %s\n", msg);

    // While message is different from ":q"
    while (strcmp(msg, ":q") != 0) {
        // Reinitialize msg
        memset(msg, 0, BUFLEN);

        // Read input
        if (fgets(msg, BUFLEN, stdin) == NULL) {
            panic(1, "fgets failure");
        }
        trim(msg);

        // send msg
        if (send(sockfd, msg, strlen(msg), 0) == -1) {
            panic(1, "send failure");
        }

        // print debug
        debug(1, "Sent msg: %s\n", msg);
        debug(0, "\t-> to %s:%d\n", target, port);
    }

    // closing socket
    close(sockfd);

    return EXIT_SUCCESS;
}
