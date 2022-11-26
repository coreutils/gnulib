#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vdprintf-posix.tmp t-vdprintf-posix.out"
${CHECKER} ./test-vdprintf-posix${EXEEXT} > t-vdprintf-posix.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vdprintf-posix.tmp > t-vdprintf-posix.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-posix.output" t-vdprintf-posix.out
result=$?

rm -fr $tmpfiles

exit $result
