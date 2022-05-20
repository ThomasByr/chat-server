#include "server.h"

client_info_t *client_info_new(void) {
    client_info_t *client_info = malloc(sizeof(client_info_t));
    if (client_info == NULL)
        panic(1, "malloc");
    return client_info;
}

void client_info_free(client_info_t *client_info) {
    if (client_info == NULL)
        return;
    free(client_info);
}

int server(void) {
    dict_t *pages = dict_new(1);
    dict_itr_t *itr = dict_itr_new(pages);
    sem_t sem_dict;
    T_CHK(sem_init(&sem_dict, 0, 1));

    // infinit loop
    for (;;) {

        int sockfd;             // socket file descriptor
        int sockfd_client;      // socket file descriptor
        fd_set readfds;         // file descriptor set
        struct timeval tv;      // timeout
        int rtval;              // return value of select
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
        my_addr.sin_port = htons(PORT_SERVER);

        // IPv4 address of the receiver
        inet_aton(IP_ADDR_SERVER, &(my_addr.sin_addr));

        // association of the socket and the address structure
        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0) {
            panic(1, "bind failure");
        }
        debug(1, "Receiver TCP listening on port %d\n", PORT_SERVER);

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

        // reception of the username and store it in the dictionary with the
        // client address
        switch (rtval) {
        case -1:
            panic(1, "select failure");
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
                debug(1, "Received username: %s\n", buf);

                // Add client to the dict
                client_info_t *client_info = client_info_new();
                strlcpy(client_info->name_id, buf, STR_LEN_MAX);
                client_info->client_addr = &client;
                dict_push(pages, strdup(buf), client_info);
            }
            break;
        }

        // Launch a thread to handle the client
        pthread_t thread;
        server_thread_t *server_thread = malloc(sizeof(server_thread_t));
        server_thread->sockfd_client = sockfd_client;
        server_thread->my_addr = my_addr;
        server_thread->client = client;
        server_thread->pages = pages;
        server_thread->sem_dict = &sem_dict;
        server_thread->readfds = &readfds;
        server_thread->tv = &tv;
        server_thread->rtval = &rtval;
        T_CHK(pthread_create(&thread, NULL, handle_client,
                             (void *)server_thread));
    }

    dict_itr_discard_all(itr, free, (void (*)(void *))client_info_free);
    dict_itr_free(itr);
    dict_free(pages);
    CHK(sem_destroy(&sem_dict));

    return EXIT_SUCCESS;
}

void *handle_client(void *arg) {
    server_thread_t *server_thread = (server_thread_t *)arg;
    int sockfd_client = server_thread->sockfd_client;
    fd_set readfds = *(server_thread->readfds);
    struct timeval tv = *(server_thread->tv);
    int rtval = *(server_thread->rtval);

    char buf[BUFLEN] = {0};

    // Monitor the socket for readability
    FD_ZERO(&readfds);
    FD_SET(sockfd_client, &readfds);

    // Wait TIMEOUT seconds for a message
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    rtval = select(sockfd_client + 1, &readfds, NULL, NULL, &tv);

    // reception of the username and store it in the dictionary with the client
    // address
    switch (rtval) {
    case -1:
        panic(1, "select failure");
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
            debug(1, "Received username: %s\n", buf);

            // TODO: store client addr and username in dictionary
        }
        break;
    }

    // infinit loop
    for (;;) {
        // TODO: wait for client asking a client username

        // TODO: send the client username to the client

        // TODO: exit if client is disconnected
    }

    return NULL;
}
