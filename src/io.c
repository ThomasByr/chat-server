#include "io.h"

void io_args_init(struct io_args *args) {
    args->get_help = false;
    args->get_version = false;
    args->get_license = false;
}

void read_io_args(struct io_args *args, int argc, char **argv) {
    int opt;
    char bad[BUFSIZ] = {0};

    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {"license", no_argument, NULL, 'l'},
        {"port", required_argument, NULL, 'p'},
        {"target", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0},
    };

    while ((opt = getopt_long(argc, argv, "hvlp:t:", long_options, NULL)) !=
           -1) {
        switch (opt) {
        case 'h':
            args->get_help = true;
            break;
        case 'v':
            args->get_version = true;
            break;
        case 'l':
            args->get_license = true;
            break;
        case 'p':
            args->port = optarg;
            break;
        case 't':
            args->target = optarg;
            break;

        default:
            snprintf_s(bad, BUFSIZ, "%s", argv[optind - 1]);
            get_help(bad);
            panic(0, "unreachable");
            break;
        }
    }
}

void check_io_args(struct io_args *args) {
    if (args->get_help) {
        get_help(NULL);
        panic(0, "unreachable");
    }
    if (args->get_version) {
        get_version();
        panic(0, "unreachable");
    }
    if (args->get_license) {
        get_license();
        panic(0, "unreachable");
    }
}

noreturn void get_help(char *restrict s) {
    int status = (s == NULL) ? EXIT_SUCCESS : EXIT_FAILURE;

    if (s != NULL) {
        fprintf(stdout, FG_RED "  ERROR: " RST);
        fprintf(stdout, "%s\n", s);
    }

    fprintf(stdout, "usage: ./bin/server || ./bin/client [OPTION]...\n");
    fprintf(stdout, "options:\n");
    fprintf(stdout, "    -h, --help\n");
    fprintf(stdout, "        print this help and exit\n");
    fprintf(stdout, "    -v, --version\n");
    fprintf(stdout, "        print version and exit\n");
    fprintf(stdout, "    -l, --license\n");
    fprintf(stdout, "        print license and exit\n");
    fprintf(stdout, "    -p, --port\n");
    fprintf(stdout, "        port to listen on or connect to\n");
    fprintf(stdout, "    -t, --target\n");
    fprintf(stdout, "        target to connect to\n");
    fflush(stdout);

    exit(status);
}

noreturn void get_version(void) {
    char v[BUFSIZ] = {0};
    snprintf_s(v, BUFSIZ, "%d.%d.%d", __VERSION_MAJOR__, __VERSION_MINOR__,
               __VERSION_PATCH__);

    fprintf(stdout, "chat-server\nauthors: %s\n", __AUTHOR__);
    fprintf(stdout, "version: %s\n", v);
    fflush(stdout);

    exit(EXIT_SUCCESS);
}

noreturn void get_license(void) {
    static const char l[] =
        "This project is licensed under the [GPL-3.0](LICENSE) license. "
        "Please see the [license](LICENSE) file for more "
        "details.\n\nRedistribution and use in source and binary forms, with "
        "or without\nmodification, are permitted provided that the following "
        "conditions are met:\n\n- Redistributions of source code must retain "
        "the above copyright notice,\n  this list of conditions and the "
        "following disclaimer.\n\n- Redistributions in binary form must "
        "reproduce the above copyright notice,\n  this list of conditions "
        "and the following disclaimer in the documentation\n  and/or other "
        "materials provided with the distribution.\n\n- Neither the name of "
        "this project authors nor the names of its\n  contributors may be "
        "used to endorse or promote products derived from\n  this software "
        "without specific prior written permission.\n\nTHIS SOFTWARE IS "
        "PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\""
        "\nAND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED "
        "TO, THE\nIMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A "
        "PARTICULAR PURPOSE\nARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT "
        "HOLDER OR CONTRIBUTORS BE\nLIABLE FOR ANY DIRECT, INDIRECT, "
        "INCIDENTAL, SPECIAL, EXEMPLARY, OR\nCONSEQUENTIAL DAMAGES "
        "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\nSUBSTITUTE GOODS OR "
        "SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\nINTERRUPTION) "
        "HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER "
        "IN\nCONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR "
        "OTHERWISE)\nARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, "
        "EVEN IF ADVISED OF THE\nPOSSIBILITY OF SUCH DAMAGE.\n";

    fprintf(stdout, "license:\n%s", l);
    fflush(stdout);

    exit(EXIT_SUCCESS);
}
