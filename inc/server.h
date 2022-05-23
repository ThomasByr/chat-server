#include "lib.h"

/**
 * @brief structure to give to the thread (ie main_server function)
 */
struct thread_arg_s {
    char *port;
    int *cpt;
};
typedef struct thread_arg_s thread_arg_t;

void *main_server(void *arg);
