#pragma once

#include "lib.h"

typedef struct client_arg_t {
    int sockfd_client;
    struct sockaddr_in dest_addr;
    struct sockaddr_in my_addr;
    char *msg;
} client_arg_t;

/**
 * @brief The function to communicate with the server before sending message
 * to the other client.
 * @param name_to_send The name of the client to send the message to.
 * @return client_arg_t* The info of the destination client.
 */
client_arg_t server_exchange(char *name_to_send);

/**
 * @brief Thread function to send a message to an another client.
 * @param arg The argument to the thread (client_arg_t).
 * @return void*
 */
void *send_to_client(void *arg);

/**
 * @brief Thread function to wait for a message from an another client.
 * @param arg The argument to the thread (NULL).
 * @return void*
 */
void *waiting_for_msg(void *arg);

/**
 * @brief The main function of the client.
 * @return int The exit code of the program.
 */
int client(void);