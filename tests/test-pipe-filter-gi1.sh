#!/bin/sh
# A small file.
./test-pipe-filter-gi1${EXEEXT} "${srcdir}/test-pipe-filter-gi1.sh" || exit 1
# A medium-sized file.
./test-pipe-filter-gi1${EXEEXT} "${srcdir}/test-pipe-filter-gi1.c" || exit 1
# A large file.
./test-pipe-filter-gi1${EXEEXT} "${srcdir}/test-vasnprintf-posix.c" || exit 1
