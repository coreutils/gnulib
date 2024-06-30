#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vzprintf-posix.tmp t-vzprintf-posix.out"
${CHECKER} ./test-vzprintf-posix${EXEEXT} > t-vzprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vzprintf-posix.tmp > t-vzprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-vzprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
