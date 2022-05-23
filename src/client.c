#include "client.h"

/**
 * @brief Structure for the reciving thread
 */
struct rcv_thread_s {
    FILE *fd;
    sem_t *sem_rw;
};
typedef struct rcv_thread_s rcv_thread_t;

/**
 * @brief Function to get the line from stdin
 * @param prompt the prompt to print
 * @return the line read
 */
static char *get_line(char *prompt) { return readline(prompt); }

/**
 * @brief Function to init the client
 * @param port the port to connect to
 * @param host the host to connect to
 * @return error code
 */
static int init_client(char *host, char *port) {
    struct addrinfo hints, *res, *rp;
    int s;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // Get address info for the server
    if ((s = getaddrinfo(host, port, &hints, &res)) != 0) {
        panic(0, "getaddrinfo: %s", gai_strerror(s));
    }

    // Try each address until we successfully connect
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sockfd, ret;
        char hostnum[NI_MAXHOST];

        // Get numeric adress
        if ((ret = getnameinfo(rp->ai_addr, rp->ai_addrlen, hostnum,
                               sizeof(hostnum), NULL, 0, NI_NUMERICHOST)) !=
            0) {
            debug(1, "getnameinfo: %s\n", gai_strerror(ret));
        } else {
            info(1, "Trying %s...\n", hostnum);
            fflush(stdout);
        }

        // Create socket
        if ((sockfd = socket(rp->ai_family, rp->ai_socktype,
                             rp->ai_protocol)) == -1) {
            if ((rp->ai_family == AF_INET6) && (errno == EAFNOSUPPORT)) {
                info(1, "IPv6 not supported\n");
            } else {
                info(1, "socket: %s\n", strerror(errno));
            }
            continue;
        }

        // Connect to server
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == -1) {
            info(1, "connect: %s\n", strerror(errno));
            close(sockfd);
            continue;
        } else {
            info(1, "Connected to %s\n", hostnum);
            return sockfd;
            break;
        }
    }
    panic(0, "Could not connect to %s", host);
}

/**
 * @brief Function to receive a message from the server using a thread
 * @param arg the argument to pass to the thread (here rcv_thread_t)
 */
void *receive_message(void *arg) {
    rcv_thread_t *rcv_thread = (rcv_thread_t *)arg;
    FILE *fd = rcv_thread->fd;
    sem_t *sem_rw = rcv_thread->sem_rw;

    // Duplicate file descriptor
    int fd_dup = dup(fileno(fd));

    frame_t frame;
    debug(1, "Ready to receive message\n");
    T_CHK(sem_post(sem_rw));
    while (read(fd_dup, &frame, sizeof(frame_t)) > 0) {
        trim(frame.msg);
        trim(frame.name_id);
        CHK(write(STDOUT_FILENO, "\033[2k\r", 5));
        info(1, "[%s]: %s\n", frame.name_id, frame.msg);
        info(0, FG_RED "$ " RST);
    }
    debug(1, "Connection closed\n");
    return NULL;
}

/**
 * @brief Function to send a message to the server
 * @param sockfd the socket to send the message to
 */
static void run_client(int sockfd, frame_t *frame) {
    FILE *fp;
    char *input;
    sem_t sem_rw; // Semaphore to sync read/write threads

    T_CHK(sem_init(&sem_rw, 0, 0));

    // Create a buffer for the socket
    if ((fp = fdopen(sockfd, "w+")) == NULL) {
        panic(0, "fdopen");
    }
    setlinebuf(fp);

    // Create a thread to receive messages
    pthread_t tid;
    rcv_thread_t rcv_thread = {.fd = fp, .sem_rw = &sem_rw};
    T_CHK(pthread_create(&tid, NULL, receive_message, &rcv_thread));

    T_CHK(sem_wait(&sem_rw));

    // Read the message
    while (((input = get_line(FG_RED "$ " RST)) != NULL) &&
           (strcmp(input, ".") != 0)) {
        // Write the message to the socket
        debug(1, "Sending: %s\n", input);
        strlcpy(frame->msg, input, BUFFSIZE);
        trim(frame->msg);
        trim(frame->name_id);
        CHK(write(fileno(fp), frame, sizeof(frame_t)));
        debug(1, "Sent\n");
        free(input);
    }

    // Close the socket
    fclose(fp);

    // Wait for the thread to finish
    T_CHK(pthread_join(tid, NULL));
}

/**
 * @brief Function to terminate the client
 * @param sockfd the socket to close
 */
static void done_client(int sockfd) {
    if (close(sockfd) == -1) {
        panic(0, "close");
    }
}

/**
 * @brief Main function of the client
 * @param port the port to connect to
 * @param target the host to connect to
 */
int main_client(char *port, char *target) {
    int sockfd;
    frame_t frame;

    // Ask user fot username
    char *username = get_line("Username: ");
    trim(username);
    strlcpy(frame.name_id, username, STR_LEN_MAX);

    sockfd = init_client(target, port);
    run_client(sockfd, &frame);
    done_client(sockfd);

    exit(EXIT_SUCCESS);
}