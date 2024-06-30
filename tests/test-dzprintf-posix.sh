#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-dzprintf-posix.tmp t-dzprintf-posix.out"
${CHECKER} ./test-dzprintf-posix${EXEEXT} > t-dzprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-dzprintf-posix.tmp > t-dzprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-dzprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
