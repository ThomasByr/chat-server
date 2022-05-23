#include "server.h"

struct server_s {
    int sock;
    char *port_name;
    int port_number;
    FILE *ferr;
};
typedef struct server_s server_t;

static server_t *new_server(void) {
    server_t *srv;

    if ((srv = (server_t *)calloc(1, sizeof(server_t))) == NULL) {
        panic(1, "calloc");
    }
    srv->ferr = stderr;
    return srv;
}

static void free_server(server_t *srv) {
    if (srv->port_name) {
        free(srv->port_name);
    }
    free(srv);
}

static server_t *init_server(char *port) {
    server_t *srv;
    char *protocol = "tcp";
    struct protoent *pp;
    struct servent *sport;
    // char *ep;
    extern const struct in6_addr in6addr_any;
    struct sockaddr_storage sa;
    int sopt = 0;
    extern int errno;

    srv = new_server();
    srv->port_name = strdup(port);

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
        if (errno == EAFNOSUPPORT) {
            if ((srv->sock = socket(AF_INET, SOCK_STREAM, pp->p_proto)) < 0) {
                panic(1, "socket");
            }
        } else {
            struct sockaddr_in *sa4 = (struct sockaddr_in *)&sa;
            sa4->sin_family = AF_INET;
            sa4->sin_port = srv->port_number;
            sa4->sin_addr.s_addr = INADDR_ANY;
        }
    } else {
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

void *run_server(server_t *srv) {
    for (;;) {
        int s;

        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        struct sockaddr *sap = (struct sockaddr *)&addr;

        memset(&addr, 0, sizeof(addr));
        if ((s = accept(srv->sock, sap, &addrlen)) >= 0) {
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];
            FILE *sfp;

            if (getpeername(s, sap, &addrlen) != 0) {
                debug(1, "getpeername\n");
                close(s);
                continue;
            } else if (getnameinfo(sap, addrlen, host, sizeof(host), service,
                                   sizeof(service), 0) != 0) {
                debug(1, "getnameinfo\n");
                close(s);
                continue;
            }
            debug(1, "accepted connection from %s:%s\n", host, service);

            if ((sfp = fdopen(s, "w+")) == NULL) {
                debug(1, "fdopen\n");
                close(s);
                continue;
            } else {
                char buf[BUFSIZ];

                setlinebuf(sfp);
                while (fgets(buf, sizeof(buf), sfp) != NULL) {
                    info(1, "client: %s\n", buf);
                    fprintf(sfp, "get %zd chars\n", strlen(buf));
                }
                debug(1, "client closed connection\n");

                if (shutdown(s, SHUT_RDWR) != 0) {
                    debug(1, "shutdown\n");
                    (void)fclose(sfp);
                }
            }
        }
    }
    return NULL;
}

static void done_server(server_t *srv) {
    if (close(srv->sock) != 0) {
        panic(1, "close");
    }
    free_server(srv);
    debug(1, "server done\n");
}

static jmp_buf sigenv;
static void on_signal(int sig) { longjmp(sigenv, sig); }

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

    debug(1, "server started\n");

    // launch thread to handle client requests
    pthread_t tid[2];
    for (int i = 0; i < 2; i++) {
        T_CHK(
            pthread_create(&tid[i], NULL, (void *(*)(void *))run_server, srv));
    }

    // wait for thread
    for (int i = 0; i < 2; i++) {
        T_CHK(pthread_join(tid[i], NULL));
    }

    return NULL;
}
