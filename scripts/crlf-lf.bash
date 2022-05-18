#!/usr/bin/env bash

# one file at a time using 16 threads
find . -type f -not -path "./bin/*" -print0 | xargs -0 -n 1 -P 16 dos2unix
