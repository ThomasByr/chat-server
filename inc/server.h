#include "lib.h"

struct thread_arg_s {
    char *port;
    int *cpt;
};

typedef struct thread_arg_s thread_arg_t;

void *main_server(void *arg);
