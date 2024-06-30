#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vdzprintf-posix.tmp t-vdzprintf-posix.out"
${CHECKER} ./test-vdzprintf-posix${EXEEXT} > t-vdzprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vdzprintf-posix.tmp > t-vdzprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-vdzprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
