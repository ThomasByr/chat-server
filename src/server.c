#include "server.h"

/**
 * @brief structure to give to the thread
 */
struct server_s {
    int sock, port_number, id;
    char *port_name;
    FILE *fds[NB_CLIENTS];
    sem_t sem;
};
typedef struct server_s server_t;

/**
 * @brief Function to create and allocate the server structure
 * @return server_t* the server infos
 */
static server_t *new_server(void) {
    server_t *srv;

    // Allocate memory for the server structure
    if ((srv = (server_t *)calloc(1, sizeof(server_t))) == NULL) {
        panic(1, "calloc");
    }
    return srv;
}

/**
 * @brief Function to close the server
 * @param srv the server infos
 */
static void free_server(server_t *srv) {
    if (srv->port_name) {
        free(srv->port_name);
    }
    free(srv);
}

/**
 * @brief Function to initialize the server
 * @param port the port to listen on
 * @return server_t* the server infos
 */
static server_t *init_server(char *port) {
    server_t *srv;
    char *protocol = "tcp"; // protocol to use
    struct protoent *pp;    // protocol info
    struct servent *sport;  // service info
    extern const struct in6_addr in6addr_any;
    struct sockaddr_storage sa;
    int sopt = 0;

    srv = new_server();
    srv->port_name = strdup(port);
    if (srv->port_name == NULL) {
        panic(1, "strdup");
    }

    if ((pp = getprotobyname(protocol)) == NULL) {
        panic(1, "getprotobyname");
    }

    if (((srv->port_number = strtoi(srv->port_name, NULL))) > 0) {
        srv->port_number = htons(srv->port_number);
    } else if ((sport = getservbyname(srv->port_name, protocol)) == NULL) {
        panic(1, "getservbyname");
    } else {
        srv->port_number = sport->s_port;
    }

    (void)memset(&sa, 0, sizeof(sa));
    if ((srv->sock = socket(AF_INET6, SOCK_STREAM, pp->p_proto)) < 0) {
        // IPv6 not supported
        if (errno == EAFNOSUPPORT) {
            if ((srv->sock = socket(AF_INET, SOCK_STREAM, pp->p_proto)) < 0) {
                // IPv4 not supported
                panic(1, "socket");
            }
        } else {
            // IPv4 supported but IPv6 not supported
            struct sockaddr_in *sa4 = (struct sockaddr_in *)&sa;
            sa4->sin_family = AF_INET;
            sa4->sin_port = srv->port_number;
            sa4->sin_addr.s_addr = INADDR_ANY;
        }
    } else {
        // IPv6 supported
        struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&sa;
        if (setsockopt(srv->sock, IPPROTO_IPV6, IPV6_V6ONLY, &sopt,
                       sizeof(sopt)) < 0) {
            panic(1, "setsockopt");
        }
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port = srv->port_number;
        sa6->sin6_addr = in6addr_any;
    }

    if (bind(srv->sock, (const struct sockaddr *)&sa, sizeof(sa)) < 0) {
        panic(1, "bind");
    }
    if (listen(srv->sock, SOMAXCONN) < 0) {
        panic(1, "listen");
    }
    return srv;
}

/**
 * @brief Function to accept a client
 * @param srv the server infos
 */
void *run_server(server_t *srv) {
    int id = srv->id;
    T_CHK(sem_post(&srv->sem));
    for (;;) {
        int s;

        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        struct sockaddr *sap = (struct sockaddr *)&addr;

        (void)memset(&addr, 0, sizeof(addr));
        if ((s = accept(srv->sock, sap, &addrlen)) >= 0) {
            // Create a new file descriptor
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            FILE *sfp;

            if (getpeername(s, sap, &addrlen) != 0) {
                debug(1, "getpeername\n");
                shutdown(s, SHUT_RDWR);
                continue;
            } else if (getnameinfo(sap, addrlen, host, sizeof(host), service,
                                   sizeof(service), 0) != 0) {
                debug(1, "getnameinfo\n");
                shutdown(s, SHUT_RDWR);
                continue;
            }
            debug(1, "accepted connection from %s:%s\n", host, service);

            if ((sfp = fdopen(s, "w+")) == NULL) {
                debug(1, "fdopen\n");
                shutdown(s, SHUT_RDWR);
                continue;
            } else {
                srv->fds[id] = sfp;
                frame_t frame;

                setlinebuf(sfp);
                while (read(fileno(sfp), &frame, sizeof(frame)) > 0) {
                    // While we receive messages
                    trim(frame.msg);
                    trim(frame.name_id);
                    info(1, "client: %s\n", frame.name_id);
                    info(1, "message: %s\n", frame.msg);

                    int cpt_client = 0;

                    for (int i = 0; i < NB_CLIENTS; i++) {
                        debug(1, "fds[%d] = %p\n", i, srv->fds[i]);
                        // Write to all the connected clients
                        if (i != id && srv->fds[i]) {
                            cpt_client++;
                            CHK(write(fileno(srv->fds[i]), &frame,
                                      sizeof(frame)));
                        }
                    }

                    // Write to the client itself
                    frame_t frame_ack;
                    strlcpy(frame_ack.name_id, "SERVER",
                            sizeof(frame_ack.name_id));
                    snprintf_s(frame_ack.msg, sizeof(frame_ack.msg),
                               "Message sent to %d user(s)", cpt_client);
                    CHK(write(fileno(sfp), &frame_ack, sizeof(frame_ack)));
                }
                srv->fds[id] = NULL;
                debug(1, "client %s:%s closed connection\n", host, service);

                // Properly close the file descriptor
                if (shutdown(s, SHUT_RDWR) != 0) {
                    debug(1, "shutdown\n");
                    (void)fclose(sfp);
                }
            }
        }
    }
    return NULL;
}

/**
 * @brief Function to terminate the server
 * @param srv the server infos
 */
static void done_server(server_t *srv) {
    if (close(srv->sock) != 0) {
        panic(1, "close");
    }
    free_server(srv);
    debug(1, "server done\n");
}

static jmp_buf sigenv;
static void on_signal(int sig) { longjmp(sigenv, sig); }

/**
 * @brief Main function of the server
 * @param arg the thread arguments (here thread_arg_t*)
 */
void *main_server(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    server_t *srv;

    srv = init_server(targ->port);

    if (setjmp(sigenv) > 0) {
        done_server(srv);
        return NULL;
    } else {
        signal(SIGHUP, on_signal);
        signal(SIGINT, on_signal);
        signal(SIGTERM, on_signal);
    }

    debug(1, "server started, listening on port %s\n", targ->port);

    // launch thread to handle client requests
    pthread_t tid[NB_CLIENTS];

    // Init file descriptors
    for (int i = 0; i < NB_CLIENTS; i++) {
        srv->fds[i] = NULL;
    }

    // Init sem to get id for thread
    T_CHK(sem_init(&srv->sem, 0, 1));

    for (int k = 0; k < NB_CLIENTS; k++) {
        T_CHK(sem_wait(&srv->sem));
        srv->id = k;
        T_CHK(
            pthread_create(&tid[k], NULL, (void *(*)(void *))run_server, srv));
    }

    // wait for thread to finish
    for (int i = 0; i < NB_CLIENTS; i++) {
        T_CHK(pthread_join(tid[i], NULL));
    }

    return NULL;
}
