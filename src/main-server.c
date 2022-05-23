//! chat-server
//!
//! Copyright (c) 2022, ThomasByr x ThomasD.
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions are met:
//!
//! * Redistributions of source code must retain the above copyright notice,
//!   this list of conditions and the following disclaimer.
//!
//! * Redistributions in binary form must reproduce the above copyright notice,
//!   this list of conditions and the following disclaimer in the documentation
//!   and/or other materials provided with the distribution.
//!
//! * Neither the name of this project authors nor the names of its
//!   contributors may be used to endorse or promote products derived from
//!   this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//! POSSIBILITY OF SUCH DAMAGE.
//!
//! @file src/main.c
//! @author ThomasByr <https://github.com/ThomasByr>
//! @author ThomasD <https://github.com/LosKeeper>
//! @brief Main entry point for this project executable.
//!
//! @see <https://github.com/ThomasByr/chat-server>
//!

#include "lib.h"

#include "server.h"

#include "io.h"

int main(int argc, char *argv[]) {
    struct io_args args;
    int cpt = 0;
    io_args_init(&args);

    read_io_args(&args, argc, argv);
    check_io_args(&args);

    thread_arg_t thread_arg;
    thread_arg.port = args.port;
    thread_arg.cpt = &cpt;

    // Launch threads
    int server_need = 1;
    for (;;) {
        if (abs(server_need - cpt) >= 2) {
            sleep(1);
            continue;
        }
        server_need++;
        pthread_t thread_server;
        // TODO: increse port for next thread
        pthread_create(&thread_server, NULL, main_server, &thread_arg);
    }
}
