#include "client.h"

static char *get_line(char *prompt) { return readline(prompt); }

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

void *receive_message(void *arg) {
    FILE *fd = (FILE *)arg;

    // Duplicate file descriptor
    int fd_dup = dup(fileno(fd));

    char buf[BUFSIZ];
    debug(1, "Ready to receive message\n");
    while (read(fd_dup, buf, BUFSIZ) > 0) {
        trim(buf);
        info(1, "Received: %s\n", buf);
        memset(buf, 0, BUFSIZ);
    }
    debug(1, "Connection closed\n");
    return NULL;
}

static void run_client(int sockfd) {
    FILE *fp;
    char *input;

    // Create a buffer for the socket
    if ((fp = fdopen(sockfd, "w+")) == NULL) {
        panic(0, "fdopen");
    }
    setlinebuf(fp);

    // Create a thread to receive messages
    pthread_t tid;
    T_CHK(pthread_create(&tid, NULL, receive_message, fp));

    // Read the message
    while (((input = get_line("$ ")) != NULL) && (strcmp(input, ".") != 0)) {
        // Write the message to the socket
        debug(1, "Sending: %s\n", input);
        fprintf(fp, "%s\n", input);
        fflush(fp);
        debug(1, "Sent\n");
        free(input);
    }

    // Close the socket
    fclose(fp);

    // Wait for the thread to finish
    T_CHK(pthread_join(tid, NULL));
}

static void done_client(int sockfd) {
    if (close(sockfd) == -1) {
        panic(0, "close");
    }
}

int main_client(char *port, char *target) {
    int sockfd;

    sockfd = init_client(target, port);
    run_client(sockfd);
    done_client(sockfd);

    exit(EXIT_SUCCESS);
}