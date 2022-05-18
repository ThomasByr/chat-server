#include "receiver-slc.h"

int receiver_slc(char *target, int port) {
    fd_set readfds;         // file descriptor set
    struct timeval tv;      // timeout
    int rtval;              // return value of select
    int sockfd;             // socket file descriptor
    int sockfd_client;      // socket file descriptor
    char buf[BUFLEN] = {0}; // buffer

    // size of the address structure (used by `recvfrom`)
    socklen_t fromlen = sizeof(struct sockaddr_in);

    struct sockaddr_in my_addr; // address structure of the receiver
    struct sockaddr_in client;  // address structure of the sender

    // socket init
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // address family
    my_addr.sin_family = AF_INET;

    // port of the receiver
    my_addr.sin_port = htons(port);

    // IPv4 address of the receiver
    inet_aton(target, &(my_addr.sin_addr));

    // association of the socket and the address structure
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
        panic(1, "bind failure");
    }

    debug(1, "Receiver TCP listening on port %d\n", port);

    // reception of the message
    if (listen(sockfd, 1) == -1) {
        panic(1, "accept failure");
    }

    // accept connection
    if ((sockfd_client =
             accept(sockfd, (struct sockaddr *)&client, &fromlen)) == -1) {
        panic(1, "accept failure");
    }

    info(1, "Received connection from %s\n", inet_ntoa(client.sin_addr));

    // Monitor the socket for readability
    FD_ZERO(&readfds);
    FD_SET(sockfd_client, &readfds);

    // Wait TIMEOUT seconds for a message
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    rtval = select(sockfd_client + 1, &readfds, NULL, NULL, &tv);

    // reception of the message
    switch (rtval) {
    case -1:
        panic(1, "select failure");
        break;
    case 0:
        info(1, "Timeout\n");
        // Monitor the socket for readability
        FD_ZERO(&readfds);
        FD_SET(sockfd_client, &readfds);

        break;
    default:
        if (FD_ISSET(sockfd_client, &readfds)) {
            if (recv(sockfd_client, buf, BUFLEN, 0) == -1) {
                panic(1, "recv failure");
            }
            info(1, "%s connected\n", buf);
        }
        break;
    }

    // While message is different from ":q"
    while (strcmp(buf, ":q") != 0) {
        // Reinitialize msg
        memset(buf, 0, BUFLEN);

        // Wait TIMEOUT seconds for a message
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        rtval = select(sockfd_client + 1, &readfds, NULL, NULL, &tv);

        // reception of the message
        switch (rtval) {
        case -1:
            panic(1, "select failure");
            break;
        case 0:
            debug(1, "Timeout\n");
            // Monitor the socket for readability
            FD_ZERO(&readfds);
            FD_SET(sockfd_client, &readfds);

            break;
        default:
            if (FD_ISSET(sockfd_client, &readfds)) {
                if (recv(sockfd_client, buf, BUFLEN, 0) == -1) {
                    panic(1, "recv failure");
                }
                trim(buf);
                info(1, "Received: %s\n", buf);
            }
            break;
        }
    }

    info(1, "Client %s disconnected\n", inet_ntoa(client.sin_addr));

    // closing socket
    CHK(close(sockfd));
    CHK(close(sockfd_client));

    return EXIT_SUCCESS;
}