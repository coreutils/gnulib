#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' 1 2 3 15

tmpfiles="$tmpfiles t-vprintf-posix.tmp"
./test-vprintf-posix${EXEEXT} > t-vprintf-posix.tmp || exit 1

: ${DIFF=diff}
${DIFF} "${srcdir}/test-fprintf-posix.out" t-vprintf-posix.tmp
result=$?

rm -fr $tmpfiles

exit $result
