#pragma once

#include "dict.h"
#include "lib.h"

typedef struct client_info_s {
    char name_id[STR_LEN_MAX];
    struct sockaddr_in *client_addr;
} client_info_t;

// client_info_t *client_info_new(void);

// void client_info_free(client_info_t *client_info);

typedef struct server_thread_s {
    int sockfd_client;          // socket file descriptor
    struct sockaddr_in my_addr; // server address
    struct sockaddr_in client;  // client address

    dict_t *pages;   // dictionary of pages
    sem_t *sem_dict; // semaphore for the dictionary

    fd_set *readfds;    // file descriptor set
    struct timeval *tv; // timeval
    int *rtval;         // return value
} server_thread_t;

int server(void);

void *handle_client(void *arg);
