#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-zprintf-posix.tmp t-zprintf-posix.out"
${CHECKER} ./test-zprintf-posix${EXEEXT} > t-zprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-zprintf-posix.tmp > t-zprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-zprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
