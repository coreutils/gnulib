#!/bin/sh

tmpfiles=""
trap 'rm -fr $tmpfiles' HUP INT QUIT TERM

tmpfiles="$tmpfiles t-vprintf-gnu.tmp t-vprintf-gnu.out"
${CHECKER} ./test-vprintf-gnu${EXEEXT} > t-vprintf-gnu.tmp || exit 1
LC_ALL=C tr -d '\r' < t-vprintf-gnu.tmp > t-vprintf-gnu.out || exit 1

: "${DIFF=diff}"
${DIFF} "${srcdir}/test-printf-gnu.output" t-vprintf-gnu.out
result=$?

rm -fr $tmpfiles

exit $result
