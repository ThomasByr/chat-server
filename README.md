# <img src="assets/logo_bd_chat.png" alt="icon" width="3%"/>TP RIO / BD Chat

[![Linux](https://svgshare.com/i/Zhy.svg)](https://docs.microsoft.com/en-us/windows/wsl/tutorials/gui-apps)
[![GitHub license](https://img.shields.io/github/license/ThomasByr/chat-server)](https://github.com/ThomasByr/chat-server/blob/master/LICENSE)
[![GitHub commits](https://badgen.net/github/commits/ThomasByr/chat-server)](https://GitHub.com/ThomasByr/chat-server/commit/)
[![GitHub latest commit](https://badgen.net/github/last-commit/ThomasByr/chat-server)](https://gitHub.com/ThomasByr/chat-server/commit/)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/ThomasByr/chat-server/graphs/commit-activity)

[![CodeQL](https://github.com/ThomasByr/chat-server/actions/workflows/codeql-analysis.yml/badge.svg)](https://github.com/ThomasByr/chat-server/actions/workflows/codeql-analysis.yml)
[![GitHub version](https://badge.fury.io/gh/ThomasByr%2Fchat-server.svg)](https://github.com/ThomasByr/chat-server)
[![Author](https://img.shields.io/badge/author-@ThomasByr-blue)](https://github.com/ThomasByr)
[![Author](https://img.shields.io/badge/author-@ThomasD-blue)](https://github.com/LosKeeper)

<summary>School project</summary>

> [Project instruction paper](https://moodle.unistra.fr/) (subject) on the Moodle page.

**If you ever get this in .7z, .zip, .tar.gz, .rar or whatever archive format, please ensure you do have the latest version by paying a visit to [the home repo](https://github.com/ThomasByr/chat-server) on GitHub !**

1. [✏️ In short](#️-in-short)
2. [🔰 Prerequisites](#-prerequisites)
3. [👩‍🏫 Usage](#-usage)
4. [⚖️ License](#️-license)
5. [🔄 Changelog](#-changelog)
6. [🐛 Bugs & TODO](#-bugs--todo)

## ✏️ In short

This is a school project for the course _[Projet RIO-SDIA](https://moodle.unistra.fr/course/view.php?id=16231)_.

We basically were required to build a chat server for many users. In our implementation, all users (clients) talk to every other user through a server. The given Makefile enables you to perform local checks of the app, as well as using it on a _real_ non-local server. This app features up to 10 (which is easily changeable) simultaneaous clients connections and real passive waiting features using posix thread, semaphores and blocking i/o.

## 🔰 Prerequisites

This project currently suppose you do run a recent enough version of Linux with `pthread` and `libreadline-dev` up and ready.

```bash
sudo apt -y install < requirements
```

Here you can find the code standard used during dev. The compatibility version corresponds to the minimum version in which the program is guaranteed (the real min version could be much lower) to perform as originally intended regarding the terms and conditions mentioned in [LICENSE](LICENSE) and the [main-client](src/main-client.c) and [main-server](src/main-server.c) source files.

| dev version used  | compatibility version |
| ----------------- | --------------------- |
| gcc-11            | gcc-9                 |
| -std=gnu17        | -std=gnu11            |
| linux kernel 5.15 | linux kernel 5.13     |
| valgrind-3.18.1   | valgrind-3.15.0       |
| python 3.10.4     | python 3.9.12         |

## 👩‍🏫 Usage

Compile debug versions with

```bash
make debug
```

Compile release versions with

```bash
make release
```

Alternatively, `make debug-client` produces a debug version of the client side executable binary, `make debug-server` a debug version of the server side executable binary ; `make release-client` and `make release-server` produce either a release version of the client side executable, or the server side executable.

We also did provide commands to quick launch our app :

```bash
make run-client-rpi
```

is kind of the command you need to quickly get started. It basically launches for you a release client side executable and tries a connection to a rpi3 where a server is supposed to be running (_hopefully_).

If you are a "I'ma try it myself" kind of person, try `make run-server &` to launch a local server and then launch a bunch of clients with `make run-client`.

Choose a username and you are good to go !

You can also run tests with

```bash
cd tests
make check
make clean
cd ..
```

## ⚖️ License

This project is licensed under the [GPL-3.0](LICENSE) license. Please see the [license](LICENSE) file for more details.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

- Neither the name of this project authors nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

## 🔄 Changelog

Please refer to the [changelog.md](changelog.md) file for the full history.

<details>
    <summary>  Beginning (click here to expand) </summary>

**v0.1.1** multi chat server

- add a logo / new name for the project
- think about the communication between client and server
- add first features for the server and client

**v0.1.2** better code basis

- created a [legacy](legacy/) folder for the old code basis
- use of posix threads
- use of the readline extern library
- created rpi targets in the Makefile

</details>

## 🐛 Bugs & TODO

**Bugs** (final correction version)

- no `select` function :/
- fixed sized descriptor table

**TODO** (first implementation version)

- [ ] peer to peer communication
- [ ] choose whom you wish to send messages
- [ ] graphical user interface
