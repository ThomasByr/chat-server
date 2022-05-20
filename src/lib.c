#include "lib.h"

noreturn void panic(int syserr, const char *restrict fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, FG_RED);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, RST "\n");
    va_end(ap);

    if (syserr == 1) {
        perror("");
    }

    exit(EXIT_FAILURE);
}

int strtoi(const char *restrict nptr, int (*f)(int)) {
    char *endptr;
    int x = strtol(nptr, &endptr, 10);

    if (endptr == nptr || *endptr != '\0') {
        panic(1, "must be a number");
    } // if not a number
    if (errno == ERANGE) {
        panic(1, "out of range [%ld, %ld]", LONG_MIN, LONG_MAX);
    } // if out of range of long (might be a problem with implicit cast)
    if (f != NULL && !f(x)) {
        panic(0, "non legal value for our program");
    } // if non legal value for our program

    return x;
}

void debug(int first, const char *restrict fmt, ...) {
#ifndef DEBUG
    (void)first;
    (void)fmt;
#else
    va_list ap;

    va_start(ap, fmt);
    if (first) {
        fprintf(stdout, FG_GRN "  DEBUG: " RST);
    }
    vfprintf(stdout, fmt, ap);
    va_end(ap);

    int n = fflush(stdout);
    if (n == EOF) {
        panic(1, "fflush failure");
    }
#endif
}

void info(int first, const char *restrict fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    if (first) {
        fprintf(stdout, FG_BLU "\n   INFO: " RST);
    }
    vfprintf(stdout, fmt, ap);
    va_end(ap);
    int n = fflush(stdout);
    if (n == EOF) {
        panic(1, "fflush failure");
    }
}

void snprintf_s(char *restrict str, size_t size, const char *restrict fmt,
                ...) {
    int n;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(str, size, fmt, ap);
    va_end(ap);

    if (n < 0) {
        panic(1, "vsnprintf failure");
    }
    if ((size_t)n >= size) {
        panic(1, "format string too long");
    }
}

size_t strlcpy(char *restrict dst, const char *restrict src, size_t siz) {
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0') {
                break;
            }
        }
    }

    if (n == 0) {
        if (siz != 0) {
            *d = '\0';
        }
        while (*s++) {
        }
    }
    return s - src - 1;
}

void trim(char *str) {
    char *end;

    while (isspace(*str)) {
        str++;
    }

    if (*str == 0) {
        return;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';
}
