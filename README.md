# <img src="assets/logo_bd_chat.png" alt="icon" width="4%"/>TP RIO / BD Chat

[![Linux](https://svgshare.com/i/Zhy.svg)](https://docs.microsoft.com/en-us/windows/wsl/tutorials/gui-apps)
[![GitHub license](https://img.shields.io/github/license/ThomasByr/chat-server)](https://github.com/ThomasByr/chat-server/blob/master/LICENSE)
[![GitHub commits](https://badgen.net/github/commits/ThomasByr/chat-server)](https://GitHub.com/ThomasByr/chat-server/commit/)
[![GitHub latest commit](https://badgen.net/github/last-commit/ThomasByr/chat-server)](https://gitHub.com/ThomasByr/chat-server/commit/)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/ThomasByr/chat-server/graphs/commit-activity)

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
5. [💁 F.A.Q.](#-faq)
6. [🔄 Changelog](#-changelog)
7. [🐛 Bugs & TODO](#-bugs--todo)

## ✏️ In short

## 🔰 Prerequisites

This project currently suppose you do run a recent enough version of Linux with `pthread` and `libreadline-dev` up and ready.

```bash
sudo apt -y install < requirements
```

Here you can find the code standard used during dev. The compatibility version corresponds to the minimum version in which the program is guaranteed (the real min version could be much lower) to perform as originally intended regarding the terms and conditions mentioned in [LICENSE](LICENSE) and the [main](src/main.c) source file.

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
make
```

Alternatively, `make debug-client` produces a debug version of the client side executable binary, `make debug-server` a debug version of the server side executable binary. You can clean with `make clean`.

You can also run tests with

```bash
cd tests
make check
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

## 💁 F.A.Q.

## 🔄 Changelog

Please refer to the [changelog.md](changelog.md) file for the full history.

<details>
    <summary>  Begining of the project (click here to expand) </summary>

**v0.1.1**

- add a logo / new name for the project
- think about the communication between cleint and server
- add first features for the server and client

</details>

## 🐛 Bugs & TODO

**Bugs** (final correction version)

**TODO** (first implementation version)
