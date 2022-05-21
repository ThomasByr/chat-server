#include "client.h"

void *waiting_for_msg(void *arg) {
    fd_set readfds;    // file descriptor set
    struct timeval tv; // timeout
    int rtval;         // return value of select
    int sockfd_client;
    struct sockaddr_in sender_addr;
    struct sockaddr_in my_addr;

    int n;
    char buf[BUFLEN];
    socklen_t len = sizeof(struct sockaddr_in);

    for (;;) {
        // socket init
        sockfd_client = socket(AF_INET, SOCK_STREAM, 0);

        // association of the socket and the address structure
        if (bind(sockfd_client, (struct sockaddr *)&my_addr, sizeof(my_addr)) !=
            0) {
            panic(1, "bind failure");
        }

        // reception of the message
        if (listen(sockfd_client, 1) == -1) {
            panic(1, "accept failure");
        }

        // accept connection
        if ((sockfd_client = accept(
                 sockfd_client, (struct sockaddr *)&sender_addr, &len)) == -1) {
            panic(1, "accept failure");
        }

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
                // Print the message and the sender's address
                info(1, "Received message: %s\n", buf);
                info(1, "From: %s\n", inet_ntoa(sender_addr.sin_addr));
                // TODO: Replace sender_addr by the name of the sender using
                // TODO: the dictionary
            }
            break;
        }
    }
    return NULL;
}

void *send_to_client(void *arg) {
    // Get arguments
    client_arg_t *client_arg = (client_arg_t *)arg;
    char *msg = client_arg->msg;

    // socket init
    int sockfd_client = socket(AF_INET, SOCK_STREAM, 0);

    // address family
    struct sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;

    // port of the receiver
    my_addr.sin_port = htons(PORT_SERVER);

    // IPv4 address of the receiver
    inet_aton(IP_ADDR_SERVER, &(my_addr.sin_addr));

    // association of the socket and the address structure
    if (connect(sockfd_client, (struct sockaddr *)&my_addr, sizeof(my_addr)) !=
        0) {
        panic(1, "connect failure");
    }

    // Send the message
    if (send(sockfd_client, msg, strlen(msg), 0) == -1) {
        panic(1, "send failure");
    }

    // closing socket
    close(sockfd_client);

    return NULL;
}

client_arg_t server_exchange(char *name_to_send) {
    client_arg_t client_arg;
    int sockfd_client;
    struct sockaddr_in dest_addr;
    struct sockaddr_in my_addr;

    socklen_t len = sizeof(struct sockaddr_in);

    // socket init
    sockfd_client = socket(AF_INET, SOCK_STREAM, 0);

    // address family
    my_addr.sin_family = AF_INET;

    // port of the receiver
    my_addr.sin_port = htons(PORT_SERVER);

    // IPv4 address of the receiver
    inet_aton(IP_ADDR_SERVER, &(my_addr.sin_addr));

    // association of the socket and the address structure
    if (bind(sockfd_client, (struct sockaddr *)&my_addr, sizeof(my_addr)) !=
        0) {
        panic(1, "bind failure");
    }

    // reception of the message
    if (listen(sockfd_client, 1) == -1) {
        panic(1, "accept failure");
    }

    // accept connection
    if ((sockfd_client = accept(sockfd_client, (struct sockaddr *)&dest_addr,
                                &len)) == -1) {
        panic(1, "accept failure");
    }

    // TODO: Send the ":s" and after the name of the receiver to receive the
    // TODO: address

    // Get the info of the client to send
    struct sockaddr_in receiver_addr;
    if (recv(sockfd_client, &receiver_addr, sizeof(receiver_addr), 0) == -1) {
        panic(1, "recv failure");
    }

    // closing socket
    close(sockfd_client);

    client_arg.dest_addr = receiver_addr;

    return client_arg;
}

int client(void) {
    // Create thread to wait for messages
    pthread_t thread;
    T_CHK(pthread_create(&thread, NULL, waiting_for_msg, NULL));

    // print the menu
    for (;;) {
    menu:;
        printf("What do you want to do?\n");
        printf("1. List the names of the clients\n");
        printf("2. Send a message to a client\n");
        printf("3. Exit\n");
        printf("Your choice (1 or 2 or 3): ");

        // Get the choice
        int choice;
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            // TODO: Answer the server the name of all the connected clients
            // TODO: Print the names of the clients
            break;
        case 2:
            char name_to_send[BUFLEN];
            printf("Enter the name of the client to send a message to: ");
            scanf("%s", name_to_send);

            // TODO: Send the name of the client to the server to get the
            // TODO: address

            // Get the message to send
            char msg[BUFLEN];
            printf("Enter the message to send: ");
            scanf("%s", msg);

            // TODO: Send a message to a client with the address
            break;
        case 3:
            T_CHK(pthread_cancel(thread));
            exit(EXIT_SUCCESS);
        default:
            printf("Wrong choice\n");
            break;
        }
    }
    // Wait for the threads to finish
    T_CHK(pthread_join(thread, NULL));

    exit(EXIT_SUCCESS);
}