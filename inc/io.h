#pragma once

#include "lib.h"

/**
 * @brief struct to hold command line arguments.
 *
 */
struct io_args {
    bool get_help;    // -h, --help
    bool get_version; // -v, --version
    bool get_license; // -l, --license
};

/**
 * @brief initialize io_args
 *
 * @param args io_args
 */
void io_args_init(struct io_args *args);

/**
 * @brief read command line arguments
 *
 * @param args struct to store arguments
 * @param argc number of arguments
 * @param argv array of arguments
 */
void read_io_args(struct io_args *args, int argc, char **argv);

/**
 * @brief check io_args
 *
 * @param args io_args
 */
void check_io_args(struct io_args *args);

/**
 * @brief print help
 *
 * @param s additional message, set to NULL if not needed
 */
noreturn void get_help(char *restrict s);

/**
 * @brief print version
 *
 */
noreturn void get_version(void);

/**
 * @brief print small license
 *
 */
noreturn void get_license(void);
