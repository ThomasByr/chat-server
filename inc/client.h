#pragma once

#include "lib.h"

typedef struct client_arg_t {
    int sockfd_client;
    struct sockaddr_in dest_addr;
    struct sockaddr_in my_addr;
    char *msg;
} client_arg_t;

void *send_to_client(void *arg);

void *receive_from_client(void *arg);

int client(void);