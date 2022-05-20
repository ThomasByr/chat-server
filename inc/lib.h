#pragma once

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <getopt.h>
#include <limits.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <wchar.h>

#define __AUTHOR__ "ThomasByr x ThomasD"

#define __VERSION_MAJOR__ 0
#define __VERSION_MINOR__ 1
#define __VERSION_PATCH__ 0

#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            panic(1, #op); \
    } while (0)

#define T_CHK(op)               \
    do {                        \
        if ((errno = (op)) > 0) \
            panic(1, #op);      \
    } while (0)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define RST "\x1b[m\x1b[0m"

#define FG_RED "\x1b[0;31m"
#define FG_GRN "\x1b[0;32m"
#define FG_YEL "\x1b[0;33m"
#define FG_BLU "\x1b[0;34m"
#define FG_MAG "\x1b[0;35m"
#define FG_CYN "\x1b[0;36m"
#define FG_WHT "\x1b[0;37m"

#define BG_BLU "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GRN "\x1b[42m"
#define BG_ORA "\x1b[43m"
#define BG_CYN1 "\x1b[44m"
#define BG_YEL "\x1b[45m"
#define BG_CYN2 "\x1b[46m"
#define BG_WHT "\x1b[47m"

#define BUFLEN 1 << 10
#define IP_ADDR_SERVER "88.170.206.241"
#define PORT_SERVER 32100
#define TIMEOUT 3600
#define STR_LEN_MAX 32

#ifndef DEBUG
#define ASSERT(x) (void)(x)
#else
#define ASSERT(x) assert(x)
#endif

typedef void *(action_callback_t)(void);
typedef void *(for_each_callback_t)(void *, void *);
typedef void(delete_callback_t)(void *);
typedef void(print_callback_t)(void *);

/**
 * @brief panic with a message.
 *
 * @param syserr 1 if errno is set, 0 otherwise
 * @param fmt formated message to print
 * @note this function never returns
 */
noreturn void panic(int syserr, const char *restrict fmt, ...);

/**
 * @brief perform a string to int conversion.
 *
 * @param nptr string to convert
 * @param f function to perform additional checks (should return 0 if not legal)
 * @return int - the converted value
 */
int strtoi(const char *restrict nptr, int (*f)(int));

/**
 * @brief print a message at the debug level.
 *
 * @param first 1 if the first message of a series, 0 otherwise
 * @param fmt formated message to print
 */
void debug(int first, const char *restrict fmt, ...);

/**
 * @brief print a message.
 *
 * @param first 1 if the first message of a series, 0 otherwise
 * @param fmt formated message to print
 */
void info(int first, const char *restrict fmt, ...);

/**
 * @brief safe call to snprintf.
 *
 * @param str string to write to
 * @param size size of the string
 * @param fmt formated message to print
 */
void snprintf_s(char *restrict str, size_t size, const char *restrict fmt, ...);

/**
 * @brief copies the string pointed to by src, including the terminating null
 * byte ('\0')
 *
 * @param dest destination string
 * @param src source string
 * @param n number of characters to copy
 * @return size_t - the number of characters copied, not including the
 * terminating null byte
 */
size_t strlcpy(char *restrict dst, const char *restrict src, size_t siz);

/**
 * @brief suppress leading and trailing whitespaces.
 *
 * @param str - string to trim
 */
void trim(char *str);
