#!/bin/sh

# Succeed on seekable stdin
./test-lseek${EXEEXT} < "$srcdir/test-lseek.sh" || exit 1
# Fail on pipe stdin
echo hi | ./test-lseek${EXEEXT} && exit 1
exit 0
