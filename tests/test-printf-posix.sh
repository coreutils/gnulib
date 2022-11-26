#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-printf-posix.tmp t-printf-posix.out"
${CHECKER} ./test-printf-posix${EXEEXT} > t-printf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-printf-posix.tmp > t-printf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-printf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
