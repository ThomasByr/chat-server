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

static void run_client(int sockfd) {
    FILE *fp;
    char *input;

    // Create a buffer for the socket
    if ((fp = fdopen(sockfd, "r+")) == NULL) {
        panic(0, "fdopen");
    }
    setlinebuf(fp);

    // Read the message
    while ((input == get_line("> ") == NULL) && (strcmp(input, ".") != 0)) {
        char buf[BUFSIZ];

        // Write the message to the socket
        fprintf(fp, "%s\n", input);
        free(input);

        // Get response
        if (fgets(buf, BUFSIZ, fp) == NULL) {
            info(1, "Connection lost\n");
            break;
        } else {
            // Print response
            info(1, "Response: %s", buf);
        }
    }
}

static void done_client(int sockfd) {
    if (close(sockfd) == -1) {
        panic(0, "close");
    }
}

static void usage_client(void) {
    printf("Usage: ./client [-p PORT] host\n");
    exit(EXIT_SUCCESS);
}

int main_client(int argc, char *argv[]) {
    int c;
    char *host = NULL;
    char *port = DEF_PORT;
    int sockfd;

    prog = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : argv[0];

    while ((c = getopt(argc, argv, "hp:")) != EOF) {
        switch (c) {
        case 'p':
            port = optarg;
            break;
        case 'h':
        default:
            usage_client();
        }
    }

    if (optind < argc) {
        host = argv[optind++];
        if (optind != argc) {
            usage_client();
        }
    } else {
        info(0, "hostname not specified");
        usage_client();
    }

    sockfd = init_client(host, port);
    run_client(sockfd);
    done_client(sockfd);

    exit(EXIT_SUCCESS);
}