#pragma once

#include "lib.h"

/**
 * @brief Get a line from stdin
 * @param prompt the prompt to get
 * @return the line of the prompt in the terminal in a string
 */
static char *get_line(char *prompt);

/**
 * @brief Initialize the client
 * @param host the host to connect to
 * @param port the port to connect to
 * @return the socket file descriptor
 */
static int init_client(char *host, char *port);

/**
 * @brief Run the client
 * @param sockfd the socket file descriptor
 */
static void run_client(int sockfd);

/**
 * @brief Close the client
 * @param sockfd the socket file descriptor
 */
static void done_client(int sockfd);

/**
 * @brief Print how to use the client
 */
static void usage(void);

/**
 * @brief The main function of the client
 * @param argc the number of arguments
 * @param argv the arguments
 * @return the exit code
 */
int main_client(int argc, char *argv[]);