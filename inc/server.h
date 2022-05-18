#pragma once

#include "lib.h"

typedef struct server_thread_t {
    int sockfd_client;
    struct sockaddr_in my_addr;
    struct sockaddr_in client;
} server_thread_t;

int server(void);

void *handle_client(void *arg);